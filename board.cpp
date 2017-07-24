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
#include <cmath>
#include "piecesquaretables.h"

#define ASSERT(b) if(!b) {std::cout << "Position not ok!\n"; Display(); abort();}

namespace Napoleon
{
    Board::Board()
    {
        MoveDatabase::InitAttacks();
        Zobrist::Init();

        pieces[PieceColor::White] = Constants::Empty;
        pieces[PieceColor::Black] = Constants::Empty;
        OccupiedSquares = Constants::Empty;
        EmptySquares = Constants::Empty;
        FirstMoveCutoff = 0;
        TotalCutoffs = 0;

        for (Color c = PieceColor::White; c < PieceColor::None; c++)
            for (File f = 0; f < 8; f++)
                pawnsOnFile[c][f] = 0;
    }

    void Board::LoadGame(std::string pos)
    {
        FenString fenString(pos);

        for (Color c = PieceColor::White; c < PieceColor::None; c++)
            for (Type t = PieceType::Pawn; t < PieceType::None; t++)
                numOfPieces[c][t] = 0;

        for (Color c = PieceColor::White; c < PieceColor::None; c++)
            for (File f = 0; f < 8; f++)
                pawnsOnFile[c][f] = 0;

        material[PieceColor::White] = 0;
        material[PieceColor::Black] = 0;
        allowNullMove = true;
        currentPly = 0;
        zobrist = 0;
        pawnKey = 0;
        initializeCastlingStatus(fenString);
        initializesideToMove(fenString);
        initializePieceSet(fenString);
        initializeEnPassantSquare(fenString);
        initializeBitBoards(fenString);

        pstValue[PieceColor::White] = calculatePST(PieceColor::White);
        pstValue[PieceColor::Black] = calculatePST(PieceColor::Black);

        ASSERT(this->PosIsOk());
    }

    Score Board::calculatePST(Color color) const
    {
        using namespace Constants::Squares;

        Piece piece;
        int pst[2][2] = {{0}};

        for (Napoleon::Square sq = IntA1; sq <= IntH8; sq++)
        {
            piece = PieceOnSquare(sq);
            if (piece.Type != PieceType::None)
            {
                Score scores = Evaluation::PieceSquareValue(piece, sq);
                pst[piece.Color][0] += scores.first;
                pst[piece.Color][1] += scores.second;
            }
        }

        return std::make_pair(pst[color][0], pst[color][1]);
    }

    void Board::AddPiece(Piece piece, Square sq)
    {
        pieceSet[sq] = piece;

        if (piece.Type != PieceType::None)
        {
            numOfPieces[piece.Color][piece.Type]++;
            material[piece.Color] += Constants::Piece::PieceValue[piece.Type];
            zobrist ^= Zobrist::Piece[piece.Color][piece.Type][sq];

            if (piece.Type == PieceType::Pawn)
            {
                pawnsOnFile[piece.Color][Utils::Square::GetFileIndex(sq)]++;
                pawnKey ^= Zobrist::Piece[piece.Color][PieceType::Pawn][sq];
            }
            //if (piece.Type == PieceType::King)
                //pawnKey ^= Zobrist::Piece[piece.Color][PieceType::King][sq];
        }
    }

    void Board::clearPieceSet()
    {
        for (Square i = 0; i < 64; i++)
        {
            pieceSet[i] = Constants::Piece::Null;
        }
    }

    void Board::updateGenericBitBoards()
    {
        pieces[PieceColor::White] =
                bitBoardSet[PieceColor::White][PieceType::Pawn] | bitBoardSet[PieceColor::White][PieceType::Knight]
                | bitBoardSet[PieceColor::White][PieceType::Bishop] | bitBoardSet[PieceColor::White][PieceType::Rook]
                | bitBoardSet[PieceColor::White][PieceType::Queen] | bitBoardSet[PieceColor::White][PieceType::King];

        pieces[PieceColor::Black] =
                bitBoardSet[PieceColor::Black][PieceType::Pawn] | bitBoardSet[PieceColor::Black][PieceType::Knight]
                | bitBoardSet[PieceColor::Black][PieceType::Bishop] | bitBoardSet[PieceColor::Black][PieceType::Rook]
                | bitBoardSet[PieceColor::Black][PieceType::Queen] | bitBoardSet[PieceColor::Black][PieceType::King];

        OccupiedSquares = pieces[PieceColor::White] | pieces[PieceColor::Black];
        EmptySquares = ~OccupiedSquares;
    }

