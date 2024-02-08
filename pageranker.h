/****************************************************************
This class finds page rank leaks, page rank sinks and calculates
the page rank of the nodes in a given directed graph. It can remove
orphan nodes (no inbound links) and nodes pointed to only by orphan
nodes. It can also remove nodes with no outgoing links (rank leaks)
although this is not done in a recursive fashion.

****************************************************************/

#ifndef PAGERANKER_H
#define PAGERANKER_H

#include "directedgraph.h"

// The number of iterations to use when looking
// for rank sinks
#define SINK_DETECT_ITERATIONS 100

// Threshold below which nodes are considered to have
// zero page rank. Used when detecting rank sinks
#define ZERO_PR_THRESHOLD 0.01

class PageRanker
{
    public:
      PageRanker();
      virtual ~PageRanker();
      // calculate page rank of nodes in graph
      void rankGraphNodes(const DirectedGraph& graph, float decayfactor, uint32_t iterations);
      // show rank leaks
      void dumpRankLeaks(const DirectedGraph& graph);
      // show rank sinks
      void dumpRankSinks(const DirectedGraph& graph);
      // show calculated page rank
      void dumpPageRank(const DirectedGraph& graph);
      // remove orphans from graph
      void removeOrphanNodes(DirectedGraph& graph);
      // remove rank leaks from graph
      void removeLeakNodes(DirectedGraph& graph);

   private:
      typedef float PageRank;
      
      // print out pagerank array
      void dumpPageRank(PageRank* array, MatrixIndex size);
      
      // find leak nodes in graph and store in bool array isNodeRankLeak
      void findLeakNodes(bool* isNodeRankLeak, const DirectedGraph& graph, MatrixIndex numberOfNodes);

      // populate array holding count of outbound links for each node in graph
      void getOutBoundLinks(const DirectedGraph& graph);

      // populate bool array indicating if a node has at least one outbound link
      void getNodesWithAtLeastOneOutBoundLink(const DirectedGraph& graph);

      // populate array holding count of inbound links for each node in graph
      void getInBoundLinks(const DirectedGraph& graph);

      // populate bool array indicating if a node has at least one inbound link
      void getNodesWithAtLeastOneInBoundLink(const DirectedGraph& graph);

      // Returns the magnitude of the difference of two vectors
      float getDifferenceVectorMagnitude(PageRank* a, PageRank* b, MatrixIndex length);
      // stores the last calculated pageranks for the nodes in the graph
      PageRank* m_pageRankVector;      
      // array holding the number of outbound links for given node
      MatrixIndex* m_outboundLinkCount;
      // array holding the number of inbound links for given node
      MatrixIndex* m_inboundLinkCount;
      
      // bool array for whether a node has at least one inbound link
      // (this array is quicker to calculate than the one holding 
      // the number of inbound links)
      bool* m_atLeastOneInboundLink;
      // bool array for whether a node has at least one outbound link
      bool* m_atLeastOneOutboundLink;
};

#endif
