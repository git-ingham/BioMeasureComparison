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
#include <unordered_map>
#include <iterator>
#include "kmerint.h"
#include "intbase.h"
#include "deBruijnGraph.h"

class deBruijnNode {
    typedef std::unordered_multimap<intbase, deBruijnNode*> in_set_t;

    deBruijnNode* out_edges[alphabet_size];
    in_set_t in_edges;
    kmerint nodevalue;
    double kmerfreq;
    bool verbose = false;

    void init_edges(void) {
        for (unsigned int i=0; i<alphabet_size; ++i) {
            out_edges[i] = nullptr;
        }
        in_edges.clear();
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
    deBruijnNode(const kmerint &value, const double nodefreq_p = 0.0) : nodevalue(value) {
        init_edges();
        kmerfreq = nodefreq_p;
    };
    // We can only create a nody by copying another node if we have access to the destination graph to be able to set the in and out pointers
    deBruijnNode(const deBruijnNode* srcnode, const deBruijnGraph *dstgraph) {
        // deep copy
        nodevalue = srcnode->nodevalue;
        kmerfreq = srcnode->kmerfreq;
        init_edges();
        // copy in and out pointers
        for (intbase b=b.begin(); b != b.end(); ++b) {
            out_edges[b] = dstgraph->find_node(srcnode->get_kmerhash, true);
            auto it = srcnode->in_edges.equal_range(base);
            for (auto p=it.first; p != it.second; ++p) {
                set_inptr(b, dstgraph->find_node(p->get_kmerhash, true));
            }
        }
    }
    ~deBruijnNode() {
        // We do not delete anything because we do not allocate anything.

        // clear all pointers that point to us
        for (unsigned i=0; i<alphabet_size; ++i) {
            if (out_edges[i] != nullptr) {
                out_edges[i]->clear_inptr(i, this);
                out_edges[i] = nullptr;
            }
            while (!in_edges.empty()) {
                deBruijnNode* n = in_edges.begin()->second;
                in_edges.erase(in_edges.begin());
                n->clear_outptr(i);
            }
        }
    };

    std::string get_kmer(void) const {
        return nodevalue.get_kmer();
    };
    // set_kmer is dangerous because it would possibly make pointers invalid
    kmer_storage_t get_kmerhash(void) {
        return nodevalue.get_kmerhash();
    };
    kmer_storage_t get_kmerbitmask(void) {
        return nodevalue.get_kmerbitmask();
    };
    std::string get_prefix(void) {
        return nodevalue.get_prefix();
    };
    std::string get_suffix(void) {
        return nodevalue.get_suffix();
    };
    double get_freq(void) const {
        return kmerfreq;
    };
    void set_freq(const double freq_p) {
        kmerfreq = freq_p;
    };

    // out edge maintenance
    deBruijnNode* get_outptr(const intbase& base) {
        return get_outptr(base.get_int());
    }
    deBruijnNode* get_outptr(const unsigned int base) {
        if (out_edges[base] != nullptr)
            return out_edges[base];
        else
            return nullptr;
    };
    deBruijnNode* get_outptr(const char base) {
        return get_outptr(intbase::base_to_int(base));
    };
    void set_outptr(const intbase& base, deBruijnNode* value) {
        assert(value != nullptr);
        // verify that this->suffix + base matches value->kmer
        inout_consistency("deBruijnNode::set_outptr", this, base, value);
        consistent_ptrs();

        if (verbose)
            std::cout << "set_outptr: asked to set " << nodevalue.get_kmer() << " + " << base
                      << " -> " << value->get_kmer() << std::endl;
        if (out_edges[base.get_int()] != value) {
            out_edges[base.get_int()] = value;
            value->set_inptr(base, this); // ensure a consistent pair of pointers
        } /*else {
            std::cout << "set_outptr: Edge already set" << std::endl;
        }*/
        // assume that if the edge is already set, so is the pointer

        consistent_ptrs();
    };
    void set_outptr(const char base, deBruijnNode* value) {
        set_outptr(intbase(base), value);
    };
    void clear_outptr(const unsigned int base) {
        if (out_edges[base] != nullptr) {
            deBruijnNode* t = out_edges[base];
            out_edges[base] = nullptr; // this prevents infinte loop in clearing in/out ptrs
            t->clear_inptr(base, this);
        }
        // else nothing to do
    };
    void clear_outptr(const char base) {
        clear_outptr(intbase::base_to_int(base));
    };

    // in edge maintenance
    auto get_inptrs(const intbase& base) -> decltype(in_edges.find(base)) {
        return in_edges.find(base);
    };
    auto get_inptrs(const char base) -> decltype(in_edges.find(base)) {
        return get_inptrs(intbase::base_to_int(base));
    };
    // More than one node can point to us
    void set_inptr(const intbase& base, deBruijnNode* value) {
        assert(value != nullptr);
        inout_consistency("deBruijnNode::set_inptr", value, base, this);
        // Consistency check passed.

        if (verbose)
            std::cout << "set_inptr: asked to set " << value->get_kmer() << " + " << base
                      << " -> " << nodevalue.get_kmer() << std::endl;

        if (!inptr_exists(base, value)) { // not found, add
            if (verbose) std::cout << "    set_inptr: setting" << std::endl;
            in_edges.emplace(base, value);
            if (value->out_edges[base.get_int()] != this) {
                if (value->out_edges[base.get_int()] != nullptr) {
                    std::cerr << "Should not change someone's outptr!" << std::endl;
                    abort();
                }
                value->out_edges[base.get_int()] = this;
            }
//             value->set_outptr(base, this); // ensure a consistent pair of pointers
        } else {
            // else: assume that if the edge is already set, so is the back pointer
            if (verbose) std::cout << "    set_inptr: Edge already set" << std::endl;
        }

        consistent_ptrs();
    };
    void set_inptr(const char base, deBruijnNode* value) {
        set_inptr(intbase(base), value);
    };
    void clear_inptr(const unsigned int base, const deBruijnNode* from) {
        auto it = in_edges.equal_range(base);
        if (it.first != in_edges.end()) {
            for (auto b=it.first; b != it.second; ++b) {
                // Look for an outptr to this
//                 if (b->second->get_outptr(base) == from) {
                if (b->second == from) {
                    // the order of operations prevents infinte loop in clearing in/out ptrs
                    deBruijnNode* t = b->second;
                    in_edges.erase(b);
                    t->clear_outptr(base);
                    break; // No need to continue
                }
            }
        }
    };
    void clear_inptr(const char base, const deBruijnNode* from) {
        clear_inptr(intbase::base_to_int(base), from);
    };
    bool inptr_exists(const intbase& base, const deBruijnNode* from) {
        bool result = false;
        auto it = in_edges.equal_range(base);
        for (auto b=it.first; b != it.second; ++b) {
            if (b->second == from) {
                result = true;
                break;
            }
        }
        return result;
    };

    void consistent_ptrs() {
        // This function does not return in the event of inconsistency
        // verify that all outptrs have inptrs from us
        for (unsigned int b=intbase::begin(); b < intbase::end(); ++b) {
            if (out_edges[b] != nullptr && !out_edges[b]->inptr_exists(b, this)) {
                std::cout << "Out edge inconsistency error." << std::endl;
                std::cout << "This node is: " << std::endl << this;
                std::cout << "Offending base is: " << intbase(b) << std::endl;
                std::cout << "Offending node is: " << std::endl << out_edges[b] << std::endl;
                assert(false);
            }
        }

        // verify that all inptrs have outptrs to us
        for (auto ip=in_edges.begin(); ip != in_edges.end(); ++ip) {
            if (ip->second->get_outptr(ip->first) != this) {
                std::cout << "In edge consistency error." << std::endl;
                std::cout << "This node is: " << std::endl << *this;
                std::cout << "offending base is: " << ip->first << std::endl;
                std::cout << "offending node is: " << std::endl << ip->second << std::endl;
            }
            assert(ip->second->get_outptr(ip->first) == this);
        }
    };
    static void inout_consistency(const std::string where, deBruijnNode *in, const intbase& ib, deBruijnNode *out) {
        char basec = ib.get_base();
        // verify that value->suffix + base matches this->kmer
        std::string suffix = in->get_suffix();
        std::string expectedkmer = suffix + basec;
        if (expectedkmer.compare(out->get_kmer()) != 0) {
            std::cerr << where << " expected value->suffix (" << suffix << ") + base (" << basec << ") == kmer (";
            std::cerr << out->get_kmer() << ").  kmer should be " << expectedkmer << std::endl;
            assert(expectedkmer.compare(out->get_kmer()) == 0);
        }
    };

    void print(const std::string p = "") const {
        std::cout << p << "Node value: " << nodevalue.get_kmer() << std::endl;
        std::string prefix = p + "    ";
        //nodevalue.print(prefix + "    ");

        // ### use intbase instead of int
        for (unsigned i=0; i<alphabet_size; ++i) {
            if (out_edges[i] != nullptr) {
                std::cout << prefix << "Out edge exists for " << intbase::int_to_base(i);
                std::cout << " to " << out_edges[i]->get_kmer() << "." << std::endl;
            } else {
                std::cout << prefix << "No out edge on " << intbase::int_to_base(i) << "." << std::endl;
            }
            auto it = in_edges.equal_range(intbase(i));
            if (it.first != in_edges.end()) {
                for (auto b=it.first; b != in_edges.end(); ++b) {
                    std::cout << prefix << "In edge from " << b->second->get_kmer();
                    std::cout << " on " << intbase::int_to_base(i) << "." << std::endl;
                }
            } else {
                std::cout << prefix << "No in edge on " << intbase::int_to_base(i) << "." << std::endl;
            }
        }
    };
    friend std::ostream& operator<< (std::ostream &stream, deBruijnNode node) {
        stream << &node;
        return stream;
    };
    friend std::ostream& operator<< (std::ostream &stream, deBruijnNode *node) {
        stream << "Node value: " << node->nodevalue << std::endl;
        stream << "    Out edges: ";
        for (unsigned i=0; i<alphabet_size; ++i) {
            if (node->out_edges[i] != nullptr) {
                stream << intbase(i) << " -> " << node->out_edges[i]->get_kmer() << "; ";
            }
        }
        stream << std::endl << "    In edges: ";
        for (intbase ib(ib.begin()); ib < ib.end(); ++ib) {
            auto it = node->in_edges.equal_range(ib);
            if (it.first != node->in_edges.end()) {
                for (auto b=it.first; b != node->in_edges.end(); ++b) {
                    stream << b->second->get_kmer() << " + " << ib << "; ";
                }
            }
        }
        stream << std::endl;

        return stream;
    };

    void graphviz(std::ofstream& outf) {
        for (unsigned i=0; i<alphabet_size; ++i) {
            std::string msg;
            if (out_edges[i] != nullptr) {
                msg = nodevalue.get_kmer() + " -> " + out_edges[i]->get_kmer();
                msg += "[label=\"" + std::to_string(intbase::int_to_base(i)) + "\"];";
                outf << msg << std::endl;
            }
            auto it = in_edges.equal_range(intbase(i));
            if (it.first != in_edges.end()) {
                for (auto b=it.first; b != in_edges.end(); ++b) {
                    msg = b->second->get_kmer() + " -> " + nodevalue.get_kmer();
                    msg += "[label=\"" + std::to_string(intbase::int_to_base(i)) + "\"];";
                    outf << msg << std::endl;
                }
            }
        };
    };

    void test(const bool verbose = true) {
        // Assumption: node is just created.

        // At creation time, all pointers are null
        for (unsigned int base=0; base<alphabet_size; ++base) {
            assert(in_edges.empty());
            assert(out_edges[base] == nullptr);
        }
        if (verbose) std::cout << "At creation, all pointers are null." << std::endl;

        // Add in pointer and corresponding out pointer is updated
        for (intbase b(b.begin()); b<b.end(); ++b) {
            kmerint next = nodevalue + b;
            deBruijnNode* n2 = new deBruijnNode(next);
            set_outptr(b, n2);
            inout_consistency("deBruijnNode::test", this, b, n2);

            assert(out_edges[b.get_int()] == n2);
            // one of n2's in_edges points to this
            auto it = in_edges.equal_range(b);
            if (it.first != in_edges.end()) {
                bool found = false;
                for (auto r=it.first; r != it.second; ++r) {
                    // look for a match
                    if (r->second->get_outptr(b) == this) {
                        found = true;
                        break;
                    }
                }
                if (!found)
                    assert(found);
            } else {
                std::cerr << "outptr was not updated when inptr was set." << std::endl;
                assert(it.first != in_edges.end());
            }

            delete n2;
        }
        if (verbose) std::cout << "Pointers work OK." << std::endl;

        // All pointers should be null due to destructor
        assert(in_edges.empty());
        for (unsigned int base=0; base<alphabet_size; ++base) {
            assert(out_edges[base] == nullptr);
        }
        if (verbose) std::cout << "Destructor works OK." << std::endl;
        if (verbose) std::cout << std::endl;
    };
};

#endif // DEBRUIJNNODE
