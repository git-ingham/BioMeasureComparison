#ifndef DEBRUIJNGRAPH_H
#define DEBRUIJNGRAPH_H

/**
* @brief DeBruijn graph, used for investigting the EMD distance as described in Mangul & Coskicki
*    dx.doi.org/10.1145%2F2975167.2975174
* 
*/

#include <cstdint>
#include <unordered_map>
#include <err.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "intbase.h"
#include "kmerint.h"
#include "deBruijnNode.h"

class deBruijnNode;

class deBruijnGraph {
    // Note that we are indexing based on the kmer hash and not string.  This is faster,
    // but the code is less clear.
    //                         key             value
    typedef std::unordered_map<kmerint, deBruijnNode*> graph_t;
    graph_t graph;

    unsigned int k;
    //intbase b;

public:
    deBruijnGraph(const unsigned int k_p);
    deBruijnGraph(deBruijnGraph &db);
    ~deBruijnGraph();
    
    deBruijnNode* find_node(const kmerint& kmer, bool create);
    void make_complete_graph(void);
    void consistency_check(void);
    void print(std::string comment = "", std::string prefix = "");
    void graphviz(std::string fname, std::string comment = "");
    
    // copy constructor for a node needs access to the containing graph.
    //friend deBruijnNode::deBruijnNode(const deBruijnNode* srcnode, const deBruijnGraph *dstgraph);
    
    //! @todo Need the Earth mover distance function(s) (somewhere; maybe not here
    //! @todo need test code for graph operations such as find_node
    //! @todo need test code for copy constructor    
};

#endif // DEBRUIJNGRAPH_H
