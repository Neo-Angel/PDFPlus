//
//  PPBase.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


#include <sstream>
#include "PPBase.h"



string stringFromInt(int n)
{
    ostringstream ostr;
    ostr << n;
    string sResult= ostr.str();
    return sResult;
}

string stringFromFloat(float n)
{
    ostringstream ostr;
    ostr << n;
    string sResult= ostr.str();
    return sResult;
}

string *newStringFromInt(int n)
{
    string *sResult = new string();
    ostringstream ostr;
    ostr << n;
    sResult->append(ostr.str());
    return sResult;
}

string *newStringFromFloat(float n)
{
    string *sResult = new string();
    ostringstream ostr;
    ostr << n;
    sResult->append(ostr.str());
    return sResult;
}

PPBase::~PPBase()
{
    
}

template<class T>
bool PPBase::isKindOfClass()
{
    T obj = dynamic_cast<T>(*this);
    if (obj != NULL) {
        return true;
    }
    return false;
}

string PPBase::description()
{
    string retstr(typeid(this).name());
    retstr = retstr + " class";
    return retstr;
}

string PPBase::typeName()
{
    return string(classType());
}

const char *PPBase::classType()
{
    return PPTN_BASE;
}


void PPBase::CopyMembers(PPBase *obj)
{

}

PPBase *PPBase::Copy()
{
	PPBase *new_obj = this->Create();
	CopyMembers(new_obj);

	return new_obj;
}