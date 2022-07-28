// aphabetris: tetris with the alphabet :D

// includes
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <thread>
#include <conio.h>
#include <fstream>
#include <unordered_set>
#include <ctime>
#include <algorithm>
#include <map>

// each piece has a coordinate and a letter to define it
struct piece
{
	char letter;
	int x;
	int y;
};

// a struct for handling word detection: we return the word and the move vector indices for each character so we can remove them
struct WordHandler {
	std::string word;
	std::vector<int> wordIndices;
};

class Dict {
private:
	std::unordered_set<std::string> dict;
	std::map<char, int> scores;

	void initialiseScores() {
		scores.insert(std::pair<char, int>('a', 1));
		scores.insert(std::pair<char, int>('b', 3));
		scores.insert(std::pair<char, int>('c', 3));
		scores.insert(std::pair<char, int>('d', 2));
		scores.insert(std::pair<char, int>('e', 1));
		scores.insert(std::pair<char, int>('f', 4));
		scores.insert(std::pair<char, int>('g', 2));
		scores.insert(std::pair<char, int>('h', 4));
		scores.insert(std::pair<char, int>('i', 1));
		scores.insert(std::pair<char, int>('j', 8));
		scores.insert(std::pair<char, int>('k', 5));
		scores.insert(std::pair<char, int>('l', 1));
		scores.insert(std::pair<char, int>('m', 3));
		scores.insert(std::pair<char, int>('n', 1));
		scores.insert(std::pair<char, int>('o', 1));
		scores.insert(std::pair<char, int>('p', 3));
		scores.insert(std::pair<char, int>('q', 10));
		scores.insert(std::pair<char, int>('r', 1));
		scores.insert(std::pair<char, int>('s', 1));
		scores.insert(std::pair<char, int>('t', 1));
		scores.insert(std::pair<char, int>('u', 1));
		scores.insert(std::pair<char, int>('v', 4));
		scores.insert(std::pair<char, int>('w', 4));
		scores.insert(std::pair<char, int>('x', 8));
		scores.insert(std::pair<char, int>('y', 4));
		scores.insert(std::pair<char, int>('z', 10));
	}

public:
	// constructor: establish contents of dictionary
	Dict() {
		std::string line;

		// read textfile with all words in
		std::ifstream scrabble("dictionary.txt");
		if (scrabble.is_open()) {
			while (std::getline(scrabble, line)) {
				if (line.length() == 4) {
					// put all 4-letter words in dictionary
					dict.insert(line);
				}
			}
			scrabble.close();
		}

		// store points
		initialiseScores();
	}

	// function to check if word exists in dictionary
	bool checkWord(std::string word) {
		// convert word to capitals
		for (int i = 0; i < word.length(); i++) {
			word[i] = std::toupper(word[i]);
		}

		// find word
		if (dict.find(word) == dict.end()) {
			return false;
		}

		return true;
	}

	int getScore(std::string word) {
		int sum = 0;
		// for each character, get score and sum
		for (int i = 0; i < word.length(); i++) {
			std::cout << word[i] << std::endl;
			std::cout << scores[word[i]] << std::endl;
			sum += scores[word[i]];
		}

		return sum;
	}

};

// a class to handle everything relating to the grid
class Grid {
private:
	int width;
	int height;
	piece p;
	Dict dict;
	int score;
	char letters[100] = {
		'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', // a: 9%
		'b', 'b', // b: 2%
		'c', 'c', 'c', 'c', 'c', // c: 5%
		'd', 'd', 'd', // d: 3%
		'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', // e: 12%
		'f', 'f', // f: 2%
		'g', 'g', // g: 2%
		'h', 'h', 'h', // h: 3%
		'i', 'i', 'i', 'i', 'i', 'i', 'i', 'i', // i: 8%
		// j: 0%
		'k', // k: 1%
		'l', 'l', 'l', 'l', 'l', // l: 5%
		'm', 'm', 'm', // m: 3%
		'n', 'n', 'n', 'n', 'n', 'n', // n: 6%
		'o', 'o', 'o', 'o', 'o', 'o', 'o', // o: 7%
		'p', 'p', 'p', // p: 3%
		// q: 0%
		'r', 'r', 'r', 'r', 'r', 'r', 'r', 'r', // r: 8%
		's', 's', 's', 's', 's', 's', // s: 6%
		't', 't', 't', 't', 't', 't', 't', // t: 7%
		'u', 'u', 'u', 'u', // u: 4%
		'v', // 1%
		'w', // 1%
		// x: 0%
		'y', 'y', // y: 2%
		// z: 0%
	};

