/****************************************************************
Class representing a directed graph. The graph is an adjacency
matrix to which edges and vertices can be added. Nodes in the 
graph also map to node objects.
****************************************************************/

#include "directedgraph.h"
#include <iostream>

// construct a square matrix of given size
DirectedGraph::DirectedGraph(MatrixIndex nodecount):m_nodecount(nodecount)
{
    m_adjacencyMatrix = new bool*[nodecount];
    
    for(MatrixIndex i = 0 ; i < nodecount ; ++i)
    {
        m_adjacencyMatrix[i] = new bool[nodecount];
	memset(m_adjacencyMatrix[i], '\0', nodecount);
    }
}

DirectedGraph::~DirectedGraph()
{
    for(MatrixIndex i = 0 ; i < m_nodecount ; ++i)
    {
        delete[] m_adjacencyMatrix[i];
    }

    delete[] m_adjacencyMatrix;
}

// returns true if there is an edge between two nodes 
// and false otherwise
bool DirectedGraph::isEdge(MatrixIndex i, MatrixIndex j) const
{
    return m_adjacencyMatrix[i][j] ? true : false;
}

// add edge to graph
void DirectedGraph::addEdge(MatrixIndex i, MatrixIndex j)
{
    m_adjacencyMatrix[i][j] = true;
}

// remove edge from graph
void DirectedGraph::removeEdge(MatrixIndex i, MatrixIndex j)
{
    m_adjacencyMatrix[i][j] = false;
}


// remove vertex from graph
void DirectedGraph::removeVertex(MatrixIndex vertex)
{
    for(MatrixIndex i = 0 ; i < m_nodecount ; ++i)
    {
        m_adjacencyMatrix[vertex][i] = false;
	m_adjacencyMatrix[i][vertex] = false;
    }
}

// show the adjacency matrix
void DirectedGraph::dumpGraph()
{
    std::cout << "Adjacency matrix: " << std::endl;

    for(MatrixIndex i = 0 ; i < m_nodecount ; ++i)
    {
        for(MatrixIndex j = 0 ; j < m_nodecount ; ++j)
	{
	    std::cout << m_adjacencyMatrix[i][j] << " ";
	}
    
	std::cout << std::endl;
    }
    
    std::cout << std::endl;
}

// returns number of nodes in the graph
MatrixIndex DirectedGraph::getNodeCount() const
{
    return m_nodecount;
}

// add entry to map of index to node objects
void DirectedGraph::addIndexToNodeLookup(MatrixIndex index, Node node)
{
    m_indexToNodeLookUp.insert(std::pair<MatrixIndex,Node>(index,node));
}

// returns the node with the given index
DirectedGraph::Node DirectedGraph::getNodeByIndex(MatrixIndex index) const
{
    IndexToNodeLookUp::const_iterator indexToNodeLookUpIter = m_indexToNodeLookUp.find(index);

    if(indexToNodeLookUpIter == m_indexToNodeLookUp.end())
    {
      return Node("NODE NAME NOT FOUND IN LOOKUP");
    }
    else
    {
        return indexToNodeLookUpIter->second;
    }
}

