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

namespace Napoleon
{
    Board::Board()
    {
        Table = TranspositionTable(67108864);
        MoveDatabase::InitAttacks();
        Zobrist::Init();

        Pieces[PieceColor::White] = Constants::Empty;
        Pieces[PieceColor::Black] = Constants::Empty;
        OccupiedSquares = Constants::Empty;
        EmptySquares = Constants::Empty;
        CurrentPly = 0;
        Nps = 0;
        FirstMoveCutoff = 0;
        TotalCutoffs = 0;
        AllowNullMove = true;

    }

    void Board::Equip()
    {
        Material[PieceColor::White] = 0;
        Material[PieceColor::Black] = 0;
        PSTValue[PieceColor::White] = 0;
        PSTValue[PieceColor::Black] = 0;
        CurrentPly = 0;
        AllowNullMove = true;
        zobrist = 0;
        initializePieceSet();
        initializeCastlingStatus();
        initializeSideToMove();
        initializeEnPassantSquare();
        initializeBitBoards();
    }

    void Board::AddPiece(Piece piece, Square sq)
    {
        PieceSet[sq] = piece;

        if (piece.Type != PieceType::None)
        {
//            NumOfPieces[piece.Color][piece.Type]++;
            Material[piece.Color] += Constants::Piece::PieceValue[piece.Type];
            PSTValue[piece.Color] += Evaluation::EvaluatePiece(piece, sq, *this);
            zobrist ^= Zobrist::Piece[piece.Color][piece.Type][sq];
        }
    }

