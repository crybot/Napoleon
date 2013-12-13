QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread

CONFIG -= qt -Wl,--no-as-needed

SOURCES += \
    main.cpp \
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
    moveselector.cpp

HEADERS += \
    move.h \
    constants.h \
    utils.h \
    piece.h \
    stopwatch.h \
    compassrose.h \
    console.h \
    pawn.h \
    board.h \
    defines.h \
    movedatabase.h \
    knight.h \
    king.h \
    rook.h \
    bishop.h \
    queen.h \
    fenstring.h \
    movegenerator.h \
    benchmark.h \
    search.h \
    evaluation.h \
    transpositiontable.h \
    zobrist.h \
    hashentry.h \
    uci.h \
    searchinfo.h \
    moveselector.h
