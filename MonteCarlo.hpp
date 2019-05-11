//
// Created by James on 2019/5/8.
//

#ifndef UCT_MONTECARLO_HPP
#define UCT_MONTECARLO_HPP

#include "Point.h"
#include "Judge.h"
#include "Node.hpp"

class MonteCarlo
{
	const int n_row, n_col;    // M , N (x, y)
	int *top, **board;    // current top and current board
	const Point prev;    // previous move
	const Point ban;    // banned move
	int player;        // current player
	int height;        // tree height
	clock_t since;        // start time
	List<int> available_cols;

	static int top_cache[12], board_cache[12][12];
	static const clock_t CLOCK_LIMIT = (clock_t) (2.6 * CLOCKS_PER_SEC);

	Node *expand(Node *v);    // try to expand v's child, if failed, return nullptr

	int simulate(Node *v);    // random simulation to get profit for current board

	void updateAbove(Node *v, int delta);    // backup profit

	inline int curWinner(int x, int y);        // current winner, -1 - not final, 0 - tie, 1 - user, 2 - machine

	inline void makeMove(int x, int y);    // make move on current board

	inline void reset();    // reset board status

	void printBoard();

public:
	MonteCarlo(int M, int N, int *top_, int **board_,
			   const int lastX, const int lastY, const int noX, const int noY) :
			n_row(M), n_col(N), top(top_), board(board_), prev(lastX, lastY), ban(noX, noY),
			player(1), height(0), since(clock())
	{
		for (int i = 0; i < n_col; ++i) top_cache[i] = top_[i];
		for (int i = 0; i < n_row; ++i)
			for (int j = 0; j < n_col; ++j) board_cache[i][j] = board_[i][j];
//		memcpy(top_cache, top, n_col * sizeof(int));
//		memcpy(board_cache, board, n_row * n_col * sizeof(int));
		available_cols.reserve(12);
	}

	Point UCTSearch();    // return the best action
};

int MonteCarlo::top_cache[12], MonteCarlo::board_cache[12][12];

Point MonteCarlo::UCTSearch()
{
	Node::buildRoot();
	Node *v, *u;
	int delta, n_iter = 0, winner;
	do {	// one iteration
		++n_iter;
		v = Node::root();
		winner = -1;
		// TreePolicy todo
		do {
			u = expand(v);
			if (u) {    // expandable
				v = u;    // goto new child
				makeMove(v->x, v->y);
				break;
			}
			else {        // not expandable
				v = v->bestChild(0.8);    // goto best child
				makeMove(v->x, v->y);
			}
		} while ((winner = curWinner(v->x, v->y)) == -1);    // repeat while game continuing

		if (winner != -1) {    // stop at a terminal
			delta = winner == player ? 2 : (winner == 0 ? 1 : 0);
			if (v->N > 1000) {    // logic shortcut: visited a terminal a number of times before
				reset();        // trace back
				break;
			}
		}
		else {
			delta = simulate(v);    // random simulation
		}
		updateAbove(v, delta);    // backup
		reset();                // trace back
	} while (clock() - since < CLOCK_LIMIT);    // time limit

	v = Node::root()->bestChild(0);

//	makeMove(v->x, v->y);
//	logger("move = (%d, %d)  with confidence = %.4f\n", v->x, v->y, 1.0 * v->Q / v->N)
//	printBoard();
//	logger("# iteration = %d\n", n_iter)
//	logger("# node      = %d\n", Node::n_node())
//	logger("tree height = %d\n", height)
//	logger("ucb:\n")
//	for (auto c : Node::root()->children) {
//		logger("(%2d, %2d)  %.4f\n", c->x, c->y, 1.0 * c->Q / c->N)
//	}

	return {v->x, v->y};
}

void MonteCarlo::updateAbove(Node *v, int delta)
{
	while (v) {
		++v->N;
		v->Q += delta;
		delta = 2 - delta;
		v = v->parent;
	}
}

Node *MonteCarlo::expand(Node *v)
{
	for (int y = v->rightmostChild() + 1; y < n_col; ++y) {
		if (top[y] > 0) {    // available move
			return v->newChild(top[y] - 1, y);
		}
	}
	return nullptr;    // not expandable
}

void MonteCarlo::makeMove(int x, int y)
{
	player = 3 - player;    // switch player
	top[y] = x;
	board[x][y] = player;
	if (ban.y == y && ban.x == x - 1) --top[y];    // assure no ban point above top
}

int MonteCarlo::curWinner(int x, int y)
{
	return player == 2
		   ? (machineWin(x, y, n_row, n_col, board) ? 2 : (isTie(n_col, top) ? 0 : -1))
		   : (userWin(x, y, n_row, n_col, board) ? 1 : (isTie(n_col, top) ? 0 : -1));
}

int MonteCarlo::simulate(Node *v)
{
	height = std::max(height, v->depth);
	available_cols.clear();
	available_cols.reserve(12);
	for (int y = 0; y < n_col; ++y) {
		if (top[y] > 0) available_cols.push_back(y);
	}
	int player_cache = player;
	int winner, x = v->x, y = v->y;
	size_t i;
	while ((winner = curWinner(x, y)) == -1) {    // simulation continues
		i = randint(available_cols.size());
		y = available_cols[i];    // randomly pick a move
		x = top[y] - 1;
		makeMove(x, y);
		if (top[y] == 0) {    // delete an available move
			available_cols[i] = available_cols.back();
			available_cols.pop_back();
		}
	}
	player = player_cache;
	return winner == player ? 2 : (winner == 0 ? 1 : 0);    // win | tie | lose
}

void MonteCarlo::reset()
{
	player = 1;
	for (int i = 0; i < n_col; ++i) top[i] = top_cache[i];
	for (int i = 0; i < n_row; ++i)
		for (int j = 0; j < n_col; ++j) board[i][j] = board_cache[i][j];
//	memcpy(top, top_cache, n_col * sizeof(int));
//	memcpy(board, board_cache, n_row * n_col * sizeof(int));
}

void MonteCarlo::printBoard()
{
	for (int y = 0; y <= n_col; ++y) {
		logger("--")
	}
	logger("-\n")
	for (int x = 0; x < n_row; ++x) {
		logger("| ")
		for (int y = 0; y < n_col; ++y) {
			logger("%c ", board[x][y] == 2 ? 'x' : board[x][y] == 1 ? 'o' : ' ')
		}
		logger("|\n")
	}
	for (int y = 0; y <= n_col; ++y) {
		logger("--")
	}
	logger("-\n\n")
}


#endif //UCT_MONTECARLO_HPP
