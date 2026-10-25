#!/usr/bin/env python3
"""Compares old sclang's bytecode with the backend's, the ground truth for legacy parity. differential_test.py runs
these steps over the class library and sc_sema's Legacy bytecode tests, with an allowlist. By hand:

1. Dump old sclang's bytes and tables, with a config from sclang_conf.yaml.in:
       build/lang/sclang -l conf.yaml langutils/sc_backend/tools/dump_old_bytecode.scd old.txt
2. Dump the backend's, given exactly what sclang compiled, as one module. sclang skips directories named after other
   platforms anywhere (windows, osx, iphone, wasm), sc_inspect doesn't, and sc_inspect doesn't follow symlinked
   directories, so copy the library (cp -rL) and delete those first:
       build/langutils/sc_inspect/sc_inspect --bytecode library_copy > new.txt
3. Compare, every method by class and name, sorting differences into inlining (B4), hard coded methods (B5d), and the
   rest, which are printed. methType isn't readable from sclang, so a shortcut is compared as a method without code
   and what its selectors hold (the literal returned or the selector sent). Methods only one compiler compiled are
   printed as MISSING (old only) and EXTRA (backend only):
       compare_bytecode.py --all old.txt new.txt
   or the methods of some classes, in order:
       compare_bytecode.py old.txt new.txt Class ...

Each function's properties (context, closed, heap context) are compared too; the old dump derives heap context from
its nested functions' contexts, as needsHeapContext isn't readable from sclang.

Accessors are shortcut methods too, printed before the class's functions, as old sclang added them first.
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
            cur = {'name': m.group(1).strip(), 'properties': '', 'shortcut': None, 'code': [], 'selectors': [],
                   'constants': []}
            funcs.append(cur)
            continue
        if cur is None:
            continue
        m = re.match(r'  properties:(.*)', line)
        if m:
            cur['properties'] = m.group(1).strip()
            continue
        m = re.match(r'  shortcut: (.*)', line)
        if m:
            cur['shortcut'] = norm_constant(m.group(1))
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
    parts, depth, cur, quote, escaped = [], 0, '', None, False
    for ch in s:
        if quote:
            cur += ch
            if escaped:
                escaped = False
            elif ch == '\\':
                escaped = True
            elif ch == quote:
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
        m = re.match(r'\|  function (\d+) (?:.*: method (\S+)$)?', line)
        if m:
            cur = {'name': m.group(2) or '  nested', 'index': int(m.group(1)), 'properties': '', 'shortcut': None,
                   'code': [], 'selectors': [], 'constants': [], 'nested': []}
            funcs.setdefault(cls, []).append(cur)
            continue
        m = re.match(r'\|  accessor .*: method (\S+)$', line)
        if m:
            cur = {'name': m.group(1), 'properties': '', 'shortcut': None, 'code': [], 'selectors': [],
                   'constants': [], 'nested': []}
            funcs.setdefault(cls, []).append(cur)
            continue
        m = re.match(r'\|  \|  properties:(.*)', line)
        if m:
            cur['properties'] = m.group(1).strip()
            continue
        m = re.match(r'\|  \|  shortcut: (.*)', line)
        if m:
            cur['shortcut'] = shortcut_payload(m.group(1), cls)
            continue
        m = re.match(r'\|  \|  selectors:(.*)', line)
        if m:
            entries = [s.strip() for s in m.group(1).split(',') if s.strip()]
            cur['nested'] = [int(e.split()[1]) for e in entries if re.fullmatch(r'function \d+', e)]
            cur['selectors'] = ['function' if re.fullmatch(r'function \d+', e) else e for e in entries]
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


def shortcut_payload(s, cls):
    """What old sclang stores in a shortcut's selectors: the literal returned, the selector sent, a class variable
    accessor's class name, or nil."""
    if re.fullmatch(r'(?:return|assign) class variable \d+', s):
        return '\\' + re.sub(r'^Meta_', '', cls)
    m = re.fullmatch(r'return literal (.*)', s)
    if m:
        return norm_constant(m.group(1))
    m = re.fullmatch(r'(?:redirect(?: to super)?|forward) (\S+).*', s)
    if m:
        return '\\' + m.group(1)
    return 'nil'


def shortcut_problem(o, n):
    """Old sclang's methType isn't readable, so a shortcut is a method without code, and its selectors."""
    if o['shortcut'] is None and n['shortcut'] is None:
        return None
    if o['shortcut'] is None:
        return 'shortcut old none new %s' % n['shortcut']
    if n['shortcut'] is None:
        return 'shortcut old %s new none' % o['shortcut']
    if o['shortcut'] != n['shortcut']:
        return 'shortcut old %s new %s' % (o['shortcut'], n['shortcut'])
    return None


