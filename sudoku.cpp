#include <stdio.h>
#include "sudoku.h"
#include <set>
#include <iostream>

using namespace std;

//find the solution for the supplied game
int solve(game_wrapper game) {

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			int val = game.board.at(i).at(j).val;
			if (val == 0) {

				//cout << "Current Game State:" << endl;
				//for (int n = 0; n < 9; n++) {
				//	for (int m = 0; m < 9; m++) {
				//		cout << game.board.at(n).at(m).val << "\t";
				//	}
				//cout << endl;
				//}
				//cout << "selected value: " << val << " at position x: " << j << " y: " << i << endl;
				//system("pause");
				int col = i, row = j;

				
				for (int n = 0; n < 9; n++) {
					game.board.at(col).at(row).erase(game.board.at(col).at(n).val);
					game.board.at(col).at(row).erase(game.board.at(n).at(row).val);
				}

				int square_x = col - col%3, square_y = row - row%3;
				for (int n = 0; n < 3; n++) {
					for (int m = 0; m < 3; m++) {
						game.board.at(col).at(row).erase(game.board.at((double)square_x + n).at((double)square_y + m).val);
					}
				}

				//cout << "possible: ";
				//for (auto new_value : game.board.at(col).at(row).valid_vals) {
				//	cout << new_value << ", ";
				//}
				//cout << endl;

				if (game.board.at(col).at(row).valid_vals.size() == 0) {
					//cout << "out of moves!" << endl;
					return 1;
				};

				set<int>::iterator iter = game.board.at(col).at(row).valid_vals.begin();

				while (iter != game.board.at(col).at(row).valid_vals.end()) {
					int new_value = *iter;
					//cout << "choosing: " << new_value << endl;
					game.board.at(col).at(row).val = new_value;
					if (solve(game) == 0) return 0;
					iter++;
					if (iter == game.board.at(col).at(row).valid_vals.end()) {
						//cout << "no more moves!" << endl;
						return 1;
					}
				}

			}
		}
	}

	cout << "The Solution:" << endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << game.board.at(i).at(j).val << "\t";
		}
		cout << endl;
	}
	system("pause");
	return 0;

}
