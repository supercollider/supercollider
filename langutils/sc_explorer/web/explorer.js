'use strict';

// Each edit is sent to serve.py, which replies with the ast, ir and diagnostics, see sc_explorer's main.cpp. Ranges
// are byte offsets into the normalised source, so they are converted to string indices before they are drawn.

const EXAMPLES = {
  interpreted: `// Each region is its own function.
a = 1;
a + 2;
(
  var b = 3;
  if (b > 2) { b * a } { 0 }
);
`,
  class: `Foo {
  var <bar = 1;
  baz { |x = 2|
    if (x > 1) { ^x };
    ^bar + x
  }
  *new { ^super.new }
}
`,
};

const $ = (id) => document.getElementById(id);
const sourceEl = $('source');
const backdropEl = $('backdrop');
const modeEl = $('mode');
const statusEl = $('status');
const diagnosticsEl = $('diagnostics');
const trees = { ast: $('ast'), ir: $('ir') };
const toggles = { ast: $('show-ast'), ir: $('show-ir') };
const panes = { ast: $('ast-pane'), ir: $('ir-pane') };

const store = {
  get(key, fallback) {
    try {
      const v = localStorage.getItem('sc-explorer:' + key);
      return v === null ? fallback : JSON.parse(v);
    } catch {
      return fallback;
    }
  },
  set(key, value) {
    try {
      localStorage.setItem('sc-explorer:' + key, JSON.stringify(value));
    } catch {}
  },
};

const state = {
  result: null,
  // Byte offsets to string indices, only when the result is for the text in the editor.
  byteToIndex: null,
  nodes: [],
  hoverRange: null,
  selectedRange: null,
  selectedNodes: [],
  diagnosticRanges: [],
  hoverDiagnostic: null,
  requestId: 0,
};

// As sc::lex::NormalisedSource.
const normalise = (s) => s.replace(/\r\n?|[\v\f]/g, '\n');

function makeByteToIndex(text) {
  const map = [];
  for (let i = 0; i < text.length; ) {
    const cp = text.codePointAt(i);
    const bytes = cp < 0x80 ? 1 : cp < 0x800 ? 2 : cp < 0x10000 ? 3 : 4;
    for (let b = 0; b < bytes; ++b) map.push(i);
    i += cp > 0xffff ? 2 : 1;
  }
  map.push(text.length);
  return map;
}

function toIndices(range) {
  if (!range || !state.byteToIndex) return null;
  const m = state.byteToIndex;
  const at = (b) => m[Math.min(b, m.length - 1)];
  return { from: at(range.begin), to: at(range.end) };
}

// ---------------------------------------------------------------------------
// Source highlighting

