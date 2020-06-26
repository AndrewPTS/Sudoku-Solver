#ifndef SUDOKU_H
#define SUDOKU_H
#include <array>
#include <set>
#include <iostream>

struct game_square {
	int val = 0;
	std::set<int> valid_vals = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

	void erase(int to_erase) {
		if(valid_vals.erase(to_erase) == 1);
			//std::cout << "removing: " << to_erase << std::endl;
	}
};
struct game_wrapper { std::array<std::array<game_square, 9>, 9> board; };

int solve(game_wrapper game);

#endif // !SUDOKU_H