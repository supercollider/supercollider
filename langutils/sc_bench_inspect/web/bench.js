'use strict';

// Draws the sessions run.py saved, served by serve.py at /api/sessions. Each session holds samples, one per
// sc_inspect process, of an input at a thread count, see run.py. Everything shown is for one input at a time, and
// the compared sessions (up to three, the first the baseline) at the chosen thread counts and metrics.

const $ = (id) => document.getElementById(id);
const SVG_NS = 'http://www.w3.org/2000/svg';
const MAX_COMPARED = 3;

// sc_inspect's stage names, see StageTimes in sc_inspect's main.cpp.
const STAGES = ['find files', 'parse', 'sema', 'interpreted parse', 'interpreted sema', 'member table', 'frames',
  'lower', 'emit'];
const stageSum = (s) => STAGES.reduce((a, k) => a + (s.stages[k] ?? 0), 0);

const METRICS = [
  ...STAGES.map((k) => ({ key: k, label: k, unit: 's', get: (s) => s.stages[k] ?? 0, group: 'stage' })),
  { key: 'total', label: 'in-process total', unit: 's', get: (s) => s.total, group: 'process',
    about: "sc_inspect's own total, every stage and the steps between them" },
  { key: 'outside', label: 'outside stages', unit: 's', get: (s) => s.wall - stageSum(s), group: 'process',
    about: 'process wall time not in a stage: starting, freeing everything at exit, printing' },
  { key: 'wall', label: 'process wall', unit: 's', get: (s) => s.wall, group: 'process',
    about: 'from starting sc_inspect until it exited' },
  { key: 'cpu', label: 'cpu time', unit: 's', get: (s) => s.cpu, group: 'process',
    about: 'user and system time over all threads' },
  { key: 'rss', label: 'peak memory', unit: 'MB', get: (s) => s.max_rss_kb / 1024, group: 'process',
    about: 'maximum resident set size' },
];
const METRIC = Object.fromEntries(METRICS.map((m) => [m.key, m]));

// The breakdown's parts, which sum to the process wall time, one categorical slot each.
const PARTS = [
  { label: 'parse', get: (s) => s.stages['parse'] },
  { label: 'sema', get: (s) => s.stages['sema'] },
  { label: 'interpreted parse', get: (s) => s.stages['interpreted parse'] },
  { label: 'interpreted sema', get: (s) => s.stages['interpreted sema'] },
  { label: 'frames', get: (s) => s.stages['frames'] },
  { label: 'lower', get: (s) => s.stages['lower'] },
  { label: 'emit', get: (s) => s.stages['emit'] },
  { label: 'other', get: (s) => s.stages['find files'] + s.stages['member table'] + (s.wall - stageSum(s)),
    about: 'finding files, the member table, starting and exiting the process' },
];

const VIEWS = [
  { key: 'compare', label: 'Compare' },
  { key: 'distributions', label: 'Distributions' },
  { key: 'breakdown', label: 'Breakdown' },
  { key: 'scaling', label: 'Scaling' },
  { key: 'history', label: 'History' },
];

const DEFAULTS = {
  view: 'compare',
  input: 'corpus',
  statistic: 'median',
  threads: null,
  metrics: ['parse', 'sema', 'interpreted parse', 'interpreted sema', 'frames', 'lower', 'emit', 'wall'],
  compared: [],
  colours: {},
  relative: false,
  fromZero: false,
  share: false,
  speedup: true,
  threshold: 1,
};

const store = {
  load() {
    try {
      return { ...DEFAULTS, ...JSON.parse(localStorage.getItem('sc-bench-inspect') ?? '{}') };
    } catch {
      return { ...DEFAULTS };
    }
  },
  save(state) {
    try {
      localStorage.setItem('sc-bench-inspect', JSON.stringify(state));
    } catch { /* private window */ }
  },
};

let state = store.load();
let sessions = [];

// A link can open a view of some sessions, e.g., #view=distributions&compare=id1,id2&input=corpus.
function readHash() {
  const params = new URLSearchParams(location.hash.slice(1));
  if (VIEWS.some((v) => v.key === params.get('view'))) state.view = params.get('view');
  if (params.get('input')) state.input = params.get('input');
  if (params.get('compare')) {
    state.compared = params.get('compare').split(',').slice(0, MAX_COMPARED);
    state.colours = Object.fromEntries(state.compared.map((id, i) => [id, i]));
  }
}
readHash();
const byId = () => Object.fromEntries(sessions.map((s) => [s.id, s]));

// ---- data

function samplesOf(session, input, threads) {
  return session.samples.filter((s) => s.input === input && s.threads === threads);
}

function valuesOf(session, input, threads, metric) {
  return samplesOf(session, input, threads).map(METRIC[metric].get);
}

function statistic(values) {
  return values.length ? Stats.STATISTICS[state.statistic](values) : NaN;
}

