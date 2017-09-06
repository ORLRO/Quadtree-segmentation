#include "Label.h"

int Label::latestIndex = 0;
vector<Label*> Label::allLabels;

Label::Label()
{
	createNewLabel();
}

void Label::setequivalentTo(int in_number)
{
	equivalentTo.push_back(in_number);
}

Label * Label::get_instance_by_id(int i)
{
	return allLabels.at(i);
}

void Label::createNewLabel()
{
	index = latestIndex++;
	allLabels.push_back(this);
}
