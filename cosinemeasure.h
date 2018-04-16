/*
 * <one line to give the program's name and a brief idea of what it does.>
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

#ifndef COSINEMEASURE_H
#define COSINEMEASURE_H

#include "kmermeasure.h"
#include "kmerset.h"
#include "FastaRecord.h"

#include <map>

class cosinemeasure : public kmermeasure
{
    static std::map<FastaRecord, long double> sums;
    const long double halfpi = 2.0 * atanl(1.0);

    void calculate_sum(const FastaRecord& fr);

public:
    cosinemeasure(const unsigned int k_p) : kmermeasure(k_p) {};
    cosinemeasure(const std::string kstr) : kmermeasure(kstr) {};
    ~cosinemeasure() {};

    long double compare(const FastaRecord& a, const FastaRecord& b);
    void init(const fastavec_t& seqs);
    void printdetails() {
        kmermeasure::printdetails();
        std::cout << "  Cosine measure." << std::endl;
    };

    void test() {}; //!< @todo implement this
};

#endif // COSINEMEASURE_H