// The chosen metrics the input ran, e.g., the class library has no interpreted code, so those stages are always 0.
function shownMetrics() {
  const sessionsShown = state.view === 'history' ? sessions : comparedSessions();
  const ran = (key) => sessionsShown.some((s) =>
    s.samples.some((x) => x.input === state.input && METRIC[key].get(x) !== 0));
  return state.metrics.filter(ran);
}

function comparedSessions() {
  const ids = byId();
  return state.compared.map((id) => ids[id]).filter(Boolean);
}

function allThreads() {
  const set = new Set();
  for (const s of sessions) for (const t of s.config.threads) set.add(t);
  return [...set].sort((a, b) => a - b);
}

function shownThreads() {
  const all = allThreads();
  return state.threads ? all.filter((t) => state.threads.includes(t)) : all;
}

function allInputs() {
  const set = new Set();
  for (const s of sessions) for (const k of Object.keys(s.config.inputs)) set.add(k);
  return [...set];
}

// A session keeps its colour while compared, so removing one doesn't repaint the rest.
function colourOf(session) {
  return `var(--series-${(state.colours[session.id] ?? 0) + 1})`;
}

function sessionName(s) {
  return s.label || s.git.subject || s.id;
}

function shortCommit(s) {
  return s.git.commit.slice(0, 7) + (s.git.dirty ? `+${s.git.diff_hash ?? 'dirty'}` : '');
}

// ---- formatting

// One unit for an axis or a column, from its largest value.
function timeUnit(max) {
  return max < 1 ? { scale: 1000, suffix: 'ms' } : { scale: 1, suffix: 's' };
}

function formatValue(value, unit, digits) {
  if (!Number.isFinite(value)) return '–';
  if (unit === 'MB') return `${value.toFixed(digits ?? 0)} MB`;
  if (unit === '%') {
    const rounded = value.toFixed(digits ?? 1);
    return `${+rounded > 0 ? '+' : +rounded < 0 ? '' : '±'}${+rounded === 0 ? (0).toFixed(digits ?? 1) : rounded}%`;
  }
  if (unit === 'x') return `${value.toFixed(digits ?? 2)}×`;
  const u = timeUnit(Math.abs(value));
  const v = value * u.scale;
  const d = digits ?? (Math.abs(v) < 10 ? 2 : Math.abs(v) < 100 ? 1 : 0);
  return `${v.toFixed(d)} ${u.suffix}`;
}

function formatPercent(ratio) {
  return formatValue((ratio - 1) * 100, '%');
}

// ---- dom and svg

function el(tag, attrs = {}, parent = null, text = null) {
  const e = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) e.setAttribute(k, v);
  if (text !== null) e.textContent = text;
  if (parent) parent.appendChild(e);
  return e;
}

function svg(tag, attrs = {}, parent = null, text = null) {
  const e = document.createElementNS(SVG_NS, tag);
  for (const [k, v] of Object.entries(attrs)) e.setAttribute(k, v);
  if (text !== null) e.textContent = text;
  if (parent) parent.appendChild(e);
  return e;
}

const tooltipEl = $('tooltip');

// rows is [value, label] pairs, the value leads, see the dataviz interaction rules.
function showTooltip(event, title, rows) {
  tooltipEl.replaceChildren();
  el('div', { class: 'row' }, tooltipEl, title);
  for (const [value, label] of rows) {
    const row = el('div', { class: 'row' }, tooltipEl);
    el('span', { class: 'value' }, row, value);
    if (label) row.append(' ', label);
  }
  tooltipEl.hidden = false;
  const pad = 14;
  const { width, height } = tooltipEl.getBoundingClientRect();
  let x = event.clientX + pad;
  let y = event.clientY + pad;
  if (x + width > window.innerWidth - 8) x = event.clientX - width - pad;
  if (y + height > window.innerHeight - 8) y = event.clientY - height - pad;
  tooltipEl.style.left = `${x}px`;
  tooltipEl.style.top = `${y}px`;
}

function hideTooltip() {
  tooltipEl.hidden = true;
}

function hover(node, contents) {
  node.addEventListener('pointermove', (e) => showTooltip(e, ...contents()));
  node.addEventListener('pointerleave', hideTooltip);
}

// About five round ticks over [lo, hi].
function ticks(lo, hi, count = 5) {
  if (!(hi > lo)) return [lo];
  const raw = (hi - lo) / count;
  const step = 10 ** Math.floor(Math.log10(raw));
  const nice = [1, 2, 2.5, 5, 10].map((m) => m * step).find((s) => s >= raw);
  const out = [];
  for (let v = Math.ceil(lo / nice) * nice; v <= hi + nice * 1e-9; v += nice) out.push(+v.toPrecision(12));
  return out;
}

