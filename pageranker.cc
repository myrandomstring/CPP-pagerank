/****************************************************************
This class finds page rank leaks, page rank sinks and calculates
the page rank of the nodes in a given directed graph. It can remove
orphan nodes (no inbound links) and nodes pointed to only by orphan
nodes. It can also remove nodes with no outgoing links (rank leaks)
although this is not done in a recursive fashion.

****************************************************************/

#include "pageranker.h"

#include <iostream>
#include <list>
#include <math.h>

PageRanker::PageRanker():m_pageRankVector(NULL),m_outboundLinkCount(NULL),m_inboundLinkCount(NULL),m_atLeastOneInboundLink(NULL),m_atLeastOneOutboundLink(NULL){}

PageRanker::~PageRanker()
{
    delete[] m_pageRankVector;
    delete[] m_outboundLinkCount;
    delete[] m_inboundLinkCount;
    delete[] m_atLeastOneInboundLink;
    delete[] m_atLeastOneOutboundLink;
}


// Rank sinks are detected by the presence of nodes which have page rank along with
// nodes which do not. Writes rank sinks to standard out.
void PageRanker::dumpRankSinks(const DirectedGraph& graph)
{   
    MatrixIndex numberOfNodes = graph.getNodeCount();

    // calculate page rank using decay factor of 1
    rankGraphNodes(graph, 1, SINK_DETECT_ITERATIONS);

    std::list<MatrixIndex> NodesWithZeroPageRank;
    std::list<MatrixIndex> NodesWithNonZeroPageRank;

    for(MatrixIndex i = 0 ; i < numberOfNodes ; ++i)
    {
	// when counting nodes with zero page rank ignore isolated nodes (no in- or outbounds links)
        if( (m_pageRankVector[i] < ZERO_PR_THRESHOLD) && (m_outboundLinkCount[i] || m_inboundLinkCount[i]))
	{
	    NodesWithZeroPageRank.push_back(i);
	}
	else if(m_pageRankVector[i] >= ZERO_PR_THRESHOLD)
	{
	    NodesWithNonZeroPageRank.push_back(i);
	}
    }

    MatrixIndex countOfNodesWithZeroPageRank = NodesWithZeroPageRank.size();
    MatrixIndex countOfNodesWithNonZeroPageRank = NodesWithNonZeroPageRank.size();
   
    std::cout << "##################" << std::endl;
    std::cout << "Rank sink summary" << std::endl;
    std::cout << "##################" << std::endl << std::endl;

    std::cout << "There are " << countOfNodesWithZeroPageRank << " nodes with zero page rank (not including isolated nodes)" << std::endl;
    std::cout << "There are " << countOfNodesWithNonZeroPageRank << " nodes with non-zero page rank " << std::endl;
    std::cout << std::endl;

    if(countOfNodesWithZeroPageRank && countOfNodesWithNonZeroPageRank)
    {    
	std::cout << "Sink node | PageRank" << std::endl;
	
	for(std::list<MatrixIndex>::iterator iter = NodesWithNonZeroPageRank.begin() ; iter != NodesWithNonZeroPageRank.end() ; ++iter)
        {
	    std::cout << graph.getNodeByIndex(*iter) << " (index " << *iter << ") " << m_pageRankVector[*iter] << std::endl;
	}
    }
    else
    {
	std::cout << "NO SINK NODES WERE FOUND" << std::endl;
    }

    std::cout << std::endl;
}

