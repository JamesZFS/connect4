//
// Created by James on 2019/5/7.
//

#ifndef UCT_STATUS_H
#define UCT_STATUS_H

#include <cstring>

struct Status
{
	int **board = nullptr;
	int *top = nullptr;

	inline void bind(int **board_, int *top_)
	{
		board = board_;
		top = top_;
	}

	inline void copyFrom(const Status &other, int M, int N)
	{
		if (!board) {
			board = new int *[M];
			for (int i = 0; i < M; ++i)
				board[i] = new int[N];
		}
		if (!top)
			top = new int[N];
		for (int i = 0; i < M; ++i) {
			memcpy(board[i], other.board[i], N * sizeof(int));
		}
		memcpy(top, other.top, N * sizeof(int));
	}
};

#endif //UCT_STATUS_H