// A y axis with gridlines, returning the scale. unit is 's', 'MB', '%' or 'x'.
function yAxis(g, lo, hi, box, unit) {
  const t = ticks(lo, hi);
  lo = Math.min(lo, t[0]);
  hi = Math.max(hi, t[t.length - 1]);
  const y = (v) => box.top + box.height * (1 - (v - lo) / (hi - lo || 1));
  const u = unit === 's' ? timeUnit(Math.max(Math.abs(lo), Math.abs(hi))) : null;
  for (const v of t) {
    svg('line', { class: v === 0 && unit === '%' ? 'axis' : 'grid', x1: box.left, x2: box.left + box.width,
      y1: y(v), y2: y(v) }, g);
    const label = u ? `${+(v * u.scale).toPrecision(4)}${v === t[t.length - 1] ? ' ' + u.suffix : ''}`
      : unit === '%' ? `${v > 0 ? '+' : ''}${v}%` : unit === 'x' ? `${v}×` : `${v}${v === t[t.length - 1] ? ' MB' : ''}`;
    svg('text', { x: box.left - 6, y: y(v) + 4, 'text-anchor': 'end' }, g, label);
  }
  return y;
}

function panel(parent, title, sub, wide = false) {
  const p = el('div', { class: wide ? 'panel wide' : 'panel' }, parent);
  el('div', { class: 'panel-title' }, p, title);
  if (sub) el('div', { class: 'panel-sub' }, p, sub);
  return p;
}

function chartWidth(p) {
  return Math.max(280, p.clientWidth - 24);
}

function legend(parent, items, shape) {
  const l = el('div', { class: 'legend' }, parent);
  for (const { label, colour } of items) {
    const k = el('span', { class: 'key' }, l);
    el('span', { class: shape, style: `background:${colour}` }, k);
    k.append(label);
  }
}

function sessionLegend(parent) {
  legend(parent, comparedSessions().map((s, i) => ({
    label: `${sessionName(s)}${i === 0 && comparedSessions().length > 1 ? ' (baseline)' : ''}`,
    colour: colourOf(s),
  })), 'dot');
}

function empty(parent, text) {
  el('div', { class: 'empty' }, parent, text);
}

// ---- views

function renderCompare(view) {
  const compared = comparedSessions();
  const [baseline, ...candidates] = compared;
  const threads = shownThreads();
  const wrap = el('div', { class: 'table-wrap' }, view);
  const table = el('table', { class: 'compare' }, wrap);
  const head = el('tr', {}, el('thead', {}, table));
  el('th', {}, head, 'metric');
  for (const t of threads) el('th', {}, head, `${t} thread${t === 1 ? '' : 's'}`);
  const body = el('tbody', {}, table);
  for (const key of shownMetrics()) {
    const m = METRIC[key];
    const tr = el('tr', {}, body);
    const name = el('td', {}, tr, m.label);
    if (m.about) name.title = m.about;
    for (const t of threads) {
      const td = el('td', {}, tr);
      const base = valuesOf(baseline, state.input, t, key);
      if (!base.length) {
        td.textContent = '–';
        continue;
      }
      const s = Stats.summary(base);
      const line = el('div', { class: 'cell-line' }, td);
      line.append(formatValue(statistic(base), m.unit));
      el('span', { class: 'key', style: `background:${colourOf(baseline)}` }, line);
      el('div', { class: 'spread', title: 'interquartile range over the median, how noisy the samples are' }, td,
        `IQR ${((s.q3 - s.q1) / s.median * 100).toFixed(1)}%, n ${s.n}`);
      for (const c of candidates) {
        const values = valuesOf(c, state.input, t, key);
        if (!values.length) continue;
        const r = Stats.compare(base, values, state.statistic);
        if (Math.abs(r.ratio - 1) * 100 < state.threshold) r.verdict = 'same';
        const verdict = m.unit === 'MB' ? { faster: 'lower', slower: 'higher', same: 'no clear change' }
          : { faster: 'faster', slower: 'slower', same: 'no clear change' };
        const l = el('div', { class: 'cell-line' }, td);
        l.append(formatValue(statistic(values), m.unit));
        el('span', { class: 'key', style: `background:${colourOf(c)}` }, l);
        const icon = r.verdict === 'faster' ? '▼ ' : r.verdict === 'slower' ? '▲ ' : '';
        el('div', { class: `change ${r.verdict}`,
          title: `${verdict[r.verdict]}: 95% interval of the ratio ${r.low.toFixed(3)} to ${r.high.toFixed(3)}` }, td,
        `${icon}${formatPercent(r.ratio)} (${formatPercent(r.low)} to ${formatPercent(r.high)}) ${verdict[r.verdict]}`);
      }
    }
  }
}

