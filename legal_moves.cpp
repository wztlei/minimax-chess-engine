/// legal_moves.cpp
///
/// Willie Lei
/// Code that checks move legality.

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "legal_moves.h"

#define ISWHITEMOVE (moveNum%2 == 1)
#define ISBLACKMOVE (moveNum%2 == 0)

#define INITLINE8 "rnbqkbnr"
#define INITLINE7 "pppppppp"
#define INITLINE6 "        "
#define INITLINE5 "        "
#define INITLINE4 "        "
#define INITLINE3 "        "
#define INITLINE2 "PPPPPPPP"
#define INITLINE1 "RNBQKBNR"

using namespace std;

// Function to update unions of the bitboard
void bitboard::updateUnions ()
{
    wPieces = wPawns | wKnights | wBishops | wRooks | wQueens | wKings;
    bPieces = bPawns | bKnights | bBishops | bRooks | bQueens | bKings;
    pieces = wPieces | bPieces;
    blank = ~pieces;
}

// Arrays of the value of each square and a bitboards of where all the pieces can move to from each square
U64 sqrVal[64];
U64 wPawn1Dir[64], wPawn2Dir[64], wPawnCapDir[64], bPawn1Dir[64], bPawn2Dir[64], bPawnCapDir[64], knightDir[64], kingDir[64];
U64 rightDir[64], leftDir[64], upDir[64], downDir[64], deg45Dir[64], deg135Dir[64], deg225Dir[64], deg315Dir[64];

// Function to allow the user to play chess as a two player game (no AI)
void twoPlayerGame ()
{
    // Initialize the arrays, the bitboard, the vector, variables etc.
    getDirections();
    bitboard bBoard;
    svec sBoard;
    plyvec legalMoves;
    string input;
    int moveNum = 1, curr, dest;

    initBoard(sBoard);
    svecToBitboard(bBoard, sBoard);

    // Loop, getting moves from the user
    while (true)
    {
        // Display the updated board
        displayBoard(sBoard);

        // Get the next move from the user
        cout << "Enter in the desired move: ";
        cin >> input;

        // Clear screen
        system("CLS");

        // Allow user to exit game
        if (input == "0")
            break;

        // Make sure the input is valid
        if (!isValidInput(input))
        {
            cout << "Invalid input. Please try again." << endl;
            continue;
        }

        // Store the current square and the destination square
        stringToSquare(input, curr, dest);

        // Make sure the user is moving the piece of the right colour
        if (!isRightColour(bBoard, curr, moveNum))
        {
            cout << "Wrong colour." << endl;
            continue;
        }

        // Get all possible legal moves
        legalMoves = getLegalMoves(bBoard, ISWHITEMOVE);

        // Check to see if the move is a regular legal move for white
        if (isLegalMove(bBoard, curr, dest))
        {
            // Update boards and variables
            bBoard = updateBitboard(bBoard, curr, dest, false);
            bitBoardToSVec(bBoard, sBoard);
            moveNum++;
            cout << curr << " " << dest << endl;
            squareToMove(curr, dest);
        }
        // What to do if an illegal move is made
        else
        {
            bitboard bb2 = updateBitboard(bBoard, curr, dest, true);

            cout << "Illegal move. " << endl;

            if (ISWHITEMOVE && isInCheck(bb2, 63-log2(bb2.wKings)))
                cout << "White king is in check" << endl;
            if (ISBLACKMOVE && isInCheck(bb2, 63-log2(bb2.bKings)))
                cout << "Black king is in check" << endl;
        }
        // Check for checkmate or stalemate
        if (!areLegalMoves(bBoard, ISWHITEMOVE))
        {
            displayBoard(sBoard);
            if ((ISWHITEMOVE && isInCheck(bBoard, getWKingLoc(bBoard))))
                cout << "Checkmate! Black wins!" << endl;
            else if ((ISBLACKMOVE && isInCheck(bBoard, getBKingLoc(bBoard))))
                cout << "Checkmate! White wins!" << endl;
            else
                cout << "Draw from stalemate." << endl;
            break;
        }

    }

//    // For executable files, since they automatically close the window when the program finishes
//    cout << "Enter anything to close: ";
//    cin >> input;
}

