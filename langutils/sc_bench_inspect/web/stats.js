'use strict';

// Summaries of a set of samples, and how sure a change between two sets is.
//
// Timings are skewed, a run is only ever slowed down (by another process, a cold cache, the cpu changing speed), so
// the median is the default statistic, the minimum is the run least disturbed, and the mean is pulled by outliers.
// The spread is the interquartile range, which ignores the outliers too. A change is the ratio of the candidate's
// statistic to the baseline's, with a 95% bootstrap confidence interval, and is only called faster or slower when
// the interval leaves out 1.

const Stats = (() => {
  // Linear between the closest ranks, as numpy's default and R's type 7.
  function quantile(sorted, p) {
    if (sorted.length === 0) return NaN;
    const h = (sorted.length - 1) * p;
    const lo = Math.floor(h);
    const hi = Math.min(lo + 1, sorted.length - 1);
    return sorted[lo] + (h - lo) * (sorted[hi] - sorted[lo]);
  }

  function mean(values) {
    return values.reduce((a, b) => a + b, 0) / values.length;
  }

  function summary(values) {
    const sorted = [...values].sort((a, b) => a - b);
    const n = sorted.length;
    const m = n ? mean(sorted) : NaN;
    const sd = n > 1 ? Math.sqrt(sorted.reduce((a, v) => a + (v - m) ** 2, 0) / (n - 1)) : 0;
    return {
      n,
      min: sorted[0],
      q1: quantile(sorted, 0.25),
      median: quantile(sorted, 0.5),
      q3: quantile(sorted, 0.75),
      max: sorted[n - 1],
      mean: m,
      sd,
    };
  }

  const STATISTICS = {
    median: (values) => quantile([...values].sort((a, b) => a - b), 0.5),
    min: (values) => Math.min(...values),
    mean,
  };

  // Seeded, so the interval doesn't move each time the page draws.
  function mulberry32(seed) {
    return () => {
      seed |= 0;
      seed = (seed + 0x6d2b79f5) | 0;
      let t = Math.imul(seed ^ (seed >>> 15), 1 | seed);
      t = (t + Math.imul(t ^ (t >>> 7), 61 | t)) ^ t;
      return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
    };
  }

  function resample(values, random) {
    const out = new Array(values.length);
    for (let i = 0; i < values.length; ++i) out[i] = values[Math.floor(random() * values.length)];
    return out;
  }

  // The candidate's statistic over the baseline's, e.g., 0.97 is 3% faster, with its 95% interval, resampling each
  // set on its own.
  function compare(baseline, candidate, statistic, resamples = 2000, seed = 1) {
    const f = STATISTICS[statistic];
    const ratio = f(candidate) / f(baseline);
    if (baseline.length < 2 || candidate.length < 2)
      return { ratio, low: NaN, high: NaN, verdict: 'same' };
    const random = mulberry32(seed);
    const ratios = new Array(resamples);
    for (let i = 0; i < resamples; ++i)
      ratios[i] = f(resample(candidate, random)) / f(resample(baseline, random));
    ratios.sort((a, b) => a - b);
    const low = quantile(ratios, 0.025);
    const high = quantile(ratios, 0.975);
    const verdict = high < 1 ? 'faster' : low > 1 ? 'slower' : 'same';
    return { ratio, low, high, verdict };
  }

  return { quantile, summary, STATISTICS, compare };
})();

if (typeof module !== 'undefined') module.exports = Stats;
