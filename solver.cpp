#include "stdafx.h"
#include <iostream>
#include <cstdint>
#include <string>
#include "position.cpp"
#ifndef SOLVER_CPP
#define SOLVER_CPP

class solver
{

public:
	unsigned long long nodeCount; // counter of explored nodes.

	int solve(const position &P, bool weak) {
		if (P.canWinNext()) // check if win in one move as the Negamax function does not support this case.
			return (position::WIDTH*position::HEIGHT + 1 - P.nbMoves()) / 2;
		int min = -(position::WIDTH*position::HEIGHT - P.nbMoves()) / 2;
		int max = (position::WIDTH*position::HEIGHT + 1 - P.nbMoves()) / 2;
		if (weak) {
			min = -1;
			max = 1;
		}
		return negamax(P, min, max);
	}

	    /**
     * Recursively score connect 4 position using negamax variant of alpha-beta algorithm.
     * @param: position to evaluate, this function assumes nobody already won and 
     *         current player cannot win next move. This has to be checked before
     * @param: alpha < beta, a score window within which we are evaluating the position.
     *
     * @return the exact score, an upper or lower bound score depending of the case:
     * - if actual score of position <= alpha then actual score <= return value <= alpha
     * - if actual score of position >= beta then beta <= return value <= actual score
     * - if alpha <= actual score <= beta then return value = actual score
     */
	int negamax(const position &P, int alpha, int beta)
	{

		for (int x = 0; x < position::WIDTH; x++) // check if current player can win next move
			if (P.canPlay(x) && P.isWinningMove(x))
				return (position::WIDTH*position::HEIGHT + 1 - P.nbMoves()) / 2;

		assert(alpha < beta);
		assert(!P.canWinNext());

		nodeCount++; // increment counter of explored nodes

	//	uint64_t possible = P.possibleNonLosingMoves();
	//	if (possible == 0)     // if no possible non losing move, opponent wins next move
	//		return -(position::WIDTH*position::HEIGHT - P.nbMoves()) / 2;

		if (P.nbMoves() >= position::WIDTH*position::HEIGHT - 2) // check for draw game
			return 0;

		int min = -(position::WIDTH*position::HEIGHT - 2 - P.nbMoves()) / 2;	// lower bound of score as opponent cannot win next move
		if (alpha < min) {
			alpha = min;                     // there is no need to keep beta above our max possible score.
			if (alpha >= beta) return alpha;  // prune the exploration if the [alpha;beta] window is empty.
		}

		int max = (position::WIDTH*position::HEIGHT - 1 - P.nbMoves()) / 2;	// upper bound of our score as we cannot win immediately
		if (beta > max) {
			beta = max;                     // there is no need to keep beta above our max possible score.
			if (alpha >= beta) return beta;  // prune the exploration if the [alpha;beta] window is empty.
		}

		for (int move = 0; move < position::WIDTH;move++) {
			if (!P.canPlay(move)) { continue; };
			//
			position P2(P);
			P2.playCol(move);  // It's opponent turn in P2 position after current player plays x column.
			int score = -negamax(P2, -beta, -alpha); // explore opponent's score within [-beta;-alpha] windows:
													 // no need to have good precision for score better than beta (opponent's score worse than -beta)
													 // no need to check for score worse than alpha (opponent's score worse better than -alpha)
			if (score >= beta) {
				return score;  // prune the exploration if we find a possible move better than what we were looking for.
			}
			if (score > alpha) alpha = score; // reduce the [alpha;beta] window for next exploration, as we only 
											  // need to search for a position that is better than the best so far.
		}

	}

	void human_vs_human(position &P) {
		std::string line;
		std::cout << "make a move, human. (Number from 1 to 7) \n";
		int current_player = 1;
		for (int l = 1; std::getline(std::cin, line); l++)
		{
			if ((line.size() != 1)) { std::cerr << "Invalid input: Must be a single digit from 1 to 7: "; continue; }
			int col = line[0] - '1';
			if (col < 0 || col >= position::WIDTH || !P.canPlay(col)) { std::cerr << "Invalid input: Must be a single digit from 1 to 7: "; continue; }
			bool winner_bool = P.isWinningMove(col);
			P.playCol(col);
			current_player ^= 3;
			P.display_board(current_player);
			std::cout << "Player " << current_player << " turn: \n";
			if (winner_bool) { std::cout << "player "<< (current_player ^ 3) << " wins"; break; } //if there is a winner, break

			if (P.nbMoves() >= position::WIDTH*position::HEIGHT - 2) {
				std::cout << "game is a draw"; break;	} // check for draw game
			
		}
	}

};

#endif