/****************************************************************
Oct 2011 BecomeJapan programming test [Adam Jackson]

Parses a text file with two strings per line where each string 
represents a node in a directed graph with an edge (link) from 
the first node to the second node. The nodes and their edges can 
then be added to an instance of a graph class.
****************************************************************/

#include "linksfileparser.h"

#include <fstream>
#include <string>
#include <iostream>
#include <sstream>

LinksFileParser::LinksFileParser(){}

// parses file and stores links and unique nodes
void LinksFileParser::parseFile(char* filepath)
{
    m_links.clear();
    m_nodes.clear();
    m_nodeLookUp.clear();

    std::cout << "####################" << std::endl;
    std::cout << "Parsing file " << filepath << std::endl;
    std::cout << "####################" << std::endl << std::endl;

    std::ifstream ifs(filepath, std::ifstream::in);
    
    if(ifs.fail())
    {
	ifs.close();
	throw LinksFileParserException("Failed to open file");
    }

    std::string linkstring;

    // check eof,fail and bad bits are not set
    while(ifs.good())
    {
	getline(ifs,linkstring);

        if(!linkstring.empty())
        {
	    std::stringstream linkss;
	    linkss << linkstring;

	    // std::cout << "linkstring " << linkstring << std::endl;

	    std::string fromnode;
	    std::string tonode;

	    linkss >> fromnode;
	    linkss >> tonode;

	    // std::cout << "From " << fromnode << std::endl;
	    // std::cout << "To " << tonode << std::endl;

	    if(!fromnode.compare(tonode))
	    {
		std::cout << "Ignoring link to self for node " << fromnode << std::endl;
	    }
	    else if(tonode.empty())
	    {
		std::cout << "Ignoring node with no link " << fromnode << std::endl;
	    }
	    else
	    {
		Link link = {fromnode, tonode};
		m_links.push_back(link);
		m_nodes.push_back(fromnode);
		m_nodes.push_back(tonode);
	    }
	}
    }

    if(m_nodes.empty())
    {
	ifs.close();
	throw LinksFileParserException("No valid nodes read from file");
    }
  
    m_nodes.sort();
    m_nodes.unique();

    Nodes::const_iterator iter = m_nodes.begin();

    // Assign each node a unique index. This will be used to look the node
    // up in the adjacency matrix
    for(NodeIndex index = 0 ; iter != m_nodes.end() ; ++iter, ++index)
    {
        m_nodeLookUp.insert(std::pair<Node, NodeIndex>(*iter, index));
        std::cout << "Assigned node " << *iter << " index " << index << std::endl;
    }
    
    std::cout << std::endl;

    ifs.close();
}

// Returns the number of unique nodes read in from file
uint32_t LinksFileParser::getNodeCount()
{
    return m_nodes.size();
}

// Adds nodes read from a text file to a directed graph and populates a lookup 
// table stored in the graph
void LinksFileParser::addNodesToGraph(DirectedGraph& graph)
{    
    // need to get indexes into the adjacency matrix for the nodes in each link
    NodeLookUp::const_iterator fromNodeLookUpIter;
    NodeLookUp::const_iterator toNodeLookUpIter;

    for(Links::const_iterator linksIter = m_links.begin() ; linksIter != m_links.end() ; ++linksIter)
    {
	// find the index of the 'from' node into the adjacency matrix
        fromNodeLookUpIter = m_nodeLookUp.find(linksIter->from);
	// find the index of the 'to' node into the adjacency matrix
	toNodeLookUpIter = m_nodeLookUp.find(linksIter->to);
	
	if(fromNodeLookUpIter != m_nodeLookUp.end() && toNodeLookUpIter != m_nodeLookUp.end())
	{
	    NodeIndex fromNodeIndex = fromNodeLookUpIter->second;
	    NodeIndex toNodeIndex = toNodeLookUpIter->second;

	    // ignore duplicate edges
	    if(graph.isEdge(fromNodeIndex, toNodeIndex))
	    {
		std::cout << "WARNING: There is already an edge " << fromNodeIndex << " (" << linksIter->from << ")"
		          << " -> " << toNodeIndex << " (" << linksIter->to << ") " << std::endl;
	    }
	    else
	    {
		std::cout << "Adding edge " << fromNodeIndex << " (" << linksIter->from << ")"
		          << " -> " << toNodeIndex << " (" << linksIter->to << ") " << std::endl;
		
		// add edge to graph
		graph.addEdge(fromNodeIndex, toNodeIndex);
	    }
	}
	else
	{
	    if(fromNodeLookUpIter == m_nodeLookUp.end())
	    {
	        std::cout << "WARNING: Unable to find adjacency matrix index for node " << linksIter->from << std::endl;
		std::cout << "Node will not be added to the graph" << std::endl;
	    }

	    if(toNodeLookUpIter == m_nodeLookUp.end())
	    {
	        std::cout << "WARNING: Unable to find adjacency matrix index for node " << linksIter->to << std::endl;
		std::cout << "Node will not be added to the graph" << std::endl;
	    }
	}
    }

    std::cout << std::endl;

    // Add the index of each node and the node name to a lookup table (index->node) in the graph.
    for(NodeLookUp::const_iterator nodeToIndexIter =  m_nodeLookUp.begin() ; nodeToIndexIter !=  m_nodeLookUp.end() ; ++nodeToIndexIter)
    {
        graph.addIndexToNodeLookup(nodeToIndexIter->second, nodeToIndexIter->first);
    }
}
