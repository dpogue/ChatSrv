CXX = g++
CXXFLAGS += -W -Wall -g
OBJDIR=obj/
SRCDIR=src/
BINDIR=bin/

OBJECTS=\
		$(OBJDIR)ini.o \
		$(OBJDIR)messages.o \
		$(OBJDIR)server.o \
		$(OBJDIR)user.o \
		$(OBJDIR)connection.o \
		$(OBJDIR)servermain.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BINDIR)server

clean:
	rm -f $(OBJECTS)

$(OBJDIR)ini.o: src/3rdParty/inih/ini.c
	$(CXX) $(CXXFLAGS) -c src/3rdParty/inih/ini.c -o $@

$(OBJDIR)messages.o: src/server/messages.cpp
	$(CXX) $(CXXFLAGS) -c src/server/messages.cpp -o $@

$(OBJDIR)server.o: src/server/server.cpp
	$(CXX) $(CXXFLAGS) -c src/server/server.cpp -o $@

$(OBJDIR)user.o: src/user.cpp
	$(CXX) $(CXXFLAGS) -c src/user.cpp -o $@

$(OBJDIR)connection.o: src/server/connection.cpp
	$(CXX) $(CXXFLAGS) -c src/server/connection.cpp -o $@

$(OBJDIR)servermain.o: src/server/main.cpp
	$(CXX) $(CXXFLAGS) -c src/server/main.cpp -o $@
