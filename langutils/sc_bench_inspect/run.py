#!/usr/bin/env python3
"""Benchmarks every stage of the compiler, saving one session of samples, which serve.py shows.

    python3 langutils/sc_bench_inspect/run.py [--label text] [--runs 10] [--threads 1,2,4,12]
                                              [--inputs class-library,corpus] [--no-build] [--results dir]

Each sample is one sc_inspect process, `--no-dump --bytecode --time`, so all stages run but nothing is printed. Each
input is run once at every thread count per repetition, in a shuffled order, so drift (heat, other processes) is
spread over all of them rather than landing on one. A warm up pass, not saved, fills the file cache first.

sc_inspect is built first, from build-release (or $SC_BENCH_BUILD), which must be a Release build. Sessions are saved
in results/ (or $SC_BENCH_RESULTS) as one json file each, named by time and commit.
"""

import argparse
import datetime
import hashlib
import json
import os
import platform
import random
import re
import subprocess
import sys
import tempfile
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
LANGUTILS = HERE.parent
REPO = LANGUTILS.parent
DEFAULT_BUILD = Path(os.environ.get('SC_BENCH_BUILD', REPO / 'build-release'))
DEFAULT_RESULTS = Path(os.environ.get('SC_BENCH_RESULTS', HERE / 'results'))
DEFAULT_CORPUS = Path('/home/jordan/.repo/sclang_corpus/corpus')
FORMAT_VERSION = 1

STAGE_LINE = re.compile(r'^  (\S.*?)\s+([0-9.]+) s$')


def inputs(corpus):
    """Each input's paths, the corpus' scd files given as paths, as sc_inspect only compiles those."""
    return {
        'class-library': [REPO / 'SCClassLibrary'],
        'corpus': [corpus] + sorted(corpus.rglob('*.scd')),
    }


def git(*args):
    return subprocess.run(['git', '-C', str(REPO), *args], capture_output=True, text=True).stdout.strip()


def git_info():
    status = git('status', '--porcelain', '--untracked-files=no')
    return {
        'commit': git('rev-parse', 'HEAD'),
        'branch': git('rev-parse', '--abbrev-ref', 'HEAD'),
        'subject': git('log', '-1', '--format=%s'),
        'dirty': bool(status),
        # Tells apart sessions of the same commit with different uncommitted changes.
        'diff_hash': hashlib.sha1(git('diff', 'HEAD').encode()).hexdigest()[:10] if status else None,
    }


def host_info():
    def read(path):
        try:
            return Path(path).read_text().strip()
        except OSError:
            return None

    cpu = None
    for line in (read('/proc/cpuinfo') or '').splitlines():
        if line.startswith('model name'):
            cpu = line.split(':', 1)[1].strip()
            break
    return {
        'hostname': platform.node(),
        'cpu': cpu,
        'cpus': os.cpu_count(),
        'governor': read('/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor'),
        'load_average': os.getloadavg(),
        'python': platform.python_version(),
    }


def build_type(build):
    cache = build / 'CMakeCache.txt'
    if not cache.is_file():
        return None
    for line in cache.read_text().splitlines():
        if line.startswith('CMAKE_BUILD_TYPE:'):
            return line.split('=', 1)[1]
    return None


