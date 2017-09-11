#include "editcost.h"
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <math.h>

void
editcost::init(std::string matrixfname)
{
    std::ifstream in(matrixfname);
    if (!in)
        err(1, "Opening '%s' failed", matrixfname.c_str());

    for (unsigned int i=0; i<nbases; ++i)
        for (unsigned int j=0; j<nbases; ++j)
	    in >> mutationcost[i][j];

    // verify we read everything from the file.
    if (!in.eof()) {
	std::string line;
	std::getline(in, line);
	
	if (line.length() != 0) 
	    errx(1, "Left-over data in '%s'.  Extra data starts with '%s'",
	         matrixfname.c_str(), line.c_str());
    }

    in.close();

    // Sanity check matrix, find indelcost (the max)
    // The matrix must be symmetric, because we do not know the mutation
    // direction (e.g., a -> t or t -> a)
    // The wasted space is worth removing the requirement to sort indices
    for (unsigned int i=0; i<nbases; ++i)
        for (unsigned int j=i; j<nbases; ++j) {
	    if (mutationcost[i][j] != mutationcost[j][i])
	        errx(1, "Cost matrix from '%s' is asymmetric; i %u j %u",
		     matrixfname.c_str(), i, j);
	    if (mutationcost[i][j] < 0)
		errx(1, "Cost matrix from '%s' value '%Lf' < 0 at i %u j %u",
			 matrixfname.c_str(), mutationcost[i][j], i, j);
	    if (i != j && mutationcost[i][j] == 0)
		warnx("Cost matrix from '%s' value '%Lf' == 0 at i %u j %u",
			 matrixfname.c_str(), mutationcost[i][j], i, j);
	    if (mutationcost[i][j] > indelcost)
	        indelcost = mutationcost[i][j];
	}
    costwidth = (int)log10(indelcost)+4;
}

unsigned int 
editcost::base_index(const char base) const
{
    size_t index = bases.find(base);
    if (index == std::string::npos)
        errx(1, "Error: base '%c' is not one of the known bases '%s'", base, bases.c_str());
    std::cerr << "base: '" << base << "'; index: " << index << std::endl;
    return index;
}

void
editcost::print() {
    std::cerr << "custom cost matrix:" << std::endl;
    std::cout << std::fixed;
    for (unsigned int j=0; j<nbases; ++j) {
	std::cout << std::setw(costwidth) << bases[j];
	if (j < nbases-1) std::cout << "  ";
    }
    std::cout << std::endl;
    for (unsigned int i=0; i<nbases; ++i) {
        std::cout << bases[i];
	for (unsigned int j=0; j<nbases; ++j) {
	    std::cout << std::setw(costwidth) << std::setprecision(costprec) << mutationcost[i][j];
	    if (j < nbases-1) std::cout << ", ";
	}
	std::cout << std::endl;
    }
}

custom_cost_s
editcost::custom_cost()
{
    static custom_cost_s c;

    // ### This is not working
    // c.insertion = &(this->insertion);
    // c.deletion = &editcost::deletion;
    c.insertion = [this] (const char a) -> unsigned int {return this->insertion(a);};
    c.deletion = [this] (const char a) -> unsigned int {return this->deletion(a);};
    c.substitution = [this] (const char a, const char b) -> unsigned int {return this->substitution(a,b);};

    std::cerr << "insertion for 'a': " << c.insertion('a') << std::endl;
    return c;
}
