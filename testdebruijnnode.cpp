#include "deBruijnNode.h"
#include <iostream>

int main()
{
    bool verbose = true;
    
    for (unsigned int k=min_k; k<max_k; ++k) {
        std::string kmer("");
        for (unsigned int i=0; i<k; ++i)
            kmer += kmerint::int_to_base(i % alphabet_size);
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;

        std::cout << "Making a node with k = " << k << std::endl;
        
        deBruijnNode* db = new deBruijnNode(k, kmer);
        db->test(verbose);
        delete db;
    }
}