// Per metric, per thread count, each compared session's samples as a box (quartiles, median, whiskers to the
// extremes) with every sample drawn over it. Each thread count has its own axis, as they differ many times over, but
// relative to the baseline they share one.
function renderDistributions(view) {
  const compared = comparedSessions();
  const baseline = compared[0];
  const threads = shownThreads();
  sessionLegend(view);
  const grid = el('div', { class: 'panels' }, view);
  const unit = (m) => (state.relative ? '%' : m.unit);
  for (const key of shownMetrics()) {
    const m = METRIC[key];
    const p = panel(grid, m.label, state.relative ? `change from the baseline's ${state.statistic}` : m.about, true);
    const plots = threads.map((t) => {
      const ref = statistic(valuesOf(baseline, state.input, t, key));
      const series = compared.map((s, si) => {
        const samples = samplesOf(s, state.input, t);
        const raw = samples.map(m.get);
        const values = state.relative ? raw.map((v) => (v / ref - 1) * 100) : raw;
        return { si, s, samples, raw, values, summary: Stats.summary(values) };
      }).filter((x) => x.values.length);
      return { t, series };
    });
    const range = (series) => {
      const all = series.flatMap((x) => x.values);
      let lo = Math.min(...all);
      let hi = Math.max(...all);
      if (state.relative) {
        lo = Math.min(lo, 0);
        hi = Math.max(hi, 0);
      }
      if (state.fromZero && !state.relative) lo = 0;
      const pad = (hi - lo) * 0.06 || Math.abs(hi) * 0.05 || 1;
      return [state.fromZero && !state.relative ? lo : lo - pad, hi + pad];
    };
    // Every thread count is drawn at the same scale, so a spread looks the same size in each. Relative values share
    // one range; absolute ones differ too much between thread counts for that, so each keeps its own centre but takes
    // the widest span (from zero, the highest top).
    const ranges = plots.map((q) => (q.series.length ? range(q.series) : null));
    const span = Math.max(0, ...ranges.filter(Boolean).map(([lo, hi]) => hi - lo));
    const top = Math.max(...ranges.filter(Boolean).map(([, hi]) => hi));
    const shared = state.relative ? range(plots.flatMap((q) => q.series)) : null;
    const rangeOf = (pi) => {
      if (shared) return shared;
      if (state.fromZero) return [0, top];
      const [lo, hi] = ranges[pi];
      // A time or size can't be negative, so the range is moved up rather than showing below zero.
      const from = Math.max((lo + hi) / 2 - span / 2, 0);
      return [from, from + span];
    };
    // Every cell first, so each is measured at its share of the row.
    const row = el('div', { class: 'subplots' }, p);
    const cells = plots.map(({ t }) => {
      const cell = el('div', { class: 'subplot' }, row);
      el('div', { class: 'subplot-title' }, cell, `${t} thread${t === 1 ? '' : 's'}`);
      return cell;
    });
    for (const [pi, { t, series }] of plots.entries()) {
      const cell = cells[pi];
      if (!series.length) continue;
      const W = Math.max(200, cell.clientWidth);
      const H = 200;
      const box = { left: 52, top: 8, width: W - 60, height: H - 16 };
      const root = svg('svg', { width: W, height: H }, cell);
      const [lo, hi] = rangeOf(pi);
      const y = yAxis(root, lo, hi, box, unit(m));
      const slotW = Math.min(box.width / compared.length, 72);
      for (const x of series) {
        const cx = box.left + box.width / 2 + (x.si - (compared.length - 1) / 2) * slotW;
        const colour = colourOf(x.s);
        const half = slotW * 0.3;
        const sm = x.summary;
        const u = unit(m);
        const g = svg('g', {}, root);
        svg('line', { x1: cx, x2: cx, y1: y(sm.min), y2: y(sm.max), stroke: colour, 'stroke-width': 1 }, g);
        svg('rect', { x: cx - half, width: half * 2, y: y(sm.q3), height: Math.max(1, y(sm.q1) - y(sm.q3)), rx: 2,
          fill: colour, 'fill-opacity': 0.18 }, g);
        svg('line', { x1: cx - half, x2: cx + half, y1: y(sm.median), y2: y(sm.median), stroke: colour,
          'stroke-width': 2, 'stroke-linecap': 'round' }, g);
        const boxHit = svg('rect', { class: 'hit', x: cx - half - 4, width: half * 2 + 8, y: y(sm.max) - 4,
          height: y(sm.min) - y(sm.max) + 8 }, g);
        hover(boxHit, () => [`${sessionName(x.s)}, ${t} thread${t === 1 ? '' : 's'}, ${sm.n} samples`, [
          [formatValue(sm.median, u), 'median'],
          [`${formatValue(sm.q1, u)} to ${formatValue(sm.q3, u)}`, 'quartiles'],
          [`${formatValue(sm.min, u)} to ${formatValue(sm.max, u)}`, 'range'],
          [formatValue(sm.mean, u), 'mean'],
        ]]);
        // Spread across the slot by repetition, so the same draw doesn't move between renders.
        for (const [i, v] of x.values.entries()) {
          const sample = x.samples[i];
          const jitter = ((sample.rep * 0.618034) % 1 - 0.5) * slotW * 0.55;
          const dot = svg('circle', { class: 'mark', cx: cx + jitter, cy: y(v), r: 3.5, fill: colour,
            stroke: 'var(--surface)', 'stroke-width': 1.5 }, g);
          hover(dot, () => [`${sessionName(x.s)}, run ${sample.rep + 1}, ${t} thread${t === 1 ? '' : 's'}`, [
            [formatValue(v, u), state.relative ? `(${formatValue(x.raw[i], m.unit)})` : ''],
            [`#${sample.order + 1}`, 'in the order run'],
          ]]);
        }
      }
    }
  }
}

