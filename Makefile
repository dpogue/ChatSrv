CXX = g++
CXXFLAGS += -W -Wall -g
OBJDIR=obj/
SRCDIR=src/
BINDIR=bin/

OBJECTS=\
		$(OBJDIR)messages.o \
		$(OBJDIR)server.o \
		$(OBJDIR)user.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BINDIR)server

$(OBJDIR)messages.o: src/server/messages.cpp
	$(CXX) $(CXXFLAGS) -c src/server/messages.cpp -o $@

$(OBJDIR)server.o: src/server/server.cpp
	$(CXX) $(CXXFLAGS) -c src/server/server.cpp -o $@

$(OBJDIR)user.o: src/user.cpp
	$(CXX) $(CXXFLAGS) -c src/user.cpp -o $@
