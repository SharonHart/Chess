EXECUTABLE = chessprog
COMMON_OBJS =  ChessCommonUtils.o ChessFlowController.o 
COMMON_OBJS += ChessLogic.o GenericMinimaxAlgorithm.o 
COMMON_OBJS += ChessCLI.o ChessGUI.o ChessGUISettings.o ChessGUIGame.o 
COMMON_OBJS += SDLGraphicsFramework.o ChessSerializer.o libXmlAdapter.o
EXE_OBJS = $(COMMON_OBJS) chessprog.o
TEST_OBJS = $(COMMON_OBJS) unit_tests/ChessUTMain.o unit_tests/ChessLogicUT.o

DEPS = ChessCommonDefs.h ChessGenericUIInterface.h ChessCLI_Strings.h CommonUtils.h GenericGraphicsFramework.h ChessGUIResources.h ChessGUILayouts.h
INCLUDE_DIRS = /usr/include/libxml2/

CC = gcc
LIBS = -lm -lxml2
LFLAGS = $(LIBS) -std=c99 -pedantic-errors `sdl-config --libs`
CFLAGS = -std=c99 -pedantic-errors -c -Wall $(LIBS) `sdl-config --cflags` -I $(INCLUDE_DIRS) -D_MAKEFILE

all: CFLAGS += -DNDEBUG
all: $(EXECUTABLE)

$(EXECUTABLE): $(EXE_OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

chesstest: CFLAGS += -D_DEBUG -g 
chesstest: LFLAGS += -g
chesstest: $(TEST_OBJS)
	$(CC) -o $@ $^ $(LFLAGS)

debug: CFLAGS += -D_DEBUG -g
debug: LFLAGS += -g
debug: all

verbose: CFLAGS += -D_DEBUG -D_VERBOSE -g
verbose: LFLAGS += -g
verbose: all

%.o: %.c %.h $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
.PHONY: clean

clean:
	-rm *.o $(EXECUTABLE) core