// Per thread count, each compared session's statistic of each part, stacked to the process wall time.
function renderBreakdown(view) {
  const compared = comparedSessions();
  const threads = shownThreads();
  legend(view, PARTS.map((p, i) => ({ label: p.label, colour: `var(--stage-${i + 1})` })), 'rect');
  const p = panel(view, state.share ? 'Share of process wall time' : 'Where the time goes',
    `${state.statistic} of each part; other is ${PARTS[7].about}`, true);
  const rows = [];
  for (const t of threads)
    for (const s of compared) {
      const samples = samplesOf(s, state.input, t);
      if (!samples.length) continue;
      const parts = PARTS.map((part) => statistic(samples.map(part.get)));
      rows.push({ t, s, parts, sum: parts.reduce((a, b) => a + b, 0) });
    }
  if (!rows.length) return;
  const W = chartWidth(p);
  const rowH = 22;
  const gap = 6;
  const groupGap = 14;
  const labelW = Math.min(240, W * 0.3);
  const H = rows.length * (rowH + gap) + (threads.length - 1) * groupGap + 28;
  const root = svg('svg', { width: W, height: H }, p);
  const box = { left: labelW, width: W - labelW - 70 };
  const max = state.share ? 1 : Math.max(...rows.map((r) => r.sum));
  const x = (v) => box.left + (v / max) * box.width;
  let y = 4;
  let lastT = rows[0].t;
  for (const r of rows) {
    if (r.t !== lastT) {
      y += groupGap;
      lastT = r.t;
    }
    svg('text', { x: labelW - 10, y: y + rowH / 2 + 4, 'text-anchor': 'end' }, root,
      `${r.t} thr · ${sessionName(r.s).slice(0, 28)}`);
    svg('circle', { cx: 6, cy: y + rowH / 2, r: 4, fill: colourOf(r.s) }, root);
    let at = 0;
    for (const [i, v] of r.parts.entries()) {
      const width = state.share ? v / r.sum : v;
      const x0 = x(at);
      const x1 = x(at + width);
      at += width;
      if (x1 - x0 < 0.5) continue;
      // A 2px gap of surface between segments.
      const seg = svg('rect', { class: 'mark', x: x0 + (i ? 1 : 0), y, width: Math.max(0.5, x1 - x0 - (i ? 2 : 1)),
        height: rowH, rx: 2, fill: `var(--stage-${i + 1})` }, root);
      hover(seg, () => [`${PARTS[i].label}, ${sessionName(r.s)}, ${r.t} thread${r.t === 1 ? '' : 's'}`, [
        [formatValue(v, 's'), state.statistic],
        [`${(v / r.sum * 100).toFixed(1)}%`, 'of the wall time'],
      ]]);
    }
    svg('text', { x: x(state.share ? 1 : r.sum) + 6, y: y + rowH / 2 + 4 }, root,
      state.share ? '' : formatValue(r.sum, 's'));
    y += rowH + gap;
  }
}

