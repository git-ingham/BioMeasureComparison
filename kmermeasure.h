/*!
 * @brief Interface for dissimilarity (distance) between sequences using kmers as a fundamental part of the measure
 * This class provides kmer set storage for sequences.
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

#ifndef KMERMEASURE_H
#define KMERMEASURE_H

#include <string>
#include <map>

#include "measure.h"
#include "kmerset.h"
#include "FastaRecord.h"

class kmermeasure : public measure
{
protected:
    static std::map<FastaRecord, kmerset*> kmers;
    unsigned int k;

    kmerset* get_counts(const FastaRecord& fr) {
        kmerset* ks;
        // if fr is new, calculate the kmerset and cache it.
        if (kmers.count(fr) == 0) {
            ks = new kmerset(k);
            ks->calculate(fr);
            kmers.emplace(fr, ks);
        } else
            ks = kmers.at(fr);

        return ks;
    };

public:
    kmermeasure(const unsigned int k_p) {
        /*! @todo need validation of k? */ k = k_p;
    };
    kmermeasure(const std::string kstr) {
        k = std::stoi(kstr);
    }
    ~kmermeasure() {};
//     virtual void init(const fastavec_t& seqs) {};
    void printdetails() {
        std::cout << "kmer measure, k = " << k << std::endl;
    };
    void test() {}; //!< @todo implement this
};

#endif // KMERMEASURE_H
