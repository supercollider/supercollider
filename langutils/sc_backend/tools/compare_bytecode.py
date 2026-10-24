#!/usr/bin/env python3
"""Compares old sclang's bytecode with the backend's, the ground truth for legacy parity, see B7 in the plan.

1. Dump old sclang's bytes and tables, with a config from sclang_conf.yaml.in:
       build/lang/sclang -l conf.yaml langutils/sc_backend/tools/dump_old_bytecode.scd old.txt
2. Dump the backend's, given exactly what sclang compiled, as one module. sclang skips directories named after other
   platforms anywhere (windows, osx, iphone, wasm), ir_dump doesn't, and ir_dump doesn't follow symlinked
   directories, so copy the library (cp -rL) and delete those first:
       build/langutils/sc_sema/utils/ir_dump/ir_dump --bytecode library_copy > new.txt
3. Compare, every method by class and name, sorting differences into inlining (B4), hard coded methods and methods
   without code (B5, methType shortcuts), and the rest, which are printed:
       compare_bytecode.py --all old.txt new.txt
   or the methods of some classes, in order:
       compare_bytecode.py old.txt new.txt Class ...

Class variable operands are masked, as the backend numbers class variables differently, the loader sets them (B8).
One line methods get methType shortcuts and no code, so a test method should start with a dummy statement, e.g. `cv;`.
"""
import re
import sys


def length(code, i):
    b = code[i]
    if b <= 0x0F:
        return [2, 2, 3, 2, 2, 3, 2, 2, 3, 3, 4, 4, 4, 2, 2, 2][b]
    if b <= 0x1F: return 1
    if b == 0x20: return 3
    if b <= 0x27: return 2
    if b <= 0x2B: return [2, 3, 4, 5][b - 0x28]
    if b <= 0x2F: return [2, 3, 4, 5][b - 0x2C]
    if b <= 0x4F: return 1
    if b <= 0x5F: return 2
    if b <= 0x7F: return 1
    if b <= 0x87: return 2
    if b == 0x88: return 3
    if b <= 0x8E: return 2
    if b == 0x8F: return 4 if code[i + 1] in (0x17, 0x18, 0x19, 0x1A, 0x1B) else 2
    if b <= 0x9F: return 2
    if b == 0xA0: return 2
    if b <= 0xAF: return 2
    if b == 0xB0: return 1
    if b <= 0xBF: return 2
    if b <= 0xCF: return 2
    if b <= 0xF7: return 1
    if b <= 0xFD: return 3
    if b == 0xFE: return 2
    return 1


def instructions(code):
    out, i = [], 0
    while i < len(code):
        n = length(code, i)
        ins = code[i:i + n]
        b = ins[0]
        if 0x50 <= b <= 0x5F or 0x90 <= b <= 0x9F:
            ins = ['CV%02X' % (b & 0xF0)]
        elif b in (0x05, 0x09):
            ins = ['CVX%02X' % b]
        else:
            ins = ['%02X' % x for x in ins]
        out.append(' '.join(ins))
        i += n
    return out


def norm_constant(c):
    c = c.strip()
    if c.startswith('"'):
        return c
    c = re.sub(r"'([^']*)'", lambda m: '\\' + m.group(1), c)
    c = re.sub(r'\[\s+', '[', c)
    c = re.sub(r'\s+\]', ']', c)
    c = re.sub(r'-?\d+\.\d+(e[-+]?\d+)?', lambda m: '%.6g' % float(m.group(0)), c)
    return c


def parse_old(path):
    funcs, cur = [], None
    for line in open(path, encoding='utf-8', errors='replace'):
        line = line.rstrip('\n')
        m = re.match(r'FUNC (.*)', line)
        if m:
            cur = {'name': m.group(1).strip(), 'code': [], 'selectors': [], 'constants': []}
            funcs.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r'  code: (.*)', line)
        if m:
            v = m.group(1).strip()
            cur['code'] = [] if v == 'nil' else [int(x, 16) for x in v.split()]
            continue
        m = re.match(r'  selectors: (.*)', line)
        if m:
            cur['selectors'] = [s.strip() for s in m.group(1).split(',') if s.strip()]
            continue
        m = re.match(r'  constants: (.*)', line)
        if m:
            cur['constants'] = [norm_constant(s) for s in split_top(m.group(1)) if s.strip()]
    return funcs


def split_top(s):
    parts, depth, cur, quote = [], 0, '', None
    for ch in s:
        if quote:
            cur += ch
            if ch == quote:
                quote = None
            continue
        if ch in '"\'':
            quote = ch
        elif ch in '[(':
            depth += 1
        elif ch in '])':
            depth -= 1
        elif ch == ',' and depth == 0:
            parts.append(cur)
            cur = ''
            continue
        cur += ch
    if cur.strip():
        parts.append(cur)
    return parts


