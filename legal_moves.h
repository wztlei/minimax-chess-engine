/// legal_moves.h
///
/// Willie Lei
/// Header file for legal_move.cpp

#ifndef LEGAL_MOVES_H_INCLUDED
#define LEGAL_MOVES_H_INCLUDED

#include <vector>
#include <string>

using namespace std;

typedef unsigned long long U64;

// Struct for a bitboard storing the locations of every piece
struct bitboard
{
    // 64-bit integers for the white pieces
    U64 wPawns;
    U64 wKnights;
    U64 wBishops;
    U64 wRooks;
    U64 wQueens;
    U64 wKings;

    // 64-bit integers for the black pieces
    U64 bPawns;
    U64 bKnights;
    U64 bBishops;
    U64 bRooks;
    U64 bQueens;
    U64 bKings;

    // Union 64 bit integers
    U64 wPieces;
    U64 bPieces;
    U64 pieces;
    U64 blank;

    // Store previous move for en passant
    int prevCurr = 0;
    int prevDest = 0;

    // Store where castling could occur
    bool wQueenSide = true;
    bool wKingSide = true;
    bool bQueenSide = true;
    bool bKingSide = true;

    // The material value of each side (starting value is 4060)
    // Pawns: 100, Knights: 310, Bishops: 320, Rooks: 500, Queens: 1000, Kings: No value/Undefined
    int wMaterialVal = 0;
    int bMaterialVal = 0;

    // Stores whether the previous move resulted in a change in material
    bool prevWasQuiet = true;

    // Function that updates the unions as declared earlier
    void updateUnions ();
};

// Structure for a ply
struct ply
{
    int curr = 0;
    int dest = 0;
};

// The rest of the typedefs
typedef vector <string> svec;
typedef vector <ply> plyvec;

// Arrays of the value of each square and a bitboards of where all the pieces can move to from each square
extern U64 sqrVal[64];
extern U64 wPawn1Dir[64], wPawn2Dir[64], wPawnCapDir[64], bPawn1Dir[64], bPawn2Dir[64], bPawnCapDir[64], knightDir[64], kingDir[64];
extern U64 rightDir[64], leftDir[64], upDir[64], downDir[64], deg45Dir[64], deg135Dir[64], deg225Dir[64], deg315Dir[64];

// Move checking functions
void twoPlayerGame ();
plyvec getLegalMoves (bitboard bBoard, bool whiteMove);
bool areLegalMoves (bitboard bBoard, bool whiteMove);
bool isLegalMove (bitboard bBoard, int curr, int dest);
bool isInCheck (bitboard bBoard, int square);
bitboard updateBitboard (bitboard oldBBoard, int curr, int dest, bool moveIsComp);
bitboard updateBBoardEnPass (bitboard oldBBoard, int curr, int dest);
bitboard updateBBoardCast (bitboard oldBBoard, int curr, int dest);
U64 getPawnMoves (bitboard bBoard, int square);
U64 getEnPassant (bitboard bBoard, int square);
U64 getKnightMoves (bitboard bBoard, int square);
U64 getBishopMoves (bitboard bBoard, int square);
U64 getRookMoves (bitboard bBoard, int square);
U64 getQueenMoves (bitboard bBoard, int square);
U64 getKingMoves (bitboard bBoard, int square);
U64 getCastlingMoves (bitboard bBoard, int square);
int getWKingLoc (bitboard bBoard);
int getBKingLoc (bitboard bBoard);

// Declare utility functions
void initBoard (svec &sBoard);
void displayBoard (svec board);
void displayU64 (U64 n);
void svecToBitboard (bitboard &bBoard, svec sBoard);
void bitBoardToSVec (bitboard bBoard, svec &sBoard);
void stringToSquare (string input, int &curr, int &dest);
void squareToMove (int curr, int dest);
bool isValidInput (string input);
bool isRightColour (bitboard board, int curr, int moveNum);
int absDiff (int a, int b);
string getPromotionPiece ();
void getDirections ();

#endif // LEGAL_MOVES_H_INCLUDED
