#ifndef DISTANCEMATRIX_H
#define DISTANCEMATRIX_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// triangular matrix, with identity data

class distancematrix {
private:
    bool valid = false;
    unsigned int size;
    int fd;
    long double *vec;
    size_t allocsize;
    unsigned int vecsize;
    const mode_t filemode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;

    unsigned int sub(const unsigned int i, const unsigned int j) const;
    void checkij(const unsigned int i, const unsigned int j) const;

public:
    distancematrix(const unsigned int sizep, const std::string filenamep);
    distancematrix(const std::string filenamep);
    distancematrix(void);
    void init(const unsigned int sizep, const std::string filenamep);
    void init(const std::string filenamep);
    ~distancematrix();
    long double get(const unsigned int, const unsigned int) const;
    void set(const unsigned int, const unsigned int, const long double);
    void checksanity();
    void print(void) const;
    unsigned int get_size() const {
        return size;
    };
};

#endif // DISTANCEMATRIX_H
