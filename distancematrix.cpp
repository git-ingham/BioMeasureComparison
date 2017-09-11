#include "distancematrix.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <sys/mman.h>
#include <iostream>
#include <thread>
#include <iomanip>
#include <unistd.h>
#include <string.h>
#include <math.h>

// triangular matrix with diagonal
// a0  a1  a2  a3  a4  a5
// 0   a6  a7  a8  a9  a10
// 0   0   a11 a12 a13 a14
// 0   0   0   a15 a16 a17
// 0   0   0   0   a18 a19
// 0   0   0   0   0   a20

distancematrix::distancematrix(const unsigned int sizep, const std::string filename)
{
    init(sizep, filename);
}

distancematrix::distancematrix(const std::string filename)
{
    init(filename);
}

distancematrix::distancematrix()
{
}

// When a size is provided, we start with an empty matrix.
void
distancematrix::init(const unsigned int sizep, const std::string filename)
{
    size = sizep;

    std::cerr << "Create empty matrix of size " << size << std::endl;

    fd = open(filename.c_str(), O_RDWR|O_CREAT|O_TRUNC, filemode);
    if (fd < 0) err(1, "Cannot open %s", filename.c_str());

    vecsize = size*size/2 + size;
    allocsize = vecsize * sizeof(long double);
    std::cerr << "vecsize: " << vecsize << "; allocsize: " << allocsize << std::endl;
    if (ftruncate(fd, allocsize) < 0)
        err(1, "ftruncate fd for %s size %lu failed", filename.c_str(), allocsize);

    vec = (long double *)mmap((void *)0, allocsize, PROT_READ|PROT_WRITE, 
                              MAP_SHARED, fd, 0);
    if (vec == MAP_FAILED) err(1, "Cannot map %s to size %lu", filename.c_str(),
                               allocsize);

    if (close(fd) < 0) err(1, "close fd for %s failed", filename.c_str());

    // Paranoia; verify completely empty
    memset(vec, 0, vecsize);

    valid = true;
}

// When a size is not provided, we open an existing matrix
void
distancematrix::init(const std::string filename)
{
    std::cerr << "Create existing matrix from " << filename << std::endl;

    fd = open(filename.c_str(), O_RDWR);
    if (fd < 0) err(1, "Cannot open %s", filename.c_str());

    struct stat sb;
    if (stat(filename.c_str(), &sb) < 0) err(1, "Cannot stat %s", filename.c_str());

    allocsize = sb.st_size;
    std::cerr << "allocsize (file size) " << allocsize << std::endl;

    vecsize = allocsize / sizeof(long double);
    std::cerr << "vecsize " << vecsize << std::endl;

    size = sqrt(2*vecsize + 1);
    std::cerr << "size (matrix n of nxn) " << size << std::endl;

    // Sanity check
    if ((size*size/2 + size) != vecsize)
        errx(1, "Size does not lead to proper vecsize");
   if ((size*size/2 + size)*sizeof(long double) != allocsize)
        errx(1, "Size does not lead to proper allocsize");

    vec = (long double *)mmap((caddr_t)0, allocsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (vec == MAP_FAILED) err(1, "Cannot map %s to size %lu", filename.c_str(), allocsize);

    if (close(fd) < 0) err(1, "close fd for %s failed", filename.c_str());

    valid = true;
}

distancematrix::~distancematrix() 
{
    if (munmap(vec, allocsize) < 0) err(1, "munmap failed");
}

unsigned int 
distancematrix::sub(const unsigned int i, const unsigned int j) const
{
    // i * size + j if we were a full matrix.
    // Each row has i unused elements; the sum of these is i*(i-1)/2
    // j >= i
    //k = i*(size - 1) + j - i*(i-1)/2;

    unsigned int k;
    //k = i + j*(j-i)/2;
    //k = i*(i+1)/2 + j;
    //k = i + j*(j+i)/2;
    k = j + size*i - i*(i+1)/2;
    if (k > vecsize) errx(1, "i %u j %u k %u out of range (vecsize: %u); this should not happen", i, j, k, vecsize);
    return k;
}

void
distancematrix::checkij(const unsigned int i, const unsigned int j) const
{
    bool error = false;
    std::string errmsg;
    if (i >= size) {
        errmsg = "i too big: " + std::to_string(i) + ".  ";
	std::cerr << errmsg << std::endl;
	error = true;
    }
    if (j >= size) {
        errmsg = "j too big: " + std::to_string(j) + ".  ";
	std::cerr << errmsg << std::endl;
	error = true;
    }
    if (j < i) {
        errmsg = "j (" + std::to_string(j) + ") < i (" + std::to_string(i) + ").  ";
	errmsg += "j must be >= i";
	std::cerr << errmsg << std::endl;
	error = true;
    }

    if (error) exit(1);
}

long double
distancematrix::get(const unsigned int i, const unsigned int j) const
{
    if (valid) {
	checkij(i, j);
	unsigned int k = sub(i, j);
	//std::cerr << "get: i " << i << "; j " << j << "; k " << k << std::endl;
	return vec[k];
    } else {
        warnx("Distance matrix is invalid.");
	abort();
    }
}

void 
distancematrix::set(const unsigned int i, const unsigned int j, const long double d)
{
    if (valid) {
	checkij(i, j);
	unsigned int k = sub(i, j);
	if (vec[k] != 0)
	    errx(1, "matrix[%u][%u] (k: %u) not zero!", i, j, k);

    //    char *msg;
    //    asprintf(&msg, "set: i %u; j %u; k %u; thread %u\n", i, j, k,
    //             std::this_thread::get_id());
    //    std::cerr << msg;
    //    free(msg);
	vec[k] = d;
    } else {
        warnx("Distance matrix is invalid.");
	abort();
    }
}

void
distancematrix::print(void) const
{
    long double max = 0;
    for (unsigned int i=0; i<vecsize; ++i) 
        if (max < vec[i]) max = vec[i];
    unsigned int w = (int)log10(max)+4;
        
    std::cout << size << std::endl;
    std::cout << std::fixed;
    for (unsigned int i=0; i<size; ++i) {
        for (unsigned int j=0; j<i; ++j)
	    std::cout << std::setprecision(2) << std::setw(w) << -1.0 << ", ";
        for (unsigned int j=i; j<size; ++j) {
	    std::cout << std::setprecision(2) << std::setw(w) << vec[sub(i, j)];
	    if (j < size-1) std::cout << ", ";
	}
	std::cout << std::endl;
    }
}
