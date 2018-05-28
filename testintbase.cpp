#include <iostream>
#include "intbaseDNA.h"
#include "intbase2.h"
#include "intbaseOPs.h"
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

int main()
{
    log4cxx::BasicConfigurator::configure();

    std::cout << "Testing intbaseDNA." << std::endl;
    intbaseDNA ibdna;
    test_base(ibdna);
    test_intbaseDNA(ibdna);
    std::cout << "All intbaseDNA tests completed successfully." << std::endl;

    std::cout << "Testing intbase2." << std::endl;
    intbase2 ib2;
    test_base(ib2);
    test_intbase2(ib2);
    std::cout << "All intbase2 tests completed successfully." << std::endl;
    
    std::cout << "Testing intbaseOPs." << std::endl;
    intbaseOPs ib3;
    test_base(ib3);
    test_intbaseOPs(ib3);
    std::cout << "All intbaseOPs tests completed successfully." << std::endl;

    // uncomment to test necessity of subclassing
//     intbase ib;
//     test_intbase(ib); // should die since tests must be on a subclass
}
