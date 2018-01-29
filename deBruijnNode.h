#ifndef DEBRUIJNNODE
#define DEBRUIJNNODE

/*
    Definition and implementation of a node in a DeBruijn graph for investigting the EMD distance as
    described in Mangul & Coskicki
    http://dx.doi.org/10.1145%2F2975167.2975174
*/

#include <string>
#include <iostream>
#include <fstream>
#include "kmerint.h"

class deBruijnNode {
    deBruijnNode* out_edges[alphabet_size];
    deBruijnNode* in_edges[alphabet_size];
    kmerint* nodevalue;
    double kmerfreq;
    void init_edges(void) {
        for (unsigned i=0; i<alphabet_size; ++i) {
            out_edges[i] = nullptr;
            in_edges[i] = nullptr;
        }
    }

public:
    deBruijnNode(const unsigned int k, const std::string value, const double nodefreq_p = 0.0) {
        nodevalue = new kmerint(k);
        init_edges();
        nodevalue->setkmer(value);
        kmerfreq = nodefreq_p;
    };
    ~deBruijnNode() {
        // We do not delete anything because we do not allocate anything.

        // clear all pointers that point to us
        for (unsigned i=0; i<alphabet_size; ++i) {
            if (out_edges[i] != nullptr) {
                out_edges[i]->clearinptr(i);
                out_edges[i] = nullptr;
            }
            if (in_edges[i] != nullptr) {
                in_edges[i]->clearoutptr(i);
                in_edges[i] = nullptr;
            }
        }
    };
    
    void clearinptr(const unsigned int base) {
        in_edges[base] = nullptr;
    };
    void clearinptr(const char base) {
        clearinptr(kmerint::base_to_int(base));
    };
    void clearoutptr(const unsigned int base) {
        out_edges[base] = nullptr;
    };
    void clearoutptr(const char base) {
        clearoutptr(kmerint::base_to_int(base));
    };

    std::string getkmer(void) const {
        return nodevalue->getkmer();
    };
    void setkmer(const std::string value_p) {
        nodevalue->setkmer(value_p);
    };
    double getfreq(void) const {
        return kmerfreq;
    };
    void setfreq(const double freq_p) {
        kmerfreq = freq_p;
    };

    // out edge maintenance
    deBruijnNode* getoutptr(const unsigned int base) {
        if (out_edges[base] != nullptr)
            return out_edges[base];
        else
            return nullptr;
    };
    deBruijnNode* getoutptr(const char base) {
        return getoutptr(kmerint::base_to_int(base));
    };
    void setoutptr(const unsigned int base, deBruijnNode* value) {
        // ### Need to verify that the base matches prefix or suffix of dest
        if (out_edges[base] != value) {
            out_edges[base] = value;
            value->setinptr(base, this); // ensure a consistent pair of pointers
        }
        // assume that if the edge is already set, so is the pointer
    };
    void setoutptr(const char base, deBruijnNode* value) {
        setoutptr(kmerint::base_to_int(base), value);
    };

    // in edge maintenance
    deBruijnNode* getinptr(const unsigned int base) {
        if (in_edges[base] != nullptr)
            return in_edges[base];
        else
            return nullptr;
    };
    deBruijnNode* getinptr(const char base) {
        return getinptr(kmerint::base_to_int(base));
    };
    void setinptr(const unsigned int base, deBruijnNode* value) {
        // ### Need to verify that the base matches prefix or suffix of dest
        if (in_edges[base] != value) {
            in_edges[base] = value;
            value->setoutptr(base, this); // ensure a consistent pair of pointers
        }
        // assume that if the edge is already set, so is the pointer
    };
    void setinptr(const char base, deBruijnNode* value) {
        setinptr(kmerint::base_to_int(base), value);
    };

    void print(const std::string prefix = "") const {
        std::cout << prefix << "Node value: " << std::endl;
        nodevalue->print(prefix + "    ");

        for (unsigned i=0; i<alphabet_size; ++i) {
            if (out_edges[i] != nullptr)
                std::cout << prefix << "out edge exists for " << kmerint::int_to_base(i) << std::endl;
            if (in_edges[i] != nullptr)
                std::cout << prefix << "in edge exists for " << kmerint::int_to_base(i) << std::endl;
        }
    };
    void graphviz_node(std::ofstream& outf) {
        for (unsigned i=0; i<alphabet_size; ++i) {
            std::string msg;
            if (out_edges[i] != nullptr) {
                msg = nodevalue->getkmer() + " -> " + out_edges[i]->getkmer();
                msg += "[label=\"" + std::to_string(kmerint::int_to_base(i)) + "\"];";
                outf << msg << std::endl;
            }
            if (in_edges[i] != nullptr) {
                msg = in_edges[i]->getkmer() + " -> " + nodevalue->getkmer();
                msg += "[label=\"" + std::to_string(kmerint::int_to_base(i)) + "\"];";
                outf << msg << std::endl;
            }
        };
    };
    
    void test(const bool verbose = true) {
        // Assumption: node is just created.

        // At creation time, all pointers are null
        for (unsigned int base=0; base<alphabet_size; ++base) {
            assert(in_edges[base] == nullptr);
            assert(out_edges[base] == nullptr);
        }
        if (verbose) std::cout << "All pointers are null." << std::endl;

        std::string kmer("");
        for (unsigned int i=nodevalue->get_k(); i>0; --i)
            kmer += kmerint::int_to_base(i % alphabet_size);
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;

        // Add in pointer and corresponding out pointer is updated
        deBruijnNode* n2 = new deBruijnNode(nodevalue->get_k(), kmer);
        for (unsigned j=0; j<alphabet_size; ++j) {
            setinptr(kmerint::int_to_base(j), n2);
            assert(in_edges[j] == n2);
            assert(n2->getoutptr(kmerint::int_to_base(j)) == this);

            out_edges[j] = nullptr;

            setinptr(j, n2);
            assert(in_edges[j] == n2);
            assert(n2->getoutptr(kmerint::int_to_base(j)) == this);
        }
        if (verbose) std::cout << "In pointers work OK." << std::endl;
        delete n2;
        
        // All pointers should be null again due to destructor
        for (unsigned int base=0; base<alphabet_size; ++base) {
            assert(in_edges[base] == nullptr);
            assert(out_edges[base] == nullptr);
        }
        if (verbose) std::cout << "Destructor works OK." << std::endl;

        // Add out pointer and corresponding in pointer is updated
        n2 = new deBruijnNode(nodevalue->get_k(), kmer);
        for (unsigned j=0; j<alphabet_size; ++j) {
            setoutptr(kmerint::int_to_base(j), n2);
            assert(out_edges[j] == n2);
            assert(n2->getinptr(kmerint::int_to_base(j)) == this);

            out_edges[j] = nullptr;

            setoutptr(j, n2);
            assert(out_edges[j] == n2);
            assert(n2->getinptr(kmerint::int_to_base(j)) == this);
        }
        if (verbose) std::cout << "Out pointers work OK." << std::endl;
        delete n2;
        if (verbose) std::cout << std::endl;
    };
};
#endif // DEBRUIJNNODE
