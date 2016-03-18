#pragma once
#include "PPElement.h"

class PPEBeginCompatibility : public PPElement
{
public:
	PPEBeginCompatibility(PPContext *gcontext) : PPElement(gcontext){}
	PPEBeginCompatibility(void){}
	~PPEBeginCompatibility(void){}

	PPBase *Create(){return new PPEBeginCompatibility;}

	string makeCommandString();
    string XMLString(int level);
	PPElementType getType() {return PPET_BEGIN_COMPATIBILITY;}

};

