#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

#include <vector>
#include <map>
#include <iostream>

namespace abstract {

template <typename NodeType, typename EdgeType> 
class Graph {

    public:    

        using node_iterator = typename std::vector<NodeType>::iterator;
        using edge_iterator = typename std::map<std::pair<int,int>,EdgeType>::iterator;
        

        Graph(int size) { nodes.reserve(size); }
        ~Graph() {}

        node_iterator node_begin() { return nodes.begin(); }
        node_iterator node_end() { return nodes.end(); }
 
        edge_iterator edge_begin() { return edges.begin(); }
        edge_iterator edge_end() { return edges.end(); }
        
        std::size_t node_size() const { return nodes.size(); }
        std::size_t edge_size() const { return edges.size(); }
        
        void add_node(int node_index, NodeType node) {
            if (node_index < nodes.capacity()) {
                nodes[node_index] = node;
            } else {

            }
        }

        void add_edge(std::pair<int,int> from_to_index_pair, EdgeType edge) {
            edges[from_to_index_pair] = edge;
        }

        EdgeType& find_edge(const std::pair<int,int>& edge) {
            auto edge_it = edges.find(edge);
            if (edge_it != edges.end()) {
                return edge_it->second;
            } else {
                return invalid_edge;
            }
        }

        void print_nodes(std::ostream& outs) {
            for (auto it = nodes.begin(); it != nodes.end(); it++) {
                outs << "node[" << it->first << "] = { " << it->second << " }" << std::endl;
            }
        }
        
    public:

        static EdgeType invalid_edge;

    private:

        std::vector<NodeType> nodes; // graph nodes are numbered as vector indecies
        std::map<std::pair<int,int>,EdgeType> edges;
};

template <typename NodeType,typename EdgeType>
EdgeType Graph<NodeType,EdgeType>::invalid_edge = EdgeType();

} // namespace abstract

#endif // #ifndef ABSTRACT_GRAPH_H
