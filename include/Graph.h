#ifndef ABSTRACT_GRAPH_H
#define ABSTRACT_GRAPH_H

namespace abstract {

template <typename NodeType>
class GraphNode {

    public:
        
        GraphNode(NodeType node_data)
            : data(node_data) {}

        ~GraphNode() {}

        NodeType get_node_data() { return data; }

    private:
        NodeType data;
};

template <typename EdgeType>
class GraphEdge {

    public:
        
        GraphEdge(EdgeType edge_data)
            : data(edge_data) {}

        ~GraphNode() {}

        EdgeType get_edge_data() { return data; }

    private:
        EdgeType data;
};

template <typename NodeType, typename EdgeType> 
class Graph {

    public:    
        
        Graph() {}
        ~Graph() {}

        add_edge(NodeType from, NodeType to, EdgeType edge);

    private:
        
        std::unordered_set<GraphNode<NodeType>> nodes;
        std::unordered_map<std::pair<GraphNode<NodeType>,GraphNode<NodeType>>,> nodes;
};

} // namespace abstract

#endif // #ifndef ABSTRACT_GRAPH_H
