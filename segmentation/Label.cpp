#include "Label.h"

int Label::latestIndex = 0;
vector<Label*> Label::allLabels;

Label::Label()
{
	initializeNewLabel();
}

void Label::initializeNewLabel()
{
	index = latestIndex++;
	allLabels.push_back(this);
}

void Label::setequivalentTo(Label* in_Label)
{
	stack<Quadrant*>& PointingToOther = in_Label->PointingToMe;
	if (PointingToMe.size() > PointingToOther.size())
	{
		while (PointingToOther.size())
		{
			auto quad = PointingToOther.top();
			PointingToOther.pop();
			quad->set_Label(this);
		}
		unusedLabels.push(in_Label);
	} 
	else
	{
		while (PointingToMe.size())
		{
			auto quad = PointingToMe.top();
			PointingToMe.pop();
			quad->set_Label(in_Label);
		}
		unusedLabels.push(this);
	}
}

Label * Label::get_Label_by_id(int i)
{
	return allLabels.at(i);
}

Label * Label::get_unusedLabel()
{
	if (unusedLabels.size())
	{
		auto lbl = unusedLabels.front();
		unusedLabels.pop();
		return lbl;
	} 
	else
	{
		return new Label();
	}
}

void Label::addToStack(Quadrant* in_quad)
{
	PointingToMe.push(in_quad);
}