// Per metric, how each compared session's statistic changes with the thread count, as a speedup over the fewest
// threads or as time.
function renderScaling(view) {
  const compared = comparedSessions();
  const threads = shownThreads();
  if (threads.length < 2) return empty(view, 'Choose at least two thread counts.');
  sessionLegend(view);
  const grid = el('div', { class: 'panels' }, view);
  for (const key of shownMetrics()) {
    const m = METRIC[key];
    const p = panel(grid, m.label, state.speedup ? `speedup over ${threads[0]} thread${threads[0] === 1 ? '' : 's'}`
      : state.statistic);
    const W = chartWidth(p);
    const H = 220;
    const box = { left: 56, top: 8, width: W - 76, height: H - 36 };
    const root = svg('svg', { width: W, height: H }, p);
    const lines = compared.map((s) => {
      const points = threads.map((t) => statistic(valuesOf(s, state.input, t, key)));
      return { s, points: state.speedup ? points.map((v) => points[0] / v) : points };
    });
    const finite = lines.flatMap((l) => l.points).filter(Number.isFinite);
    if (!finite.length) continue;
    const ideal = threads.map((t) => t / threads[0]);
    let lo = state.speedup ? 0 : 0;
    let hi = Math.max(...finite) * 1.08;
    if (state.speedup) hi = Math.max(hi, Math.min(Math.max(...ideal), Math.max(...finite) * 1.6));
    const y = yAxis(root, lo, hi, box, state.speedup ? 'x' : m.unit);
    const x = (i) => box.left + (box.width * i) / (threads.length - 1);
    for (const [i, t] of threads.entries())
      svg('text', { x: x(i), y: H - 8, 'text-anchor': 'middle' }, root, `${t} thr`);
    if (state.speedup) {
      // Stops where it leaves the top, rather than running flat along it.
      const pts = [];
      for (const [i, v] of ideal.entries()) {
        if (v <= hi) {
          pts.push([x(i), y(v)]);
          continue;
        }
        const prev = ideal[i - 1];
        pts.push([x(i - 1) + (x(i) - x(i - 1)) * (hi - prev) / (v - prev), y(hi)]);
        break;
      }
      svg('polyline', { points: pts.map((q) => q.join(',')).join(' '), fill: 'none', stroke: 'var(--axis)',
        'stroke-width': 1 }, root);
      const last = pts[pts.length - 1];
      // Below the line where it leaves the top, so it stays clear of the subtitle.
      const top = last[1] <= box.top + 1;
      svg('text', { x: last[0] + (top ? 6 : -4), y: last[1] + (top ? 12 : -6), 'text-anchor': top ? 'start' : 'end' },
        root, 'ideal');
    }
    for (const { s, points } of lines) {
      const colour = colourOf(s);
      const pts = points.map((v, i) => [x(i), y(v)]).filter((q) => Number.isFinite(q[1]));
      svg('polyline', { points: pts.map((q) => q.join(',')).join(' '), fill: 'none', stroke: colour,
        'stroke-width': 2, 'stroke-linejoin': 'round', 'stroke-linecap': 'round' }, root);
      for (const [i, v] of points.entries()) {
        if (!Number.isFinite(v)) continue;
        const g = svg('g', {}, root);
        const hit = svg('circle', { class: 'hit', cx: x(i), cy: y(v), r: 12 }, g);
        svg('circle', { cx: x(i), cy: y(v), r: 4, fill: colour, stroke: 'var(--surface)', 'stroke-width': 2 }, g);
        hover(hit, () => [`${sessionName(s)}, ${threads[i]} thread${threads[i] === 1 ? '' : 's'}`, [
          [state.speedup ? formatValue(v, 'x') : formatValue(v, m.unit), state.speedup ? 'speedup' : state.statistic],
          ...(state.speedup ? [[`${(v / ideal[i] * 100).toFixed(0)}%`, 'of ideal']] : []),
        ]]);
      }
    }
  }
}

// Per metric, every session in the order made, a line per thread count, each point the statistic with its
// quartiles, so a trend over many changes shows.
function renderHistory(view) {
  const ordered = [...sessions].filter((s) => s.config.inputs[state.input])
    .sort((a, b) => a.created.localeCompare(b.created));
  const threads = shownThreads();
  if (!ordered.length) return empty(view, 'No sessions have this input.');
  const ordinal = (i) => `var(--ordinal-${Math.min(4, 1 + Math.round(i * 3 / Math.max(1, threads.length - 1)))})`;
  legend(view, threads.map((t, i) => ({ label: `${t} thread${t === 1 ? '' : 's'}`, colour: ordinal(i) })), 'line');
  const grid = el('div', { class: 'panels' }, view);
  for (const key of shownMetrics()) {
    const m = METRIC[key];
    const baseline = comparedSessions()[0];
    const relative = state.relative && baseline;
    const p = panel(grid, m.label, relative
      ? `change of each session's ${state.statistic} and quartiles from the baseline's, oldest first`
      : `${state.statistic} and quartiles of each session, oldest first`);
    const W = chartWidth(p);
    const H = 240;
    const box = { left: 56, top: 8, width: W - 80, height: H - 52 };
    const root = svg('svg', { width: W, height: H }, p);
    const u = relative ? '%' : m.unit;
    const lines = threads.map((t) => {
      const ref = relative ? statistic(valuesOf(baseline, state.input, t, key)) : 1;
      return ordered.map((s) => {
        const raw = valuesOf(s, state.input, t, key);
        const values = relative ? raw.map((v) => (v / ref - 1) * 100) : raw;
        return values.length ? { s, v: statistic(values), sm: Stats.summary(values) } : null;
      });
    });
    const finite = lines.flat().filter(Boolean);
    if (!finite.length) continue;
    let lo = Math.min(...finite.map((q) => q.sm.q1));
    let hi = Math.max(...finite.map((q) => q.sm.q3));
    const pad = (hi - lo) * 0.08 || Math.abs(hi) * 0.05 || 1;
    lo = relative ? Math.min(0, lo - pad) : state.fromZero ? 0 : lo - pad;
    hi = relative ? Math.max(0, hi + pad) : hi + pad;
    const y = yAxis(root, lo, hi, box, u);
    const x = (i) => box.left + (ordered.length === 1 ? box.width / 2 : (box.width * i) / (ordered.length - 1));
    const every = Math.ceil(ordered.length / Math.max(1, Math.floor(box.width / 90)));
    for (const [i, s] of ordered.entries())
      if (i % every === 0) {
        svg('text', { x: x(i), y: H - 26, 'text-anchor': 'middle' }, root, s.git.commit.slice(0, 7));
        const name = sessionName(s);
        svg('text', { x: x(i), y: H - 12, 'text-anchor': 'middle' }, root,
          name.length > 14 ? name.slice(0, 13) + '…' : name);
      }
    for (const [ti, line] of lines.entries()) {
      const colour = ordinal(ti);
      const pts = line.map((q, i) => (q ? [x(i), y(q.v)] : null)).filter(Boolean);
      svg('polyline', { points: pts.map((q) => q.join(',')).join(' '), fill: 'none', stroke: colour,
        'stroke-width': 2, 'stroke-linejoin': 'round' }, root);
      for (const [i, q] of line.entries()) {
        if (!q) continue;
        const g = svg('g', {}, root);
        svg('line', { x1: x(i), x2: x(i), y1: y(q.sm.q1), y2: y(q.sm.q3), stroke: colour, 'stroke-width': 1 }, g);
        const hit = svg('circle', { class: 'hit', cx: x(i), cy: y(q.v), r: 12 }, g);
        svg('circle', { cx: x(i), cy: y(q.v), r: 4, fill: colour, stroke: 'var(--surface)', 'stroke-width': 2 }, g);
        hover(hit, () => [`${sessionName(q.s)} (${shortCommit(q.s)}), ${threads[ti]} thread${threads[ti] === 1 ? '' : 's'}`, [
          [formatValue(q.v, u), state.statistic],
          [`${formatValue(q.sm.q1, u)} to ${formatValue(q.sm.q3, u)}`, 'quartiles'],
          [q.s.created.replace('T', ' ').slice(0, 16), ''],
        ]]);
      }
    }
  }
}

