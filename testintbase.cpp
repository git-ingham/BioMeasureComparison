#include <iostream>
#include "intbaseDNA.h"
#include "intbase2.h"
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

int main()
{
    log4cxx::BasicConfigurator::configure();

    intbaseDNA ibdna;
    test_intbase(ibdna);
    test_intbaseDNA(ibdna);
    std::cout << "All intbaseDNA tests completed successfully." << std::endl;

    intbase2 ib2;
    test_intbase(ib2);
    test_intbase2(ib2);
    std::cout << "All intbase2 tests completed successfully." << std::endl;

    // uncomment to test necessity of subclassing
//     intbase ib;
//     test_intbase(ib); // should die since tests must be on a subclass
}