    void Board::initializePieceSet()
    {
        clearPieceSet();

        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 8);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 9);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 10);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 11);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 12);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 13);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 14);
        AddPiece(Piece(PieceColor::White, PieceType::Pawn), 15);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 48);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 49);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 50);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 51);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 52);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 53);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 54);
        AddPiece(Piece(PieceColor::Black, PieceType::Pawn), 55);

        /*TORRI*/
        AddPiece(Piece(PieceColor::White, PieceType::Rook), 0);
        AddPiece(Piece(PieceColor::White, PieceType::Rook), 7);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Rook), 56);
        AddPiece(Piece(PieceColor::Black, PieceType::Rook), 63);

        /*CAVALLI*/
        AddPiece(Piece(PieceColor::White, PieceType::Knight), 1);
        AddPiece(Piece(PieceColor::White, PieceType::Knight), 6);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Knight), 57);
        AddPiece(Piece(PieceColor::Black, PieceType::Knight), 62);

        /*ALFIERI*/
        AddPiece(Piece(PieceColor::White, PieceType::Bishop), 2);
        AddPiece(Piece(PieceColor::White, PieceType::Bishop), 5);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Bishop), 58);
        AddPiece(Piece(PieceColor::Black, PieceType::Bishop), 61);

        /*RE*/
        AddPiece(Piece(PieceColor::White, PieceType::King), 4);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::King), 60);

        /*REGINE*/
        AddPiece(Piece(PieceColor::White, PieceType::Queen), 3);
        //
        AddPiece(Piece(PieceColor::Black, PieceType::Queen), 59);
    }

    void Board::clearPieceSet()
    {
        for (int i=0; i<64; i++)
        {
            PieceSet[i] = Constants::Piece::Null;
        }
    }

    void Board::initializeCastlingStatus()
    {
        CastlingStatus = Constants::Castle::FullCastlingRights;
        zobrist ^= Zobrist::Castling[CastlingStatus];
    }

    void Board::initializeSideToMove()
    {
        SideToMove = PieceColor::White;
    }

    void Board::initializeEnPassantSquare()
    {
        EnPassantSquare = Constants::Squares::Invalid;
    }

    void Board::initializeHalfMoveClock()
    {
        HalfMoveClock = 0;
    }

    void Board::initializeBitBoards()
    {
        KingSquare[PieceColor::White] = 4;
        KingSquare[PieceColor::Black] = 60;

        bitBoardSet[PieceColor::White][PieceType::Pawn] = Constants::InitialPositions::WhitePawns;
        bitBoardSet[PieceColor::White][PieceType::Knight] = Constants::InitialPositions::WhiteKnights;
        bitBoardSet[PieceColor::White][PieceType::Bishop] = Constants::InitialPositions::WhiteBishops;
        bitBoardSet[PieceColor::White][PieceType::Rook] = Constants::InitialPositions::WhiteRooks;
        bitBoardSet[PieceColor::White][PieceType::Queen] = Constants::InitialPositions::WhiteQueen;
        bitBoardSet[PieceColor::White][PieceType::King] = Constants::InitialPositions::WhiteKing;

        bitBoardSet[PieceColor::Black][PieceType::Pawn] = Constants::InitialPositions::BlackPawns;
        bitBoardSet[PieceColor::Black][PieceType::Knight] = Constants::InitialPositions::BlackKnights;
        bitBoardSet[PieceColor::Black][PieceType::Bishop] = Constants::InitialPositions::BlackBishops;
        bitBoardSet[PieceColor::Black][PieceType::Rook] = Constants::InitialPositions::BlackRooks;
        bitBoardSet[PieceColor::Black][PieceType::Queen] = Constants::InitialPositions::BlackQueen;
        bitBoardSet[PieceColor::Black][PieceType::King] = Constants::InitialPositions::BlackKing;

        updateGenericBitBoards();
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

        std::cout << "Enpassant Square: " << Utils::Square::ToAlgebraic(EnPassantSquare) << std::endl;
        std::cout << "Side To Move: " << (SideToMove == PieceColor::White ? "White" : "Black") << std::endl;
        std::cout << "Castling Rights: ";
        std::cout << (CastlingStatus & Constants::Castle::WhiteCastleOO ? "K" : "");
        std::cout << (CastlingStatus & Constants::Castle::WhiteCastleOOO ? "Q" : "");
        std::cout << (CastlingStatus & Constants::Castle::BlackCastleOO ? "k" : "");
        std::cout << (CastlingStatus & Constants::Castle::BlackCastleOOO ? "q" : "") << std::endl;
        std::cout << "HalfMove Clock: " << HalfMoveClock << std::endl;
        std::cout << "Ply: " << CurrentPly << std::endl;
        std::cout << "PST Value: " << PSTValue[SideToMove] << std::endl;
    }

    void Board::LoadGame(const FenString& fenString)
    {
        Material[PieceColor::White] = 0;
        Material[PieceColor::Black] = 0;
        PSTValue[PieceColor::White] = 0;
        PSTValue[PieceColor::Black] = 0;
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
        for (int i=0; i<64; i++)
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
        for (int i=PieceType::Pawn; i<PieceType::None; i++)
        {
            for (int l = PieceColor::White; l<PieceColor::None; l++)
            {
                bitBoardSet[l][i] = 0;
//                NumOfPieces[l][i] = 0;
            }
        }

        for (int i = 0; i < 64; i++)
        {
            if (fenString.PiecePlacement[i].Type == PieceType::King)
                KingSquare[fenString.PiecePlacement[i].Color] = i;
            if (fenString.PiecePlacement[i].Color != PieceColor::None)
                bitBoardSet[fenString.PiecePlacement[i].Color][fenString.PiecePlacement[i].Type] |= Constants::Masks::SquareMask[i];
        }

        updateGenericBitBoards();
    }

    Move Board::ParseMove(std::string str) const
    {
        Byte from = Utils::Square::Parse(str.substr(0, 2));
        Byte to = Utils::Square::Parse(str.substr(2));
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
        {
            move = Move(from, to, 0x8 | (Utils::Piece::GetPiece(str[4]) -1) );
        }

        else
            move = Move(from, to);

        return move;
    }

    void Board::MakeMove(Move move)
    {
        bool incrementClock = true;

        int from = move.FromSquare();
        int to = move.ToSquare();
        Byte promoted;
        Byte captured = move.IsEnPassant() ? PieceType::Pawn : PieceSet[to].Type;

        castlingStatus[CurrentPly] = CastlingStatus;  // salva i diritti di arrocco correnti
        enpSquares[CurrentPly] = EnPassantSquare; // salva l'attuale casella enpassant
        halfMoveClock[CurrentPly] = HalfMoveClock; // salva l'attuale contatore di semi-mosse
        hash[CurrentPly] = zobrist;
        capturedPiece[CurrentPly] = captured;

        zobrist ^= Zobrist::Color; // aggiorna il colore della posizione

        Byte pieceMoved = PieceSet[from].Type;

        //ARRAY
        PieceSet[to] = PieceSet[from]; // muove il pezzo
        PieceSet[from] = Constants::Piece::Null; // svuota la casella di partenza
        PSTValue[SideToMove] -= Evaluation::EvaluatePiece(Piece(SideToMove, pieceMoved), from, *this);
        PSTValue[SideToMove] += Evaluation::EvaluatePiece(Piece(SideToMove, pieceMoved), to, *this);

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[from];
        BitBoard To = Constants::Masks::SquareMask[to];
        BitBoard FromTo = From | To;

        Byte enemy = Utils::Piece::GetOpposite(SideToMove);

        // aggiorna la bitboard
        bitBoardSet[SideToMove][pieceMoved] ^= FromTo;
        zobrist ^= Zobrist::Piece[SideToMove][pieceMoved][from]; // aggiorna zobrist key (rimuove il pezzo mosso)
        zobrist ^= Zobrist::Piece[SideToMove][pieceMoved][to]; // aggiorna zobrist key (sposta il pezzo mosso)

        // aggiorna i pezzi del giocatore
        Pieces[SideToMove] ^= FromTo;

        // se ilpezzo mosso e` il re si aggiorna la sua casella
        if (pieceMoved == PieceType::King)
        {
            KingSquare[SideToMove] = to;

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

            PSTValue[SideToMove] -= Evaluation::EvaluatePiece(Piece(SideToMove, PieceType::Pawn), to, *this);
            PSTValue[SideToMove] += Evaluation::EvaluatePiece(Piece(SideToMove, promoted), to, *this);

            Material[SideToMove] -= Constants::Piece::PieceValue[PieceType::Pawn];
            Material[SideToMove] += Constants::Piece::PieceValue[promoted];
            zobrist ^= Zobrist::Piece[SideToMove][PieceType::Pawn][to];
            zobrist ^= Zobrist::Piece[SideToMove][promoted][to];
        }

        if (captured != PieceType::None)
        {
            if (move.IsEnPassant())
            {
                BitBoard piece;
                if (SideToMove == PieceColor::White)
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare - 8];
                    PieceSet[EnPassantSquare - 8] = Constants::Piece::Null;
                    PSTValue[enemy] -= Evaluation::EvaluatePiece(Piece(enemy, PieceType::Pawn), EnPassantSquare - 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                }
                else
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare + 8];
                    PieceSet[EnPassantSquare + 8] = Constants::Piece::Null;
                    PSTValue[enemy] -= Evaluation::EvaluatePiece(Piece(enemy, PieceType::Pawn), EnPassantSquare + 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare + 8]; // rimuove il pedone bianco catturato en passant
                }

                Pieces[enemy] ^= piece;
                bitBoardSet[enemy][PieceType::Pawn] ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;
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

                PSTValue[enemy] -= Evaluation::EvaluatePiece(Piece(enemy, captured), to, *this);
                bitBoardSet[enemy][captured] ^= To;
                Pieces[enemy] ^= To; //aggiorna i pezzi dell'avversario
                OccupiedSquares ^= From;
                EmptySquares ^= From;
                zobrist ^= Zobrist::Piece[enemy][captured][to]; // rimuove il pezzo catturato
            }

