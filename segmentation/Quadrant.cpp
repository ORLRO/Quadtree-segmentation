#include "Quadrant.h"

Quadrant::Quadrant(Mat_<unsigned char> in_img, const Quadrant * in_parent, int in_x0, int in_y0) :
	img(in_img),
	parent(in_parent),
	x0(in_x0), y0(in_y0)
{
	Qs[0][0] = NULL;		Qs[0][1] = NULL;
	Qs[1][0] = NULL;		Qs[1][1] = NULL;
	// assume all inner quadrants are NOT similar on creation
	siLabel[0][0] = 0;		siLabel[0][1] = 1;
	siLabel[1][0] = 2;		siLabel[1][1] = 3;

	//assume each quadrant is similar to its outer neighbors
	svoLabel[0][0] = true;	svoLabel[0][1] = true;
	svoLabel[1][0] = true;	svoLabel[1][1] = true;

	shoLabel[0][0] = true;	shoLabel[0][1] = true;
	shoLabel[1][0] = true;	shoLabel[1][1] = true;

}

Quadrant::~Quadrant()
{
	if (!isLeaf())
	{
		delete Qs[0][0]; delete Qs[0][1];
		delete Qs[1][0]; delete Qs[1][1];
	}
}

const Quadrant * Quadrant::get_first_right_parent(stack<bool>& vertical_moves) const
{
	if (isRoot()) return NULL;
	if (isTopLeft())
	{
		vertical_moves.push(0);
		return parent->get_first_right_parent(vertical_moves);
	}
	if (isBottomLeft())
	{
		vertical_moves.push(1);
		return parent->get_first_right_parent(vertical_moves);
	}
	return this;
}
const Quadrant * Quadrant::get_first_left_parent(stack<bool>& vertical_moves) const
{
	if (isRoot()) return NULL;
	if (isTopRight())
	{
		vertical_moves.push(0);
		return parent->get_first_left_parent(vertical_moves);
	}
	if (isBottomRight())
	{
		vertical_moves.push(1);
		return parent->get_first_left_parent(vertical_moves);
	}
	return this;
}
const Quadrant * Quadrant::get_first_top_parent(stack<bool>& horizontal_moves) const
{
	if (isRoot()) return NULL;
	if (isBottomLeft())
	{
		horizontal_moves.push(0);
		return parent->get_first_top_parent(horizontal_moves);
	}
	if (isBottomRight())
	{
		horizontal_moves.push(1);
		return parent->get_first_top_parent(horizontal_moves);
	}
	return this;
}
const Quadrant * Quadrant::get_first_bottom_parent(stack<bool>& horizontal_moves) const
{
	if (isRoot()) return NULL;
	if (isTopLeft())
	{
		horizontal_moves.push(0);
		return parent->get_first_bottom_parent(horizontal_moves);
	}
	if (isTopRight())
	{
		horizontal_moves.push(1);
		return parent->get_first_bottom_parent(horizontal_moves);
	}
	return this;
}

const Quadrant * Quadrant::get_immediate_left() const
{
	if (isLeft() || isRoot())
		return NULL;
	if (isTopRight())
		return parent->Qs[0][0];
	else //(this == parent->Qs[1][1])
		return parent->Qs[1][0];
}
const Quadrant * Quadrant::get_immediate_right() const
{
	if (isRight() || isRoot())
		return NULL;
	if (isTopLeft())
		return parent->Qs[0][1];
	else //(this == parent->Qs[1][0])
		return parent->Qs[1][1];
}
const Quadrant * Quadrant::get_immediate_top() const
{
	if (isTop() || isRoot())
		return NULL;
	if (isBottomLeft())
		return parent->Qs[0][0];
	else //(this == parent->Qs[1][1])
		return parent->Qs[0][1];
}
const Quadrant * Quadrant::get_immediate_bottom() const
{
	if (isBottom() || isRoot())
		return NULL;
	if (isTopLeft())
		return parent->Qs[1][0];
	else //(this == parent->Qs[0][1])
		return parent->Qs[1][1];
}

const Quadrant * Quadrant::get_right_child(stack<bool>& vertical_moves) const
{
	if (!vertical_moves.empty())
	{
		if (isLeaf())
			return this;
		bool v = vertical_moves.top();
		vertical_moves.pop();
		return Qs[v][1]->get_right_child(vertical_moves);
	}
	return this;
}
const Quadrant * Quadrant::get_left_child(stack<bool>& vertical_moves) const
{
	if (!vertical_moves.empty())
	{
		if (isLeaf())
			return this;
		bool v = vertical_moves.top();
		vertical_moves.pop();
		return Qs[v][0]->get_left_child(vertical_moves);
	}
	return this;
}
const Quadrant * Quadrant::get_top_child(stack<bool>& horizontal_moves) const
{
	if (!horizontal_moves.empty())
	{
		if (isLeaf())
			return this;
		bool h = horizontal_moves.top();
		horizontal_moves.pop();
		return Qs[0][h]->get_top_child(horizontal_moves);
	}
	return this;
}
const Quadrant * Quadrant::get_bottom_child(stack<bool>& horizontal_moves) const
{
	if (!horizontal_moves.empty())
	{
		if (isLeaf())
			return this;
		bool h = horizontal_moves.top();
		horizontal_moves.pop();
		return Qs[1][h]->get_bottom_child(horizontal_moves);
	}
	return this;
}

