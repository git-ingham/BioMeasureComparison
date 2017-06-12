#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// triangular matrix, with identity data

class distancematrix {
    private: 
	unsigned int size;
	int fd;
	long double *vec;
	size_t allocsize;
	unsigned int vecsize;
	const mode_t filemode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;

	unsigned int sub(const unsigned int i, const unsigned int j);
	void checkij(const unsigned int i, const unsigned int j);

    public:
	distancematrix(const unsigned int sizep, const std::string filenamep);
	distancematrix(const std::string filenamep);
	~distancematrix();
	long double get(const unsigned int, const unsigned int);
	void set(const unsigned int, const unsigned int, const long double);
	void print(void);
};

#endif // DISTANCEMATRIX_H
