#pragma once
#include "PPElement.h"

class PPEBeginCompatibility : public PPElement
{
public:
	PPEBeginCompatibility(PPContext *gcontext) : PPElement(gcontext){}
	PPEBeginCompatibility(void){}
	~PPEBeginCompatibility(void){}

	PPBase *Create(){return new PPEBeginCompatibility;}

	string MakeCommandString();
    string XMLString(int level);
	PPElementType Type() {return PPET_BEGIN_COMPATIBILITY;}

};

