/*!
 * @brief A set of kmers, normally calculated from a sequence
 * Copyright (C) 2018  Kenneth Ingham <email>
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

#include "kmerset.h"

void
kmerset::calculate(const FastaRecord& seq)
{
    calculate(seq.get_seq());
}

void
kmerset::calculate(const std::string seq)
{
    for (unsigned int i=0; i<seq.length()-k; ++i) {
        std::string kmer_s = seq.substr(i, k);
        kmer_t km(k, kmer_s);
        if (kmers.count(km) > 0)
            kmers[km]++;
        else
            kmers.emplace(km, 1);
    }
}

void kmerset::test()
{
}
