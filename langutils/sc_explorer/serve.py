#!/usr/bin/env python3
"""Serves the explorer's web page, which shows the ast and ir of the code typed into it.

    python3 langutils/sc_explorer/serve.py [--port 8000] [--backend path] [--prelude file.sc]... [--no-prelude]

Then open http://localhost:8000. Each edit is compiled by a new sc_explorer process, so one that crashes or asserts
on half typed code only loses that request, the page shows its exit status and stderr.

The backend is sc_explorer from the build directory, or $SC_EXPLORER. The prelude is the error regression tests',
so the interpreted code has an Interpreter to compile in.
"""

import argparse
import json
import os
import subprocess
from http import HTTPStatus
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

HERE = Path(__file__).resolve().parent
LANGUTILS = HERE.parent
DEFAULT_BACKEND = LANGUTILS.parent / 'build' / 'langutils' / 'sc_explorer' / 'sc_explorer'
DEFAULT_PRELUDE = LANGUTILS / 'sc_sema' / 'tests' / 'error_regression' / 'prelude.sc'
TIMEOUT_SECONDS = 10


def compile_source(backend, preludes, source, mode):
    args = [str(backend)]
    if mode == 'class':
        args.append('--class')
    for p in preludes:
        args += ['--prelude', str(p)]
    try:
        run = subprocess.run(args, input=source.encode('utf-8'), capture_output=True, timeout=TIMEOUT_SECONDS)
    except subprocess.TimeoutExpired:
        return {'crash': {'status': None, 'stderr': f'Timed out after {TIMEOUT_SECONDS} seconds.'}}
    if run.returncode != 0:
        return {'crash': {'status': run.returncode, 'stderr': run.stderr.decode('utf-8', 'replace')}}
    return json.loads(run.stdout.decode('utf-8', 'replace'))


def make_handler(backend, preludes):
    class Handler(SimpleHTTPRequestHandler):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, directory=str(HERE / 'web'), **kwargs)

        def do_POST(self):
            if self.path != '/compile':
                self.send_error(HTTPStatus.NOT_FOUND)
                return
            request = json.loads(self.rfile.read(int(self.headers.get('Content-Length', 0))))
            body = json.dumps(compile_source(backend, preludes, request['source'], request['mode'])).encode('utf-8')
            self.send_response(HTTPStatus.OK)
            self.send_header('Content-Type', 'application/json')
            self.send_header('Content-Length', str(len(body)))
            self.end_headers()
            self.wfile.write(body)

        # Only errors, not a line per keystroke.
        def log_request(self, code='-', size='-'):
            if isinstance(code, HTTPStatus):
                code = code.value
            if str(code)[0] != '2':
                super().log_request(code, size)

    return Handler


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--port', type=int, default=8000)
    parser.add_argument('--backend', type=Path, default=Path(os.environ.get('SC_EXPLORER', DEFAULT_BACKEND)))
    parser.add_argument('--prelude', type=Path, action='append', help='replaces the default prelude')
    parser.add_argument('--no-prelude', action='store_true')
    args = parser.parse_args()

    if not args.backend.is_file():
        parser.error(f'no backend at {args.backend}, build the sc_explorer target or pass --backend')
    preludes = [] if args.no_prelude else (args.prelude or [DEFAULT_PRELUDE])

    server = ThreadingHTTPServer(('127.0.0.1', args.port), make_handler(args.backend, preludes))
    print(f'Serving on http://localhost:{args.port}, prelude: {", ".join(map(str, preludes)) or "none"}')
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == '__main__':
    main()