const escapeHtml = (s) => s.replace(/[&<>]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;' })[c]);

function renderBackdrop() {
  const text = sourceEl.value;
  const marks = [];
  const add = (range, cls) => {
    const r = toIndices(range);
    if (r) marks.push({ ...r, cls });
  };
  for (const d of state.diagnosticRanges) add(d.range, 'diag-' + d.severity);
  if (state.selectedRange) add(state.selectedRange, 'selected');
  if (state.hoverRange) add(state.hoverRange, 'hover');
  if (state.hoverDiagnostic) for (const l of state.hoverDiagnostic.locations) add(l.range, 'hover');

  const points = new Set([0, text.length]);
  for (const m of marks) points.add(m.from).add(m.to);
  const sorted = [...points].sort((a, b) => a - b);
  let html = '';
  for (let p = 0; p < sorted.length; ++p) {
    const at = sorted[p];
    // Empty ranges are drawn as a caret before the text at their position.
    const empty = marks.filter((m) => m.from === at && m.to === at).map((m) => m.cls);
    if (empty.length) html += `<span class="empty ${empty.join(' ')}"></span>`;
    if (p + 1 === sorted.length) break;
    const next = sorted[p + 1];
    const cls = marks.filter((m) => m.from <= at && next <= m.to && m.from !== m.to).map((m) => m.cls);
    const chunk = escapeHtml(text.slice(at, next));
    html += cls.length ? `<span class="${cls.join(' ')}">${chunk}</span>` : chunk;
  }
  // So a trailing newline still has a line, and the backdrop scrolls as far as the textarea.
  backdropEl.innerHTML = html + '\n ';
  syncScroll();
}

function syncScroll() {
  backdropEl.scrollTop = sourceEl.scrollTop;
  backdropEl.scrollLeft = sourceEl.scrollLeft;
}

function scrollSourceTo(range) {
  if (!range) return;
  const lineHeight = parseFloat(getComputedStyle(sourceEl).lineHeight) || 19.5;
  const top = (range.line - 1) * lineHeight;
  if (top < sourceEl.scrollTop || top > sourceEl.scrollTop + sourceEl.clientHeight - 2 * lineHeight)
    sourceEl.scrollTop = Math.max(0, top - sourceEl.clientHeight / 3);
  syncScroll();
}

// ---------------------------------------------------------------------------
// Trees

const sameRange = (a, b) => a && b && a.begin === b.begin && a.end === b.end;
const where = (r) => (r ? `${r.line}:${r.column}` : '');

function buildNode(data, tree, parent) {
  const node = { ...data, tree, parent, id: state.nodes.length };
  state.nodes.push(node);
  const el = document.createElement('div');
  el.className = 'node' + (data.range ? '' : ' no-range');
  const row = document.createElement('div');
  row.className = 'row';
  row.dataset.id = node.id;
  const toggle = document.createElement('span');
  toggle.className = 'toggle';
  toggle.textContent = data.children.length ? '▾' : '';
  const name = document.createElement('span');
  name.className = 'name';
  name.textContent = data.name;
  const detail = document.createElement('span');
  detail.className = 'detail';
  detail.textContent = data.detail;
  detail.title = data.detail;
  const loc = document.createElement('span');
  loc.className = 'where';
  loc.textContent = where(data.range);
  row.append(toggle, name, detail, loc);
  el.append(row);
  node.el = el;
  node.row = row;
  if (data.children.length) {
    const kids = document.createElement('div');
    kids.className = 'kids';
    node.children = data.children.map((c) => buildNode(c, tree, node));
    for (const c of node.children) kids.append(c.el);
    el.append(kids);
  } else {
    node.children = [];
  }
  return node;
}

function renderTree(key, sections) {
  const el = trees[key];
  el.replaceChildren();
  el.classList.remove('stale');
  const roots = [];
  for (const [title, list] of sections) {
    if (!list.length) continue;
    if (title) {
      const h = document.createElement('div');
      h.className = 'section';
      h.textContent = `${title} (${list.length})`;
      el.append(h);
    }
    for (const r of list) {
      const n = buildNode(r, key, null);
      roots.push(n);
      el.append(n.el);
    }
  }
  if (!roots.length) {
    const note = document.createElement('div');
    note.className = 'empty-note';
    note.textContent = key === 'ir' ? 'No IR, see the diagnostics.' : 'Nothing parsed.';
    el.append(note);
  }
  return roots;
}

function setCollapsed(node, collapsed) {
  if (!node.children.length) return;
  node.el.classList.toggle('collapsed', collapsed);
  node.row.firstChild.textContent = collapsed ? '▸' : '▾';
}

function revealNode(node) {
  for (let p = node.parent; p; p = p.parent) setCollapsed(p, false);
  node.row.scrollIntoView({ block: 'nearest' });
}

// Rows in the other tree with the same range, e.g., the ir made from the hovered ast node.
function markSame(range) {
  for (const n of state.nodes) n.row.classList.toggle('same', !!range && sameRange(n.range, range));
}

function select(nodes, range) {
  for (const n of state.selectedNodes) n.row.classList.remove('selected');
  state.selectedNodes = nodes;
  for (const n of nodes) {
    n.row.classList.add('selected');
    revealNode(n);
  }
  state.selectedRange = range;
  renderBackdrop();
}

// The innermost node of a tree containing the byte offset, preferring the last, which is the deepest.
function deepestAt(key, offset) {
  let best = null;
  for (const n of state.nodes) {
    if (n.tree !== key || !n.range || !toggles[key].checked) continue;
    const { begin, end } = n.range;
    if (begin <= offset && offset < end && (!best || end - begin <= best.range.end - best.range.begin)) best = n;
  }
  return best;
}

for (const [key, el] of Object.entries(trees)) {
  el.addEventListener('mouseover', (e) => {
    const row = e.target.closest('.row');
    if (!row) return;
    const node = state.nodes[row.dataset.id];
    state.hoverRange = node.range;
    markSame(node.range);
    renderBackdrop();
  });
  el.addEventListener('mouseleave', () => {
    state.hoverRange = null;
    markSame(null);
    renderBackdrop();
  });
  el.addEventListener('click', (e) => {
    const row = e.target.closest('.row');
    if (!row) return;
    const node = state.nodes[row.dataset.id];
    if (e.target.classList.contains('toggle')) {
      setCollapsed(node, !node.el.classList.contains('collapsed'));
      return;
    }
    select([node], node.range);
    scrollSourceTo(node.range);
  });
}

document.querySelectorAll('[data-expand], [data-collapse]').forEach((b) =>
  b.addEventListener('click', () => {
    const key = b.dataset.expand || b.dataset.collapse;
    const collapsed = !!b.dataset.collapse;
    for (const n of state.nodes) if (n.tree === key) setCollapsed(n, collapsed && !!n.parent);
  })
);

// Clicking or moving the caret in the source selects the innermost node there, in each tree.
function selectAtCaret() {
  if (!state.byteToIndex) return;
  const index = sourceEl.selectionStart;
  const offset = state.byteToIndex.indexOf(index);
  if (offset < 0) return;
  const found = ['ast', 'ir'].map((k) => deepestAt(k, offset)).filter(Boolean);
  select(found, found.length ? found[0].range : null);
}
sourceEl.addEventListener('click', selectAtCaret);
sourceEl.addEventListener('keyup', (e) => {
  if (e.key.startsWith('Arrow') || e.key === 'Home' || e.key === 'End') selectAtCaret();
});

// ---------------------------------------------------------------------------
// Diagnostics

function renderDiagnostics(result) {
  diagnosticsEl.replaceChildren();
  state.diagnosticRanges = [];
  $('diag-count').textContent = result.diagnostics.length ? `(${result.diagnostics.length})` : '';
  for (const d of result.diagnostics) {
    for (const l of d.locations) state.diagnosticRanges.push({ range: l.range, severity: d.severity });
    const el = document.createElement('div');
    el.className = 'diagnostic ' + d.severity;
    const title = document.createElement('span');
    title.className = 'title';
    title.textContent = d.name;
    const at = document.createElement('span');
    at.className = 'where';
    at.textContent = d.locations.length ? where(d.locations[d.locations.length - 1].range) : 'no location';
    const text = document.createElement('pre');
    text.textContent = d.text.trim().replace(/^─+\n|\n─+$/g, '');
    text.hidden = true;
    el.append(title, at, text);
    el.addEventListener('mouseenter', () => {
      state.hoverDiagnostic = d;
      renderBackdrop();
    });
    el.addEventListener('mouseleave', () => {
      state.hoverDiagnostic = null;
      renderBackdrop();
    });
    el.addEventListener('click', () => {
      text.hidden = !text.hidden;
      const last = d.locations[d.locations.length - 1];
      if (last) scrollSourceTo(last.range);
    });
    diagnosticsEl.append(el);
  }
}

function showCrash(crash) {
  statusEl.textContent = crash.status === null ? 'Timed out' : `Backend crashed (exit ${crash.status})`;
  statusEl.className = 'status crash';
  for (const el of Object.values(trees)) el.classList.add('stale');
  diagnosticsEl.replaceChildren();
  const pre = document.createElement('pre');
  pre.className = 'crash-text';
  pre.textContent = crash.stderr || 'No output.';
  diagnosticsEl.append(pre);
  state.byteToIndex = null;
  state.diagnosticRanges = [];
  renderBackdrop();
}

// ---------------------------------------------------------------------------
// Compiling

function show(result) {
  state.result = result;
  state.byteToIndex = result.source === normalise(sourceEl.value) ? makeByteToIndex(sourceEl.value) : null;
  state.nodes = [];
  state.selectedNodes = [];
  state.selectedRange = null;
  state.hoverRange = null;
  renderTree('ast', [
    ['', result.ast.roots],
    ['Orphans', result.ast.orphans],
  ]);
  renderTree('ir', [['', result.ir.roots]]);
  renderDiagnostics(result);
  const errors = result.diagnostics.filter((d) => d.severity === 'error').length;
  statusEl.textContent = errors ? `${errors} error${errors > 1 ? 's' : ''}` : 'OK';
  statusEl.className = 'status';
  renderBackdrop();
}

async function compile() {
  const id = ++state.requestId;
  statusEl.textContent = 'Compiling…';
  statusEl.className = 'status';
  try {
    const response = await fetch('/compile', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ source: sourceEl.value, mode: modeEl.value }),
    });
    const result = await response.json();
    // A later edit has been sent.
    if (id !== state.requestId) return;
    if (result.crash) showCrash(result.crash);
    else show(result);
  } catch (e) {
    if (id !== state.requestId) return;
    statusEl.textContent = 'Server unreachable: ' + e.message;
    statusEl.className = 'status crash';
  }
}

