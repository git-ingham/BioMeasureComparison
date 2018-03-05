#include "deBruijnGraph.h"
#include <iostream>

int main()
{
    for (unsigned int k=2; k<14; ++k) {
        std::cout << "Making a graph of size " << k << std::endl;
        deBruijnGraph* db = new deBruijnGraph(k);
        db->make_complete_graph();
        db->consistency_check();
        delete db;
    }
}
