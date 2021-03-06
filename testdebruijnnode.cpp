#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

#include "deBruijnNode.h"

int main()
{
    bool verbose = true;
    log4cxx::BasicConfigurator::configure();

    for (unsigned int k=kmer::min_k; k<kmer::max_k; ++k) {
        std::string kmer("");
        for (unsigned int i=0; i<k; ++i)
            kmer += intbase::int_to_base(i % alphabet_size);
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;

        std::cout << "Making a node with k = " << k << std::endl;

        deBruijnNode* db = new deBruijnNode(k, kmer);
        db->test(verbose);
        delete db;
    }
}