def main():
    old = parse_old(sys.argv[1])
    new = parse_new(sys.argv[2], sys.argv[3:])
    news = [f for c in (sys.argv[3:] or new.keys()) for f in new.get(c, [])]
    same = 0
    for o, n in zip(old, news):
        problems = []
        if p := shortcut_problem(o, n):
            problems.append(p)
        if o['name'].strip() != n['name'].strip():
            problems.append('name %s vs %s' % (o['name'], n['name']))
        oi, ni = instructions(o['code']), instructions(n['code'])
        if oi != ni:
            problems.append('code\n      old: %s\n      new: %s' % (' | '.join(oi), ' | '.join(ni)))
        if o['selectors'] != n['selectors']:
            problems.append('selectors old %s new %s' % (o['selectors'], n['selectors']))
        if o['constants'] != n['constants']:
            problems.append('constants old %s new %s' % (o['constants'], n['constants']))
        if o['properties'] != n['properties']:
            problems.append('properties old %s new %s' % (o['properties'], n['properties']))
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


def in_selector_order(fs):
    """A method's functions depth first through their selectors, the order the old dump writes them in, which isn't
    the order the backend numbers them, e.g., an if's else function is added to the selectors first."""
    if not fs or 'index' not in fs[0]:
        return fs
    by_index = {f['index']: f for f in fs}
    out = []

    def visit(f):
        out.append(f)
        for n in f['nested']:
            if n in by_index:
                visit(by_index[n])

    visit(fs[0])
    return out if len(out) == len(fs) else fs


def switch_table(c):
    """An inlined switch's table, an identity dictionary of keys to offsets whose order is the runtime's hashes, as
    `switch {key: offset, ...; default}`, from old sclang's array or the backend's description."""
    m = re.fullmatch(r'switch table \(capacity (\d+)(.*), default: (\d+)\)', c)
    if m:
        pairs = [p.rsplit(':', 1) for p in split_top(m.group(2)) if p.strip()]
        keys = sorted('%s: %s' % (k.strip(), v.strip()) for k, v in pairs)
        return 'switch %s {%s; default %s}' % (m.group(1), ', '.join(keys), m.group(3))
    if not (c.startswith('[') and c.endswith(']')):
        return c
    items = [x.strip() for x in split_top(c[1:-1])]
    if len(items) < 4 or len(items) & (len(items) - 1) or not all(re.fullmatch(r'\d+', v) for v in items[1::2]):
        return c
    defaults = {v for k, v in zip(items[::2], items[1::2]) if k == 'nil'}
    if len(defaults) != 1:
        return c
    keys = sorted('%s: %s' % (k, v) for k, v in zip(items[::2], items[1::2]) if k != 'nil')
    return 'switch %d {%s; default %s}' % (len(items), ', '.join(keys), defaults.pop())


def normalise_switches(fs):
    for f in fs:
        if any(i.startswith('8F 1C') for i in instructions(f['code'])):
            f['constants'] = [switch_table(c) for c in f['constants']]


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
    """Returns the counts, the differences as [((class, method), problems)], the methods only old sclang compiled
    and those only the backend did."""
    old = group(parse_old_classes(old_path))
    new_funcs = parse_new(new_path, [])
    new = group([(c, f) for c, fs in new_funcs.items() for f in fs])
    stats = {'match': 0, 'shortcut': 0, 'hard coded': 0, 'inlining': 0, 'other': 0, 'missing': 0}
    others, missing = [], []
    for key, ofs in old.items():
        if key not in new:
            stats['missing'] += 1
            missing.append(key)
            continue
        nfs = in_selector_order(new[key])
        normalise_switches(ofs)
        normalise_switches(nfs)
        problems = []
        if p := shortcut_problem(ofs[0], nfs[0]):
            problems.append(p)
        elif ofs[0]['shortcut'] is not None:
            stats['shortcut'] += 1
            continue
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
            if o['properties'] != n['properties']:
                problems.append('properties old %s new %s' % (o['properties'] or '-', n['properties'] or '-'))
        if not problems:
            stats['match'] += 1
        elif ofs[0]['code'] and ofs[0]['code'][0] == 0x8F and len(ofs) == 1:
            stats['hard coded'] += 1
        elif uses_inlining(ofs):
            stats['inlining'] += 1
            others.append((key, problems))
        else:
            stats['other'] += 1
            others.append((key, problems))
    extra = [key for key in new if key not in old]
    return stats, others, missing, extra


def print_difference(key, problems):
    print('DIFF %s:%s\n    %s' % (key[0], key[1], '\n    '.join(problems)))


def print_all(old_path, new_path):
    stats, others, missing, extra = compare_all(old_path, new_path)
    for key, problems in others:
        print_difference(key, problems)
    for key in missing:
        print('MISSING %s:%s' % key)
    for key in extra:
        print('EXTRA %s:%s' % key)
    print(stats)


if __name__ == '__main__':
    if len(sys.argv) == 4 and sys.argv[1] == '--all':
        print_all(sys.argv[2], sys.argv[3])
    elif len(sys.argv) >= 3 and sys.argv[1] != '--all':
        main()
    else:
        print(__doc__)
        sys.exit(1)
