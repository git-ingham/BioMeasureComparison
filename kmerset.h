/*!
 * @brief a set of kmers from a given sequence, with frequency information
 *
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

#ifndef KMERSET_H
#define KMERSET_H

#include "FastaRecord.h"
#include "kmer.h"
#include "kmerstring.h"
#include "kmerint.h"

// Underlying structure is chosen at compile time.  Pick exactly one.
typedef kmerint kmer_t;
//typedef kmerstring kmer_t;

class kmerset
{
    typedef double freq_t;
    typedef std::map <kmer_t, freq_t> kmerfreqs_t;
    kmerfreqs_t kmers;
    unsigned int k;

public:
    typedef kmerfreqs_t::iterator iterator;
    typedef kmerfreqs_t::const_iterator const_iterator;

    kmerset(const unsigned int k_p) {
        k = k_p;
    };
    //kmerset(const kmerset& other);
    ~kmerset() {};
    //kmerset& operator=(const kmerset& other);
    //bool operator==(const kmerset& other) const;
    const freq_t& operator[](const kmer_t idx) {
        return kmers[idx];
    };
    const freq_t& at(const kmer_t idx) {
        return kmers[idx];
    };

    void calculate(const FastaRecord& seq);
    void calculate(const std::string seq);

    unsigned int count(const kmer_t km) const {
        return kmers.count(km);
    };
    // accepting a string is dangerous, as we might get a non-kmer.  Therefore no count with a string.

    iterator begin() {
        return kmers.begin();
    };
    iterator end() {
        return kmers.end();
    };
//     const_iterator begin() {
//         return kmers.begin();
//     };
//     const_iterator end() {
//         return kmers.end();
//     };

    void test();
};

#endif // KMERSET_H
