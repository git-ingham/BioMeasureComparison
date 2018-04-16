/*!
 * @brief
 * 
 * Copyright (C) 2018  Kenneth Ingham
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "euclideanmeasure.h"


long double
euclideanmeasure::compare(const FastaRecord& a, const FastaRecord& b)
{
    long double dist = 0.0;
    kmerset* ksa = get_counts(a);
    kmerset* ksb = get_counts(b);

    // a -> b
    for (auto kmi : *ksa) {
        int t;
        kmer_t km = kmi.first;
        if (ksb->count(km) > 0)
            t = ksa->at(km) - ksb->at(km);
        else
            t = ksa->at(km);
        dist += t*t;
    }

    // b -> a, but only for the kmers that are in b and not in a
    for (auto kmi : *ksb) {
        int t;
        kmer_t km = kmi.first;
        if (ksa->count(km) == 0) {
            t = ksb->at(km);
            dist += t*t;
        }
    }

    // mapped into [0,1]
    //return dist == 0 ? 0 : 1.0 - 1.0/sqrt(dist);

    // Actual Euclidean distance
    //return sqrt(dist);

    // Save time and do not do the sqrt, since the relative distances
    // remain the same, even if the absolute distances are different.
    return dist;
};
