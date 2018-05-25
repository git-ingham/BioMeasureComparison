#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

#include "kmerint.h"

int main()
{
    log4cxx::BasicConfigurator::configure();

    for (unsigned int k=kmer::min_k; k<=kmer::max_k; ++k) {
        std::cout << "testing k = " << k << std::endl;
        kmerint kmer(k);
        test_kmerint(kmer, true);
    }
}