def parse_new(path, classes):
    text = open(path, encoding='utf-8', errors='replace').read()
    text = text[text.index('BYTECODE DUMP'):]
    funcs, cls, cur = {}, None, None
    for line in text.splitlines():
        if line and not line.startswith('|'):
            cls = line.strip()
            continue
        m = re.match(r'\|  function \d+ (?:.*: method (\S+)$)?', line)
        if m:
            cur = {'name': m.group(1) or '  nested', 'code': [], 'selectors': [], 'constants': []}
            funcs.setdefault(cls, []).append(cur)
            continue
        m = re.match(r'\|  \|  selectors:(.*)', line)
        if m:
            cur['selectors'] = ['function' if re.fullmatch(r'function \d+', s.strip()) else s.strip()
                                for s in m.group(1).split(',') if s.strip()]
            continue
        m = re.match(r'\|  \|  constants:(.*)', line)
        if m:
            body = m.group(1).strip().rstrip(',')
            cur['constants'] = [norm_constant(s) for s in split_top(body) if s.strip()]
            continue
        m = re.match(r'\|  \|  \d+\s+((?:[0-9A-F]{2} )+)', line)
        if m:
            cur['code'] += [int(x, 16) for x in m.group(1).split()]
    return funcs


def main():
    old = parse_old(sys.argv[1])
    new = parse_new(sys.argv[2], sys.argv[3:])
    news = [f for c in (sys.argv[3:] or new.keys()) for f in new.get(c, [])]
    same = 0
    for o, n in zip(old, news):
        problems = []
        if o['name'].strip() != n['name'].strip():
            problems.append('name %s vs %s' % (o['name'], n['name']))
        oi, ni = instructions(o['code']), instructions(n['code'])
        if oi != ni:
            problems.append('code\n      old: %s\n      new: %s' % (' | '.join(oi), ' | '.join(ni)))
        if o['selectors'] != n['selectors']:
            problems.append('selectors old %s new %s' % (o['selectors'], n['selectors']))
        if o['constants'] != n['constants']:
            problems.append('constants old %s new %s' % (o['constants'], n['constants']))
        if problems:
            print('DIFF %s:\n    %s' % (o['name'], '\n    '.join(problems)))
        else:
            same += 1
    if len(old) != len(news):
        print('count old %d new %d' % (len(old), len(news)))
    print('%d of %d match' % (same, len(old)))



def group(funcs_with_class):
    """[(class, func)] -> {(class, method): [method, nested...]}"""
    out, key = {}, None
    for cls, f in funcs_with_class:
        if f['name'].strip() != 'nested':
            key = (cls, f['name'].strip())
            out[key] = [f]
        elif key:
            out[key].append(f)
    return out


def parse_old_classes(path):
    res, cls = [], None
    funcs = parse_old(path)
    # parse_old loses the classes, so read them again in step.
    names = []
    for line in open(path, encoding='utf-8', errors='replace'):
        m = re.match(r'CLASS (.*)', line)
        if m:
            cls = m.group(1).strip()
        elif line.startswith('FUNC'):
            names.append(cls)
    return list(zip(names, funcs))


INLINED = {0x20, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD}
CONTROL = {'if', 'while', 'and', 'or', 'case', 'switch', 'loop', '?', '??', '!?'}


def uses_inlining(fs):
    for f in fs:
        for ins in instructions(f['code']):
            b = ins.split()
            if b[0].startswith('CV'):
                continue
            first = int(b[0], 16)
            if first in INLINED or (first == 0x8F and len(b) > 1 and int(b[1], 16) >= 0x16):
                return True
    return False


def compare_all(old_path, new_path):
    old = group(parse_old_classes(old_path))
    new_funcs = parse_new(new_path, [])
    new = group([(c, f) for c, fs in new_funcs.items() for f in fs])
    stats = {'match': 0, 'nil code': 0, 'hard coded': 0, 'inlining': 0, 'other': 0, 'missing': 0}
    others = []
    for key, ofs in old.items():
        if key not in new:
            stats['missing'] += 1
            continue
        nfs = new[key]
        if not ofs[0]['code']:
            stats['nil code'] += 1
            continue
        problems = []
        if len(ofs) != len(nfs):
            problems.append('functions old %d new %d' % (len(ofs), len(nfs)))
        for o, n in zip(ofs, nfs):
            oi, ni = instructions(o['code']), instructions(n['code'])
            if oi != ni:
                k = next((j for j in range(min(len(oi), len(ni))) if oi[j] != ni[j]), min(len(oi), len(ni)))
                lo = max(0, k - 4)
                problems.append('code at %d\n      old: %s\n      new: %s' % (k, ' | '.join(oi[lo:k + 6]), ' | '.join(ni[lo:k + 6])))
            if o['selectors'] != n['selectors']:
                problems.append('selectors old %s new %s' % (o['selectors'][:12], n['selectors'][:12]))
            if o['constants'] != n['constants']:
                k = next((j for j in range(min(len(o['constants']), len(n['constants']))) if o['constants'][j] != n['constants'][j]), 0)
                problems.append('constants at %d old %s new %s' % (k, o['constants'][k:k + 3], n['constants'][k:k + 3]))
        if not problems:
            stats['match'] += 1
        elif ofs[0]['code'][0] == 0x8F and len(ofs) == 1:
            stats['hard coded'] += 1
        elif uses_inlining(ofs):
            stats['inlining'] += 1
        else:
            stats['other'] += 1
            others.append((key, problems))
    for key, problems in others:
        print('DIFF %s:%s\n    %s' % (key[0], key[1], '\n    '.join(problems)))
    print(stats)



if __name__ == '__main__':
    if len(sys.argv) == 4 and sys.argv[1] == '--all':
        compare_all(sys.argv[2], sys.argv[3])
    elif len(sys.argv) >= 3 and sys.argv[1] != '--all':
        main()
    else:
        print(__doc__)
        sys.exit(1)
