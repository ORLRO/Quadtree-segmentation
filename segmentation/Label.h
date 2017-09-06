#pragma once

#include <vector>
using namespace std;

class Label
{
public:
	Label();
	void setequivalentTo(int in_number);
	static Label* get_instance_by_id(int i);
private:
	void createNewLabel();
private:
	int index;
	vector<int> equivalentTo;
	static int latestIndex;// by default initilized to 0!
	static vector<Label*> allLabels;
};
