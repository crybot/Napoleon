#include "board.h"
#include "utils.h"
#include "console.h"
#include "movedatabase.h"
#include "movegenerator.h"
#include "piece.h"
#include "fenstring.h"
#include "transpositiontable.h"
#include "evaluation.h"
#include <iostream>
#include <cstring>

#define ASSERT(b) if(!b) {std::cout << "Position not ok!\n"; abort();}

namespace Napoleon
{
    Board::Board()
    {
        Table = TranspositionTable(67108864 * 2);
        MoveDatabase::InitAttacks();
        Zobrist::Init();

        Pieces[PieceColor::White] = Constants::Empty;
        Pieces[PieceColor::Black] = Constants::Empty;
        OccupiedSquares = Constants::Empty;
        EmptySquares = Constants::Empty;
        FirstMoveCutoff = 0;
        TotalCutoffs = 0;

        for (Color c = PieceColor::White; c < PieceColor::None; c++)
            for (File f = 0; f < 8; f++)
                pawnsOnFile[c][f] = 0;
    }

    void Board::AddPiece(Piece piece, Square sq)
    {
        PieceSet[sq] = piece;

        if (piece.Type != PieceType::None)
        {
            //            NumOfPieces[piece.Color][piece.Type]++;
            material[piece.Color] += Constants::Piece::PieceValue[piece.Type];
            pstValue[piece.Color] += Evaluation::CalculatePST(piece, sq, *this);
            zobrist ^= Zobrist::Piece[piece.Color][piece.Type][sq];

            if (piece.Type == PieceType::Pawn)
                pawnsOnFile[piece.Color][Utils::Square::GetFileIndex(sq)]++;
        }
    }

    void Board::clearPieceSet()
    {
        for (Square i=0; i<64; i++)
        {
            PieceSet[i] = Constants::Piece::Null;
        }
    }

    void Board::updateGenericBitBoards()
    {
        Pieces[PieceColor::White] =
                bitBoardSet[PieceColor::White][PieceType::Pawn] | bitBoardSet[PieceColor::White][PieceType::Knight]
                | bitBoardSet[PieceColor::White][PieceType::Bishop] | bitBoardSet[PieceColor::White][PieceType::Rook]
                | bitBoardSet[PieceColor::White][PieceType::Queen] | bitBoardSet[PieceColor::White][PieceType::King];

        Pieces[PieceColor::Black] =
                bitBoardSet[PieceColor::Black][PieceType::Pawn] | bitBoardSet[PieceColor::Black][PieceType::Knight]
                | bitBoardSet[PieceColor::Black][PieceType::Bishop] | bitBoardSet[PieceColor::Black][PieceType::Rook]
                | bitBoardSet[PieceColor::Black][PieceType::Queen] | bitBoardSet[PieceColor::Black][PieceType::King];

        OccupiedSquares = Pieces[PieceColor::White] |  Pieces[PieceColor::Black];
        EmptySquares = ~OccupiedSquares;
    }

    void Board::Display() const
    {
        Piece piece;

        for (int r = 7; r >= 0; r--)
        {
            std::cout << "   ------------------------\n";

            std::cout << " " << r+1 << " ";

            for (int c = 0; c <= 7; c++)
            {
                piece = PieceSet[Utils::Square::GetSquareIndex(c, r)];
                std::cout << '[';
                if (piece.Type != PieceType::None)
                {
                    std::cout << (piece.Color == PieceColor::White ? Console::Green : Console::Red);

                    std::cout << Utils::Piece::GetInitial(PieceSet[Utils::Square::GetSquareIndex(c, r)].Type);
                }
                else
                {
                    std::cout << Console::Red;
                    std::cout << ' ';
                }

                std::cout << Console::Reset;
                std::cout << ']';
            }
            std::cout << std::endl;
        }
        std::cout << "\n    A  B  C  D  E  F  G  H" << std::endl;

        std::cout << "FEN: " << GetFen() << std::endl;
        std::cout << "Enpassant Square: " << Utils::Square::ToAlgebraic(EnPassantSquare) << std::endl;
        std::cout << "Side To Move: " << (SideToMove == PieceColor::White ? "White" : "Black") << std::endl;
        std::cout << "Castling Rights: ";
        std::cout << (CastlingStatus & Constants::Castle::WhiteCastleOO ? "K" : "");
        std::cout << (CastlingStatus & Constants::Castle::WhiteCastleOOO ? "Q" : "");
        std::cout << (CastlingStatus & Constants::Castle::BlackCastleOO ? "k" : "");
        std::cout << (CastlingStatus & Constants::Castle::BlackCastleOOO ? "q" : "") << std::endl;
        std::cout << "HalfMove Clock: " << HalfMoveClock << std::endl;
        std::cout << "Ply: " << CurrentPly << std::endl;
    }

