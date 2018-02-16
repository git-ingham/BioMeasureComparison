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
    kmerint nodevalue;
    double kmerfreq;
    void init_edges(void) {
        for (unsigned i=0; i<alphabet_size; ++i) {
            out_edges[i] = nullptr;
            in_edges[i] = nullptr;
        }
    }

public:
    deBruijnNode(const unsigned int k, const std::string value, const double nodefreq_p = 0.0) : nodevalue(k, value) {
        init_edges();
        kmerfreq = nodefreq_p;
    };
    deBruijnNode(const unsigned int k, const kmer_storage_t value, const double nodefreq_p = 0.0) : nodevalue(k, value) {
        init_edges();
        kmerfreq = nodefreq_p;
    };
    ~deBruijnNode() {
        // We do not delete anything because we do not allocate anything.

        // clear all pointers that point to us
        for (unsigned i=0; i<alphabet_size; ++i) {
            if (out_edges[i] != nullptr) {
                out_edges[i]->clear_inptr(i);
                out_edges[i] = nullptr;
            }
            if (in_edges[i] != nullptr) {
                in_edges[i]->clear_outptr(i);
                in_edges[i] = nullptr;
            }
        }
    };

    std::string get_kmer(void) const {
        return nodevalue.get_kmer();
    };
    void set_kmer(const std::string value_p) {
        nodevalue.set_kmer(value_p);
    };
    kmer_storage_t get_kmerhash(void) {
        return nodevalue.get_kmerhash();
    };
    kmer_storage_t get_kmerbitmask(void) {
        return nodevalue.get_kmerbitmask();
    };

    double get_freq(void) const {
        return kmerfreq;
    };
    void set_freq(const double freq_p) {
        kmerfreq = freq_p;
    };

    // out edge maintenance
    deBruijnNode* get_outptr(const unsigned int base) {
        if (out_edges[base] != nullptr)
            return out_edges[base];
        else
            return nullptr;
    };
    deBruijnNode* get_outptr(const char base) {
        return get_outptr(intbase::base_to_int(base));
    };
    void set_outptr(const unsigned int base, deBruijnNode* value) {
        assert(value != nullptr);
        
        // verify that value->kmer matches this->suffix + base
        std::string suffix = nodevalue.get_suffix();
        char basec = intbase::int_to_base(base);
        std::string expectedkmer = suffix + basec;
        if (expectedkmer.compare(value->get_kmer()) != 0) {
            std::cerr << "deBruijnNode::set_outptr expected this->suffix (";
            std::cerr << suffix << ") + base (" << basec << ") == kmer (" << value->get_kmer() << ")";
            assert(expectedkmer.compare(this->get_kmer()) == 0);
        }

        if (out_edges[base] != value) {
            out_edges[base] = value;
            value->set_inptr(base, this); // ensure a consistent pair of pointers
        }
        // assume that if the edge is already set, so is the pointer
    };
    void set_outptr(const char base, deBruijnNode* value) {
        set_outptr(intbase::base_to_int(base), value);
    };
    void clear_outptr(const unsigned int base) {
        if (out_edges[base] != nullptr) {
            deBruijnNode* t = out_edges[base];
            out_edges[base] = nullptr; // this prevents infinte loop in clearing in/out ptrs
            t->clear_inptr(base);
        }
        // else nothing to do
    };
    void clear_outptr(const char base) {
        clear_outptr(intbase::base_to_int(base));
    };

    // in edge maintenance
    deBruijnNode* get_inptr(const unsigned int base) {
        if (in_edges[base] != nullptr)
            return in_edges[base];
        else
            return nullptr;
    };
    deBruijnNode* get_inptr(const char base) {
        return get_inptr(intbase::base_to_int(base));
    };
    void set_inptr(const unsigned int base, deBruijnNode* value) {
        assert(value != nullptr);

        // verify that this->kmer matches value->suffix + base
        std::string suffix = nodevalue.get_suffix();
        char basec = intbase::int_to_base(base);
        std::string expectedkmer = suffix + basec;
        if (expectedkmer.compare(this->get_kmer()) != 0) {
            std::cerr << "deBruijnNode::set_inptr expected value->suffix (";
            std::cerr << suffix << ") + base (" << basec << ") == kmer (";
            std::cerr << this->get_kmer() << ")" << std::endl;
            assert(expectedkmer.compare(this->get_kmer()) == 0);
        }
        // Consistency check passed.

        if (in_edges[base] != value) {
            in_edges[base] = value;
            value->set_outptr(base, this); // ensure a consistent pair of pointers
        }
        // else: assume that if the edge is already set, so is the back pointer
    };
    void set_inptr(const char base, deBruijnNode* value) {
        set_inptr(intbase::base_to_int(base), value);
    };
    void clear_inptr(const unsigned int base) {
        if (in_edges[base] != nullptr) {
            deBruijnNode* t = in_edges[base];
            in_edges[base] = nullptr;  // this prevents infinte loop in clearing in/out ptrs
            t->clear_outptr(base);
        }
    };
    void clear_inptr(const char base) {
        clear_inptr(intbase::base_to_int(base));
    };

    void print(const std::string p = "") const {
        std::cout << p << "Node value: " << nodevalue.get_kmer() << std::endl;
        std::string prefix = p + "    ";
        //nodevalue.print(prefix + "    ");

        for (unsigned i=0; i<alphabet_size; ++i) {
            if (out_edges[i] != nullptr) {
                std::cout << prefix << "Out edge exists for " << intbase::int_to_base(i);
                std::cout << " to " << out_edges[i]->get_kmer() << "." << std::endl;
            } else {
                std::cout << prefix << "No out edge on " << intbase::int_to_base(i) << "." << std::endl;
            }
            if (in_edges[i] != nullptr) {
                std::cout << prefix << "In edge from " << in_edges[i]->get_kmer();
                std::cout << " on " << intbase::int_to_base(i) << "." << std::endl;
            } else {
                std::cout << prefix << "No in edge on " << intbase::int_to_base(i) << "." << std::endl;
            }
        }
    };
    void graphviz(std::ofstream& outf) {
        for (unsigned i=0; i<alphabet_size; ++i) {
            std::string msg;
            if (out_edges[i] != nullptr) {
                msg = nodevalue.get_kmer() + " -> " + out_edges[i]->get_kmer();
                msg += "[label=\"" + std::to_string(intbase::int_to_base(i)) + "\"];";
                outf << msg << std::endl;
            }
            if (in_edges[i] != nullptr) {
                msg = in_edges[i]->get_kmer() + " -> " + nodevalue.get_kmer();
                msg += "[label=\"" + std::to_string(intbase::int_to_base(i)) + "\"];";
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
        for (unsigned int i=nodevalue.get_k(); i>0; --i)
            kmer += intbase::int_to_base(i % alphabet_size);
        if (verbose) std::cout << "test kmer is '" << kmer << "'" << std::endl;

        // Add in pointer and corresponding out pointer is updated
        deBruijnNode* n2 = new deBruijnNode(nodevalue.get_k(), kmer);
        for (unsigned j=0; j<alphabet_size; ++j) {
            // ----- Test using the character base
            char base = intbase::int_to_base(j);
            set_inptr(base, n2);
            assert(in_edges[j] == n2);
            assert(n2->get_outptr(base) == this);

            clear_inptr(base);
            assert(in_edges[j] == nullptr);
            assert(n2->get_outptr(base) == nullptr);

            // ----- Now, use j and not character form of the base

            set_inptr(j, n2);
            assert(in_edges[j] == n2);
            assert(n2->get_outptr(intbase::int_to_base(j)) == this);

            clear_inptr(j);
            assert(in_edges[j] == nullptr);
            assert(n2->get_outptr(j) == nullptr);
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
        n2 = new deBruijnNode(nodevalue.get_k(), kmer);
        for (unsigned j=0; j<alphabet_size; ++j) {
            set_outptr(intbase::int_to_base(j), n2);
            assert(out_edges[j] == n2);
            assert(n2->get_inptr(intbase::int_to_base(j)) == this);

            out_edges[j] = nullptr;

            set_outptr(j, n2);
            assert(out_edges[j] == n2);
            assert(n2->get_inptr(intbase::int_to_base(j)) == this);
        }
        if (verbose) std::cout << "Out pointers work OK." << std::endl;
        delete n2;
        if (verbose) std::cout << std::endl;
    };
};
#endif // DEBRUIJNNODE