//            NumOfPieces[enemy][captured]--;
            Material[enemy] -= Constants::Piece::PieceValue[captured];
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
    }

    void Board::UndoMove(Move move)
    {
        int from = move.FromSquare();
        int to = move.ToSquare();
        bool promotion = move.IsPromotion();
        Byte promoted;
        Byte captured;

        // decrementa profondita`
        CurrentPly--;

        captured = capturedPiece[CurrentPly];

        zobrist ^= Zobrist::Color; // aggiorna il colore della posizione

        if (castlingStatus[CurrentPly] != CastlingStatus)
            zobrist ^= Zobrist::Castling[CastlingStatus]; // cambia i diritti di arrocco

        if (EnPassantSquare != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(EnPassantSquare)];

        if (enpSquares[CurrentPly] != Constants::Squares::Invalid)
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquares[CurrentPly])];

        HalfMoveClock = halfMoveClock[CurrentPly];

        Byte pieceMoved;

        // se la mossa e` stata una promozione il pezzo mosso e` un pedone
        if (promotion)
            pieceMoved = PieceType::Pawn;
        else
            pieceMoved = PieceSet[to].Type;

        // reimposta il turno
        Byte enemy = SideToMove;
        SideToMove = Utils::Piece::GetOpposite(SideToMove);

        // ARRAY
        PieceSet[from] = PieceSet[to]; // muove il pezzo

        if (!promotion)
        {
            PSTValue[SideToMove] -= Evaluation::EvaluatePiece(Piece(SideToMove, pieceMoved), to, *this);
            PSTValue[SideToMove] += Evaluation::EvaluatePiece(Piece(SideToMove, pieceMoved), from, *this);
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
            KingSquare[SideToMove] = from;

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

            PSTValue[SideToMove] += Evaluation::EvaluatePiece(Piece(SideToMove, PieceType::Pawn), from, *this);
            PSTValue[SideToMove] -= Evaluation::EvaluatePiece(Piece(SideToMove, promoted), to, *this);

            Material[SideToMove] += Constants::Piece::PieceValue[PieceType::Pawn];
            Material[SideToMove] -= Constants::Piece::PieceValue[promoted];
            PieceSet[from] = Piece(SideToMove, PieceType::Pawn);
            bitBoardSet[SideToMove][promoted] ^= To;
            zobrist ^= Zobrist::Piece[SideToMove][PieceType::Pawn][to];
            zobrist ^= Zobrist::Piece[SideToMove][promoted][to];
        }

        // reimposta la casella enpassant
        EnPassantSquare = enpSquares[CurrentPly];

        if (captured != PieceType::None)
        {
            if (move.IsEnPassant())
            {
                PieceSet[to] = Constants::Piece::Null; // svuota la casella di partenza perche` non c'erano pezzi prima
                BitBoard piece;

                if (SideToMove == PieceColor::White)
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare - 8];
                    PieceSet[EnPassantSquare - 8] = Piece(PieceColor::Black, PieceType::Pawn);
                    PSTValue[enemy] += Evaluation::EvaluatePiece(Piece(enemy, PieceType::Pawn), EnPassantSquare - 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                }
                else
                {
                    piece = Constants::Masks::SquareMask[EnPassantSquare + 8];
                    PieceSet[EnPassantSquare + 8] = Piece(PieceColor::White, PieceType::Pawn);
                    PSTValue[enemy] += Evaluation::EvaluatePiece(Piece(enemy, PieceType::Pawn), EnPassantSquare + 8, *this);
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][EnPassantSquare + 8]; // rimuove il pedone nero catturato en passant
                }

                Pieces[enemy] ^= piece;
                bitBoardSet[enemy][PieceType::Pawn] ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;
            }
            else
            {
                assert(captured != PieceType::None);
                assert(captured == capturedPiece[CurrentPly]);

                if (captured == PieceType::Rook)
                {
                    CastlingStatus = castlingStatus[CurrentPly];
                }

                PSTValue[enemy] += Evaluation::EvaluatePiece(Piece(enemy, captured), to, *this);

                // reinserisce il pezzo catturato nella sua casella
                PieceSet[to] = Piece(enemy, captured);
                bitBoardSet[enemy][captured] ^= To;

                Pieces[enemy] ^= To; //aggiorna i pezzi dell'avversario
                OccupiedSquares ^= From;
                EmptySquares ^= From;

                zobrist ^= Zobrist::Piece[enemy][captured][to]; // rimuove il pezzo catturato
            }

