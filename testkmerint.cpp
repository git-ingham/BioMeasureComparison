#include <iostream>
#include "kmerint.h"
#include "intbase.h"

int main()
{
    for (unsigned int k=min_k; k<=max_k; ++k) {
        std::cout << "testing k = " << k << std::endl;
        kmerint* kmer = new kmerint(k);
        kmer->test(false);
        delete kmer;
    }
}