    void Board::Display() const
    {
        Piece piece;

        for (int r = 7; r >= 0; r--)
        {
            std::cout << "   ------------------------\n";

            std::cout << " " << r + 1 << " ";

            for (int c = 0; c <= 7; c++)
            {
                piece = pieceSet[Utils::Square::GetSquareIndex(c, r)];
                std::cout << '[';
                if (piece.Type != PieceType::None)
                {
                    std::cout << (piece.Color == PieceColor::White ? Console::Green : Console::Red);

                    std::cout << Utils::Piece::GetInitial(pieceSet[Utils::Square::GetSquareIndex(c, r)].Type);
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
        std::cout << "Enpassant Square: " << Utils::Square::ToAlgebraic(enPassantSquare) << std::endl;
        std::cout << "Side To Move: " << (sideToMove == PieceColor::White ? "White" : "Black") << std::endl;
        std::cout << "Castling Rights: ";
        std::cout << (castlingStatus & Constants::Castle::WhiteCastleOO ? "K" : "");
        std::cout << (castlingStatus & Constants::Castle::WhiteCastleOOO ? "Q" : "");
        std::cout << (castlingStatus & Constants::Castle::BlackCastleOO ? "k" : "");
        std::cout << (castlingStatus & Constants::Castle::BlackCastleOOO ? "q" : "") << std::endl;
        std::cout << "HalfMove Clock: " << halfMoveClock << std::endl;
        std::cout << "Ply: " << currentPly << std::endl;
        std::cout << "Game Phase: " << Phase() << std::endl;
        //        std::cout << "pstValue[white]: " << pstValue[PieceColor::White] << "\t calculated: " << CalculatePST(PieceColor::White) << std::endl;
        //        std::cout << "pstValue[black]: " << pstValue[PieceColor::Black] << "\t calculated: " << CalculatePST(PieceColor::Black) << std::endl;
        //        std::cout << "GameStage: " << (Stage() == GameStage::EndGame ? "Endgame" : "Opening or MidGame") << std::endl;

        //for (Color c = PieceColor::White; c < PieceColor::None; c++)
        //{
        //    for (Type t = PieceType::Pawn; t < PieceType::None; t++)
        //    {
        //        if (numOfPieces[c][t] != Utils::BitBoard::PopCount(Pieces(c, t)))
        //            std::cout << int(t) << ": " << numOfPieces[c][t] << std::endl;
        //    }
        //}

        //        using namespace Constants::Squares;

        //        Piece piece2;
        //        int pst[2] = {0};

        //        for (Napoleon::Square sq = IntA1; sq <= IntH8; sq++)
        //        {
        //            piece2 = PieceOnSquare(sq);
        //            if (piece2.Type != PieceType::None && piece2.Color == SideToMove())
        //            {
        //                std::cout << "sq: " << Utils::Square::ToAlgebraic(sq) << "\t score: " << Evaluation::PieceSquareValue(piece2, sq, *this) << std::endl;
        //            }
        //        }

    }

    void Board::initializeCastlingStatus(const FenString& fenString)
    {
        castlingStatus = 0;

        if (fenString.CanWhiteShortCastle)
            castlingStatus |= Constants::Castle::WhiteCastleOO;

        if (fenString.CanWhiteLongCastle)
            castlingStatus |= Constants::Castle::WhiteCastleOOO;

        if (fenString.CanBlackShortCastle)
            castlingStatus |= Constants::Castle::BlackCastleOO;

        if (fenString.CanBlackLongCastle)
            castlingStatus |= Constants::Castle::BlackCastleOOO;

        zobrist ^= Zobrist::Castling[castlingStatus];
    }

    void Board::initializesideToMove(const FenString& fenString)
    {
        sideToMove = fenString.sideToMove;
        if (sideToMove == PieceColor::Black)
            zobrist ^= Zobrist::Color;
    }

    void Board::initializePieceSet(const FenString& fenString)
    {
        for (Square i = 0; i < 64; i++)
        {
            AddPiece(fenString.PiecePlacement[i], i);
        }
    }

    void Board::initializeEnPassantSquare(const FenString& fenString)
    {
        enPassantSquare = fenString.EnPassantSquare;
        if (enPassantSquare != Constants::Squares::Invalid)
        {
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
            pawnKey ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
        }
    }

    void Board::initializeHalfMoveClock(const FenString& fenString)
    {
        halfMoveClock = fenString.HalfMove;
    }

    void Board::initializeBitBoards(const FenString& fenString)
    {
        for (Type i = PieceType::Pawn; i < PieceType::None; i++)
            for (Color l = PieceColor::White; l < PieceColor::None; l++)
                bitBoardSet[l][i] = 0;

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

        if (to == enPassantSquare && pieceSet[from].Type == PieceType::Pawn)
            move = Move(from, to, EnPassant);

        else if (str == "e1g1")
            move = Constants::Castle::WhiteCastlingOO;

        else if (str == "e8g8")
            move = Constants::Castle::BlackCastlingOO;

        else if (str == "e1c1")
            move = Constants::Castle::WhiteCastlingOOO;

        else if (str == "e8c8")
            move = Constants::Castle::BlackCastlingOOO;

        else if (str.size() == 5)
            move = Move(from, to, 0x8 | (Utils::Piece::GetPiece(str[4]) - 1));

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
        Type captured = move.IsEnPassant() ? static_cast<Type>(PieceType::Pawn) : pieceSet[to].Type;
        Type pieceMoved = pieceSet[from].Type;
        Color enemy = Utils::Piece::GetOpposite(sideToMove);

        bool capture = captured != PieceType::None;

        castlingStatusHistory[currentPly] = castlingStatus;  // salva i diritti di arrocco correnti
        enpSquaresHistory[currentPly] = enPassantSquare; // salva l'attuale casella enpassant
        halfMoveClockHistory[currentPly] = halfMoveClock; // salva l'attuale contatore di semi-mosse
        hashHistory[currentPly] = zobrist; // salva l'hash key per verificare se la posizione si e` ripetuta
        capturedPieceHistory[currentPly] = captured; // salva il pezzo catturato

        zobrist ^= Zobrist::Color; // aggiorna il colore della posizione

        //ARRAY
        pieceSet[to] = pieceSet[from]; // muove il pezzo
        pieceSet[from] = Constants::Piece::Null; // svuota la casella di partenza

        updatePstvalue<Sub>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, pieceMoved), from));
        updatePstvalue<Add>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, pieceMoved), to));

        //BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[from];
        BitBoard To = Constants::Masks::SquareMask[to];
        BitBoard FromTo = From | To;

        // aggiorna la bitboard
        bitBoardSet[sideToMove][pieceMoved] ^= FromTo;
        zobrist ^= Zobrist::Piece[sideToMove][pieceMoved][from]; // aggiorna zobrist key (rimuove il pezzo mosso)
        zobrist ^= Zobrist::Piece[sideToMove][pieceMoved][to]; // aggiorna zobrist key (sposta il pezzo mosso)

        if (pieceMoved == PieceType::Pawn /*|| pieceMoved == PieceType::King*/)
        {
            pawnKey ^= Zobrist::Piece[sideToMove][pieceMoved][from]; 
            pawnKey ^= Zobrist::Piece[sideToMove][pieceMoved][to]; 
        }

        // aggiorna i pezzi del giocatore
        pieces[sideToMove] ^= FromTo;

        // se ilpezzo mosso e` il re si aggiorna la sua casella
        if (pieceMoved == PieceType::King)
        {
            kingSquare[sideToMove] = to;

            if (move.IsCastle())
            {
                makeCastle(from, to);
            }

            if (sideToMove == PieceColor::White)
                castlingStatus &= ~(Constants::Castle::WhiteCastleOO | Constants::Castle::WhiteCastleOOO); // azzera i diritti di arrocco per il bianco
            else
                castlingStatus &= ~(Constants::Castle::BlackCastleOO | Constants::Castle::BlackCastleOOO); // azzera i diritti di arrocco per il nero
        }
        else if (pieceMoved == PieceType::Rook) // se e` stata mossa una torre cambia i diritti di arrocco
        {
            if (castlingStatus) // se i giocatori possono ancora muovere
            {
                if (sideToMove == PieceColor::White)
                {
                    if (from == Constants::Squares::IntA1)
                        castlingStatus &= ~Constants::Castle::WhiteCastleOOO;
                    else if (from == Constants::Squares::IntH1)
                        castlingStatus &= ~Constants::Castle::WhiteCastleOO;
                }
                else
                {
                    if (from == Constants::Squares::IntA8)
                        castlingStatus &= ~Constants::Castle::BlackCastleOOO;
                    else if (from == Constants::Squares::IntH8)
                        castlingStatus &= ~Constants::Castle::BlackCastleOO;
                }
            }
        }
        else if (move.IsPromotion())
        {
            promoted = move.PiecePromoted();
            pieceSet[to] = Piece(sideToMove, promoted);
            bitBoardSet[sideToMove][PieceType::Pawn] ^= To;
            bitBoardSet[sideToMove][promoted] ^= To;
            numOfPieces[sideToMove][PieceType::Pawn]--;
            numOfPieces[sideToMove][promoted]++;

            material[sideToMove] -= Constants::Piece::PieceValue[PieceType::Pawn];
            material[sideToMove] += Constants::Piece::PieceValue[promoted];
            zobrist ^= Zobrist::Piece[sideToMove][PieceType::Pawn][to];
            zobrist ^= Zobrist::Piece[sideToMove][promoted][to];
            pawnKey ^= Zobrist::Piece[sideToMove][PieceType::Pawn][to];
            updatePstvalue<Sub>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, PieceType::Pawn), to));
            updatePstvalue<Add>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, promoted), to));

            if (!capture)
                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(from)]--;
            else
                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(to)]--;
        }
        if (capture)
        {
            if (move.IsEnPassant())
            {
                BitBoard piece;
                if (sideToMove == PieceColor::White)
                {
                    piece = Constants::Masks::SquareMask[enPassantSquare - 8];
                    pieceSet[enPassantSquare - 8] = Constants::Piece::Null;
                    updatePstvalue<Sub>(enemy, Evaluation::PieceSquareValue(Piece(enemy, PieceType::Pawn), enPassantSquare - 8));
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                    pawnKey ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                }
                else
                {
                    piece = Constants::Masks::SquareMask[enPassantSquare + 8];
                    pieceSet[enPassantSquare + 8] = Constants::Piece::Null;
                    updatePstvalue<Sub>(enemy, Evaluation::PieceSquareValue(Piece(enemy, PieceType::Pawn), enPassantSquare + 8));
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare + 8]; // rimuove il pedone bianco catturato en passant
                    pawnKey ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare + 8]; // rimuove il pedone bianco catturato en passant
                }

                pieces[enemy] ^= piece;
                bitBoardSet[enemy][PieceType::Pawn] ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;

                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(from)]--;
                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(to)]++;
                pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]--;
            }
            else
            {
                if (captured == PieceType::Rook)
                {
                    if (enemy == PieceColor::White)
                    {
                        if (to == Constants::Squares::IntH1)
                            castlingStatus &= ~Constants::Castle::WhiteCastleOO;
                        else if (to == Constants::Squares::IntA1)
                            castlingStatus &= ~Constants::Castle::WhiteCastleOOO;
                    }
                    else
                    {
                        if (to == Constants::Squares::IntH8)
                            castlingStatus &= ~Constants::Castle::BlackCastleOO;
                        else if (to == Constants::Squares::IntA8)
                            castlingStatus &= ~Constants::Castle::BlackCastleOOO;
                    }
                }
                else if (captured == PieceType::Pawn)
                {
                    pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]--;
                    pawnKey ^= Zobrist::Piece[enemy][PieceType::Pawn][to];
                }

                if (pieceMoved == PieceType::Pawn)
                {
                    pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(from)]--;
                    pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(to)]++;
                }

                updatePstvalue<Sub>(enemy, Evaluation::PieceSquareValue(Piece(enemy, captured), to));
                bitBoardSet[enemy][captured] ^= To;
                pieces[enemy] ^= To; //aggiorna i pezzi dell'avversario
                OccupiedSquares ^= From;
                EmptySquares ^= From;
                zobrist ^= Zobrist::Piece[enemy][captured][to]; // rimuove il pezzo catturato
            }

            numOfPieces[enemy][captured]--;
            material[enemy] -= Constants::Piece::PieceValue[captured];
            incrementClock = false; // non incrementare il contatore di semi-mosse perche` e` stato catturato un pezzo
        }
        else
        {
            OccupiedSquares ^= FromTo;
            EmptySquares ^= FromTo;
        }

        if (enPassantSquare != Constants::Squares::Invalid)
        {
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
            pawnKey ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
        }

        // azzera la casella enpassant
        enPassantSquare = Constants::Squares::Invalid;

        // se il pedone si muove di due caselle si aggiorna la casella enpassant
        if (pieceMoved == PieceType::Pawn)
        {
            incrementClock = false; // non incrementare il contatore di semi-mosse perche` e` stato mosso un pedone
            int sq = to - from; // calcola la distanza

            if (sq == 16 || sq == -16) // doppio spostamento del pedone
            {
                enPassantSquare = to - sq / 2;
                zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
                pawnKey ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
            }
        }

        if (castlingStatusHistory[currentPly] != castlingStatus)
            zobrist ^= Zobrist::Castling[castlingStatus]; // cambia i diritti di arrocco

        if (incrementClock) halfMoveClock++; // incrementa il contatore
        else halfMoveClock = 0; // resetta il contatore

        // cambia turno
        sideToMove = enemy;

        // aumenta profondita`
        currentPly++;

        ASSERT(PosIsOk());
    }

    void Board::UndoMove(Move move)
    {
        Square from = move.FromSquare();
        Square to = move.ToSquare();
        Color enemy = sideToMove;
        bool promotion = move.IsPromotion();
        bool capture;
        Type promoted;
        Type captured;
        Type pieceMoved;

        // decrementa profondita`
        currentPly--;

        captured = capturedPieceHistory[currentPly];
        capture = captured != PieceType::None;

        zobrist ^= Zobrist::Color; // aggiorna il colore della posizione

        if (castlingStatusHistory[currentPly] != castlingStatus)
            zobrist ^= Zobrist::Castling[castlingStatus]; // cambia i diritti di arrocco

        if (enPassantSquare != Constants::Squares::Invalid)
        {
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
            pawnKey ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enPassantSquare)];
        }

        if (enpSquaresHistory[currentPly] != Constants::Squares::Invalid)
        {
            zobrist ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquaresHistory[currentPly])];
            pawnKey ^= Zobrist::Enpassant[Utils::Square::GetFileIndex(enpSquaresHistory[currentPly])];
        }

        halfMoveClock = halfMoveClockHistory[currentPly];

        // se la mossa e` stata una promozione il pezzo mosso e` un pedone
        if (promotion)
            pieceMoved = PieceType::Pawn;
        else
            pieceMoved = pieceSet[to].Type;

        // reimposta il turno
        sideToMove = Utils::Piece::GetOpposite(sideToMove);

        // ARRAY
        pieceSet[from] = pieceSet[to]; // muove il pezzo

        if (!promotion)
        {
           updatePstvalue<Sub>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, pieceMoved), to));
           updatePstvalue<Add>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, pieceMoved), from));
        }

        // BITBOARDS
        BitBoard From = Constants::Masks::SquareMask[from];
        BitBoard To = Constants::Masks::SquareMask[to];
        BitBoard FromTo = From | To;

        // aggiorna la bitboard
        bitBoardSet[sideToMove][pieceMoved] ^= FromTo;
        zobrist ^= Zobrist::Piece[sideToMove][pieceMoved][from]; // aggiorna zobrist key (rimuove il pezzo mosso)
        zobrist ^= Zobrist::Piece[sideToMove][pieceMoved][to]; // aggiorna zobrist key (sposta il pezzo mosso)

        if (pieceMoved == PieceType::Pawn /*|| pieceMoved == PieceType::King*/)
        {
            pawnKey ^= Zobrist::Piece[sideToMove][pieceMoved][from]; 
            pawnKey ^= Zobrist::Piece[sideToMove][pieceMoved][to]; 
        }

        // aggiorna i pezzi del giocatore
        pieces[sideToMove] ^= FromTo;

        // se ilpezzo mosso e` il re si aggiorna la sua casella
        if (pieceMoved == PieceType::King)
        {
            kingSquare[sideToMove] = from;

            if (move.IsCastle())
            {
                undoCastle(from, to);
            }

            castlingStatus = castlingStatusHistory[currentPly]; // resetta i diritti di arrocco dello stato precedente
        }
        else if (pieceMoved == PieceType::Rook)
        {
            castlingStatus = castlingStatusHistory[currentPly];
        }
        else if (promotion)
        {
            promoted = move.PiecePromoted();
            numOfPieces[sideToMove][PieceType::Pawn]++;
            numOfPieces[sideToMove][promoted]--;

            material[sideToMove] += Constants::Piece::PieceValue[PieceType::Pawn];
            material[sideToMove] -= Constants::Piece::PieceValue[promoted];
            pieceSet[from] = Piece(sideToMove, PieceType::Pawn);
            bitBoardSet[sideToMove][promoted] ^= To;
            bitBoardSet[sideToMove][PieceType::Pawn] ^= To;
            zobrist ^= Zobrist::Piece[sideToMove][PieceType::Pawn][to];
            zobrist ^= Zobrist::Piece[sideToMove][promoted][to];
            pawnKey ^= Zobrist::Piece[sideToMove][PieceType::Pawn][to];
            updatePstvalue<Add>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, PieceType::Pawn), from));
            updatePstvalue<Sub>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, promoted), to));

            if (!capture)
                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(from)]++;
            else
                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(to)]++;
        }

        // reimposta la casella enpassant
        enPassantSquare = enpSquaresHistory[currentPly];

        if (capture)
        {
            if (move.IsEnPassant())
            {
                pieceSet[to] = Constants::Piece::Null; // svuota la casella di partenza perche` non c'erano pezzi prima
                BitBoard piece;

                if (sideToMove == PieceColor::White)
                {
                    piece = Constants::Masks::SquareMask[enPassantSquare - 8];
                    pieceSet[enPassantSquare - 8] = Piece(PieceColor::Black, PieceType::Pawn);
                    updatePstvalue<Add>(enemy, Evaluation::PieceSquareValue(Piece(enemy, PieceType::Pawn), enPassantSquare - 8));
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                    pawnKey ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare - 8]; // rimuove il pedone nero catturato en passant
                }
                else
                {
                    piece = Constants::Masks::SquareMask[enPassantSquare + 8];
                    pieceSet[enPassantSquare + 8] = Piece(PieceColor::White, PieceType::Pawn);
                    updatePstvalue<Add>(enemy, Evaluation::PieceSquareValue(Piece(enemy, PieceType::Pawn), enPassantSquare + 8));
                    zobrist ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare + 8]; // rimuove il pedone nero catturato en passant
                    pawnKey ^= Zobrist::Piece[enemy][PieceType::Pawn][enPassantSquare + 8]; // rimuove il pedone nero catturato en passant
                }

                pieces[enemy] ^= piece;
                bitBoardSet[enemy][PieceType::Pawn] ^= piece;
                OccupiedSquares ^= FromTo ^ piece;
                EmptySquares ^= FromTo ^ piece;

                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(from)]++;
                pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(to)]--;
                pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]++;
            }
            else
            {
                assert(captured != PieceType::None);
                assert(captured == capturedPieceHistory[currentPly]);

                if (captured == PieceType::Rook)
                {
                    castlingStatus = castlingStatusHistory[currentPly];
                }
                else if (captured == PieceType::Pawn)
                {
                    pawnsOnFile[enemy][Utils::Square::GetFileIndex(to)]++;
                    pawnKey ^= Zobrist::Piece[enemy][PieceType::Pawn][to];
                }
                if (pieceMoved == PieceType::Pawn)
                {
                    pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(from)]++;
                    pawnsOnFile[sideToMove][Utils::Square::GetFileIndex(to)]--;
                }

                updatePstvalue<Add>(enemy, Evaluation::PieceSquareValue(Piece(enemy, captured), to));

                // reinserisce il pezzo catturato nella sua casella
                pieceSet[to] = Piece(enemy, captured);
                bitBoardSet[enemy][captured] ^= To;

                pieces[enemy] ^= To; //aggiorna i pezzi dell'avversario
                OccupiedSquares ^= From;
                EmptySquares ^= From;

                zobrist ^= Zobrist::Piece[enemy][captured][to]; // rimuove il pezzo catturato
            }

            numOfPieces[enemy][captured]++;
            material[enemy] += Constants::Piece::PieceValue[captured];

        }
        else
        {
            // svuota la casella di partenza perche` non c'erano pezzi prima
            pieceSet[to] = Constants::Piece::Null;
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
            if (sideToMove == PieceColor::White)
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
            if (sideToMove == PieceColor::White)
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
        pieces[sideToMove] ^= rook;
        bitBoardSet[sideToMove][PieceType::Rook] ^= rook;
        OccupiedSquares ^= rook;
        EmptySquares ^= rook;
        pieceSet[fromR] = Constants::Piece::Null; // sposta la torre
        pieceSet[toR] = Piece(sideToMove, PieceType::Rook); // sposta la torre

        updatePstvalue<Sub>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, PieceType::Rook), fromR));
        updatePstvalue<Add>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, PieceType::Rook), toR));

        zobrist ^= Zobrist::Piece[sideToMove][PieceType::Rook][fromR];
        zobrist ^= Zobrist::Piece[sideToMove][PieceType::Rook][toR];
        castled[sideToMove] = true;
    }

    void Board::undoCastle(Square from, Square to)
    {
        BitBoard rook;
        Square fromR;
        Square toR;

        if (from < to) // Castle O-O
        {
            if (sideToMove == PieceColor::White)
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
            if (sideToMove == PieceColor::White)
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
        pieces[sideToMove] ^= rook;
        bitBoardSet[sideToMove][PieceType::Rook] ^= rook;
        OccupiedSquares ^= rook;
        EmptySquares ^= rook;
        pieceSet[fromR] = Piece(sideToMove, PieceType::Rook); // sposta la torre
        pieceSet[toR] = Constants::Piece::Null; // sposta la torre

        updatePstvalue<Add>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, PieceType::Rook), fromR));
        updatePstvalue<Sub>(sideToMove, Evaluation::PieceSquareValue(Piece(sideToMove, PieceType::Rook), toR));

        castlingStatus = castlingStatusHistory[currentPly]; // ripristina i diritti di arrocco dello stato precedente

        zobrist ^= Zobrist::Piece[sideToMove][PieceType::Rook][fromR];
        zobrist ^= Zobrist::Piece[sideToMove][PieceType::Rook][toR];
        castled[sideToMove] = false;
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

            if ((Pieces(enemyColor, PieceType::Pawn) & pawnAttacks) != 0) return true;
            if ((Pieces(enemyColor, PieceType::Knight) & MoveDatabase::KnightAttacks[to]) != 0) return true;
            if ((Pieces(enemyColor, PieceType::King) & MoveDatabase::KingAttacks[to]) != 0) return true;

            // file / rank attacks
            slidingAttackers = Pieces(enemyColor, PieceType::Queen) | Pieces(enemyColor, PieceType::Rook);

            if (slidingAttackers != 0)
            {
                if ((MoveDatabase::GetRookAttacks(OccupiedSquares, to) & slidingAttackers) != 0) return true;
            }

            // diagonals
            slidingAttackers = Pieces(enemyColor, PieceType::Queen) | Pieces(enemyColor, PieceType::Bishop);

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
        for (int r = 7; r >= 0; r--)
        {
            int empty = 0;
            for (int c = 0; c < 8; c++)
            {
                if (pieceSet[GetSquareIndex(c, r)].Type == PieceType::None)
                    empty++;

                else
                {
                    if (empty != 0)
                    {
                        fen += (char)empty + '0';
                        empty = 0;
                    }

                    fen += GetInitial(pieceSet[GetSquareIndex(c, r)]);
                }
            }
            if (empty != 0)
                fen += (char)empty + '0';

            if (r > 0)
                fen += '/';
        }

        fen += " ";

        // side to move
        if (sideToMove == PieceColor::White)
            fen += "w";
        else
            fen += "b";

        fen += " ";

        // castling rights
        if (castlingStatus)
        {
            fen += (castlingStatus & Constants::Castle::WhiteCastleOO ? "K" : "");
            fen += (castlingStatus & Constants::Castle::WhiteCastleOOO ? "Q" : "");
            fen += (castlingStatus & Constants::Castle::BlackCastleOO ? "k" : "");
            fen += (castlingStatus & Constants::Castle::BlackCastleOOO ? "q" : "");
        }
        else
            fen += '-';

        fen += " ";

        // en passant
        if (enPassantSquare != Constants::Squares::Invalid)
            fen += ToAlgebraic(enPassantSquare);
        else
            fen += '-';

        fen += " ";

        fen += "0 1";

        return fen;
    }

}
