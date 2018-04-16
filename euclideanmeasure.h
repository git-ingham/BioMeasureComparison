/*!
 * @brief Interface for Euclidean measure between sequences using kmers
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

#ifndef EUCLIDEANMEASURE_H
#define EUCLIDEANMEASURE_H

#include <iostream>
#include "kmermeasure.h"
#include "kmerset.h"
#include "FastaRecord.h"

#include <map>

class euclideanmeasure : public kmermeasure
{
public:
    euclideanmeasure(const unsigned int k_p) : kmermeasure(k_p) {};
    ~euclideanmeasure() {};

    long double compare(const FastaRecord& a, const FastaRecord& b);
    void printdetails() {
        kmermeasure::printdetails();
        std::cout << "  Euclidean measure." << std::endl;
    };
    void test() {}; //!< @todo implement this
    void init(const fastavec_t& seqs);
};

#endif // EUCLIDEANMEASURE_H