// Remove orphan nodes (nodes with no inbound links) from the graph. Orphans
// are removed recursively so nodes which are only pointed to by orphans are
// also removed.
void PageRanker::removeOrphanNodes(DirectedGraph& graph)
{
    std::cout << "#####################" << std::endl;
    std::cout << "Removing orphan nodes" << std::endl;
    std::cout << "#####################" << std::endl;

    MatrixIndex numberOfNodes = graph.getNodeCount();
    MatrixIndex numberOfOrphans = 0;

    getNodesWithAtLeastOneInBoundLink(graph);
    getNodesWithAtLeastOneOutBoundLink(graph);

    for(MatrixIndex index = 0 ; index < numberOfNodes ; )
    {
	if(!m_atLeastOneInboundLink[index] && m_atLeastOneOutboundLink[index])
	{
	    std::cout << "removing orphan node " << graph.getNodeByIndex(index) << std::endl;
	    // removing an orphan might introduce other orphans
	    // (i.e. nodes pointed to only by the orphan just removed 
	    // so need to restart)
	    graph.removeVertex(index);
	    index = 0;
	    ++numberOfOrphans;
	    getNodesWithAtLeastOneInBoundLink(graph);
	    getNodesWithAtLeastOneOutBoundLink(graph);	
	}
	else
	{
	    ++index;
	}
    }

    std::cout << "Number of orphans removed: " << numberOfOrphans << std::endl << std::endl;
}
 
// Show rank leaks in a graph.
void PageRanker::dumpRankLeaks(const DirectedGraph& graph)
{
    std::cout << "#########################" << std::endl;
    std::cout << "Looking for rank leaks..." << std::endl;
    std::cout << "#########################" << std::endl << std::endl;

    MatrixIndex numberOfNodes = graph.getNodeCount();
    MatrixIndex numberOfRankLeaks = 0;
    bool* isNodeRankLeak = new bool[numberOfNodes];

    findLeakNodes(isNodeRankLeak, graph, numberOfNodes);

    for(MatrixIndex index = 0 ; index < numberOfNodes ; ++index)
    {
	if(isNodeRankLeak[index])
	{
	    std::cout << "Node " << graph.getNodeByIndex(index) << " is a rank leak " << std::endl;
	    ++numberOfRankLeaks;
	}
    }

    std::cout << "Number of rank leaks detected: " << numberOfRankLeaks << std::endl << std::endl;

    delete[] isNodeRankLeak;
}

// Remove leak nodes (nodes with no outbound links, but with at least 1 inbound
// link) from the graph
void PageRanker::removeLeakNodes(DirectedGraph& graph)
{
    std::cout << "###################" << std::endl;
    std::cout << "Removing rank leaks" << std::endl;
    std::cout << "###################" << std::endl;

    MatrixIndex numberOfNodes = graph.getNodeCount();
    MatrixIndex numberOfRankLeaks = 0;
    bool* isNodeRankLeak = new bool[numberOfNodes];

    findLeakNodes(isNodeRankLeak, graph, numberOfNodes);

    for(MatrixIndex index = 0 ; index < numberOfNodes ; ++index)
    {
	if(isNodeRankLeak[index])
	{
	    std::cout << "Removing rank leak node " << graph.getNodeByIndex(index) << std::endl;
	    graph.removeVertex(index); 
	    ++numberOfRankLeaks;
	}
    }

    std::cout << "Number of rank leak nodes removed: " << numberOfRankLeaks << std::endl << std::endl;

    delete[] isNodeRankLeak;
}
 
// Find leak nodes (nodes with no outbound links, but with at least 1 inbound
// link) in graph and store in bool array isNodeRankLeak
void PageRanker::findLeakNodes(bool* isNodeRankLeak, const DirectedGraph& graph, MatrixIndex numberOfNodes)
{
    // used to distinguish a leak from a node with neither 
    // outbound nor inbound links
    bool atLeastOneInboundLink = false;

    getOutBoundLinks(graph);
    getInBoundLinks(graph);

    // find which nodes are rank leaks
    for(MatrixIndex index = 0 ; index < numberOfNodes ; ++index)
    {
	if(m_outboundLinkCount[index] == 0 && m_inboundLinkCount[index])
	{
	    isNodeRankLeak[index] = true;
	}
	else
	{
	    isNodeRankLeak[index] = false;
	}
    } 
}

