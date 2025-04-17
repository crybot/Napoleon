# --- Compiler and Linker ---
CXX = g++

# --- Target ---
TARGET = NapoleonPP

# --- Source Files ---
SRCS = main.cpp \
       move.cpp \
       utils.cpp \
       piece.cpp \
       stopwatch.cpp \
       pawn.cpp \
       board.cpp \
       movedatabase.cpp \
       knight.cpp \
       king.cpp \
       queen.cpp \
       fenstring.cpp \
       movegenerator.cpp \
       benchmark.cpp \
       search.cpp \
       evaluation.cpp \
       transpositiontable.cpp \
       zobrist.cpp \
       hashentry.cpp \
       uci.cpp \
       searchinfo.cpp \
       moveselector.cpp \
       pawntable.cpp \
			 csv.cpp

# --- Object Files (Derived from Sources) ---
OBJS = $(SRCS:.cpp=.o)

# --- Build Flags ---
# Optimization & Architecture Flags (Keep consistent for CXXFLAGS and LDFLAGS if using LTO)
OPTIMIZE = -flto -m64 -O3 -funroll-loops -march=native -mavx2

# Compiler Flags
# -std=c++17: C++ Standard
# -pthread: Enable thread support (compilation)
# -pipe: Use pipes for communication between compiler stages (faster)
# -mtune=native: Tune code for the build machine's CPU
# -Wall -W: Enable common warnings
# -I.: Include current directory (for local headers like "constants.h")
CXXFLAGS = $(OPTIMIZE) -pipe -std=c++17 -pthread -mtune=native -Wall -W -I.

# Linker Flags
# -flto: Enable Link-Time Optimization (must match CXXFLAGS)
# -pthread: Enable thread support (linking)
# -Wl,...: Pass options directly to the linker (ld)
#   --sort-common: Linker optimization
#   --as-needed: Link only necessary libraries
#   -z,relro: Security feature (Read-Only Relocations)
LDFLAGS = $(OPTIMIZE) -pthread -Wl,--sort-common,--as-needed,-z,relro

# Libraries to link (usually just -pthread is needed here if covered in LDFLAGS)
LIBS = # Add libraries like -lm if needed

# --- Utility Commands ---
DEL_FILE = rm -f

# --- Build Rules ---

# Phony targets are not files
.PHONY: all clean

# Default target: Build the executable
all: $(TARGET)

# Linking rule: Create the target executable from object files
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Implicit rule handles .cpp -> .o compilation using CXX and CXXFLAGS
# Example: make will automatically use:
# $(CXX) $(CXXFLAGS) -c file.cpp -o file.o

# Clean rule: Remove generated files
clean:
	$(DEL_FILE) $(OBJS) $(TARGET)

# Note: Add header file dependencies if you want object files to rebuild
# automatically when headers change. This adds complexity but improves correctness.
# Example for one file (can be automated):
# main.o: main.cpp uci.h encoder.h constants.h ...
#   $(CXX) $(CXXFLAGS) -c $< -o $@
