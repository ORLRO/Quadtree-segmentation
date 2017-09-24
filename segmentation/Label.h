#pragma once

#include <vector>
using namespace std;

class Label
{
public:
	Label();
	void setequivalentTo(Label* in_Label);
	static Label* get_Label_by_id(int i);
	int get_id() { return index; }
private:
	void createNewLabel();
private:
	int index;
	vector<Label*> equivalentTo;
	static int latestIndex;
	static vector<Label*> allLabels;
};