    void Board::LoadGame(std::string pos)
    {
        FenString fenString(pos);

        material[PieceColor::White] = 0;
        material[PieceColor::Black] = 0;
        pstValue[PieceColor::White] = 0;
        pstValue[PieceColor::Black] = 0;
        AllowNullMove = true;
        CurrentPly = 0;
        zobrist = 0;
        initializeCastlingStatus(fenString);
        initializeSideToMove(fenString);
        initializePieceSet(fenString);
        initializeEnPassantSquare(fenString);
        initializeBitBoards(fenString);
    }

    void Board::initializeCastlingStatus(const FenString& fenString)
    {
        CastlingStatus = 0;

        if (fenString.CanWhiteShortCastle)
            CastlingStatus |= Constants::Castle::WhiteCastleOO;

        if (fenString.CanWhiteLongCastle)
            CastlingStatus |= Constants::Castle::WhiteCastleOOO;

        if (fenString.CanBlackShortCastle)
            CastlingStatus |= Constants::Castle::BlackCastleOO;

        if (fenString.CanBlackLongCastle)
            CastlingStatus |= Constants::Castle::BlackCastleOOO;

        zobrist ^= Zobrist::Castling[CastlingStatus];
    }

    void Board::initializeSideToMove(const FenString& fenString)
    {
        SideToMove = fenString.SideToMove;
        if (SideToMove == PieceColor::Black)
            zobrist ^= Zobrist::Color;
    }

    void Board::initializePieceSet(const FenString& fenString)
    {
        for (Square i=0; i<64; i++)
        {
            AddPiece(fenString.PiecePlacement[i], i);
        }
    }

    void Board::initializeEnPassantSquare(const FenString& fenString)
    {
        EnPassantSquare = fenString.EnPassantSquare;
        if (EnPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(EnPassantSquare)];
    }

    void Board::initializeHalfMoveClock(const FenString& fenString)
    {
        HalfMoveClock = fenString.HalfMove;
    }

    void Board::initializeBitBoards(const FenString& fenString)
    {
        for (Type i=PieceType::Pawn; i<PieceType::None; i++)
        {
            for (Color l = PieceColor::White; l<PieceColor::None; l++)
            {
                bitBoardSet[l][i] = 0;
                //                NumOfPieces[l][i] = 0;
            }
        }

        for (Square i = 0; i < 64; i++)
        {
            if (fenString.PiecePlacement[i].Type == PieceType::King)
                kingSquare[fenString.PiecePlacement[i].Color] = i;
            if (fenString.PiecePlacement[i].Color != PieceColor::None)
                bitBoardSet[fenString.PiecePlacement[i].Color][fenString.PiecePlacement[i].Type] |= Constants::Masks::SquareMask[i];
        }

        updateGenericBitBoards();
    }

    Move Board::ParseMove(std::string str) const
    {
        Square from = Utils::Square::Parse(str.substr(0, 2));
        Square to = Utils::Square::Parse(str.substr(2));
        Move move;

        if (to == EnPassantSquare && PieceSet[from].Type == PieceType::Pawn)
            move =  Move(from, to, EnPassant);

        else if (str == "e1g1")
            move = Constants::Castle::WhiteCastlingOO;

        else if (str == "e8g8")
            move = Constants::Castle::BlackCastlingOO;

        else if (str == "e1c1")
            move = Constants::Castle::WhiteCastlingOOO;

        else if (str == "e8c8")
            move = Constants::Castle::BlackCastlingOOO;

        else if (str.size() == 5)
            move = Move(from, to, 0x8 | (Utils::Piece::GetPiece(str[4]) -1) );

        else
            move = Move(from, to);

        return move;
    }

