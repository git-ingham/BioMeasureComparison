#ifndef EDITCOST_H
#define EDITCOST_H

#include <string>
#include <err.h>

/*
    Costs based on PAM250:
    http://prowl.rockefeller.edu/aainfo/pam250.htm
    because negative values are low probability (i.e., have high edit
    cost), I am using:
                 { 0 if pam(a, b) > 0
    cost(a, b) = { 1 if pam(a,b) == 0
                 { 10^(-pam(a,b)) if pam(a,b) < 0
    unsigned int pam250mutationcost[nbases][nbases] = {
	 //   A     C     G   T
    A {   0,  100,   10, 10 },
    C { 100,    0, 1000, 10 },
    G {  10, 1000,    0,  1},
    T {  10,   10,    1,  0},
    };
*/

typedef struct {
    unsigned int (*insertion)(const char);
    unsigned int (*deletion)(const char);
    unsigned int (*substitution)(const char, const char);
} custom_cost_s;

class editcost {
    const static unsigned int nbases = 4; 
    const std::string bases = "acgt";
    int mutationcost[nbases][nbases];
    unsigned int costwidth = 0; /* width of widest value, for printing */

    /* The cost of inertion and deletion is max(mutationcost); this
    means that a point mutation always has a lower cost than the pair of
    operations deletion + insertion. */
    unsigned int indelcost = 0;

    unsigned int base_index(const char base) const;

    public:
	editcost(std::string matrixfname);
	editcost() {};
	~editcost() {};
	void init(std::string matrixfname);
        void print();
	unsigned int insertion(const char a) { return indelcost; }
	unsigned int deletion(const char a) { return indelcost; }
	unsigned int substitution(const char a, const char b) { 
	    return mutationcost[base_index(a)][base_index(b)];
	};
	custom_cost_s custom_cost();
};

/* for calling the edit distance function */
#endif // EDITCOST_H