const RENDERERS = {
  compare: renderCompare,
  distributions: renderDistributions,
  breakdown: renderBreakdown,
  scaling: renderScaling,
  history: renderHistory,
};

// ---- controls

function renderSessions() {
  const list = $('sessions');
  list.replaceChildren();
  if (!sessions.length) return empty(list, 'No sessions yet, run run.py.');
  const newestFirst = [...sessions].sort((a, b) => b.created.localeCompare(a.created));
  for (const s of newestFirst) {
    const compared = state.compared.includes(s.id);
    const row = el('label', { class: 'session' }, list);
    const box = el('input', { type: 'checkbox' }, row);
    box.checked = compared;
    box.disabled = !compared && state.compared.length >= MAX_COMPARED;
    box.addEventListener('change', () => toggleCompared(s.id));
    el('span', { class: 'swatch', style: compared ? `background:${colourOf(s)}` : '' }, row);
    el('span', { class: 'name' }, row, sessionName(s));
    const star = el('button', { class: state.compared[0] === s.id ? 'star on' : 'star',
      title: 'make this the baseline' }, row, state.compared[0] === s.id ? '★' : '☆');
    star.addEventListener('click', (e) => {
      e.preventDefault();
      makeBaseline(s.id);
    });
    const counts = Object.keys(s.config.inputs).join(', ');
    el('span', { class: 'meta' }, row,
      `${shortCommit(s)} · ${s.created.replace('T', ' ').slice(0, 16)} · ${s.config.runs} runs · ${counts}`);
    row.title = `${s.git.branch}: ${s.git.subject}\n${s.host.cpu}, governor ${s.host.governor}, load ${s.host.load_average.map((v) => v.toFixed(2)).join(' ')}`;
  }
}

function toggleCompared(id) {
  if (state.compared.includes(id)) {
    state.compared = state.compared.filter((x) => x !== id);
    delete state.colours[id];
  } else if (state.compared.length < MAX_COMPARED) {
    state.compared.push(id);
    const used = new Set(Object.values(state.colours));
    state.colours[id] = [0, 1, 2].find((i) => !used.has(i));
  }
  update();
}

function makeBaseline(id) {
  if (!state.compared.includes(id)) {
    if (state.compared.length >= MAX_COMPARED) {
      const dropped = state.compared.pop();
      delete state.colours[dropped];
    }
    toggleCompared(id);
  }
  state.compared = [id, ...state.compared.filter((x) => x !== id)];
  update();
}