	// store all pieces on grid for plotting purposes
	std::vector <piece> moves;

	// a function to check if piece's y coord is on top of another piece or at bottom
	bool canMove() {
		for (int i = 0; i < moves.size(); i++) {
			if (p.y + 1 == moves[i].y && p.x == moves[i].x) {
				return false;
			}
		}

		if (p.y == height - 2) {
			return false;
		}

		return true;
	}

	// a function used to print piece's current coordinate
	char checkCoord(int x, int y) {
		for (int i = 0; i < moves.size(); i++) {
			if (moves[i].x == x && moves[i].y == y) {
				return moves[i].letter;
			}
		}
		return ' ';
	}

	// a function to randomly select a new letter for the piece
	void getNewPiece() {
		char newChar = letters[rand() % 100];
		p.letter = newChar;
	}

	// a function to check if the last piece made a 4 letter word and if so return word
	WordHandler detectWord(int y) {
		WordHandler wordHandler;

		wordHandler.word = "xxxx";
		int counter = 0;

		// get all pieces on this level
		for (int i = 0; i < moves.size(); i++) {
			if (moves[i].y == y) {
				wordHandler.word[moves[i].x - 1] = moves[i].letter;
				wordHandler.wordIndices.push_back(i);
				counter++;
			}
		}

		if (counter != 4) {
			wordHandler.word = "";
		}

		std::sort(wordHandler.wordIndices.begin(), wordHandler.wordIndices.end());

		return wordHandler;
	}

	// a function to remove moves that have now formed a word
	void removeMoves(std::vector<int> wordIndices) {
		// go through the indices of move to remove and erase them one-by-one
		for (int i = wordIndices.size() - 1; i >= 0; i--) {
			moves.erase(moves.begin() + wordIndices[i]);
		}
	}

	// a function to move all stored pieces above a certain layer down one
	void moveAllDown(int y) {
		for (int i = 0; i < moves.size(); i++) {
			if (moves[i].y <= y) {
				moves[i].y++;
			}
		}
	}

public: 
	// constructor for initialisation
	Grid() {
		// set grid dimensions
		width = 6;
		height = 10;
		p.y = 0;
		p.x = 1;
		score = 0;
		getNewPiece();
	}

	// functions to update x coord of piece with keypress
	void updateX(int direction) {
		if (direction == 97 && p.x > 1) {
			// move left
			p.x--;
		}
		else if (direction == 100 && p.x < width - 2) {
			// move right
			p.x++;
		}
	}

	void drawGrid() {
		// show score
		std::cout << "SCORE: " << score << std::endl;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				// print current letter at coord
				if (i == p.y && j == p.x) {
					std::cout << p.letter;
				}
				// while not at bottom:
				else if (i != height - 1) {
					// draw 
					if (j == 0 || j == width - 1) {
						std::cout << '|';
					}
					else {
						std::cout << checkCoord(j, i);
					}
				}
				// bottom:
				else {
					std::cout << '-';
				}

				// add a newline
				if (j == width - 1) {
					std::cout << std::endl;
				}
			}
		}
	}
	
	bool movePiece() {
		// draw grid
		drawGrid();
		
		// if piece on top of another or at bottom of grid, add to moves vector and make another piece
		if (!canMove()) {
			// check if coordinate is 0, so game over
			if (p.y == 0) {
				return false;
			}
			else {
				// put piece into moves
				moves.push_back(p);

				// handle detecting word: if the word is the right length, check if it's valid; if it is, remove it
				WordHandler wordHandler = detectWord(p.y);

				if (wordHandler.word.length() == 4) {
					if (dict.checkWord(wordHandler.word)) {
						removeMoves(wordHandler.wordIndices);
						moveAllDown(p.y);
						score += dict.getScore(wordHandler.word);
					}
				}

				// reset piece
				p.y = 0;
				p.x = 1;
				getNewPiece();
			}
		}
		// else move down
		else {
			p.y++;
		}

		return true;
	}
};

void getMove(Grid* grid) {
	while (true) {
		grid->updateX(_getch());
	}
}

int main() {
	srand((int)time(0));

	Grid grid;

	// get input thread
	std::thread th1(getMove, &grid);

	while (grid.movePiece()) {
		Sleep(500);
		system("cls");
	}

	std::cout << "GAME OVER" << std::endl;

	th1.join();

	return 0;
}