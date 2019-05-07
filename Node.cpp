//
// Created by James on 2019/5/7.
//

#include "Node.h"

#define MAX_N_NODE 3000000

Node *Node::nodes = new Node[MAX_N_NODE];
int Node::cur_node = 0;
int Node::M = 0;	// n_row
int Node::N = 0;	// n_row

Node *Node::newChild(const Point &action_)
{
	assure(cur_node < MAX_N_NODE, "(cur_node < MAX_N_NODE) fails");
	auto &child = nodes[cur_node++];    // new child
	assure(n_child < N, "(n_child < N) fails");
	children[n_child++] = &child;
	child.action = action_;
	child.profit = child.n_visited = 0;
	child.parent = this;
	child.n_child = 0;
	child.right_most_child = -1;
	return &child;
}

Node *Node::buildRoot()
{
	auto &root = nodes[0];
	root.action = Point(-1, -1);
	root.profit = root.n_visited = 0;
	root.parent = nullptr;
	root.n_child = 0;
	root.right_most_child = -1;
	cur_node = 1;
	return &root;
}

Node &Node::root()
{
	return nodes[0];
}
