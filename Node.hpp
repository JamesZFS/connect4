//
// Created by James on 2019/5/7.
//

#ifndef UCT_NODE_HPP
#define UCT_NODE_HPP

#include "libs.h"
#include "Point.h"
#include <cmath>

#define MAX_N_NODE 3000000

// Monte Carlo tree node
struct Node
{
	int x, y;  // action of the parent
	int Q;         // Q
	int N;         // N
	int depth;

	Node *parent = nullptr;
	List<Node *> children;

	Node *newChild(int x, int y);

	Node *bestChild(double c);

	static Node *buildRoot();

	inline static Node *root();

	inline int rightmostChild();

	inline static int n_node();

private:
	Node() : x(0), y(0), Q(0), N(0), depth(0), parent(nullptr)
	{ children.reserve(12); } // maximum n_col

	static Node nodes[MAX_N_NODE];

	static int cur_node;
};


Node Node::nodes[MAX_N_NODE];    // init in the beginning
int Node::cur_node = 0;

Node *Node::newChild(int x, int y)
{
	assure(cur_node < MAX_N_NODE, "! (cur_node < MAX_N_NODE) fails")
	auto &child = nodes[cur_node++];    // new child
	child.x = x;
	child.y = y;
	child.Q = child.N = 0;
	child.depth = this->depth + 1;
	child.parent = this;
	child.children.clear();
	child.children.reserve(12);
	this->children.push_back(&child);
	return &child;
}

Node *Node::buildRoot()
{
	auto &root = nodes[0];
	cur_node = 1;
	root.x = root.y = -1;
	root.Q = root.N = 0;
	root.depth = 0;
	root.parent = nullptr;
	root.children.clear();
	root.children.reserve(12);
	return &root;
}

Node *Node::bestChild(double c)
{
	double maximum = -INFINITY, f;
	Node *argmax = nullptr;
	for (auto x : children) {
		f = 1.0 * x->Q / x->N + c * sqrt(2.0 * log(this->N) / x->N);
		if (f > maximum) {
			maximum = f;
			argmax = x;
		}
	}
	return argmax;
}

Node *Node::root()
{
	return &nodes[0];
}

int Node::rightmostChild()
{
	return children.empty() ? -1 : children.back()->y;
}

int Node::n_node()
{
	return cur_node;
}

#endif //UCT_NODE_HPP
