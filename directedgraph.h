/****************************************************************
Oct 2011 BecomeJapan programming test [Adam Jackson]

Class representing a directed graph. The graph is an adjacency
matrix to which edges and vertices can be added. Nodes in the 
graph also map to node objects.
****************************************************************/

#ifndef DIRECTEDGRAPH_H
#define DIRECTEDGRAPH_H

#include <stdint.h>
#include <map>
#include <string>
#include <cstring>

typedef uint32_t MatrixIndex;

class DirectedGraph
{
    public:  
	DirectedGraph(){};
	DirectedGraph(MatrixIndex nodecount);
	virtual ~DirectedGraph();
  
	// returns true if there is an edge, false otherwise
	bool isEdge(MatrixIndex i, MatrixIndex j) const;
	void addEdge(MatrixIndex i, MatrixIndex j);
	void removeEdge(MatrixIndex i, MatrixIndex j);
	void removeVertex(MatrixIndex vertex);
  
	// print the graph to standard out
	void dumpGraph();
    
	// returns count of how many nodes in the graph
	MatrixIndex getNodeCount() const;
	
	typedef std::string Node;
	// add entry to map of index to node
	void addIndexToNodeLookup(MatrixIndex index, Node node);
	// returns the node with the given index
	Node getNodeByIndex(MatrixIndex index) const;
  
    private:
	bool** m_adjacencyMatrix;
	// count of nodes in graph
	MatrixIndex m_nodecount;
	
	// given the index of a node get the node itself
	typedef std::map<MatrixIndex, Node> IndexToNodeLookUp;
	IndexToNodeLookUp m_indexToNodeLookUp;
};

#endif
