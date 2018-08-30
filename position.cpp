/**
* A class storing a Connect 4 position.
* Functions are relative to the current player to play.
* Position containing alignment are not supported by this class.
*
* A binary bitboard representationis used.
* Each column is encoded on HEIGH+1 bits.
*
* Example of bit order to encode for a 7x6 board
* .  .  .  .  .  .  .
* 5 12 19 26 33 40 47
* 4 11 18 25 32 39 46
* 3 10 17 24 31 38 45
* 2  9 16 23 30 37 44
* 1  8 15 22 29 36 43
* 0  7 14 21 28 35 42
*
* Position is stored as
* - a bitboard "mask" with 1 on any color stones
* - a bitboard "current_player" with 1 on stones of current player

* key is an unique representation of a board key = position + mask + bottom
*/

#ifndef POSITION_CPP
#define POSITION_CPP
#include <cstdint>
#include <cassert>
#include <iostream>
#include <bitset>

/*
* Generate a bitmask containing one for the bottom slot of each colum
* must be defined outside of the class definition to be available at compile time for bottom_mask
*/
constexpr static uint64_t bottom(int width, int height) {
	return width == 0 ? 0 : bottom(width - 1, height) | 1LL << (width - 1)*(height + 1);
}


class position {
public:
	static const int HEIGHT = 6;
	static const int WIDTH = 7;

	position() : current_position{ 0 }, mask{ 0 }, moves{ 0 } {}
	position(uint64_t current, uint64_t pos_mask, unsigned int n_moves) : current_position{ current }, mask{ pos_mask }, moves{ n_moves } {}
	
	uint64_t test_fun()
	{
	}

	void play(uint64_t move)
	{
		current_position ^= mask;
		mask |= move;
		moves++;
	}
	/*
	* return true if current player can win next move
	*/
	bool canWinNext() const
	{
		return winning_position() & possible();
	}

	/**
	* @return number of moves played from the beginning of the game.
	*/
	int nbMoves() const
	{
		return moves;
	}

	/**
	* @return a compact representation of a position on WIDTH*(HEIGHT+1) bits.
	*/
	uint64_t key() const
	{
		return current_position + mask;
	}

	/*
	* Return a bitmap of all the possible next moves the do not lose in one turn.
	* A losing move is a move leaving the possibility for the opponent to win directly.
	*
	* Warning this function is intended to test position where you cannot win in one turn
	* If you have a winning move, this function can miss it and prefer to prevent the opponent
	* to make an alignment.
	*/
	uint64_t possibleNonLosingMoves() const {
		assert(!canWinNext());
		uint64_t possible_mask = possible();
		uint64_t opponent_win = opponent_winning_position();
		uint64_t forced_moves = possible_mask & opponent_win;
		if (forced_moves) {
			if (forced_moves & (forced_moves - 1)) // check if there is more than one forced move
				return 0;                           // the opponnent has two winning moves and you cannot stop him
			else possible_mask = forced_moves;    // enforce to play the single forced move
		}
		return possible_mask & ~(opponent_win >> 1);  // avoid to play below an opponent winning spot
	}

	/**
	* Score a possible move.
	*
	* @param move, a possible move given in a bitmap format.
	*
	* The score we are using is the number of winning spots
	* the current player has after playing the move.
	*/
	int moveScore(uint64_t move) const {
		return popcount(compute_winning_position(current_position | move, mask));
	}

	/*
	* Plays a sequence of successive played columns, mainly used to initilize a board.
	* @param seq: a sequence of digits corresponding to the 1-based index of the column played.
	*
	* @return number of played moves. Processing will stop at first invalid move that can be:
	*           - invalid character (non digit, or digit >= WIDTH)
	*           - playing a colum the is already full
	*           - playing a column that makes an alignment (we only solve non).
	*         Caller can check if the move sequence was valid by comparing the number of
	*         processed moves to the length of the sequence.
	*/
	unsigned int play(std::string seq)
	{
		for (unsigned int i = 0; i < seq.size(); i++) {
			int col = seq[i] - '1';
			if (col < 0 || col >= position::WIDTH || !canPlay(col) || isWinningMove(col)) return i; // invalid move
			playCol(col);
		}
		return seq.size();
	}

	/**
	* Plays a playable column.
	* This function should not be called on a non-playable column or a column making an alignment.
	*
	* @param col: 0-based index of a playable column.
	*/
	void playCol(int col)
	{
		play((mask + bottom_mask_col(col)) & column_mask(col));
	}

	bool canPlay(int col) const
	{
		return (mask & top_mask_col(col)) == 0;
	}

	/**
	* Indicates whether the current player wins by playing a given column.
	* This function should never be called on a non-playable column.
	* @param col: 0-based index of a playable column.
	* @return true if current player makes an alignment by playing the corresponding column col.
	*/
	bool isWinningMove(int col) const
	{
		return winning_position() & possible() & column_mask(col);
	}