    void Board::MakeMove(Move move)
    {
        bool incrementClock = true;

        Square from = move.FromSquare();
        Square to = move.ToSquare();
        Type promoted;
        Type captured = move.IsEnPassant() ? PieceType::Pawn : PieceSet[to].Type;
        Type pieceMoved = PieceSet[from].Type;
        Color enemy = Utils::Piece::GetOpposite(SideToMove);

        bool capture = captured != PieceType::None;

        castlingStatus[CurrentPly] = CastlingStatus;  // salva i diritti di arrocco correnti
        enpSquares[CurrentPly] = EnPassantSquare; // salva l'attuale casella enpassant
        halfMoveClock[CurrentPly] = HalfMoveClock; // salva l'attuale contatore di semi-mosse
        hash[CurrentPly] = zobrist; // debug
        capturedPiece[CurrentPly] = captured; // salva il pezzo catturato

        zobrist ^= Zobrist::Color; // aggiorna il colore della posizione

        //ARRAY
        PieceSet[to] = PieceSet[from]; // muove il pezzo
        PieceSet[from] = Constants::Piece::Null; // svuota la casella di partenza
        pstValue[SideToMove] -= Evaluation::CalculatePST(Piece(SideToMove, pieceMoved), from, *this);
        pstValue[SideToMove] += Evaluation::CalculatePST(Piece(SideToMove, pieceMoved), to, *this);

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[from];
        BitBoard To = Constants::Masks::SquareMask[to];
        BitBoard FromTo = From | To;

        // aggiorna la bitboard
        bitBoardSet[SideToMove][pieceMoved] ^= FromTo;
        zobrist ^= Zobrist::Piece[SideToMove][pieceMoved][from]; // aggiorna zobrist key (rimuove il pezzo mosso)
        zobrist ^= Zobrist::Piece[SideToMove][pieceMoved][to]; // aggiorna zobrist key (sposta il pezzo mosso)

        // aggiorna i pezzi del giocatore
        Pieces[SideToMove] ^= FromTo;

        // se ilpezzo mosso e` il re si aggiorna la sua casella
        if (pieceMoved == PieceType::King)
        {
            kingSquare[SideToMove] = to;

            if (move.IsCastle())
            {
                makeCastle(from, to);
            }

            if (SideToMove == PieceColor::White)
                CastlingStatus &= ~(Constants::Castle::WhiteCastleOO | Constants::Castle::WhiteCastleOOO); // azzera i diritti di arrocco per il bianco
            else
                CastlingStatus &= ~(Constants::Castle::BlackCastleOO | Constants::Castle::BlackCastleOOO); // azzera i diritti di arrocco per il nero
        }
        else if (pieceMoved == PieceType::Rook) // se e` stata mossa una torre cambia i diritti di arrocco
        {
            if (CastlingStatus) // se i giocatori possono ancora muovere
            {
                if (SideToMove == PieceColor::White)
                {
                    if (from == Constants::Squares::IntA1)
                        CastlingStatus &= ~Constants::Castle::WhiteCastleOOO;
                    else if (from == Constants::Squares::IntH1)
                        CastlingStatus &= ~Constants::Castle::WhiteCastleOO;
                }
                else
                {
                    if (from == Constants::Squares::IntA8)
                        CastlingStatus &= ~Constants::Castle::BlackCastleOOO;
                    else if (from == Constants::Squares::IntH8)
                        CastlingStatus &= ~Constants::Castle::BlackCastleOO;
                }
            }
        }
        else if (move.IsPromotion())
        {
            promoted = move.PiecePromoted();
            PieceSet[to] = Piece(SideToMove, promoted);
            bitBoardSet[SideToMove][PieceType::Pawn] ^= To;
            bitBoardSet[SideToMove][promoted] ^= To;
            //            NumOfPieces[SideToMove][PieceType::Pawn]--;
            //            NumOfPieces[SideToMove][promoted]++;

            pstValue[SideToMove] -= Evaluation::CalculatePST(Piece(SideToMove, PieceType::Pawn), to, *this);
            pstValue[SideToMove] += Evaluation::CalculatePST(Piece(SideToMove, promoted), to, *this);

            material[SideToMove] -= Constants::Piece::PieceValue[PieceType::Pawn];
            material[SideToMove] += Constants::Piece::PieceValue[promoted];
            zobrist ^= Zobrist::Piece[SideToMove][PieceType::Pawn][to];
            zobrist ^= Zobrist::Piece[SideToMove][promoted][to];

            if (!capture)
                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(from)]--;
            else
                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(to)]--;
        }
        if (capture)
        {
            if (move.IsEnPassant())
            {
                BitBoard piece;
                if (SideToMove == PieceColor::White)
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare - 8];
                    PieceSet[EnPassantSquare - 8] = Constants::Piece::Null;
                    pstValue[enemy] -= Evaluation::CalculatePST(Piece(enemy, PieceType::Pawn), EnPassantSquare - 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                }
                else
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare + 8];
                    PieceSet[EnPassantSquare + 8] = Constants::Piece::Null;
                    pstValue[enemy] -= Evaluation::CalculatePST(Piece(enemy, PieceType::Pawn), EnPassantSquare + 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare + 8]; // rimuove il pedone bianco catturato en passant
                }

                Pieces[enemy] ^= piece;
                bitBoardSet[enemy][PieceType::Pawn] ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;

                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(from)]--;
                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(to)]++;
                pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]--;
            }
            else
            {
                if (captured == PieceType::Rook)
                {
                    if (enemy == PieceColor::White)
                    {
                        if (to == Constants::Squares::IntH1)
                            CastlingStatus &= ~Constants::Castle::WhiteCastleOO;
                        else if (to == Constants::Squares::IntA1)
                            CastlingStatus &= ~Constants::Castle::WhiteCastleOOO;
                    }
                    else
                    {
                        if (to == Constants::Squares::IntH8)
                            CastlingStatus &= ~Constants::Castle::BlackCastleOO;
                        else if (to == Constants::Squares::IntA8)
                            CastlingStatus &= ~Constants::Castle::BlackCastleOOO;
                    }
                }
                else if (captured == PieceType::Pawn)
                {
                    pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]--;
                }

                if (pieceMoved == PieceType::Pawn)
                {
                    pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(from)]--;
                    pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(to)]++;
                }

                pstValue[enemy] -= Evaluation::CalculatePST(Piece(enemy, captured), to, *this);
                bitBoardSet[enemy][captured] ^= To;
                Pieces[enemy] ^= To; //aggiorna i pezzi dell'avversario
                OccupiedSquares ^= From;
                EmptySquares ^= From;
                zobrist ^= Zobrist::Piece[enemy][captured][to]; // rimuove il pezzo catturato
            }

            //            NumOfPieces[enemy][captured]--;
            material[enemy] -= Constants::Piece::PieceValue[captured];
            incrementClock = false; // non incrementare il contatore di semi-mosse perche` e` stato catturato un pezzo
        }
        else
        {
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }

        if (EnPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(EnPassantSquare)];

        // azzera la casella enpassant
        EnPassantSquare = Constants::Squares::Invalid;

        // se il pedone si muove di due caselle si aggiorna la casella enpassant
        if (pieceMoved == PieceType::Pawn)
        {
            incrementClock = false; // non incrementare il contatore di semi-mosse perche` e` stato mosso un pedone
            int sq = to - from; // calcola la distanza

            if (sq == 16 || sq == -16) // doppio spostamento del pedone
            {
                EnPassantSquare = to - sq/2;
                zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(EnPassantSquare)];
            }
        }

        if (castlingStatus[CurrentPly] != CastlingStatus)
            zobrist ^= Zobrist::Castling[CastlingStatus]; // cambia i diritti di arrocco

        if (incrementClock) HalfMoveClock++; // incrementa il contatore
        else HalfMoveClock = 0; // resetta il contatore

        // cambia turno
        SideToMove = enemy;

        // aumenta profondita`
        CurrentPly++;

        ASSERT(PosIsOk());
    }

    void Board::UndoMove(Move move)
    {
        Square from = move.FromSquare();
        Square to = move.ToSquare();
        Color enemy = SideToMove;
        bool promotion = move.IsPromotion();
        bool capture;
        Type promoted;
        Type captured;
        Type pieceMoved;

        // decrementa profondita`
        CurrentPly--;

        captured = capturedPiece[CurrentPly];

        capture = captured != PieceType::None;

        zobrist ^= Zobrist::Color; // aggiorna il colore della posizione

        if (castlingStatus[CurrentPly] != CastlingStatus)
            zobrist ^= Zobrist::Castling[CastlingStatus]; // cambia i diritti di arrocco

        if (EnPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(EnPassantSquare)];

        if (enpSquares[CurrentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquares[CurrentPly])];

        HalfMoveClock = halfMoveClock[CurrentPly];

        // se la mossa e` stata una promozione il pezzo mosso e` un pedone
        if (promotion)
            pieceMoved = PieceType::Pawn;
        else
            pieceMoved = PieceSet[to].Type;

        // reimposta il turno
        SideToMove = Utils::Piece::GetOpposite(SideToMove);

        // ARRAY
        PieceSet[from] = PieceSet[to]; // muove il pezzo

        if (!promotion)
        {
            pstValue[SideToMove] -= Evaluation::CalculatePST(Piece(SideToMove, pieceMoved), to, *this);
            pstValue[SideToMove] += Evaluation::CalculatePST(Piece(SideToMove, pieceMoved), from, *this);
        }

        // BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[from];
        BitBoard To = Constants::Masks::SquareMask[to];
        BitBoard FromTo = From | To;

        // aggiorna la bitboard
        bitBoardSet[SideToMove][pieceMoved] ^= FromTo;
        zobrist ^= Zobrist::Piece[SideToMove][pieceMoved][from]; // aggiorna zobrist key (rimuove il pezzo mosso)
        zobrist ^= Zobrist::Piece[SideToMove][pieceMoved][to]; // aggiorna zobrist key (sposta il pezzo mosso)

        // aggiorna i pezzi del giocatore
        Pieces[SideToMove] ^= FromTo;

        // se ilpezzo mosso e` il re si aggiorna la sua casella
        if (pieceMoved == PieceType::King)
        {
            kingSquare[SideToMove] = from;

            if (move.IsCastle())
            {
                undoCastle(from, to);
            }

            CastlingStatus = castlingStatus[CurrentPly]; // resetta i diritti di arrocco dello stato precedente
        }
        else if (pieceMoved == PieceType::Rook)
        {
            CastlingStatus = castlingStatus[CurrentPly];
        }
        else if (promotion)
        {
            promoted = move.PiecePromoted();
            //            NumOfPieces[SideToMove][PieceType::Pawn]++;
            //            NumOfPieces[SideToMove][promoted]--;

            pstValue[SideToMove] += Evaluation::CalculatePST(Piece(SideToMove, PieceType::Pawn), from, *this);
            pstValue[SideToMove] -= Evaluation::CalculatePST(Piece(SideToMove, promoted), to, *this);

            material[SideToMove] += Constants::Piece::PieceValue[PieceType::Pawn];
            material[SideToMove] -= Constants::Piece::PieceValue[promoted];
            PieceSet[from] = Piece(SideToMove, PieceType::Pawn);
            bitBoardSet[SideToMove][promoted] ^= To;
            bitBoardSet[SideToMove][PieceType::Pawn] ^= To;
            zobrist ^= Zobrist::Piece[SideToMove][PieceType::Pawn][to];
            zobrist ^= Zobrist::Piece[SideToMove][promoted][to];

            if (!capture)
                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(from)]++;
            else
                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(to)]++;
        }

        // reimposta la casella enpassant
        EnPassantSquare = enpSquares[CurrentPly];

        if (capture)
        {
            if (move.IsEnPassant())
            {
                PieceSet[to] = Constants::Piece::Null; // svuota la casella di partenza perche` non c'erano pezzi prima
                BitBoard piece;

                if (SideToMove == PieceColor::White)
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare - 8];
                    PieceSet[EnPassantSquare - 8] = Piece(PieceColor::Black, PieceType::Pawn);
                    pstValue[enemy] += Evaluation::CalculatePST(Piece(enemy, PieceType::Pawn), EnPassantSquare - 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                }
                else
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare + 8];
                    PieceSet[EnPassantSquare + 8] = Piece(PieceColor::White, PieceType::Pawn);
                    pstValue[enemy] += Evaluation::CalculatePST(Piece(enemy, PieceType::Pawn), EnPassantSquare + 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare + 8]; // rimuove il pedone nero catturato en passant
                }

                Pieces[enemy] ^= piece;
                bitBoardSet[enemy][PieceType::Pawn] ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;

                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(from)]++;
                pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(to)]--;
                pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]++;
            }
            else
            {
                assert(captured != PieceType::None);
                assert(captured == capturedPiece[CurrentPly]);

                if (captured == PieceType::Rook)
                {
                    CastlingStatus = castlingStatus[CurrentPly];
                }
                else if (captured == PieceType::Pawn)
                {
                    pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]++;
                }
                if (pieceMoved == PieceType::Pawn)
                {
                    pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(from)]++;
                    pawnsOnFile[SideToMove][Utils::Square::GetFileIndex(to)]--;
                }

                pstValue[enemy] += Evaluation::CalculatePST(Piece(enemy, captured), to, *this);

                // reinserisce il pezzo catturato nella sua casella
                PieceSet[to] = Piece(enemy, captured);
                bitBoardSet[enemy][captured] ^= To;

                Pieces[enemy] ^= To; //aggiorna i pezzi dell'avversario
                OccupiedSquares ^= From;
                EmptySquares ^= From;

                zobrist ^= Zobrist::Piece[enemy][captured][to]; // rimuove il pezzo catturato
            }

            //            NumOfPieces[enemy][captured]++;
            material[enemy] += Constants::Piece::PieceValue[captured];

        }
        else
        {
            // svuota la casella di partenza perche` non c'erano pezzi prima
            PieceSet[to] = Constants::Piece::Null;
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }

        ASSERT(PosIsOk());
    }

    void Board::makeCastle(Square from, Square to)
    {
        BitBoard rook;
        Square fromR;
        Square toR;

        if (from < to) // Castle O-O
        {
            if (SideToMove == PieceColor::White)
            {
                fromR = Constants::Squares::IntH1;
                toR = Constants::Squares::IntF1;
            }
            else
            {
                fromR = Constants::Squares::IntH8;
                toR = Constants::Squares::IntF8;
            }
        }
        else // Castle O-O-O
        {
            if (SideToMove == PieceColor::White)
            {
                fromR = Constants::Squares::IntA1;
                toR = Constants::Squares::IntD1;
            }
            else
            {
                fromR = Constants::Squares::IntA8;
                toR = Constants::Squares::IntD8;
            }
        }

        rook = Constants::Masks::SquareMask[fromR] | Constants::Masks::SquareMask[toR];
        Pieces[SideToMove] ^= rook;
        bitBoardSet[SideToMove][PieceType::Rook] ^= rook;
        OccupiedSquares ^= rook;
        EmptySquares ^= rook;
        PieceSet[fromR] = Constants::Piece::Null; // sposta la torre
        PieceSet[toR] = Piece(SideToMove, PieceType::Rook); // sposta la torre

        pstValue[SideToMove] -= Evaluation::CalculatePST(Piece(SideToMove, PieceType::Rook), fromR, *this);
        pstValue[SideToMove] += Evaluation::CalculatePST(Piece(SideToMove, PieceType::Rook), toR, *this);

        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][fromR];
        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][toR];
    }

    void Board::undoCastle(Square from, Square to)
    {
        BitBoard rook;
        Square fromR;
        Square toR;

        if (from < to) // Castle O-O
        {
            if (SideToMove == PieceColor::White)
            {
                fromR = Constants::Squares::IntH1;
                toR = Constants::Squares::IntF1;
            }
            else
            {
                fromR = Constants::Squares::IntH8;
                toR = Constants::Squares::IntF8;
            }
        }
        else // Castle O-O-O
        {
            if (SideToMove == PieceColor::White)
            {
                fromR = Constants::Squares::IntA1;
                toR = Constants::Squares::IntD1;
            }
            else
            {
                fromR = Constants::Squares::IntA8;
                toR = Constants::Squares::IntD8;
            }
        }

        rook = Constants::Masks::SquareMask[fromR] | Constants::Masks::SquareMask[toR];
        Pieces[SideToMove] ^= rook;
        bitBoardSet[SideToMove][PieceType::Rook] ^= rook;
        OccupiedSquares ^= rook;
        EmptySquares ^= rook;
        PieceSet[fromR] = Piece(SideToMove, PieceType::Rook); // sposta la torre
        PieceSet[toR] = Constants::Piece::Null; // sposta la torre

        pstValue[SideToMove] += Evaluation::CalculatePST(Piece(SideToMove, PieceType::Rook), fromR, *this);
        pstValue[SideToMove] -= Evaluation::CalculatePST(Piece(SideToMove, PieceType::Rook), toR, *this);

        CastlingStatus = castlingStatus[CurrentPly]; // ripristina i diritti di arrocco dello stato precedente

        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][fromR];
        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][toR];
    }

    bool Board::IsAttacked(BitBoard target, Color side) const
    {
        BitBoard slidingAttackers;
        BitBoard pawnAttacks;
        Byte enemyColor = Utils::Piece::GetOpposite(side);
        Square to;

        while (target != 0)
        {
            to = Utils::BitBoard::BitScanForwardReset(target);
            pawnAttacks = MoveDatabase::PawnAttacks[side][to];

            if ((GetPieces(enemyColor, PieceType::Pawn) & pawnAttacks) != 0) return true;
            if ((GetPieces(enemyColor, PieceType::Knight) & MoveDatabase::KnightAttacks[to]) != 0) return true;
            if ((GetPieces(enemyColor, PieceType::King) & MoveDatabase::KingAttacks[to]) != 0) return true;

            // file / rank attacks
            slidingAttackers = GetPieces(enemyColor, PieceType::Queen) | GetPieces(enemyColor, PieceType::Rook);

            if (slidingAttackers != 0)
            {
                if ((MoveDatabase::GetRankAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
                if ((MoveDatabase::GetFileAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
            }

            // diagonals
            slidingAttackers = GetPieces(enemyColor, PieceType::Queen) | GetPieces(enemyColor, PieceType::Bishop);

            if (slidingAttackers != 0)
            {
                if ((MoveDatabase::GetH1A8DiagonalAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
                if ((MoveDatabase::GetA1H8DiagonalAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
            }
        }
        return false;
    }


    std::string Board::GetFen() const
    {
        using namespace Utils::Square;
        using namespace Utils::Piece;
        std::string fen = "";

        // piece placement
        for (int r=7; r>=0; r--)
        {
            int empty = 0;
            for (int c=0; c<8; c++)
            {
                if (PieceSet[GetSquareIndex(c, r)].Type == PieceType::None)
                    empty++;

                else
                {
                    if (empty != 0)
                    {
                        fen += (char)empty + '0';
                        empty = 0;
                    }

                    fen += GetInitial(PieceSet[GetSquareIndex(c, r)]);
                }
            }
            if (empty != 0)
                fen += (char)empty + '0';

            if (r > 0)
                fen += '/';
        }

        fen += " ";

        // side to move

        if (SideToMove == PieceColor::White)
            fen += "w";
        else
            fen += "b";

        fen += " ";

        // castling rights

        if (CastlingStatus)
        {
            fen += (CastlingStatus & Constants::Castle::WhiteCastleOO ? "K" : "");
            fen += (CastlingStatus & Constants::Castle::WhiteCastleOOO ? "Q" : "");
            fen += (CastlingStatus & Constants::Castle::BlackCastleOO ? "k" : "");
            fen += (CastlingStatus & Constants::Castle::BlackCastleOOO ? "q" : "");
        }
        else
            fen += '-';

        fen += " ";

        // en passant

        if (EnPassantSquare != Constants::Squares::Invalid)
            fen += ToAlgebraic(EnPassantSquare);
        else
            fen += '-';

        fen += " ";

        fen += "0 1";

        return fen;
    }

}
