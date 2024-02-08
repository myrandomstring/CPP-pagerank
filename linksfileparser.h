/****************************************************************
Parses a text file with two strings per line where each string 
represents a node in a directed graph with an edge (link) from 
the first node to the second node. The nodes and their edges can 
then be added to an instance of a graph class.
****************************************************************/

#ifndef LINKSFILEPARSER_H
#define LINKSFILEPARSER_H

#include "directedgraph.h"

#include <list>
#include <string>
#include <map>
#include <exception>

// Exception class for errors in file parsing
class LinksFileParserException : public std::exception
{
    public:
	LinksFileParserException():std::exception(){}
	LinksFileParserException(const char* message):std::exception(),m_message(message){}
	virtual ~LinksFileParserException() throw(){}
	virtual const char* what() const throw()
	{
	    return m_message.c_str();
	}

    private:
	std::string m_message;
};


// class for parsing links file
class LinksFileParser
{
    public:
	LinksFileParser();
	virtual ~LinksFileParser(){};
	// parses file and stores links and unique nodes
        void parseFile(char* filepath);
	// add nodes to the graph
	void addNodesToGraph(DirectedGraph& graph);
	// returns how many nodes in the graph
	uint32_t getNodeCount();

    private:
        typedef std::string Node;

	struct Link
	{
	    Node from;
	    Node to;
	};

	typedef uint32_t NodeIndex;
	typedef std::list<Link> Links;
	typedef std::list<Node> Nodes;
	typedef std::map<Node, NodeIndex> NodeLookUp;

	// list of links from file
	Links m_links;
	// list of unique nodes from file
	Nodes m_nodes;
	// map a node to it's assigned index
	NodeLookUp m_nodeLookUp;
};
      


#endif
