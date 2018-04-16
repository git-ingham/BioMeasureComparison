#include <iostream>
#include "intbase.h"
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>

int main()
{
    log4cxx::BasicConfigurator::configure();

    intbase ib;
    ib.test();
    std::cout << "All tests completed successfully." << std::endl;
}
