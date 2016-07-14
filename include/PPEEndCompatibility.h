#pragma once
#include "ppelement.h"
class PPEEndCompatibility :
	public PPElement
{
public:
	PPEEndCompatibility(PPContext *gcontext) : PPElement(gcontext){}
	PPEEndCompatibility(void){};
	~PPEEndCompatibility(void){};



	PPBase *Create(){return new PPEEndCompatibility;}

	string MakeCommandString();
    string XMLString(int level);
	PPElementType Type() {return PPET_END_COMPATIBILITY;}

};