// Function to return all the legal moves for a particular colour
plyvec getLegalMoves (bitboard bBoard, bool whiteMove)
{
    plyvec legalMoves;

    // Check if the move is for white or black
    if (whiteMove)
    {
        // Loop through all the possible source and destination squares
        for (int curr = 0; curr < 64; curr++)
        {
            U64 moves = 0;

            // Check to see what piece occupies that square
            if ((bBoard.blank | bBoard.bPieces) & sqrVal[curr])
                continue;
            else if (bBoard.wPawns & sqrVal[curr])
                moves = getPawnMoves(bBoard, curr) | getEnPassant(bBoard, curr);
            else if (bBoard.wKnights & sqrVal[curr])
                moves = getKnightMoves(bBoard, curr);
            else if (bBoard.wBishops & sqrVal[curr])
                moves = getBishopMoves(bBoard, curr);
            else if (bBoard.wRooks & sqrVal[curr])
                moves = getRookMoves(bBoard, curr);
            else if (bBoard.wQueens & sqrVal[curr])
                moves = getQueenMoves(bBoard, curr);
            else if (bBoard.wKings & sqrVal[curr])
                moves = getKingMoves(bBoard, curr) | getCastlingMoves(bBoard, curr);

            if (moves == 0)
                continue;

            // Go through all the destination squares to see if that move is legal
            for (int dest = 0; dest < 64; dest++)
            {
                // Check if the piece can indeed move to the dest. square
                if (moves & sqrVal[dest])
                {
                    bitboard bBoard2 = updateBitboard(bBoard, curr, dest, true);

                    // Check for checks
                    if (!isInCheck(bBoard2, getWKingLoc(bBoard2)))
                    {
                        ply p;
                        p.curr = curr;
                        p.dest = dest;
                        legalMoves.push_back(p);
                    }
                }
            }
        }
    }
    else
    {
        // Loop through all the possible source and destination squares
        for (int curr = 0; curr < 64; curr++)
        {
            U64 moves = 0;

            // Check to see what piece occupies that square
            if ((bBoard.blank | bBoard.wPieces) & sqrVal[curr])
                continue;
            else if (bBoard.bPawns & sqrVal[curr])
                moves = getPawnMoves(bBoard, curr) | getEnPassant(bBoard, curr);
            else if (bBoard.bKnights & sqrVal[curr])
                moves = getKnightMoves(bBoard, curr);
            else if (bBoard.bBishops & sqrVal[curr])
                moves = getBishopMoves(bBoard, curr);
            else if (bBoard.bRooks & sqrVal[curr])
                moves = getRookMoves(bBoard, curr);
            else if (bBoard.bQueens & sqrVal[curr])
                moves = getQueenMoves(bBoard, curr);
            else if (bBoard.bKings & sqrVal[curr])
                moves = getKingMoves(bBoard, curr) | getCastlingMoves(bBoard, curr);

            if (moves == 0)
                continue;

            // Go through all the destination squares to see if that move is legal
            for (int dest = 0; dest < 64; dest++)
            {
                // Check if the piece can indeed move to the dest. square
                if (moves & sqrVal[dest])
                {
                    bitboard bBoard2 = updateBitboard(bBoard, curr, dest, true);

                    // Check for checks
                    if (!isInCheck(bBoard2, getBKingLoc(bBoard2)))
                    {
                        ply p;
                        p.curr = curr;
                        p.dest = dest;
                        legalMoves.push_back(p);
                    }
                }
            }
        }
    }

    return legalMoves;
}

