//! @file measure.h

#ifndef MEASURE_H
#define MEASURE_H

#include "FastaRecord.h"

/*! @class measure
 * @brief Interface for all measure functions
 *
 * known subclasses: editmeasure, cosinemeasure, euclideanmeasure
 */

class measure {
protected:
    //! print debugging statements
    bool verbose = false;
public:
    //! @brief optional measure initialization
    virtual void init(const fastavec_t& seqs) {};
    //! @brief print the details about the measure function, any parameters, etc
    virtual void printdetails(void) = 0;
    static std::string validatemeasure(std::string name)
    {
        if (name.compare("edit") == 0) return "";
        if (name.compare("kmer") == 0) return "";
        return std::string("Unknown measure '") + name + std::string("'.\n") +
               std::string("Known measures are: edit, kmer.");
    };

    /*
     * @brief compare two sequences with a result in [0,1] with 0 is completely different and 1 is identical
     * @params a,b the two sequences to compare
     *
     * no final "const" because some measurements cache data for a given FastaRecord
     */
    virtual long double compare(const FastaRecord& a, const FastaRecord& b) = 0;

    /*!
     * @brief do a full test of the measure function
     */
    virtual void test(void) = 0;
};

#endif // MEASURE_H
