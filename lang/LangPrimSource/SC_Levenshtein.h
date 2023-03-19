/*
    SC_Levenshtein.h
    Copyright (c) 2019 James Surgenor

    This is a header-only, templated functor to compute the Levenshtein distance between two arrays.

    It uses the reduced memory footprint version, based on Martin Ettl's implementation, and is written as a functor
    to facilitate even greater memory savings if a single instance is used (although only one comparison can be made
    at a time). It is self-contained, so could easily be multithreaded.

    Thanks to Brian Heim

    ====================================================================

    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include <vector>
#include <functional>
#include <numeric>

template <typename T> struct levenshteinDistance {
    size_t operator()(
        const T* dataA, size_t sizeA, const T* dataB, size_t sizeB,
        std::function<bool(const T&, const T&)> compareFunc = [](const T& a, const T& b) { return a == b; }) {
        // reduce memory by comparing against the smallest array
        if (sizeA < sizeB) {
            std::swap(dataA, dataB);
            std::swap(sizeA, sizeB);
        }

        // empty array optimisation
        if (sizeA == 0 || sizeB == 0) {
            return sizeA;
        }

        matrix.resize(sizeB + 1);

        // initialise matrix
        std::iota(matrix.begin(), matrix.end(), 0);

        // calculate the distances
        for (size_t indX = 0; indX < sizeA; ++indX) {
            matrix[0] = indX + 1;
            size_t corner = indX;

            for (size_t indY = 0; indY < sizeB; ++indY) {
                size_t upper = matrix[indY + 1];

                if (compareFunc(dataA[indX], dataB[indY]))
                    matrix[indY + 1] = corner;
                else
                    matrix[indY + 1] = std::min({ upper, corner, matrix[indY] }) + 1;

                corner = upper;
            }
        }

        return matrix[sizeB];
    }

private:
    std::vector<size_t> matrix;
};
