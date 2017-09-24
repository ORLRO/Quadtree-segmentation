#include "Label.h"

int Label::latestIndex = 0;
vector<Label*> Label::allLabels;

Label::Label()
{
	createNewLabel();
}

void Label::setequivalentTo(Label in_Label)
{
	equivalentTo.push_back(in_Label);
}

Label * Label::get_Label_by_id(int i)
{
	return allLabels.at(i);
}

void Label::createNewLabel()
{
	index = latestIndex++;
	allLabels.push_back(this);
}
