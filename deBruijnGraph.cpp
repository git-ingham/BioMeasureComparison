/*!
 * A DeBruijn graph for comparing ssequence distances
 *  https://en.wikipedia.org/wiki/De_Bruijn_graph
 *  Mangul & Coskicki used deBruijn graphs compared with the earth-mover distance to copare sequences.  Read about it here: http://dx.doi.org/10.1145%2F2975167.2975174
 */

#include <iostream>
#include <fstream>
#include <unordered_map>
#include "deBruijnGraph.h"

deBruijnGraph::deBruijnGraph(const unsigned int k_p) {
    k = k_p;
}
//! @todo commented out because of a circular dependency in the graph and node.
// deBruijnGraph::deBruijnGraph(deBruijnGraph &db) {
//     // deep copy constructor
//     k = db.k;
//     //b = db.b;
//     for (auto it=db.graph.begin(); it != db.graph.end(); ++it) {
//         graph[it->first] = new deBruijnNode(it->second, this);
//     }
// }

deBruijnGraph::~deBruijnGraph() { // free up all storage allocated in this class
    auto it = graph.begin();
    while (it != graph.end()) {
        delete it->second;
        graph.erase(it);
        it = graph.begin();
    }
}

deBruijnNode* deBruijnGraph::find_node(const kmerint& kmer, bool create) {
    if (graph.count(kmer) == 0 && create) {
        graph.emplace(kmer, new deBruijnNode(k));
    }
    return graph.count(kmer) == 0 ? nullptr : graph[kmer];
}

void
deBruijnGraph::print(std::string comment, std::string p) {
    std::cout << "Graph: " << comment << std::endl;
    std::string prefix = p + "    ";
    for (auto it=graph.begin(); it != graph.end(); ++it) {
        if (it->second != nullptr) {
            //std::cout << "Node: " << it->second->get_kmer() << std::endl;
            it->second->print(prefix);
        }
        else
            std::cerr << "Warning: null pointer in graph at node " << it->first << std::endl;
    }
}

void
deBruijnGraph::graphviz(std::string fname, std::string comment) {
    std::ofstream outf;
    outf.open(fname);
    outf << "digraph G {" << std::endl;
    outf << "graph [fontname = \"helvetica\"];" << std::endl;
    outf << "graph [label = \"" << comment << "\"];" << std::endl;
    for (auto it=graph.begin(); it != graph.end(); ++it) {
        if (it->second != nullptr)
            it->second->graphviz(outf);
        else
            std::cerr << "Warning: null pointer in graph at node: " << it->first << std::endl;
    }
    outf << "}"  << std::endl;
    outf.close();
}

// create complete deBruijn graph for size k; this gets large for large k; beware!
// number of nodes is alphabet_size ^ k
void
deBruijnGraph::make_complete_graph(void) {
    // Set up all nodes in the graph
    // We cannot set pointers until all the nodes are created.
    kmerint ki(k);
    graph.reserve(ki.end());
    //std::cout << "start: " << ki.begin() << "; end: " << ki.end() << std::endl;
    for (ki.set_kmerhash(ki.begin()); ki<ki.end(); ++ki) {
        //std::cout << "Loop 1: " << ki << std::endl;
        if (graph[ki] == nullptr) {
            graph[ki] = new deBruijnNode(ki);
        } else {
            std::cerr << "Trying to create duplicate node for " << ki << std::endl;
            exit(1);
        }
    }

    // Now that all nodes exist, set up pointers.  Only set out pointers since the
    // deBruijnNode code maintains the corresponding in pointers.
    for (ki.set_kmerhash(ki.begin()); ki<ki.end(); ++ki) {
        for (intbase b(b.begin()); b<b.end(); ++b) {
            kmerint next = ki + b;
            //std::cout << "ki: " << ki << "; base: " << b << "; next: " << next << std::endl;
            deBruijnNode *nextp = graph[next];
            assert(nextp != nullptr);
            graph[ki]->set_outptr(b, nextp);
        }
    }

    std::string comment = "Complete deBruijn graph for k = " + std::to_string(k);
    //std::string fname = "graph" + std::to_string(k) + ".dot";
    //graphviz(fname, comment);
    print(comment);
}

void
deBruijnGraph::consistency_check(void) {
    for (auto it = graph.begin(); it != graph.end(); ++it) {
        kmerint kmer = it->first;
        deBruijnNode* node = it->second;
        // graph[kmer]->getkmer() == kmer?
        assert(kmer == node->get_kmerhash());
        // check out pointers
        node->consistent_ptrs();
    }
    std::cout << "Passed consistency check\n";
}
