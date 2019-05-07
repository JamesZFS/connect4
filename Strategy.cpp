#include <unistd.h>#include <cmath>#include "Strategy.h"const clock_t CLOCK_LIMIT = (clock_t) (1.5 * CLOCKS_PER_SEC);std::string LOG_DIR = "/Users/james/Test/AI/Chess/Strategy/logs/";std::ofstream fout;Buffer buffer;int height = 0;unsigned int round_cnt = 0;clock_t since;/*	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误		input:		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性		M, n_visited : 棋盘大小 M - 行数 n_visited - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])				todo board[x][y]表示第x行、第y列的点(从0开始计)				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略		noX, noY : 棋盘上的不可落子点(注:涫嫡饫锔?龅膖op已经替你处理了不可落子点，也就是说如果某一步				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)		以上参数实际上包含了当前状态(M n_visited _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点	output:		你的落子点Point*/extern "C" Point *getPoint(const int M, const int N, const int *top, const int *_board,						   const int lastX, const int lastY, const int noX, const int noY){	/*		不要更改这段代码	*/	int x = -1, y = -1;//todo 最终将你的落子点存到x,y中	int **board = new int *[M];	for (int i = 0; i < M; i++) {		board[i] = new int[N];		for (int j = 0; j < N; j++) {			board[i][j] = _board[i * N + j];		}	}	/*		根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值		该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件	*/	Node::N = N;	Node::M = M;	++round_cnt;	srand(round_cnt);	// todo use time	sprintf(buffer, "%s/move - %d.txt", LOG_DIR.data(), round_cnt);	fout.open(buffer);                // open debugger	if (!fout.is_open()) exit(1);	since = clock();    // timing start	int *top_ = new int[N];	memcpy(top_, top, N * sizeof(int));	root_status.bind(board, top_);    // init status	auto move = uctSearch();        // search from root	x = move.x;	y = move.y;	logger("x = %d, y = %d\n\n", x, y)	logger("total nodes: %d\nheight: %d\n", Node::cur_node, height)	fout.close();	/*		不要更改这段代码	*/	clearArray(M, N, board);	return new Point(x, y);}/*	getPoint函数返回的Point指针是在本dll模块中声明的，为避免产生堆错误，应在外部调用本dll中的	函数来释放空间，而不应该在外部直接delete*/extern "C" void clearPoint(Point *p){	delete p;}/*	清除top和board数组*/void clearArray(int M, int N, int **board){	for (int i = 0; i < M; i++) {		delete[] board[i];	}	delete[] board;}// my strategies:int cur_player;Point uctSearch(){	debug("uctSearch()\n")	Node *v0 = Node::buildRoot(), *vl;	int delta;	while (clock() - since < CLOCK_LIMIT) {		vl = treePolicy(v0);		delta = randPolicy();		updateAbove(vl, delta);	}	return bestChild(v0, 0)->action;}Node *treePolicy(Node *v){	debug("treePolicy()\n")	cur_player = 0;    // user move at root	resetStatus();	while (!atTerminal(v->action)) {	// check terminal and switch cur_player		auto expansion = expand(v);		if (expansion) {			debug("expand: action(%d, %d)\n", expansion->action.x, expansion->action.y);			makeMove(expansion->action, cur_player);			return expansion;		}		else {	// nullptr			v = bestChild(v, 0.8);			debug("select bestChild: action(%d, %d)\n", v->action.x, v->action.y);			makeMove(v->action, cur_player);		}	}	return v;	// at best terminal child}Node *expand(Node *v){	for (int i = v->right_most_child + 1; i < Node::N; ++i) {		if (cur_status.top[i] > 0) {	// an available move			Point action(cur_status.top[i] - 1, i);			v->right_most_child = action.y;			return v->newChild(action);		}	}	debug("expansion failed\n")	return nullptr;	// no available move}Node *bestChild(Node *v, double c){	double maximum = -INFINITY, f;	Node *vc, *argmax = nullptr;//	float maximum = -100;	for (int i = 0; i < v->n_child; ++i) {		vc = v->children[i];		f = vc->profit * 1.0 / vc->n_visited + c * sqrt(2.0 * log(v->n_visited) / vc->n_visited);		if (f > maximum) {			maximum = f;			argmax = vc;		}	}	return argmax;}int randPolicy(){	debug("randPolicy()\n")	switchPlayer(cur_player);	int _player = cur_player;	// cache	std::vector<int> cans;	// candidate tops	cans.reserve(12);	for (int i = 0; i < Node::N; ++i) {		if (cur_status.top[i] > 0) cans.push_back(i);	}//	debug("candidate tops in column: ")//	for (auto c: cans) {//		debug("%d ", c)//	}//	debug("\n\nsimulation starts\n")	Point action;	size_t i;	do {		i = randint(cans.size());		action.y = cans[i];	// take random action		action.x = cur_status.top[action.y] - 1;//		debug("current player = %d, move = (%d, %d)\n", cur_player, action.x, action.y)		makeMove(action, cur_player);		if (action.x == 0) {			cans[i] = cans.back();    // delete one candidate			cans.pop_back();		}//		debug("cans left: ")//		for (auto c: cans) {//			debug("%d ", c)//		}//		debug("\n\n")	} while (/*!cans.empty() && */!atTerminal(action));	// reach a terminal	switchPlayer(cur_player);	int w = winner(action, cur_player);	debug("winner: %d\n", w)	return w == _player ? 2 : (w == 0 ? 1 : 0);//	debug("simulation ends, Q = %d\n", Q)}void updateAbove(Node *v, int delta){	debug("updateAbove()\n")	int depth = 0;	while (v) {		++depth;		++v->n_visited;		v->profit += delta;		delta = 2 - delta;		v = v->parent;	}	if (depth > height) height = depth;}bool atTerminal(const Point &action){	switch (cur_player) {		case 0:    // root (user)			cur_player = 2;			return false;		case 1:    // user			cur_player = 2;    // switch cur_player			return userWin(action.x, action.y, Node::M, Node::N, cur_status.board) ||				   isTie(Node::N, cur_status.top);		case 2:    // machine			cur_player = 1;			return machineWin(action.x, action.y, Node::M, Node::N, cur_status.board) ||				   isTie(Node::N, cur_status.top);		default:			exit(1);	}}