// minimax_c4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "chrono"
#include "position.cpp"
#include "solver.cpp"


void display_bitmask(uint64_t bitmask)
{
	for (int row_num = 6; row_num >= 0; row_num--) {
		for (int col_num = 0; col_num <= 6; col_num++)
		{
			uint64_t slot_value = (uint64_t(1) << row_num) << (7 * col_num);
			if (bitmask & slot_value) { std::cout << " X "; }
			else { std::cout << " - "; }
		}
		std::cout << "\n";
	}
}

int main()
{
	/*
	auto start_time = std::chrono::high_resolution_clock::now(); 
	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time).count();
	*/
	//let's do a test! create a position, and give it a position. then 

	position P;
	solver ss;
	int current_player = P.create_easy_pos();
	P.display_board(current_player);
	int pos_score = ss.solve(P, true);
	std::cout << pos_score << "\n";
	/*
	// iterate through possible moves:
	for (int move = 0; move < position::WIDTH; move++) {
		if (!P.canPlay(move)) { continue; };
		position P2(P);
		P2.playCol(move);  // It's opponent turn in P2 position after current player plays x column.
		P2.display_board(current_player ^ 3);	// Show the board we are evaluating
		std::cout << "\n";
		int pos_score = ss.solve(P2, true);
		std::cout << pos_score << "\n";
	}
	*/
	// print their scores ! 
	//ss.human_vs_human(P);
	std::cin.ignore();

}