//            NumOfPieces[enemy][captured]++;
            Material[enemy] += Constants::Piece::PieceValue[captured];

        }
        else
        {
            // svuota la casella di partenza perche` non c'erano pezzi prima
            PieceSet[to] = Constants::Piece::Null;
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }
    }

    void Board::makeCastle(int from, int to)
    {
        BitBoard rook;
        int fromR;
        int toR;

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

        PSTValue[SideToMove] -= Evaluation::EvaluatePiece(Piece(SideToMove, PieceType::Rook), fromR, *this);
        PSTValue[SideToMove] += Evaluation::EvaluatePiece(Piece(SideToMove, PieceType::Rook), toR, *this);

        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][fromR];
        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][toR];
    }

    void Board::undoCastle(int from, int to)
    {
        BitBoard rook;
        int fromR;
        int toR;

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

        PSTValue[SideToMove] += Evaluation::EvaluatePiece(Piece(SideToMove, PieceType::Rook), fromR, *this);
        PSTValue[SideToMove] -= Evaluation::EvaluatePiece(Piece(SideToMove, PieceType::Rook), toR, *this);

        CastlingStatus = castlingStatus[CurrentPly]; // ripristina i diritti di arrocco dello stato precedente

        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][fromR];
        zobrist ^= Zobrist::Piece[SideToMove][PieceType::Rook][toR];
    }

    bool Board::IsAttacked(BitBoard target, Byte side) const
    {
        BitBoard slidingAttackers;
        BitBoard pawnAttacks;
        Byte enemyColor = Utils::Piece::GetOpposite(side);
        int to;

        while (target != 0)
        {
            to = Utils::BitBoard::BitScanForwardReset(target);
            pawnAttacks = MoveDatabase::PawnAttacks[side][to];

            if ((GetPieceSet(enemyColor, PieceType::Pawn) & pawnAttacks) != 0) return true;
            if ((GetPieceSet(enemyColor, PieceType::Knight) & MoveDatabase::KnightAttacks[to]) != 0) return true;
            if ((GetPieceSet(enemyColor, PieceType::King) & MoveDatabase::KingAttacks[to]) != 0) return true;

            // file / rank attacks
            slidingAttackers = GetPieceSet(enemyColor, PieceType::Queen) | GetPieceSet(enemyColor, PieceType::Rook);

            if (slidingAttackers != 0)
            {
                if ((MoveDatabase::GetRankAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
                if ((MoveDatabase::GetFileAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
            }

            // diagonals
            slidingAttackers = GetPieceSet(enemyColor, PieceType::Queen) | GetPieceSet(enemyColor, PieceType::Bishop);

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