// populate bool array indicating if a node has at least one outbound link
// This is quicker than counting the total number of outbound links
void PageRanker::getNodesWithAtLeastOneOutBoundLink(const DirectedGraph& graph)
{
    MatrixIndex numberOfNodes = graph.getNodeCount();

    delete[] m_atLeastOneOutboundLink;

    m_atLeastOneOutboundLink = new bool[numberOfNodes];
    memset(m_atLeastOneOutboundLink, '\0', sizeof(bool)*numberOfNodes);

    for(MatrixIndex fromnode = 0 ; fromnode < numberOfNodes ; ++fromnode)
    {
        for(MatrixIndex tonode = 0 ; tonode < numberOfNodes ; ++tonode)
	{
	    if(graph.isEdge(fromnode,tonode))
	    {
		m_atLeastOneOutboundLink[fromnode] = true;
		break;
	    }
	}
    }
}

// populate bool array indicating if a node has at least one inbound link
// This is quicker than counting the total number of inbound links
void PageRanker::getNodesWithAtLeastOneInBoundLink(const DirectedGraph& graph)
{
    MatrixIndex numberOfNodes = graph.getNodeCount();

    delete[] m_atLeastOneInboundLink;

    m_atLeastOneInboundLink = new bool[numberOfNodes];
    memset(m_atLeastOneInboundLink, '\0', sizeof(bool)*numberOfNodes);

    for(MatrixIndex tonode = 0 ; tonode < numberOfNodes ; ++tonode)
    {
        for(MatrixIndex fromnode = 0 ; fromnode < numberOfNodes ; ++fromnode)
	{
	    if(graph.isEdge(fromnode,tonode))
	    {
		m_atLeastOneInboundLink[tonode] = true;
		break;
	    }
	}
    }
}

// populate array holding count of outbound links for each node in graph
void PageRanker::getOutBoundLinks(const DirectedGraph& graph)
{
    MatrixIndex numberOfNodes = graph.getNodeCount();

    delete[] m_outboundLinkCount;

    m_outboundLinkCount = new MatrixIndex[numberOfNodes];
    memset(m_outboundLinkCount, '\0', sizeof(MatrixIndex)*numberOfNodes);

    for(MatrixIndex fromnode = 0 ; fromnode < numberOfNodes ; ++fromnode)
    {
        for(MatrixIndex tonode = 0 ; tonode < numberOfNodes ; ++tonode)
	{
	    if(graph.isEdge(fromnode,tonode))
	    {
		++m_outboundLinkCount[fromnode];
	    }
	}
    }
}

// populate array holding count of inbound links for each node in graph
void PageRanker::getInBoundLinks(const DirectedGraph& graph)
{
    MatrixIndex numberOfNodes = graph.getNodeCount();

    delete[] m_inboundLinkCount;

    m_inboundLinkCount = new MatrixIndex[numberOfNodes];
    memset(m_inboundLinkCount, '\0', sizeof(MatrixIndex)*numberOfNodes);

    for(MatrixIndex tonode = 0 ; tonode < numberOfNodes ; ++tonode)
    {
        for(MatrixIndex fromnode = 0 ; fromnode < numberOfNodes ; ++fromnode)
	{
	    if(graph.isEdge(fromnode,tonode))
	    {
		++m_inboundLinkCount[tonode];
	    }
	}
    }
}