function renderControls() {
  const tabs = $('views');
  tabs.replaceChildren();
  for (const v of VIEWS) {
    const b = el('button', { role: 'tab', 'aria-selected': String(state.view === v.key) }, tabs, v.label);
    b.addEventListener('click', () => {
      state.view = v.key;
      update();
    });
  }

  const input = $('input');
  input.replaceChildren();
  for (const name of allInputs()) {
    const o = el('option', { value: name }, input, name);
    o.selected = name === state.input;
  }
  $('statistic').value = state.statistic;
  $('relative').checked = state.relative;
  $('from-zero').checked = state.fromZero;
  $('share').checked = state.share;
  $('speedup').checked = state.speedup;
  $('threshold').value = String(state.threshold);

  const threads = $('threads');
  threads.replaceChildren();
  for (const t of allThreads()) {
    const chip = el('label', { class: 'chip' }, threads);
    const box = el('input', { type: 'checkbox' }, chip);
    box.checked = shownThreads().includes(t);
    box.addEventListener('change', () => {
      const shown = new Set(shownThreads());
      if (box.checked) shown.add(t);
      else shown.delete(t);
      state.threads = [...shown];
      update();
    });
    chip.append(`${t} thr`);
  }

  const metrics = $('metrics');
  metrics.replaceChildren();
  for (const group of ['stage', 'process']) {
    el('span', { class: 'label' }, metrics, group === 'stage' ? 'Stages' : 'Process');
    const g = el('span', { class: 'group' }, metrics);
    for (const m of METRICS.filter((x) => x.group === group)) {
      const chip = el('label', { class: 'chip' }, g);
      if (m.about) chip.title = m.about;
      const box = el('input', { type: 'checkbox' }, chip);
      box.checked = state.metrics.includes(m.key);
      box.addEventListener('change', () => {
        // Kept in the metrics' own order, not the order ticked.
        const on = new Set(state.metrics);
        if (box.checked) on.add(m.key);
        else on.delete(m.key);
        state.metrics = METRICS.map((x) => x.key).filter((k) => on.has(k));
        update();
      });
      chip.append(m.label);
    }
  }

  for (const node of document.querySelectorAll('.view-option'))
    node.hidden = !node.dataset.views.split(' ').includes(state.view);
}

// Sessions whose input did different work can't be compared, e.g., a change that made more classes fail.
function renderWarnings() {
  const warnings = $('warnings');
  warnings.replaceChildren();
  const compared = comparedSessions();
  const work = (s) => {
    const samples = s.samples.filter((x) => x.input === state.input);
    return samples.length ? `${samples[0].diagnostics} diagnostics, ${samples[0].classes} classes` : null;
  };
  const kinds = new Set(compared.map(work).filter(Boolean));
  if (kinds.size > 1)
    el('div', { class: 'warning' }, warnings, `The compared sessions did different work on ${state.input}: ${
      compared.map((s) => `${sessionName(s)} has ${work(s)}`).join('; ')}.`);
  const governors = new Set(compared.map((s) => s.host.governor));
  const hosts = new Set(compared.map((s) => s.host.hostname));
  if (hosts.size > 1) el('div', { class: 'warning' }, warnings, 'The compared sessions ran on different machines.');
  else if (governors.size > 1)
    el('div', { class: 'warning' }, warnings, `The compared sessions ran with different cpu governors: ${
      [...governors].join(', ')}.`);
}

function update() {
  // Sessions that went away keep no slot.
  const ids = byId();
  state.compared = state.compared.filter((id) => ids[id]);
  if (!state.compared.length && sessions.length) {
    const newest = [...sessions].sort((a, b) => b.created.localeCompare(a.created))[0];
    state.compared = [newest.id];
    state.colours = { [newest.id]: 0 };
  }
  if (!allInputs().includes(state.input) && allInputs().length) state.input = allInputs()[0];
  store.save(state);

  renderControls();
  renderSessions();
  renderWarnings();
  hideTooltip();
  const view = $('view');
  view.replaceChildren();
  if (state.view !== 'history' && !comparedSessions().length) return empty(view, 'Tick a session to show it.');
  if (state.view !== 'breakdown' && !state.metrics.length) return empty(view, 'Choose a metric.');
  const unrun = state.view === 'breakdown' ? [] : state.metrics.filter((k) => !shownMetrics().includes(k));
  if (unrun.length)
    el('div', { class: 'panel-sub' }, view, `Not run on ${state.input}: ${unrun.map((k) => METRIC[k].label).join(', ')}.`);
  if (!shownThreads().length) return empty(view, 'Choose a thread count.');
  RENDERERS[state.view](view);
}

async function load() {
  $('status').textContent = 'Loading…';
  try {
    const response = await fetch('/api/sessions');
    sessions = await response.json();
    $('status').textContent = `${sessions.length} session${sessions.length === 1 ? '' : 's'}`;
  } catch (e) {
    $('status').textContent = `Couldn't load the sessions: ${e}`;
  }
  update();
}

$('reload').addEventListener('click', load);
$('input').addEventListener('change', (e) => {
  state.input = e.target.value;
  update();
});
$('threshold').addEventListener('change', (e) => {
  state.threshold = +e.target.value;
  update();
});
$('statistic').addEventListener('change', (e) => {
  state.statistic = e.target.value;
  update();
});
for (const [id, key] of [['relative', 'relative'], ['from-zero', 'fromZero'], ['share', 'share'],
  ['speedup', 'speedup']])
  $(id).addEventListener('change', (e) => {
    state[key] = e.target.checked;
    update();
  });
let resizeTimer;
window.addEventListener('resize', () => {
  clearTimeout(resizeTimer);
  resizeTimer = setTimeout(update, 150);
});

load();
