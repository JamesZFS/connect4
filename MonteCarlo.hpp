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
	int top_cache[12], board_cache[12][12];
	int player;		// current player
	int height;		// tree height
	clock_t since;        // start time
	List<int> available_cols;
	static const clock_t CLOCK_LIMIT = (clock_t) (2.8 * CLOCKS_PER_SEC);

	Node *expand(Node *v);	// try to expand v's child, if failed, return nullptr

	int simulate(Node *v);	// random simulation to get profit for current board

	void updateAbove(Node *v, int delta);	// backup profit

	inline bool curIsFinal(int x, int y);	// current board with move v is at final

	inline int curWinner(int x, int y);		// current winner, -1 - not final, 0 - tie, 1 - user, 2 - machine

	inline int inverse(int delta);

	inline void makeMove(int x, int y);	// make move on current board

	inline void reset();	// reset board status

	void printBoard();

public:
	MonteCarlo(int M, int N, int *top_, int **board_,
			   const int lastX, const int lastY, const int noX, const int noY) :
			n_row(M), n_col(N), top(top_), board(board_), prev(lastX, lastY), ban(noX, noY),
			top_cache{}, board_cache{}, player(0), height(0), since(clock())
	{
		for (int i = 0; i < n_col; ++i) top_cache[i] = top_[i];
		for (int i = 0; i < n_row; ++i)
			for (int j = 0; j < n_col; ++j) board_cache[i][j] = board_[i][j];
//		memcpy(top_cache, top, n_col * sizeof(int));
//		memcpy(board_cache, board, n_row * n_col * sizeof(int));
		available_cols.reserve(12);
	}

	Point UCTSearch();	// return the best action
};

Point MonteCarlo::UCTSearch()
{
	Node::buildRoot();
	Node *v, *u;
	int delta, update_cnt = 0;
	do {
		v = Node::root();
		reset();
		// TreePolicy
		do {
			u = expand(v);
			if (u) {	// expandable
				v = u;
				makeMove(v->x, v->y);
				break;
			}
			else {		// not expandable
				v = v->bestChild(0.8);
				makeMove(v->x, v->y);
			}
		} while (!curIsFinal(v->x, v->y));

		delta = simulate(v);	// random simulation

		updateAbove(v, delta);

		++update_cnt;
	} while (clock() - since < CLOCK_LIMIT);	// time limit
	v = Node::root()->bestChild(0);
	logger("\n================\n\n")
	logger("x = %d, y = %d\n", v->x, v->y)
	logger("update_cnt = %d\n", update_cnt)
	logger("height = %d\n", height)
	logger("n_node = %d\n", Node::n_node())
	logger("ucb:\n")
	for (auto c : Node::root()->children) {
		logger("(%d, %d) :  %.4f\n", c->x, c->y, 1.0 * c->Q / c->N)
	}
	return {v->x, v->y};
}

bool MonteCarlo::curIsFinal(int x, int y)
{
	return player == 2
		   ? machineWin(x, y, n_row, n_col, board) || isTie(n_col, top)
		   : userWin(x, y, n_row, n_col, board) || isTie(n_col, top);
}

void MonteCarlo::updateAbove(Node *v, int delta)
{
	while (v) {
		++v->N;
		v->Q += delta;
		delta = inverse(delta);
		v = v->parent;
	}
}

int MonteCarlo::inverse(int delta)
{
	return 2 - delta;
}

Node *MonteCarlo::expand(Node *v)
{
	for (int y = v->rightmostChild() + 1; y < n_col; ++y) {
		if (top[y] > 0) {	// available move
			return v->newChild(top[y] - 1, y);
		}
	}
	return nullptr;	// not expandable
}

void MonteCarlo::makeMove(int x, int y)
{
	player = 3 - player;	// switch player
	top[y] = x;
	board[x][y] = player;
	if (ban.y == y && ban.x == x - 1) --top[y];	// assure no ban point above top
}

int MonteCarlo::simulate(Node *v)
{
	height = std::max(height, v->depth);
	available_cols.clear();
	for (int y = 0; y < n_col; ++y) {
		if (top[y] > 0) available_cols.push_back(y);
	}
	int player_cache = player;
	int winner, x = v->x, y = v->y, sim_cnt = 0;
	size_t i;
	while ((winner = curWinner(x, y)) == -1) {	// simulation continues
		++sim_cnt;
		i = randint(available_cols.size());
		y = available_cols[i];	// randomly pick a move
		x = top[y] - 1;
		makeMove(x, y);
		if (top[y] == 0) {	// delete an available move
			available_cols[i] = available_cols.back();
			available_cols.pop_back();
		}
	}
//	logger("depth = %d, sim_cnt = %d,  winner = %d\n", v->depth, sim_cnt, winner)
//	printBoard();
	player = player_cache;
	return winner == player ? 2 : (winner == 0 ? 1 : 0);	// win | tie | lose
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

int MonteCarlo::curWinner(int x, int y)
{
	return player == 2
		   ? (machineWin(x, y, n_row, n_col, board) ? 2 : (isTie(n_col, top) ? 0 : -1))
		   : (userWin(x, y, n_row, n_col, board) ? 1 : (isTie(n_col, top) ? 0 : -1));
}

static int __print_cnt__ = 0;

void MonteCarlo::printBoard()
{
	if (__print_cnt__ > 100) return;
	++__print_cnt__;
	logger(" ")
	for (int y = 0; y < n_col; ++y) {
		logger("--")
	}
	logger("\n")
	for (int x = 0; x < n_row; ++x) {
		logger("| ")
		for (int y = 0; y < n_col; ++y) {
			logger("%c ", board[x][y] == 2 ? 'x' : board[x][y] == 1 ? 'o' : ' ')
		}
		logger("|\n")
	}
	logger(" ")
	for (int y = 0; y < n_col; ++y) {
		logger("--")
	}
	logger("\n\n")
}


#endif //UCT_MONTECARLO_HPP
