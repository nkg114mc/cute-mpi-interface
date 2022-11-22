# files

EXE = cute-mpi-interface

SRCDIR = src
PGNLIBDIR = src/pgnlib

OBJS = $(SRCDIR)/cute_mpi.o \
	$(SRCDIR)/main.o \
	$(SRCDIR)/open_split.o \
	$(SRCDIR)/util.o \
	$(PGNLIBDIR)/PGNCommentText.o \
	$(PGNLIBDIR)/PGNGameCollection.o \
	$(PGNLIBDIR)/PGNMoveList.o \
	$(PGNLIBDIR)/PGNPly.o \
	$(PGNLIBDIR)/PGNTag.o \
	$(PGNLIBDIR)/PGNException.o \
	$(PGNLIBDIR)/PGNGameResult.o \
	$(PGNLIBDIR)/PGNParser.o \
	$(PGNLIBDIR)/PGNPosition.o \
	$(PGNLIBDIR)/PGNTagList.o \
	$(PGNLIBDIR)/PGNGame.o \
	$(PGNLIBDIR)/PGNMove.o \
	$(PGNLIBDIR)/PGNPiece.o \
	$(PGNLIBDIR)/PGNSquare.o

# rules

all: $(EXE)

clean:
	$(RM) $(OBJS) .depend # keep exe

# general

CXX = mpic++

# warnings

CXXFLAGS += -ansi -pedantic -Wall -Wextra

# C++

CXXFLAGS += -std=c++11 -fno-rtti

# optimisation

CXXFLAGS += -O2 -mpopcnt -flto
LDFLAGS  += -O2 -flto

# dependencies

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $(OBJS)

.depend:
	$(CXX) $(CXXFLAGS) -MM $(OBJS:.o=.cpp) > $@

include .depend
