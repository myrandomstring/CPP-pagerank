CPPFLAGS=-I/usr/include -I.
LDFLAGS=-L/usr/lib
LIBS=

TARGET = pagerank
SOURCES= linksfileparser.cc directedgraph.cc pageranker.cc pagerank.cc

OBJS=$(patsubst %.cc,%.o,$(SOURCES))

$(TARGET) : $(OBJS)
	g++ -g -Wall $(OBJS) $(CPPFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)

#use inference rule
%.o: %.cpp
	g++ -c -g -Wall $(CPPFLAGS) $(SOURCES)


.PHONEY: clean

clean:
	rm $(OBJS) $(TARGET)
