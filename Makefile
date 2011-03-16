CXX = g++
CXXFLAGS += -W -Wall -Wno-write-strings -g
OBJDIR=obj/
SRCDIR=src/
BINDIR=bin/

OBJECTS=\
		$(OBJDIR)channel.o \
		$(OBJDIR)connection.o \
		$(OBJDIR)ini.o \
		$(OBJDIR)messages.o \
		$(OBJDIR)server.o \
		$(OBJDIR)servermain.o \
		$(OBJDIR)user.o

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(BINDIR)server

clean:
	rm -f $(OBJECTS)

$(OBJDIR)channel.o: src/channel.cpp
	$(CXX) $(CXXFLAGS) -c src/channel.cpp -o $@

$(OBJDIR)connection.o: src/server/connection.cpp
	$(CXX) $(CXXFLAGS) -c src/server/connection.cpp -o $@

$(OBJDIR)ini.o: src/3rdParty/inih/ini.c
	$(CXX) $(CXXFLAGS) -c src/3rdParty/inih/ini.c -o $@

$(OBJDIR)messages.o: src/server/messages.cpp
	$(CXX) $(CXXFLAGS) -c src/server/messages.cpp -o $@

$(OBJDIR)server.o: src/server/server.cpp
	$(CXX) $(CXXFLAGS) -c src/server/server.cpp -o $@

$(OBJDIR)servermain.o: src/server/main.cpp
	$(CXX) $(CXXFLAGS) -c src/server/main.cpp -o $@

$(OBJDIR)user.o: src/user.cpp
	$(CXX) $(CXXFLAGS) -c src/user.cpp -o $@
