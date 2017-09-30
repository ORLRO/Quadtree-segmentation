#pragma once
//#include "Quadrant.h"

#include <queue>
#include <stack>

using namespace std;

class Label
{
private:
	Label();
	void initializeNewLabel();
public:
	void setequivalentTo(Label* in_Label);
	static Label* get_Label_by_id(int i);
	int get_id() { return index; }
	static Label* get_unusedLabel();
	void addToStack(Quadrant*);
	
private:
	int index;
	stack<Quadrant*> PointingToMe;
	static int latestIndex;
	static vector<Label*> allLabels;
	static queue<Label*> unusedLabels;
};