let timer = null;
function scheduleCompile() {
  // Until the new result arrives, the old ranges don't match the text.
  state.byteToIndex = null;
  renderBackdrop();
  clearTimeout(timer);
  timer = setTimeout(compile, 200);
}

sourceEl.addEventListener('input', () => {
  store.set('source:' + modeEl.value, sourceEl.value);
  scheduleCompile();
});
sourceEl.addEventListener('scroll', syncScroll);
// Tab indents, rather than leaving the editor.
sourceEl.addEventListener('keydown', (e) => {
  if (e.key !== 'Tab' || e.ctrlKey || e.altKey || e.metaKey) return;
  e.preventDefault();
  document.execCommand('insertText', false, '    ');
});

modeEl.addEventListener('change', () => {
  store.set('mode', modeEl.value);
  sourceEl.value = store.get('source:' + modeEl.value, EXAMPLES[modeEl.value]);
  scheduleCompile();
});

function layoutPanes() {
  let columns = 1;
  for (const key of ['ast', 'ir']) {
    panes[key].classList.toggle('hidden', !toggles[key].checked);
    columns += toggles[key].checked ? 1 : 0;
    store.set('show:' + key, toggles[key].checked);
  }
  document.getElementById('panes').style.setProperty('--columns', columns);
}
for (const t of Object.values(toggles)) t.addEventListener('change', layoutPanes);

modeEl.value = store.get('mode', 'interpreted') === 'class' ? 'class' : 'interpreted';
for (const key of ['ast', 'ir']) toggles[key].checked = store.get('show:' + key, true);
sourceEl.value = store.get('source:' + modeEl.value, EXAMPLES[modeEl.value]);
layoutPanes();
renderBackdrop();
compile();