// Calculate the page rank of nodes in a graph. The provided decay factor and number of iterations
// are used (100 iterations is usually enough for convergence). Any nodes in the graph which have
// no inbound or outbounds links will be ignored (i.e edges removed from orphan nodes or rank leaks).
// Before calling this you may first want to call removeLeakNodes() and removeOrphanNodes() on the graph.
void PageRanker::rankGraphNodes(const DirectedGraph& graph, float decayfactor, uint32_t iterations)
{
    std::cout << "########################" << std::endl;
    std::cout << "Calculating page rank..." << std::endl;
    std::cout << "########################" << std::endl;

    MatrixIndex numberOfNodes = graph.getNodeCount();

    getOutBoundLinks(graph);
    getInBoundLinks(graph);

    // Count of how many nodes have no inbound or outbound links
    MatrixIndex isolatedNodeCount = 0;

    // Find nodes with no edges (this includes the nodes removed by removeLeakNodes() 
    // and removeOrphanNodes()). These nodes will be ignored during pagerank calculation.
    for(MatrixIndex index = 0 ; index < numberOfNodes ; ++index)
    {
        if(!m_outboundLinkCount[index] && !m_inboundLinkCount[index])
	{
	    ++isolatedNodeCount;
	    std::cout << "Isolated node " << graph.getNodeByIndex(index) << " will be ignored " << std::endl;
	}
    }
    std::cout << std::endl;

    delete[] m_pageRankVector;
    m_pageRankVector = new PageRank[numberOfNodes];
    PageRank* previousPageRankVector = new float[numberOfNodes];

    // initial page rank is evenly distributed
    PageRank initialrank = (float)1/ (numberOfNodes-isolatedNodeCount);

    for(MatrixIndex i = 0 ; i < numberOfNodes ; ++i)
    {
	if(!m_outboundLinkCount[i] && !m_inboundLinkCount[i])
	{
	    // isolated nodes have zero page rank
	    previousPageRankVector[i] = 0;
	}
	else
	{
            previousPageRankVector[i] = initialrank;
        }
    }

    memset(m_pageRankVector,'\0',sizeof(MatrixIndex)*numberOfNodes);

    // perform pagerank calculation
    for(uint32_t iteration = 0 ; iteration < iterations ; ++iteration)
    {
        for(MatrixIndex tonode = 0 ; tonode < numberOfNodes ; ++tonode)
	{
	    m_pageRankVector[tonode] = 0;

	    if(m_outboundLinkCount[tonode] || m_inboundLinkCount[tonode])
	    {
	        for(MatrixIndex fromnode = 0 ; fromnode < numberOfNodes ; ++fromnode)
	        {        
	            if(graph.isEdge(fromnode,tonode))
	 	    {
		        m_pageRankVector[tonode] += 1 * (PageRank)1/m_outboundLinkCount[fromnode] * previousPageRankVector[fromnode];
		    }
	        }

	        // apply the decay factor
	        m_pageRankVector[tonode] = (decayfactor * m_pageRankVector[tonode]) + (PageRank)( (1 - decayfactor) / (numberOfNodes-isolatedNodeCount) );
            }
	}
    
	PageRank* tmpPreviousPageRankVector = previousPageRankVector;
	previousPageRankVector = m_pageRankVector;
	m_pageRankVector = tmpPreviousPageRankVector;
    }

    std::cout << "Magnitude of difference between page rank vectors in final two iterations: ";
    std::cout << getDifferenceVectorMagnitude(m_pageRankVector, previousPageRankVector, numberOfNodes) << std::endl << std::endl;

    delete[] previousPageRankVector;
}

// Returns the magnitude of the difference of two vectors
float PageRanker::getDifferenceVectorMagnitude(PageRank* a, PageRank* b, MatrixIndex length)
{
    float magnitude = 0;

    for(MatrixIndex i = 0 ; i < length ; ++i)
    {
        magnitude += powf((a[i] - b[i]),2);
    }

    magnitude = sqrt(magnitude);

    return magnitude;
}

// to be used for printing page rank vector
void PageRanker::dumpPageRank(PageRank* array, MatrixIndex size)
{
    for(MatrixIndex i = 0 ; i < size ; ++i)
    {
        std::cout << array[i] << " " ;
    }

    std::cout << std::endl;
}

// print page rank vector with node labels provided by the graph object
void PageRanker::dumpPageRank(const DirectedGraph& graph)
{
    if(!m_pageRankVector)
    {
        std::cout << "Page rank has not yet been calculated" << std::endl;
    }

    MatrixIndex nodeCount = graph.getNodeCount();
    
    std::cout << "Node | PageRank" << std::endl;
    for(MatrixIndex i = 0 ; i < nodeCount ; ++i)
    {
	std::cout << graph.getNodeByIndex(i) << " " << m_pageRankVector[i] << std::endl;
    }
}
