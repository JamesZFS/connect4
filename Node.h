//
// Created by James on 2019/5/7.
//

#ifndef UCT_NODE_H
#define UCT_NODE_H

#include "libs.h"
#include "Point.h"

// Monte Carlo tree node
struct Node
{
//	int **status;	// board
	Point action;    // action move
	int profit = 0;            // Q
	int n_visited = 0;         // N

//	Node *parent;
//	Node **children;	// array of ptrs
//	short n_child;
	Node *parent = nullptr;
	short n_child = 0;
	Node *children[12] = {};    // array of ptrs
	int right_most_child = -1;

	Node *newChild(const Point &action_);

	static Node *nodes;
	static int cur_node;

	static int M;	// n_row
	static int N;	// n_column

	static Node *buildRoot();
};

#include "Node.cpp"

#endif //UCT_NODE_H
