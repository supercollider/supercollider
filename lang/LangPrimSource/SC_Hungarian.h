#pragma once
#include <vector>
#include <limits>
#include <utility>
#include <algorithm>

namespace schungarian { // keep it distinct from other headers

// Hungarian (Munkres) — rows = tasks, cols = workers (rectangular OK).
// Returns {min_cost, assignment}, assignment[i] = chosen column for row i (or -1).
// Works with negative costs. Header-only, no deps beyond STL.
template <class T> inline std::pair<T, std::vector<int>> solve(const std::vector<std::vector<T>>& A) {
    const T INF = std::numeric_limits<T>::max() / 4;
    int n = (int)A.size();
    int m = n ? (int)A[0].size() : 0;
    if (n == 0 || m == 0)
        return { T(0), {} };

    bool transposed = false;
    std::vector<std::vector<T>> a;
    if (n <= m)
        a = A;
    else {
        transposed = true;
        a.assign(m, std::vector<T>(n));
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                a[j][i] = A[i][j];
        std::swap(n, m); // rows <= cols
    }

    std::vector<T> u(n + 1), v(m + 1);
    std::vector<int> p(m + 1), way(m + 1);

    for (int i = 1; i <= n; ++i) {
        p[0] = i;
        int j0 = 0;
        std::vector<T> minv(m + 1, INF);
        std::vector<char> used(m + 1, false);
        do {
            used[j0] = true;
            int i0 = p[j0], j1 = 0;
            T delta = INF;
            for (int j = 1; j <= m; ++j)
                if (!used[j]) {
                    T cur = a[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < minv[j]) {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta) {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            for (int j = 0; j <= m; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);

        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    std::vector<int> assign_rows(n, -1);
    for (int j = 1; j <= m; ++j)
        if (p[j])
            assign_rows[p[j] - 1] = j - 1;
    T min_cost = -v[0];

    if (!transposed)
        return { min_cost, assign_rows };
    // map back if original had rows>cols
    std::vector<int> assign_orig(m, -1);
    for (int i = 0; i < n; ++i)
        if (assign_rows[i] != -1) {
            int r = assign_rows[i];
            assign_orig[r] = i;
        }
    return { min_cost, assign_orig };
}

} // namespace schungarian
