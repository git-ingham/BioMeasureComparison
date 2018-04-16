/*!
 * @brief cosine dissimilarity (distance) between sequences based on kmer frequency counts
 *
 * Copyright (C) 2018 Kenneth Ingham
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

#include "cosinemeasure.h"

//! @brief precalculate the sums needed by the consine measure
void
cosinemeasure::calculate_sum(const FastaRecord& fr)
{
    long double count;
    long double sum = 0.0;
    kmerset* ks;

    if (kmers.count(fr) == 0) {
        ks = new kmerset(k);
        ks->calculate(fr);
        kmers.emplace(fr, ks);
    } else
        ks = kmers.at(fr);

    for (auto kmi : *ks) {
        kmer_t km = kmi.first;
        count = ks->at(km);
        sum += count*count;
    }
    sums.emplace(fr, sum);
};

/*!
 * @brief cosine distance based on frequency counts
 * https://en.wikipedia.org/wiki/Cosine_similarity
 */
long double
cosinemeasure::compare(const FastaRecord& a, const FastaRecord& b)
{
// long double
// cosinecompare(const std::string& aseq, const std::string& bseq) const
// {
    long double dotproduct = 0.0;
    kmerset* ksa = get_counts(a);
    kmerset* ksb = get_counts(b);

    if (sums.count(a) == 0)
        calculate_sum(a);
    if (sums.count(b) == 0)
        calculate_sum(b);

    // for the dot product, all we care about are those kmers in common;
    // others have a 0 product and contribute nothing to the final result.
    for (auto kmi : *ksa) {
        kmer_t km = kmi.first;
        if (ksb->count(km) > 0)
            dotproduct += ksa->at(km) * ksb->at(km);
    }

    long double cosine  = dotproduct / (sums[a] * sums[b]);
    if (cosine < -1.0) {
        //std::cerr << "Warning: cosine " << cosine << " is < -1.0." << std::endl;
        cosine = -1.0;
    }
    if (cosine > 1.0) {
        //std::cerr << "Warning: cosine " << cosine << " is > 1.0." << std::endl;
        cosine = 1.0;
    }

    long double result = acosl(cosine)/halfpi;
    if (result <= 2.09629e-10)
        return 0.0;
    else
        return result;
};


// //------- old code start
// enum variants {euclidean, cosine};
// //     variants algorithm = euclidean;
// 
// variants algorithm = cosine;
// 
// long double
// kmerstring::compare(const FastaRecord& a, const FastaRecord& b) const
// {
//     const std::string aseq = a.get_seq();
//     const std::string bseq = b.get_seq();
//     if (aseq.length() == 0)
//         errx(1, "kmerstring::compare: aseq is 0-length");
//     if (bseq.length() == 0)
//         errx(1, "kmerstring::compare: bseq is 0-length");
// 
//     // sanity check
//     if (kmers.count(aseq) == 0) {
//         std::cerr << "kmerstring::compare aseq not pre-calculated!" << std::endl;
//         exit(1);
//     }
//     if (kmers.count(bseq) == 0) {
//         std::cerr << "kmerstring::compare bseq not pre-calculated!" << std::endl;
//         exit(1);
//     }
// 
//     if (algorithm == cosine)
//         return cosinecompare(aseq, bseq);
//     if (algorithm == euclidean)
//         return euclideancompare(aseq, bseq);
// 
//     // If we are here, it is a bug
//     std::cerr << "No matching algorithm for kmerstring::compare." << std::endl;
//     exit(1);
// };
// 
// variants
// kmerstring::stringtoa(const std::string& v)
// {
//     if (v.compare("euclidean") == 0) return euclidean;
//     if (v.compare("cosine") == 0) return cosine;
//     errx(1, "Unknown variant in kmerstring::atostring '%s'", v.c_str());
// };
// 
// std::string
// kmerstring::atostring(variants v)
// {
//     if (v == euclidean) return "Euclidean";
//     if (v == cosine) return "cosine";
//     errx(1, "Unknown variant in kmerstring::atostring");
// };
// 
// void
// kmer(const unsigned int k, const fastavec_t& seqs)
// {
//     n = in;
//     fastavec_t::const_iterator s;
//     for (s=seqs.begin(); s != seqs.end(); ++s) {
//         calculate_set(s->get_seq());
//     }
// };
// 
// variants get_algorithm(void) {
//     return algorithm;
// }
// variants set_algorithm(variants a) {
//     variants old = algorithm;
//     algorithm = a;
//     return old;
// }
// variants set_algorithm(const std::string& a) {
//     variants old = algorithm;
//     algorithm = stringtoa(a);
//     return old;
// }
// 
// void calculate_set(const std::string& seq);
// long double calculate_sum(const std::string& seq);
// 
// long double euclideancompare(const std::string& aseq, const std::string& bseq) const;
// long double cosinecompare   (const std::string& aseq, const std::string& bseq) const;
// kmersum_t sums; // for caching cosine calculated data
// 
// kmerstrings_t kmers;
// 
// 
// void printdetails() {
//     std::cerr << "kmer distance:" << std::endl;
//     std::cerr << "    k = " << n << std::endl;
//     std::cerr << "    variant: " << atostring(algorithm) << std::endl;
// };
// 
// long double compare(const FastaRecord& a, const FastaRecord& b) const;
// 
// 
// std::string atostring(variants v);
// variants stringtoa(const std::string& v);
// 
// //------- old code end