def run_once(sc_inspect, paths, threads):
    """One sc_inspect process, its stage times, and its wall time, cpu time and peak memory from the os."""
    args = [str(sc_inspect), '--no-dump', '--bytecode', '--time', '--threads', str(threads), *map(str, paths)]
    # Output goes to a file, rather than a pipe, so wait4 can reap the process and give its own resource usage.
    with tempfile.TemporaryFile() as out:
        start = time.perf_counter()
        process = subprocess.Popen(args, stdout=out, stderr=subprocess.STDOUT)
        _, status, usage = os.wait4(process.pid, 0)
        wall = time.perf_counter() - start
        process.returncode = os.waitstatus_to_exitcode(status)
        out.seek(0)
        text = out.read().decode('utf-8', 'replace')
    if process.returncode != 0:
        sys.exit(f'sc_inspect exited with {process.returncode}:\n{text[-2000:]}')

    lines = text.splitlines()
    stages = {}
    for line in lines[lines.index('Stage times:') + 1:]:
        match = STAGE_LINE.match(line)
        if match:
            stages[match[1]] = float(match[2])
    diagnostics = next(int(l.split()[0]) for l in lines if l.endswith(' diagnostics.'))
    classes = next(int(l.split()[2]) for l in lines if l.startswith('Successfully compiled '))
    total = stages.pop('total')
    return {
        'stages': stages,
        'total': total,
        'wall': wall,
        'cpu': usage.ru_utime + usage.ru_stime,
        'max_rss_kb': usage.ru_maxrss,
        'diagnostics': diagnostics,
        'classes': classes,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--label', default='', help='a name for the session, e.g., what changed')
    parser.add_argument('--runs', type=int, default=10, help='samples per input and thread count')
    parser.add_argument('--threads', default='1,2,4,12')
    parser.add_argument('--inputs', default='class-library,corpus')
    parser.add_argument('--corpus', type=Path, default=DEFAULT_CORPUS)
    parser.add_argument('--build', type=Path, default=DEFAULT_BUILD)
    parser.add_argument('--no-build', action='store_true', help="don't build sc_inspect first")
    parser.add_argument('--results', type=Path, default=DEFAULT_RESULTS)
    parser.add_argument('--seed', type=int, default=None, help='for the shuffled order')
    args = parser.parse_args()

    threads = [int(t) for t in args.threads.split(',')]
    all_inputs = inputs(args.corpus)
    names = args.inputs.split(',')
    for name in names:
        if name not in all_inputs:
            parser.error(f"unknown input '{name}', expected one of {', '.join(all_inputs)}")
    if build_type(args.build) != 'Release':
        parser.error(f'{args.build} is not a Release build, see --build')
    sc_inspect = args.build / 'langutils' / 'sc_inspect' / 'sc_inspect'

    if not args.no_build:
        subprocess.run(['cmake', '--build', str(args.build), '--target', 'sc_inspect'], check=True)
    if not sc_inspect.is_file():
        parser.error(f'no sc_inspect at {sc_inspect}')

    # The git state is read after building, as that is what was built.
    session = {
        'format': FORMAT_VERSION,
        'label': args.label,
        'created': datetime.datetime.now().astimezone().isoformat(timespec='seconds'),
        'git': git_info(),
        'host': host_info(),
        'config': {
            'runs': args.runs,
            'threads': threads,
            'sc_inspect': str(sc_inspect),
            'inputs': {
                name: {
                    'paths': len(all_inputs[name]),
                    'scd_files': sum(1 for p in all_inputs[name] if p.suffix == '.scd'),
                }
                for name in names
            },
        },
        'samples': [],
    }

    configs = [(name, t) for name in names for t in threads]
    print(f'Warming up {", ".join(names)}', flush=True)
    for name in names:
        run_once(sc_inspect, all_inputs[name], max(threads))

    rng = random.Random(args.seed)
    order = 0
    for rep in range(args.runs):
        rng.shuffle(configs)
        for name, t in configs:
            sample = run_once(sc_inspect, all_inputs[name], t)
            session['samples'].append({'input': name, 'threads': t, 'rep': rep, 'order': order, **sample})
            order += 1
        print(f'Run {rep + 1}/{args.runs} done', flush=True)

    args.results.mkdir(parents=True, exist_ok=True)
    stamp = datetime.datetime.now().strftime('%Y%m%d-%H%M%S')
    path = args.results / f'{stamp}-{session["git"]["commit"][:10]}.json'
    path.write_text(json.dumps(session, indent=1))
    print(f'Saved {path}')


if __name__ == '__main__':
    main()