	/*
	* @parmam position, a bitmap of the player to evaluate the winning pos
	* @param mask, a mask of the already played spots
	*
	* @return a bitmap of all the winning free spots making an alignment
	*/
	static uint64_t compute_winning_position(uint64_t position, uint64_t mask) {
		// vertical;
		uint64_t r = (position << 1) & (position << 2) & (position << 3);

		//horizontal
		uint64_t p = (position << (HEIGHT + 1)) & (position << 2 * (HEIGHT + 1));
		r |= p & (position << 3 * (HEIGHT + 1));
		r |= p & (position >> (HEIGHT + 1));
		p = (position >> (HEIGHT + 1)) & (position >> 2 * (HEIGHT + 1));
		r |= p & (position << (HEIGHT + 1));
		r |= p & (position >> 3 * (HEIGHT + 1));

		//diagonal 1
		p = (position << HEIGHT) & (position << 2 * HEIGHT);
		r |= p & (position << 3 * HEIGHT);
		r |= p & (position >> HEIGHT);
		p = (position >> HEIGHT) & (position >> 2 * HEIGHT);
		r |= p & (position << HEIGHT);
		r |= p & (position >> 3 * HEIGHT);

		//diagonal 2
		p = (position << (HEIGHT + 2)) & (position << 2 * (HEIGHT + 2));
		r |= p & (position << 3 * (HEIGHT + 2));
		r |= p & (position >> (HEIGHT + 2));
		p = (position >> (HEIGHT + 2)) & (position >> 2 * (HEIGHT + 2));
		r |= p & (position << (HEIGHT + 2));
		r |= p & (position >> 3 * (HEIGHT + 2));

		return r & (board_mask ^ mask);
	}


	void display_board(unsigned int current_player) const {
		/*  Current Player = 1: Then current_position pieces become 'X'
		Current_player = 2: Then current_position pieces become 'O'
		*/
		assert(current_player == 1 || current_player == 2, "Current player must be 1 or 2");
		char* current_string; char* opponent_string;
		switch (current_player) {
		case 1: { current_string = " X "; opponent_string = " O "; break; }
		case 2: { current_string = " O "; opponent_string = " X "; break; }
		};

		for (int row_num = 5; row_num >= 0; row_num--) {
			for (int col_num = 0; col_num <= 6; col_num++)
			{
				uint64_t slot_value = (uint64_t(1) << row_num) << (7*col_num);
				if (current_position & slot_value) { std::cout << current_string; }
				else if ( (current_position^mask) & slot_value) {std::cout<<opponent_string; }
				else { std::cout << " - "; }
				
			}
			std::cout << "\n";
		}
	}
	
	uint64_t current_position = 0; // bitmap of the current_player stones
	uint64_t mask = 0;             // bitmap of all the already palyed spots
	unsigned int moves;        // number of moves played since the beinning of the game.

	int create_easy_pos() {
		current_position = 0;  mask = 0; moves = 0; //reset board
		play("44444433");
		return 1;
	}

private:

	/*
	* Return a bitmask of the possible winning positions for the current player
	*/
	uint64_t winning_position() const {
		return compute_winning_position(current_position, mask);
	}

	/*
	* Return a bitmask of the possible winning positions for the opponent
	*/
	uint64_t opponent_winning_position() const {
		return compute_winning_position(current_position ^ mask, mask);
	}

	/*
	* Bitmap of the next possible valid moves for the current player
	* Including losing moves.
	*/
	uint64_t possible() const {
		return (mask + bottom_mask) & board_mask;
	}

	/*
	* counts number of bit set to one in a 64bits integer
	*/
	static unsigned int popcount(uint64_t m) {
		unsigned int c = 0;
		for (c = 0; m; c++) m &= m - 1;
		return c;
	}


	// Static bitmaps

	const static uint64_t bottom_mask = bottom(WIDTH, HEIGHT);
	const static uint64_t board_mask = bottom_mask * ((1LL << HEIGHT) - 1);

	// return a bitmask containg a single 1 corresponding to the top cel of a given column
	static constexpr uint64_t top_mask_col(int col) {
		return UINT64_C(1) << ((HEIGHT - 1) + col*(HEIGHT + 1));
	}

	// return a bitmask containg a single 1 corresponding to the bottom cell of a given column
	static constexpr uint64_t bottom_mask_col(int col) {
		return UINT64_C(1) << col*(HEIGHT + 1);
	}

	public:
		// return a bitmask 1 on all the cells of a given column
		static constexpr uint64_t column_mask(int col) {
			return ((UINT64_C(1) << HEIGHT) - 1) << col*(HEIGHT + 1);
		}
};

#endif