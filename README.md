NapoleonPP
==========

Motore scacchistico cross-platform cross-compiler scritto in C++.

Il programma utilizza un generatore di mosse basato sulle bitboard.
Per saperne di piu` leggere il seguente articolo: http://sdrv.ms/14KHXQV

Features:
- Cross platform (Windows, Unix, Mac)
- Cross compiler (GCC, Visual C++, others)
- C++ source
- 32/64 bit architectures
- Magic bitboards move generator
- Pseudo legal move generator
- 16 bit move encoding
- Object-oriented code
- Enhanced readability
- Well commented
- Alpha beta search
- Principal variation search
- Iterative deepening (ID)
- Internal iterative deepening (IID)
- Zobrist hashing (used for repetition detection too)
- 4 buckets Transposition table
- Depth replacement scheme for transposition table
- Aspiration windows
- Quiescent search + delta pruning + MVV-LVA
- Killer moves heuristic
- History moves heuristic
- Enhanced selective move ordering
- Deep razoring
- Extended futility pruning
- Adaptive null move pruning
- Transposition table best-move probing
- MVV-LVA ordering
- Principal variation extraction from transposition table
- Uci protocol
