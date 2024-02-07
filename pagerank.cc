/*******************************************************************************
Oct 2011 BecomeJapan programming test [Adam Jackson]

This application identifies page rank leaks (nodes with no outbound links), rank 
sinks (groups of interconnected nodes not connected to the rest of the graph) 
and calculates the page rank for nodes in a directed graph. The graph is input
as a text file where each line of the file has two strings representing two nodes 
in the graph with an edge from the first node to the second node. The application
can be run in "check" mode to identify rank leaks and sinks; and in "run" mode to 
calculate the page rank of nodes in the graph. In "run mode" orphan nodes with no 
inbound links and all nodes pointed to only by orphan nodes will first be removed; 
rank leaks, with no outbound links will also be removed (although not recursively).
**********************************************************************************/

#include "pagerank.h"
#include "linksfileparser.h"
#include "directedgraph.h"
#include "pageranker.h"

#include <iostream>
#include <sstream>
#include <iomanip>

int main(int argc, char* argv[])
{   
    try
    {
        parseArguments(argc,argv);
    }
    catch (LinksFileParserException e)
    {
	std::cout << "EXCEPTION THROWN: " << e.what() << std::endl;
	showUsage();
	return 1;
    }
    catch (InputArgumentException e)
    {
	std::cout << "EXCEPTION THROWN: " << e.what() << std::endl;
	showUsage();
	return 1;
    }
    catch(...)
    {
	std::cout << "Caught default exception" << std::endl;
	return 1;
    }

    return 0;
}


// show program usage
void showUsage()
{
    std::cout << "Run mode usage: pagerank run <filename> <iterations> <decay factor (0 < d <= 1)>" << std::endl;
    std::cout << "Check mode usage: pagerank check <filename>" << std::endl;
    std::cout << std::endl;
}

// Parses command line arguments
void parseArguments(int argc, char* argv[])
{
    // "check" mode
    if(!strcmp(argv[1], "check") && argc == 3)
    {
	LinksFileParser linksFileParser;
	linksFileParser.parseFile(argv[2]);
	DirectedGraph directedGraph(linksFileParser.getNodeCount());
	linksFileParser.addNodesToGraph(directedGraph);
	directedGraph.dumpGraph();
	PageRanker pageRanker;	  
	// remove orphans and nodes only pointed to by orphans first
	pageRanker.removeOrphanNodes(directedGraph);
	directedGraph.dumpGraph();
	pageRanker.dumpRankLeaks(directedGraph);
	// remove leaks as these would otherwise prevent us from
	// seeing the sink nodes
	pageRanker.removeLeakNodes(directedGraph);
	directedGraph.dumpGraph();
	pageRanker.dumpRankSinks(directedGraph);
	pageRanker.dumpPageRank(directedGraph);
    } 
    else if(!strcmp(argv[1], "check"))
    {
       throw InputArgumentException("Checking graph incorrect arguments provided");
    }
    else if(!strcmp(argv[1], "run") && argc == 5)
    {
	// "run" mode
        uint32_t iterations;
	std::stringstream ss;
	ss << argv[3];
	    
	if(!(ss >> iterations))
	{
	   // if extraction fails (stream error / input of wrong type)
	    throw InputArgumentException("failed to parse iterations argument");
	}

	// need to clear eof flag set by last extraction
        // and empty string
	ss.clear();
	ss.str("");

	float decayfactor;
	ss << std::setprecision(2) << argv[4];
	   
	if(!(ss >> decayfactor))
 	{
	    // if extraction fails (stream error / input of wrong type)
	    throw InputArgumentException("failed to parse decay factor argument");
	} 

	if(decayfactor <= 0 || decayfactor > 1)
	{
	    throw InputArgumentException("decay factor not in range 0 < d <= 1");
	}

	LinksFileParser linksFileParser;
	linksFileParser.parseFile(argv[2]);
	DirectedGraph directedGraph(linksFileParser.getNodeCount());
	linksFileParser.addNodesToGraph(directedGraph);
	directedGraph.dumpGraph();
	    
	PageRanker pageRanker;
	pageRanker.removeOrphanNodes(directedGraph);
	pageRanker.removeLeakNodes(directedGraph);
	pageRanker.rankGraphNodes(directedGraph, decayfactor, iterations);
	pageRanker.dumpPageRank(directedGraph);
    }
    else if(!strcmp(argv[1], "run"))
    {
	throw InputArgumentException("Run mode incorrect arguments provided");
    }
    else
    {
	throw InputArgumentException("Arguments not understood/incomplete");
    }
}
