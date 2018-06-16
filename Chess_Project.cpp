/// Chess_Project.cpp
///
/// Will Lei
/// This is where int main lives, along with the chess engine

#include <iostream>
#include <cstdlib>
#include <cmath>
#include "legal_moves.h"

#define ISWHITEMOVE (moveNum%2 == 1)
#define ISBLACKMOVE (moveNum%2 == 0)
#define ISFORWHITE (isCompMove == compIsWhite)
#define ISFORBLACK (isCompMove != compIsWhite)

using namespace std;

// Declare functions
ply findBestMove (bitboard bBoard, int depth, bool compIsWhite);
int alphabeta (bitboard bBoard, int depth, int alpha, int beta, bool isCompMove, bool compIsWhite);
int calcBoardVal (bitboard bBoard, bool forWhite);
int calcLocVal (int square);
string enterUserMove (bitboard bBoard, svec sBoard, int moveNum);

int main()
{
    // Allow user to play chess
    //twoPlayerGame();

    // Initialize the arrays, the bitboard, the vector, variables etc.
    getDirections();
    bitboard bBoard;
    svec sBoard;
    plyvec legalMoves;
    string input;
    int moveNum = 1, curr = 0, dest = 0;
    bool compIsWhite = true;

    // Initialise boards
    initBoard(sBoard);
    svecToBitboard(bBoard, sBoard);

    // Loop continuously getting moves from the AI and the user
    while (true)
    {
        // Let the AI generate the move
        if (compIsWhite && ISWHITEMOVE)
        {
            // Calculate move
            ply compMove = findBestMove(bBoard, 3, compIsWhite);
            curr = compMove.curr;
            dest = compMove.dest;

            // Output what move the AI chose
            //cout << curr << " " << dest << endl;
            squareToMove(curr, dest);

        }
        // Allow user to enter move
        else
        {
            input = enterUserMove(bBoard, sBoard, moveNum);

            if (input == "0")
                break;

            stringToSquare(input, curr, dest);
        }

        // Update the bitboard, the string vector, and the number of moves
        bBoard = updateBitboard(bBoard, curr, dest, false);
        bitBoardToSVec(bBoard, sBoard);
        moveNum++;

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

    // For exe files where the window automatically closes after mate
    string s;
    cout << "Enter anything to close: ";
    cin >> s;

    // Finish program
    return 0;
}

// Function to call alpha-beta to find the best move for the computer
ply findBestMove (bitboard bBoard, int depth, bool compIsWhite)
{
    // Get all the moves available for the computer
    plyvec legalMoves = getLegalMoves(bBoard, compIsWhite);
    int bestVal, bestMove = 0;

    // Go through all the legal moves of the computer
    for (unsigned int i = 0; i < legalMoves.size(); i++)
    {
        // Update the bitboard after a move
        bitboard bb2 = updateBitboard(bBoard, legalMoves[i].curr, legalMoves[i].dest, true);

        // Check for checkmate/stalemate
        if (compIsWhite && !areLegalMoves(bb2, false) && isInCheck(bb2, getBKingLoc(bb2)))
            return legalMoves[i];
        if (!compIsWhite && !areLegalMoves(bb2, true) && isInCheck(bb2, getWKingLoc(bb2)))
            return legalMoves[i];

        // Call the alpha-beta algorithm to evaluate the position at hand
        int alpha = -2000000000, beta = 2000000000;
        int boardVal = alphabeta(bb2, depth-1, alpha, beta, false, compIsWhite);

        // Update the best move and the best value
        if (i == 0 || boardVal > bestVal)
        {
            bestVal = boardVal;
            bestMove = i;
        }
    }

    //cout << "Computer value is: " << bestVal << endl;
    return legalMoves[bestMove];
}

// Function that uses the recursive alpha-beta algorithm to return the value of an updated bitboard
int alphabeta (bitboard bBoard, int depth, int alpha, int beta, bool isCompMove, bool compIsWhite)
{
    // Get all the legal moves for whoever is supposed to move
    plyvec legalMoves = getLegalMoves(bBoard, ISFORWHITE);

    // Stop search if there are no more legal moves or if the search has reached the maximum depth
    if (legalMoves.size() == 0 || depth == 0)
    {
        return calcBoardVal(bBoard, compIsWhite);
    }

    // Maximize the value if it is the computer's turn to move
    if (isCompMove)
    {
        int bestVal = 0;

        // Go through all the legal moves, searching for the move that is worst for the computer
        for (unsigned int i = 0; i < legalMoves.size(); i++)
        {
            // Update bitboard and recursively call the alpha-beta algorithm
            bitboard bBoard2 = updateBitboard(bBoard, legalMoves[i].curr, legalMoves[i].dest, true);
            int boardVal = alphabeta(bBoard2, depth-1, alpha, beta, !isCompMove, compIsWhite);

            // Update the best board value and alpha, the best position the computer is guaranteed of
            if (i == 0 || boardVal > bestVal)
                bestVal = boardVal;
            if (bestVal > alpha)
                alpha = bestVal;

            // Stop if the move is worse than all the previous moves
            if (beta <= alpha)
                break;
        }

        return bestVal;
    }
    // Minimize the board's value if it is the opponent's turn to move
    else
    {
        int bestVal = 0;

        // Go through all the legal moves, searching for the move that is worst for the computer
        for (unsigned int i = 0; i < legalMoves.size(); i++)
        {
            // Update bitboard and recursively call the alpha-beta algorithm
            bitboard bBoard2 = updateBitboard(bBoard, legalMoves[i].curr, legalMoves[i].dest, true);
            int boardVal = alphabeta(bBoard2, depth-1, alpha, beta, !isCompMove, compIsWhite);

            // Update the best board value and beta, the best position the user is guaranteed of
            if (i == 0 || boardVal < bestVal)
                bestVal = boardVal;
            if (bestVal < beta)
                beta = bestVal;

            // Stop if the move is worse than all the previous moves
            if (beta <= alpha)
                break;
        }

        return bestVal;
    }
}

// Function to return the value of a board for a side
int calcBoardVal (bitboard bBoard, bool forWhite)
{
    int wPositionVal = 0, bPositionVal = 0;

    // Return a million points if checkmate is achieved
    if (forWhite && !areLegalMoves(bBoard, true) && isInCheck(bBoard, getWKingLoc(bBoard)))
        return -1000000;
    if (forWhite && !areLegalMoves(bBoard, false) && isInCheck(bBoard, getBKingLoc(bBoard)))
        return 1000000;
    if (!forWhite && !areLegalMoves(bBoard, false) && isInCheck(bBoard, getBKingLoc(bBoard)))
        return -1000000;
    if (!forWhite && !areLegalMoves(bBoard, true) && isInCheck(bBoard, getWKingLoc(bBoard)))
        return 1000000;

    // During the opening, give points for a minor pieces and pawns closer to the centre of the board
    if (bBoard.wMaterialVal + bBoard.bMaterialVal < 6000)
    {
        U64 wGoodPieces = bBoard.wPawns | bBoard.wKnights | bBoard.wBishops;
        U64 bGoodPieces = bBoard.bPawns | bBoard.bKnights | bBoard.bBishops;
        U64 wBadPieces = bBoard.wRooks | bBoard.wQueens;
        U64 bBadPieces = bBoard.bRooks | bBoard.bQueens;

        // Go through all the squares determining where all the pieces are
        for (int i = 0; i < 64; i++)
        {
            if (wGoodPieces & sqrVal[i])
                wPositionVal += 2*calcLocVal(i);
            if (bGoodPieces & sqrVal[i])
                bPositionVal += 2*calcLocVal(i);
            if (wBadPieces & sqrVal[i])
                wPositionVal -= 5*calcLocVal(i);
            if (bBadPieces & sqrVal[i])
                bPositionVal -= 5*calcLocVal(i);
        }
    }
    // Otherwise just give points for any piece closer to the centre of the board
    else
    {
        // Go through all the squares determining where all the pieces are
        for (int i = 0; i < 64; i++)
        {
            if (bBoard.wPieces & sqrVal[i])
                wPositionVal += calcLocVal(i);
            if (bBoard.bPieces & sqrVal[i])
                bPositionVal += calcLocVal(i);
        }
    }

    // Return the board value and add material value
    if (forWhite)
        return (bBoard.wMaterialVal-bBoard.bMaterialVal) + (wPositionVal-bPositionVal);
    else
        return (bBoard.bMaterialVal-bBoard.wMaterialVal) + (bPositionVal-wPositionVal);
}

// Calculate the value of a piece located on a particular square (centre is better)
int calcLocVal (int square)
{
    // The values are as follows:
    //
    // -10 -10 -10 -10 -10 -10 -10 -10
    // -10   0   0   0   0   0   0 -10
    // -10   0  10  10  10  10   0 -10
    // -10   0  10  20  20  10   0 -10
    // -10   0  10  20  20  10   0 -10
    // -10   0  10  10  10  10   0 -10
    // -10   0   0   0   0   0   0 -10
    // -10 -10 -10 -10 -10 -10 -10 -10
    //

    // Determine which concentric box the square is a part of
    if (square/8 == 0 || square/8 == 7 || square%8 == 0 || square%8 == 7)
        return -10;
    else if (square/8 == 1 || square/8 == 6 || square%8 == 1 || square%8 == 6)
        return 0;
    else if (square/8 == 2 || square/8 == 5 || square%8 == 2 || square%8 == 5)
        return 10;
    else
        return 20;
}

// Function to allow the user to enter in a move
string enterUserMove (bitboard bBoard, svec sBoard, int moveNum)
{
    string input;
    int curr = 0, dest = 0;

    // Loop until the user enters in a valid move
    while (true)
    {
        // Display the board
        displayBoard(sBoard);

        // Get the next move from the user
        cout << "Enter in the desired move: ";
        cin >> input;

        // Clear screen
        system("CLS");

        // Allow user to exit game
        if (input == "0")
            return input;

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

        // Check to see if the move is a regular legal move for white
        if (isLegalMove(bBoard, curr, dest))
            return input;

        // What to do if an illegal move is made
        else
        {
            cout << "Illegal move." << endl;

            bitboard bb2 = updateBitboard(bBoard, curr, dest, true);;

            // Check if it is because of check
            if (isInCheck(bb2, 63-log2(bb2.wKings)))
                cout << "White king is in check" << endl;
            if (isInCheck(bb2, 63-log2(bb2.bKings)))
                cout << "Black king is in check" << endl;
        }
    }
}
