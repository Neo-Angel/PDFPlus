//
//  PPBase.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


#include <sstream>
#include "PPBase.h"




const char *PPClassTypeName[] = {
	"PPTN_BASE",
	"PPToken",
	"PPTNumber",
	"PPTName",
	"PPTString",
	"PPTIndirectObj",
	"PPTIndirectRef",
	"PPTDictionary",
	"PPTArray",
	"PPTStream",
	"PPTTrailer",
	"PPTXRef",
	"PPTN_COMMAND_PARSER"
};

///////////////////////////////
//
//    Utility Functions
//
///////////////////////////////
string PPStringFromInt(int n)
{
    ostringstream ostr;
    ostr << n;
    string sResult= ostr.str();
    return sResult;
}

string PPStringFromFloat(float n)
{
    ostringstream ostr;
    ostr << n;
    string sResult= ostr.str();
    return sResult;
}

string *PPNewStringFromInt(int n)
{
    string *sResult = new string();
    ostringstream ostr;
    ostr << n;
    sResult->append(ostr.str());
    return sResult;
}

string *PPNewStringFromFloat(float n)
{
    string *sResult = new string();
    ostringstream ostr;
    ostr << n;
    sResult->append(ostr.str());
    return sResult;
}

///////////////////////////////////
//
//    Class Methods Definition
//
//////////////////////////////////
/*
template<class T>
bool PPBase::isKindOfClass()
{
    T obj = dynamic_cast<T>(*this);
    if (obj != NULL) {
        return true;
    }
    return false;
}
*/

// 클래스에 대한 설명혹은 정보를 문자열로 리턴
string PPBase::Description()
{
    string retstr(typeid(this).name());
    retstr = retstr + " class";
    return retstr;
}

// class type 을 C string으로 변환해서 리턴
// C++에서 클래스 타입을 알아낼 수 있는 표준화된 방법이 마땅치 않아 
// 궁여지책으로 생각해 낸 방법
PPClassType PPBase::ClassType()
{
    return PPTN_BASE;
}

// class type 을 string으로 변환해서 리턴
string PPBase::TypeName()
{
    return string(PPClassTypeName[ClassType()]);
}

// 객체를 복사하기 위한 실제 함수
// 객체 복사에 필요한 멤버 변수들을 복사하는 기능을 구현해야 함.
void PPBase::CopyMembersTo(PPBase *obj)
{

}

// 객체를 복사하기위한 기반함수. 상속할 팔요는 없음
PPBase *PPBase::Copy()
{
	PPBase *new_obj = this->Create();
	new_obj->_clone = this;
	CopyMembersTo(new_obj);

	return new_obj;
}