// Function to check the legality of a move
bool areLegalMoves (bitboard bBoard, bool whiteMove)
{
    // Check if the move is for white or black
    if (whiteMove)
    {
        // Loop through all the possible source and destination squares
        for (int curr = 0; curr < 64; curr++)
        {
            for (int dest = 0; dest < 64; dest++)
            {
                // If the move is legal, return true
                if ((bBoard.wPieces & sqrVal[curr]) && isLegalMove(bBoard, curr, dest))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        // Loop through all the possible source and destination squares
        for (int curr = 0; curr < 64; curr++)
        {
            for (int dest = 0; dest < 64; dest++)
            {
                // If the move is legal, return true
                if ((bBoard.bPieces & sqrVal[curr]) && isLegalMove(bBoard, curr, dest))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// Function to check if something is a legal move
bool isLegalMove (bitboard bBoard, int curr, int dest)
{
    // Check for an invalid move
    // Check if the square is blank
    if (bBoard.blank & sqrVal[curr])
        return false;
    // Check if the move is an invalid pawn move
    if (((bBoard.wPawns | bBoard.bPawns) & sqrVal[curr]) && !(getPawnMoves(bBoard, curr) & sqrVal[dest]) && !(getEnPassant(bBoard, curr) & sqrVal[dest]))
        return false;
    // Check if the move is an invalid knight move
    if (((bBoard.wKnights | bBoard.bKnights) & sqrVal[curr]) && !(getKnightMoves(bBoard, curr) & sqrVal[dest]))
        return false;
    // Check if the move is an invalid bishop move
    if (((bBoard.wBishops | bBoard.bBishops) & sqrVal[curr]) && !(getBishopMoves(bBoard, curr) & sqrVal[dest]))
        return false;
    // Check if the move is an invalid rook move
    if (((bBoard.wRooks | bBoard.bRooks) & sqrVal[curr]) && !(getRookMoves(bBoard, curr) & sqrVal[dest]))
        return false;
    // Check if the move is an invalid queen move
    if (((bBoard.wQueens | bBoard.bQueens) & sqrVal[curr]) && !(getQueenMoves(bBoard, curr) & sqrVal[dest]))
        return false;
    // Check if the move is an invalid king move
    if (((bBoard.wKings | bBoard.bKings) & sqrVal[curr]) && !(getKingMoves(bBoard, curr) & sqrVal[dest]) && !(getCastlingMoves(bBoard, curr) & sqrVal[dest]))
        return false;

    // Check to see if the white king is in check if white is moving
    if (bBoard.wPieces & sqrVal[curr])
    {
        bitboard bBoard2 = updateBitboard(bBoard, curr, dest, true);
        return !isInCheck (bBoard2, getWKingLoc(bBoard2));
    }
    // Check to see if the black king is in check if black is moving
    else
    {
        bitboard bBoard2 = updateBitboard(bBoard, curr, dest, true);
        return !isInCheck (bBoard2, getBKingLoc(bBoard2));
    }
}

// Function to determine if a square is under attack by a piece of the opposite colour
bool isInCheck (bitboard bBoard, int square)
{
    // For a white king
    if (bBoard.wPieces & sqrVal[square])
    {
        // Pretend the king is a piece and see if it can capture an identical piece of the opposite colour
        if (getPawnMoves(bBoard, square) & bBoard.bPawns)
            return true;
        if (getKnightMoves(bBoard, square) & bBoard.bKnights)
            return true;
        if (getBishopMoves(bBoard, square) & bBoard.bBishops)
            return true;
        if (getRookMoves(bBoard, square) & bBoard.bRooks)
            return true;
        if ((getQueenMoves(bBoard, square)) & bBoard.bQueens)
            return true;
        if (getKingMoves(bBoard, square) & bBoard.bKings)
            return true;
    }
    // For a black king
    else
    {
        // Pretend the king is a piece and see if it can capture an identical piece of the opposite colour
        if (getPawnMoves(bBoard, square) & bBoard.wPawns)
            return true;
        if (getKnightMoves(bBoard, square) & bBoard.wKnights)
            return true;
        if (getBishopMoves(bBoard, square) & bBoard.wBishops)
            return true;
        if (getRookMoves(bBoard, square) & bBoard.wRooks)
            return true;
        if ((getQueenMoves(bBoard, square)) & bBoard.wQueens)
            return true;
        if (getKingMoves(bBoard, square) & bBoard.wKings)
            return true;
    }

    return false;
}

// Function to update a bitboard after a regular move
bitboard updateBitboard (bitboard oldBBoard, int curr, int dest, bool moveIsComp)
{
    bitboard bBoard = oldBBoard;

    // Update the bitboard after en passant
    if (getEnPassant(bBoard, curr) & sqrVal[dest])
    {
        bBoard.prevWasQuiet = false;

        if (bBoard.wPieces & sqrVal[curr])
            bBoard.bMaterialVal -= 100;
        else
            bBoard.wMaterialVal -= 100;

        return updateBBoardEnPass(bBoard, curr, dest);;
    }
    // Update the bitboard after castling
    if (getCastlingMoves(bBoard, curr) & sqrVal[dest])
    {
        bBoard.prevWasQuiet = true;
        return updateBBoardCast(bBoard, curr, dest);
    }

    // Record that the previous move was quiet and only change that if there was a capture or a promotion
    bBoard.prevWasQuiet = true;

    // Remove a piece after a capture
    if (bBoard.pieces & sqrVal[dest])
    {
        bBoard.prevWasQuiet = false;

        // Check what piece is captured
        // If a white piece is captured ...
        if (bBoard.wPawns & sqrVal[dest])
        {
            bBoard.wPawns -= sqrVal[dest];
            bBoard.wMaterialVal -= 100;
        }
        else if (bBoard.wKnights & sqrVal[dest])
        {
            bBoard.wKnights -= sqrVal[dest];
            bBoard.wMaterialVal -= 310;
        }
        else if (bBoard.wBishops & sqrVal[dest])
        {
            bBoard.wBishops -= sqrVal[dest];
            bBoard.wMaterialVal -= 320;
        }
        else if (bBoard.wRooks & sqrVal[dest])
        {
            bBoard.wRooks -= sqrVal[dest];
            bBoard.wMaterialVal -= 500;
        }
        else if (bBoard.wQueens & sqrVal[dest])
        {
            bBoard.wQueens -= sqrVal[dest];
            bBoard.wMaterialVal -= 1000;
        }
        // If a black piece is captured ...
        else if (bBoard.bPawns & sqrVal[dest])
        {
            bBoard.bPawns -= sqrVal[dest];
            bBoard.bMaterialVal -= 100;
        }
        else if (bBoard.bKnights & sqrVal[dest])
        {
            bBoard.bKnights -= sqrVal[dest];
            bBoard.bMaterialVal -= 310;
        }
        else if (bBoard.bBishops & sqrVal[dest])
        {
            bBoard.bBishops -= sqrVal[dest];
            bBoard.bMaterialVal -= 320;
        }
        else if (bBoard.bRooks & sqrVal[dest])
        {
            bBoard.bRooks -= sqrVal[dest];
            bBoard.bMaterialVal -= 500;
        }
        else if (bBoard.bQueens & sqrVal[dest])
        {
            bBoard.bQueens -= sqrVal[dest];
            bBoard.bMaterialVal -= 1000;
        }
    }

    // Check what piece was moved
    // For white pieces
    // White pawns including promotion
    if (bBoard.wPawns & sqrVal[curr])
    {
        bBoard.wPawns -= sqrVal[curr];

        // Check if the pawn promoted
        if (dest/8 == 0)
        {
            bBoard.prevWasQuiet = false;

            // If the move isn't by a computer, ask the user what piece the pawn should be promoted to
            if (!moveIsComp)
            {
                string piece = getPromotionPiece();

                // Check which piece the user wants the pawn to be promoted to and update material values
                switch (piece[0])
                {
                    case 'n':
                        bBoard.wKnights += sqrVal[dest];
                        bBoard.wMaterialVal += 310;
                        break;
                    case 'b':
                        bBoard.wBishops += sqrVal[dest];
                        bBoard.wMaterialVal += 320;
                        break;
                    case 'r':
                        bBoard.wRooks += sqrVal[dest];
                        bBoard.wMaterialVal += 500;
                        break;
                    case 'q':
                        bBoard.wQueens += sqrVal[dest];
                        bBoard.wMaterialVal += 1000;
                        break;
                }
            }
            // Default is to promote to a queen
            else
            {
                bBoard.wQueens += sqrVal[dest];
                bBoard.wMaterialVal += 1000;
            }
        }
        // For a regular pawn move
        else
        {
             bBoard.wPawns += sqrVal[dest];
        }
    }
    // White knights
    else if (bBoard.wKnights & sqrVal[curr])
    {
        bBoard.wKnights -= sqrVal[curr];
        bBoard.wKnights += sqrVal[dest];
    }
    // White bishops
    else if (bBoard.wBishops & sqrVal[curr])
    {
        bBoard.wBishops -= sqrVal[curr];
        bBoard.wBishops += sqrVal[dest];
    }
    // White rooks
    else if (bBoard.wRooks & sqrVal[curr])
    {
        bBoard.wRooks -= sqrVal[curr];
        bBoard.wRooks += sqrVal[dest];

        // Check to see if any rooks have moved for castling
        if (!(bBoard.wRooks & 128))
            bBoard.wQueenSide = false;
        if (!(bBoard.wRooks & 1))
            bBoard.wKingSide = false;
    }
    // White queens
    else if (bBoard.wQueens & sqrVal[curr])
    {
        bBoard.wQueens -= sqrVal[curr];
        bBoard.wQueens += sqrVal[dest];
    }
    // White kings
    else if (bBoard.wKings & sqrVal[curr])
    {
        bBoard.wKings -= sqrVal[curr];
        bBoard.wKings += sqrVal[dest];
        bBoard.wQueenSide = bBoard.wKingSide = false;
    }
    // For black pieces
    // For black pawns
    else if (bBoard.bPawns & sqrVal[curr])
    {
        bBoard.bPawns -= sqrVal[curr];

        // Check if the pawn promoted
        if (dest/8 == 7)
        {
            bBoard.prevWasQuiet = false;

            // If the move isn't by a computer, ask the user what piece the pawn should be promoted to
            if (!moveIsComp)
            {
                string piece = getPromotionPiece();

                // Check which piece the user wants the pawn to be promoted to and update material values
                switch (piece[0])
                {
                    case 'n':
                        bBoard.bKnights += sqrVal[dest];
                        bBoard.bMaterialVal += 310;
                        break;
                    case 'b':
                        bBoard.bBishops += sqrVal[dest];
                        bBoard.bMaterialVal += 320;
                        break;
                    case 'r':
                        bBoard.bRooks += sqrVal[dest];
                        bBoard.bMaterialVal += 500;
                        break;
                    case 'q':
                        bBoard.bQueens += sqrVal[dest];
                        bBoard.bMaterialVal += 1000;
                        break;
                }
            }
            // Default is to promote to a queen
            else
            {
                bBoard.bQueens += sqrVal[dest];
                bBoard.bMaterialVal += 1000;
            }
        }
        // For a regular pawn move
        else
        {
            bBoard.bPawns += sqrVal[dest];
        }
    }
    // For black knights
    else if (bBoard.bKnights & sqrVal[curr])
    {
        bBoard.bKnights -= sqrVal[curr];
        bBoard.bKnights += sqrVal[dest];
    }
    // For black bishops
    else if (bBoard.bBishops & sqrVal[curr])
    {
        bBoard.bBishops -= sqrVal[curr];
        bBoard.bBishops += sqrVal[dest];
    }
    // For black rooks
    else if (bBoard.bRooks & sqrVal[curr])
    {
        bBoard.bRooks -= sqrVal[curr];
        bBoard.bRooks += sqrVal[dest];

        if (!(bBoard.bRooks & 9223372036854775808U))
            bBoard.bQueenSide = false;
        if (!(bBoard.bRooks & 72057594037927936))
            bBoard.bKingSide = false;
    }
    // For black queens
    else if (bBoard.bQueens & sqrVal[curr])
    {
        bBoard.bQueens -= sqrVal[curr];
        bBoard.bQueens += sqrVal[dest];
    }
    // For black kings
    else if (bBoard.bKings & sqrVal[curr])
    {
        bBoard.bKings -= sqrVal[curr];
        bBoard.bKings += sqrVal[dest];
        bBoard.bQueenSide = bBoard.bKingSide = false;
    }

    // Update moves and the union sets
    bBoard.prevCurr = curr;
    bBoard.prevDest = dest;
    bBoard.updateUnions();

    return bBoard;
}

// Function to update a bitboard after en passant
bitboard updateBBoardEnPass (bitboard oldBBoard, int curr, int dest)
{
    bitboard bBoard = oldBBoard;

    // For white pawns
    if (bBoard.wPawns & sqrVal[curr])
    {
        bBoard.bPawns -= sqrVal[dest+8];
        bBoard.wPawns -= sqrVal[curr];
        bBoard.wPawns += sqrVal[dest];
    }
    // For black pawns
    else
    {
        bBoard.wPawns -= sqrVal[dest-8];
        bBoard.bPawns -= sqrVal[curr];
        bBoard.bPawns += sqrVal[dest];
    }

    // Update moves and the union sets
    bBoard.prevCurr = curr;
    bBoard.prevDest = dest;
    bBoard.updateUnions();

    return bBoard;
}

// Function to update a bitboard after castling
bitboard updateBBoardCast (bitboard oldBBoard, int curr, int dest)
{
    bitboard bBoard = oldBBoard;

    // White queenside castling
    if (curr == 60 && dest == 58)
    {
        bBoard.wKings -= sqrVal[60];
        bBoard.wKings += sqrVal[58];
        bBoard.wRooks -= sqrVal[56];
        bBoard.wRooks += sqrVal[59];
    }
    // White kingside castling
    else if (curr == 60 && dest == 62)
    {
        bBoard.wKings -= sqrVal[60];
        bBoard.wKings += sqrVal[62];
        bBoard.wRooks -= sqrVal[63];
        bBoard.wRooks += sqrVal[61];
    }
    // Black queenside castling
    else if (curr == 4 && dest == 2)
    {
        bBoard.bKings -= sqrVal[4];
        bBoard.bKings += sqrVal[2];
        bBoard.bRooks -= sqrVal[0];
        bBoard.bRooks += sqrVal[3];
    }
    // Black kingside castling
    else if (curr == 4 && dest == 6)
    {
        bBoard.bKings -= sqrVal[4];
        bBoard.bKings += sqrVal[6];
        bBoard.bRooks -= sqrVal[7];
        bBoard.bRooks += sqrVal[5];
    }

    // Update moves and the union sets
    bBoard.prevCurr = curr;
    bBoard.prevDest = dest;
    bBoard.updateUnions();

    return bBoard;
}

// Function to return a 64-bit integer of all the pawn moves for a particular square
U64 getPawnMoves (bitboard bBoard, int square)
{
    // For white pawns
    if (bBoard.wPieces & sqrVal[square])
    {
        U64 pawnMoves = wPawn1Dir[square] & bBoard.blank;
        if (48 <= square && square <= 55 && pawnMoves != false)
            pawnMoves |= wPawn2Dir[square] & bBoard.blank;
        pawnMoves |= wPawnCapDir[square] & bBoard.bPieces;
        return pawnMoves;
    }
    // For black pawns
    else
    {
        U64 pawnMoves = bPawn1Dir[square] & bBoard.blank;
        if (8 <= square && square <= 15 && pawnMoves != false)
            pawnMoves |= bPawn2Dir[square] & bBoard.blank;
        pawnMoves |= bPawnCapDir[square] & bBoard.wPieces;
        return pawnMoves;
    }
}

// Function to return a 64-bit integer, where an en passant capture could occur, for a particular square
U64 getEnPassant (bitboard bBoard, int square)
{
    // Ensure that the previous move was a pawn advance of 2 squares
    if (absDiff(bBoard.prevCurr/8, bBoard.prevDest/8) == 2 && ((bBoard.wPawns | bBoard.bPawns) & sqrVal[bBoard.prevDest])
        && (square/8 == bBoard.prevDest/8) && absDiff (square%8, bBoard.prevDest%8) == 1)
    {
        // For white pawns - ensure the king isn't in check
        if (bBoard.prevDest/8 == 3 && !isInCheck(bBoard, getWKingLoc(bBoard)))
            return sqrVal[bBoard.prevDest-8];
        // For black pawns - ensure the king isn't in check
        else if (bBoard.prevDest/8 == 4 && !isInCheck(bBoard, getBKingLoc(bBoard)))
            return sqrVal[bBoard.prevDest+8];
    }
    return 0;
}

// Function to return a 64-bit integer of all the knight moves for a particular square
U64 getKnightMoves (bitboard bBoard, int square)
{
    // For white knights
    if (bBoard.wPieces & sqrVal[square])
        return (bBoard.blank | bBoard.bPieces) & knightDir[square];
    // For black knights
    else
        return (bBoard.blank | bBoard.wPieces) & knightDir[square];
}


// Function to return a 64-bit integer of all the bishop moves for a particular square
U64 getBishopMoves (bitboard bBoard, int square)
{
    U64 deg45Moves, deg135Moves, deg225Moves, deg315Moves, blankOrEnemy;

    // Determine where the enemy pieces are
    if (bBoard.wPieces & sqrVal[square])
        blankOrEnemy = bBoard.blank | bBoard.bPieces;
    else
        blankOrEnemy = bBoard.blank | bBoard.wPieces;

    // Get the first blocking piece
    deg45Moves = deg45Dir[square] & bBoard.pieces;

    // Get the blocked squares
    deg45Moves = deg45Dir[square] & ((deg45Moves<<7) | (deg45Moves<<14) | (deg45Moves<<21) | (deg45Moves<<28) | (deg45Moves<<35) | (deg45Moves<<42));

    // Get the unblocked squares
    deg45Moves = (deg45Moves ^ deg45Dir[square]) & blankOrEnemy;

    // Repeat process for the other directions
    deg135Moves = deg135Dir[square] & bBoard.pieces;
    deg135Moves = deg135Dir[square] & ((deg135Moves<<9) | (deg135Moves<<18) | (deg135Moves<<27) | (deg135Moves<<36) | (deg135Moves<<45) | (deg135Moves<<54));
    deg135Moves = (deg135Moves ^ deg135Dir[square]) & blankOrEnemy;

    deg225Moves = deg225Dir[square] & bBoard.pieces;
    deg225Moves = deg225Dir[square] & ((deg225Moves>>7) | (deg225Moves>>14) | (deg225Moves>>21) | (deg225Moves>>28) | (deg225Moves>>35) | (deg225Moves>>42));
    deg225Moves = (deg225Moves ^ deg225Dir[square]) & blankOrEnemy;

    deg315Moves = deg315Dir[square] & bBoard.pieces;
    deg315Moves = deg315Dir[square] & ((deg315Moves>>9) | (deg315Moves>>18) | (deg315Moves>>27) | (deg315Moves>>36) | (deg315Moves>>45) | (deg315Moves>>54));
    deg315Moves = (deg315Moves ^ deg315Dir[square]) & blankOrEnemy;

    return deg45Moves | deg135Moves | deg225Moves | deg315Moves;
}

// Function to return a 64-bit integer of all the rook moves for
// a particular square
U64 getRookMoves (bitboard bBoard, int square)
{
    U64 rightMoves, leftMoves, upMoves, downMoves, blankOrEnemy;

    // Determine where the enemy pieces are
    if (bBoard.wPieces & sqrVal[square])
        blankOrEnemy = bBoard.blank | bBoard.bPieces;
    else
        blankOrEnemy = bBoard.blank | bBoard.wPieces;

    // Get the first blocking piece
    rightMoves = rightDir[square] & bBoard.pieces;

    // Get the blocked squares
    rightMoves = rightDir[square] & ((rightMoves>>1) | (rightMoves>>2) |
                                     (rightMoves>>3) | (rightMoves>>4) |
                                     (rightMoves>>5) | (rightMoves>>6));

    // Get the unblocked squares
    rightMoves = (rightMoves ^ rightDir[square]) & blankOrEnemy;

    // Repeat process for the other directions
    leftMoves = leftDir[square] & bBoard.pieces;
    leftMoves = leftDir[square] & ((leftMoves<<1) | (leftMoves<<2) |
                                   (leftMoves<<3) | (leftMoves<<4) |
                                   (leftMoves<<5) | (leftMoves<<6));
    leftMoves = (leftMoves ^ leftDir[square]) & blankOrEnemy;

    upMoves = upDir[square] & bBoard.pieces;
    upMoves = upDir[square] & ((upMoves<<8) | (upMoves<<16) |
                               (upMoves<<24) | (upMoves<<32) |
                               (upMoves<<40) | (upMoves<<48));
    upMoves = (upMoves ^ upDir[square]) & blankOrEnemy;

    downMoves = downDir[square] & bBoard.pieces;
    downMoves = downDir[square] & ((downMoves>>8) | (downMoves>>16) |
                                   (downMoves>>24) | (downMoves>>32) |
                                   (downMoves>>40) | (downMoves>>48));
    downMoves = (downMoves ^ downDir[square]) & blankOrEnemy;

    return rightMoves | leftMoves | upMoves | downMoves;
}

// Function to return a 64-bit integer of king moves
U64 getQueenMoves (bitboard bBoard, int square)
{
    return getBishopMoves(bBoard, square) | getRookMoves(bBoard, square);
}

// Function to return a 64-bit integer of king moves
U64 getKingMoves (bitboard bBoard, int square)
{
    // For white king
    if (bBoard.wPieces & sqrVal[square])
        return kingDir[square] & (bBoard.bPieces | bBoard.blank);
    // For black king
    else
        return kingDir[square] & (bBoard.wPieces | bBoard.blank);
}

// Function to return a 64-bit integer of all the castling moves
U64 getCastlingMoves (bitboard bBoard, int square)
{
    U64 moves = 0;

    // White king castling
    if (square == 60)
    {
        // Make sure that there are no blocking pieces to the queenside
        if (bBoard.wQueenSide && !(bBoard.pieces & 112))
        {
            // Pretend that there are kings at the squares where it castles through
            bBoard.wKings += 48;
            bBoard.updateUnions();

            // Ensure that the king does not castle through check
            if (!isInCheck(bBoard, 58) && !isInCheck(bBoard, 59) && !isInCheck(bBoard, 60))
                moves += 32;
            bBoard.wKings -= 48;
            bBoard.updateUnions();
        }
        // Make sure that there are no blocking pieces to the kingside
        if (bBoard.wKingSide && !(bBoard.pieces & 6))
        {
            // Pretend that there are kings at the squares where it castles through
            bBoard.wKings += 6;
            bBoard.updateUnions();

            // Ensure that the king does not castle through check
            if (!isInCheck(bBoard, 60) && !isInCheck(bBoard, 61) && !isInCheck(bBoard, 62))
                moves += 2;
        }
    }
    // Black king castling
    else if (square == 4)
    {
        // Make sure that there are no blocking pieces to the queenside
        if (bBoard.bQueenSide && !(bBoard.pieces & 8070450532247928832))
        {
            // Pretend that there are kings at the squares where it castles through
            bBoard.bKings += 3458764513820540928;
            bBoard.updateUnions();

            // Ensure that the king does not castle through check
            if (!isInCheck(bBoard, 2) && !isInCheck(bBoard, 3) && !isInCheck(bBoard, 4))
                moves += 2305843009213693952;
            bBoard.wKings -= 3458764513820540928;
            bBoard.updateUnions();
        }
        // Make sure that there are no blocking pieces to the kingside
        if (bBoard.bKingSide && !(bBoard.pieces & 432345564227567616))
        {
            // Pretend that there are kings at the squares where it castles through
            bBoard.bKings += 432345564227567616;
            bBoard.updateUnions();

            // Ensure that the king does not castle through check
            if (!isInCheck(bBoard, 4) && !isInCheck(bBoard, 5) && !isInCheck(bBoard, 6))
                moves += 144115188075855872;
        }
    }

    return moves;
}

// Function to get the location of a white king
int getWKingLoc (bitboard bBoard)
{
    return 63-log2(bBoard.wKings);
}

// Function to get the location of a black king
int getBKingLoc (bitboard bBoard)
{
    return 63-log2(bBoard.bKings);
}

// Initialize both boards
void initBoard (svec &sBoard)
{
    sBoard.push_back(INITLINE8);
    sBoard.push_back(INITLINE7);
    sBoard.push_back(INITLINE6);
    sBoard.push_back(INITLINE5);
    sBoard.push_back(INITLINE4);
    sBoard.push_back(INITLINE3);
    sBoard.push_back(INITLINE2);
    sBoard.push_back(INITLINE1);
}

// Function to output a chess board (from a string vector)
void displayBoard (svec board)
{
    // Output top line
    cout << "     0   1   2   3   4   5   6   7" << endl;
    cout << "    ";
    for (int i = 1; i <= 31; i++)
        cout << "_";
    cout << endl;

    // Output pieces
    for (int i = 0; i < 8; i++)
    {
        cout << " " << 8-i << " ";
        for (int j = 0; j < 8; j++)
            cout << "| " << board[i][j] << " ";

        cout << "| "<< 8*i << endl << "   ";
        for (int i = 0; i <= 32; i++)
        {
            if (i%4 == 0)
                cout << "|";
            else
                cout << "_";
        }
        cout << endl;
    }

    // Output bottom line
    cout << "     A   B   C   D   E   F   G   H" << endl;
    cout << endl;
}

// Display a 64-bit integer in an 8x8 grid
void displayU64 (U64 n)
{
    for (int i = 0; i < 64; i++)
    {
        cout << (bool)(n & sqrVal[i]);
        if (i%8 == 7)
            cout << endl;
    }
    cout << endl;
}

// Convert a string vector to a bitboard
void svecToBitboard (bitboard &bBoard, svec sBoard)
{
    // Reset 64-bit integers
    bBoard.wPawns = 0;
    bBoard.wKnights = 0;
    bBoard.wBishops = 0;
    bBoard.wRooks = 0;
    bBoard.wQueens = 0;
    bBoard.wKings = 0;
    bBoard.bPawns = 0;
    bBoard.bKnights = 0;
    bBoard.bBishops = 0;
    bBoard.bRooks = 0;
    bBoard.bQueens = 0;
    bBoard.bKings = 0;
    bBoard.wMaterialVal = 0;
    bBoard.bMaterialVal = 0;

    // Go through all the squares in the vector
    for (int i = 0; i < 64; i++)
    {
        // Check which piece occupies the square
        switch (sBoard[i/8][i%8])
        {
            case 'P':
                bBoard.wPawns += sqrVal[i];
                bBoard.wMaterialVal += 100;
                break;
            case 'N':
                bBoard.wKnights += sqrVal[i];
                bBoard.wMaterialVal += 310;
                break;
            case 'B':
                bBoard.wBishops += sqrVal[i];
                bBoard.wMaterialVal += 320;
                break;
            case 'R':
                bBoard.wRooks += sqrVal[i];
                bBoard.wMaterialVal += 500;
                break;
            case 'Q':
                bBoard.wQueens += sqrVal[i];
                bBoard.wMaterialVal += 1000;
                break;
            case 'K':
                bBoard.wKings += sqrVal[i];
                break;
            case 'p':
                bBoard.bPawns += sqrVal[i];
                bBoard.bMaterialVal += 100;
                break;
            case 'n':
                bBoard.bKnights += sqrVal[i];
                bBoard.bMaterialVal += 310;
                break;
            case 'b':
                bBoard.bBishops += sqrVal[i];
                bBoard.bMaterialVal += 320;
                break;
            case 'r':
                bBoard.bRooks += sqrVal[i];
                bBoard.bMaterialVal += 500;
                break;
            case 'q':
                bBoard.bQueens += sqrVal[i];
                bBoard.bMaterialVal += 1000;
                break;
            case 'k':
                bBoard.bKings += sqrVal[i];
                break;
        }
    }

    // Check if any rooks / kings are not in their starting spots
    // Left white rook
    if (!(bBoard.wRooks & 128))
        bBoard.wQueenSide = false;
    // Right white rook
    if (!(bBoard.wRooks & 1))
        bBoard.wKingSide = false;
    // Left black rook
    if (!(bBoard.bRooks & 9223372036854775808U))
        bBoard.bQueenSide = false;
    // Right black rook
    if (!(bBoard.bRooks & 72057594037927936))
        bBoard.bKingSide = false;
    // White king
    if (!(bBoard.wKings & sqrVal[60]))
        bBoard.wQueenSide = bBoard.wKingSide = false;
    // Black king
    if (!(bBoard.bKings & sqrVal[4]))
        bBoard.bQueenSide = bBoard.bKingSide = false;

    // Update the bitboard's union 64-bit integers
    bBoard.updateUnions();
}

// Function to convert a bitboard into a string vector
void bitBoardToSVec (bitboard bBoard, svec &sBoard)
{
    // Go through all the squares to see what piece occupies that square
    for (int i = 0; i < 64; i++)
    {
        if (bBoard.wPawns & sqrVal[i])
            sBoard[i/8][i%8] = 'P';
        else if (bBoard.wKnights & sqrVal[i])
            sBoard[i/8][i%8] = 'N';
        else if (bBoard.wBishops & sqrVal[i])
            sBoard[i/8][i%8] = 'B';
        else if (bBoard.wRooks & sqrVal[i])
            sBoard[i/8][i%8] = 'R';
        else if (bBoard.wQueens & sqrVal[i])
            sBoard[i/8][i%8] = 'Q';
        else if (bBoard.wKings & sqrVal[i])
            sBoard[i/8][i%8] = 'K';
        else if (bBoard.bPawns & sqrVal[i])
            sBoard[i/8][i%8] = 'p';
        else if (bBoard.bKnights & sqrVal[i])
            sBoard[i/8][i%8] = 'n';
        else if (bBoard.bBishops & sqrVal[i])
            sBoard[i/8][i%8] = 'b';
        else if (bBoard.bRooks & sqrVal[i])
            sBoard[i/8][i%8] = 'r';
        else if (bBoard.bQueens & sqrVal[i])
            sBoard[i/8][i%8] = 'q';
        else if (bBoard.bKings & sqrVal[i])
            sBoard[i/8][i%8] = 'k';
        else
            sBoard[i/8][i%8] = ' ';
    }
}

// Convert a string to an actual move represented by two integers
void stringToSquare (string input, int &curr, int &dest)
{
    curr = (56 - input[1]) * 8;
    curr += input[0] -  97;
    dest = (56 - input[3]) * 8;
    dest += input[2] -  97;
}

// Output a move in algebraic notation
void squareToMove (int curr, int dest)
{
    cout << (char)((curr%8)+97) << 8 - curr/8 << " " << (char)((dest%8)+97) << 8 - dest/8 << endl << endl;
}

// Checks to see if the inputted move contains valid chess coordinates
bool isValidInput (string input)
{
    if (input.length() != 4)
        return false;
    if (input[0] < 97 || input[0] > 104)
        return false;
    if (input[1] < 49 || input[1] > 57)
        return false;
    if (input[2] < 97 || input[2] > 104)
        return false;
    if (input[3] < 49 || input[3] > 57)
        return false;
    if (input[0] == input[2] && input[1] == input[3])
        return false;
    return true;
}

// Function to update a bitboard after castling
bool isRightColour (bitboard board, int curr, int moveNum)
{
    // Ensure that the square is not blank
    if (board.pieces & sqrVal[curr])
    {
        // Ensure that the square has a piece of the right colour
        if (moveNum % 2 == 1)
            return (bool)(board.wPieces & sqrVal[curr]);
        else
            return (bool)(board.bPieces & sqrVal[curr]);
    }
    else
        return false;
}

// Function to calculate the absolute difference
int absDiff (int a, int b)
{
    return (a<b) ? b-a:a-b;
}

// Function to ask what piece a pawn should be promoted to
string getPromotionPiece ()
{
    string piece = "p";
    while (piece != "n" && piece != "b" && piece != "r" && piece != "q")
    {
        cout << "Enter what piece it should be promoted to (n, b, r, q are the options): ";
        cin >> piece;
    }
    return piece;
}

// Functions to read the textfile containing various calculated 64-bit integers
void getDirections()
{
    ifstream inFile;
    string line;
    string fileName = "storedU64s.txt";

    // Open text file
    while (true)
    {

        inFile.open (fileName.c_str(), ifstream::in);

        // Make sure file is openable
        if (!inFile.is_open())
        {
            cout << "Enter the name of the textfile containing stored U64s: ";
            getline(cin, fileName);
        }
        // Exit if the file can open
        else
            break;
    }

    // Read text file
    // Input the value of each square
    int numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            sqrVal[numInputted] = strtoull (line.c_str(), NULL, 10);
            numInputted++;
        }
    }

    // Input the white pawn moves of 1 square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            wPawn1Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the white pawn moves of 2 squares
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            wPawn2Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the white pawn captures
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            wPawnCapDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the black pawn moves of 1 square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            bPawn1Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the black pawn moves of 2 squares
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            bPawn2Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the black pawn captures
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            bPawnCapDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the knight moves
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            knightDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the king moves
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            kingDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are to the right of each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            rightDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are to the left of each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            leftDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are above each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            upDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are below each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            downDir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are to the 45 degrees from each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            deg45Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are to the 135 degrees from each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            deg135Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are to the 225 degrees from each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            deg225Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    // Input the squares that are to the 315 degrees from each square
    numInputted = 0;
    while (numInputted < 64)
    {
        getline (inFile, line);
        if (line.size() > 0 && line[0] != '/')
        {
            deg315Dir[numInputted] = strtoull (line.c_str(), NULL, 2);
            numInputted++;
        }
    }

    inFile.close();
}
