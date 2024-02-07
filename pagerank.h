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

#include <exception>
#include <string>

void parseArguments(int argc, char* argv[]);
void showUsage();

// Exception class for command line arg parsing
class InputArgumentException : public std::exception
{
    public:
	InputArgumentException():std::exception(){}
	InputArgumentException(const char* message):std::exception(),m_message(message){}
	virtual ~InputArgumentException() throw(){}
	virtual const char* what() const throw()
	{
	    return m_message.c_str();
	}

    private:
	std::string m_message;
};

