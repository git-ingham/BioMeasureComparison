#include "distancematrix.h"

int
main(int argc, char *argv[])
{
    const unsigned int size=10;
    unsigned int i, j, k;
    distancematrix d(size, "DMtest");
    //distancematrix d(0, "DMtest"); // should die with an error

    k = 0;
    for (i=0; i<size; ++i)
        for (j=i; j<size; ++j) {
	    d.set(i, j, (long double) k++);
	}
    
    d.print();

    // Other errors to test
    //d.set(size, 0, 0.0);
    //d.set(0, size, 0.0);
    //d.set(size, size, 0.0);
    //d.set(2, 1, 0.0);
    //d.set(2, 0-1, 0.0);
}