//finds all child nodes touching the right edge  
void Quadrant::get_right_children(vector<const Quadrant*>& results) const
{
	if (isLeaf())
		results.push_back(this);
	else
	{
		Qs[0][1]->get_right_children(results);
		Qs[1][1]->get_right_children(results);
	}
}
//finds all child nodes touching the bottom edge 
void Quadrant::get_bottom_children(vector<const Quadrant*>& results) const
{
	if (isLeaf())
		results.push_back(this);
	else
	{
		Qs[1][0]->get_bottom_children(results);
		Qs[1][1]->get_bottom_children(results);
	}
}

vector<const Quadrant*> Quadrant::get_left_adjacencies() const
{
	const Quadrant* left = get_left();
	if (left != NULL)
	{
		vector<const Quadrant*> adjs;
		left->get_right_children(adjs);
		return adjs;
	}
	return{};
}
vector<const Quadrant*> Quadrant::get_top_adjacencies() const
{
	const Quadrant* top = get_top();
	if (top != NULL)
	{
		vector<const Quadrant*> adjs;
		top->get_bottom_children(adjs);
		return adjs;
	}
	return{};
}

const Quadrant * Quadrant::get_left() const
{
	if (isLeft())
	{
		std::stack<bool> vertical_moves;
		//get first non left parent
		const Quadrant* first_non_left_parent = get_first_right_parent(vertical_moves);
		if (first_non_left_parent == NULL)
			return NULL;
		//get the left neighbor
		const Quadrant* parent_left = first_non_left_parent->get_immediate_left();
		//get right quadrant by back tracing the same vertical movements
		return parent_left->get_right_child(vertical_moves);
	}
	else
	{
		return get_immediate_left();
	}

}
const Quadrant * Quadrant::get_right() const
{
	if (isRight())
	{
		std::stack<bool> vertical_moves;
		//get first non left parent
		const Quadrant* first_non_right_parent = get_first_left_parent(vertical_moves);
		if (first_non_right_parent == NULL)
			return NULL;
		//get the left neighbor
		const Quadrant* parent_right = first_non_right_parent->get_immediate_right();
		//get right quadrant by back tracing the same vertical movements
		return parent_right->get_left_child(vertical_moves);
	}
	else
	{
		return get_immediate_right();
	}
}
const Quadrant * Quadrant::get_top() const
{
	if (isTop())
	{
		std::stack<bool> horizontal_moves;
		//get first non left parent
		const Quadrant* first_non_top_parent = get_first_bottom_parent(horizontal_moves);
		if (first_non_top_parent == NULL)
			return NULL;
		//get the left neighbor
		const Quadrant* parent_top = first_non_top_parent->get_immediate_top();
		//get right quadrant by back tracing the same vertical movements
		return parent_top->get_bottom_child(horizontal_moves);
	}
	else
	{
		return get_immediate_top();
	}
}
const Quadrant * Quadrant::get_bottom() const
{
	if (isBottom())
	{
		std::stack<bool> horizontal_moves;
		//get first non left parent
		const Quadrant* first_non_bottom_parent = get_first_top_parent(horizontal_moves);
		if (first_non_bottom_parent == NULL)
			return NULL;
		//get the left neighbor
		const Quadrant* parent_bottom = first_non_bottom_parent->get_immediate_bottom();
		//get right quadrant by back tracing the same vertical movements
		return parent_bottom->get_top_child(horizontal_moves);
	}
	else
	{
		return get_immediate_bottom();
	}
}

bool Quadrant::isRoot() const
{
	return parent == NULL;
}
bool Quadrant::isLeaf() const
{
	return Qs[0][0] == NULL;
}

bool Quadrant::isTopLeft() const { return this == parent->Qs[0][0]; }
bool Quadrant::isTopRight() const { return this == parent->Qs[0][1]; }
bool Quadrant::isBottomLeft() const { return this == parent->Qs[1][0]; }
bool Quadrant::isBottomRight() const { return this == parent->Qs[1][1]; }

bool Quadrant::isTop() const { return isTopLeft() || isTopRight(); }
bool Quadrant::isBottom() const { return isBottomLeft() || isBottomRight(); }
bool Quadrant::isRight() const { return isTopRight() || isBottomRight(); }
bool Quadrant::isLeft() const { return isTopLeft() || isBottomLeft(); }
