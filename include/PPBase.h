//
//  PPBase.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPBase__
#define __PDFPlusLib__PPBase__

#include <iostream>
#include "PPDefines.h"

using namespace std;

extern const char *PPClassTypeName[];

typedef enum _PPClassType{
	PPTN_BASE = 0,
	PPTN_TOKEN,
	PPTN_NUMBER,
	PPTN_NAME,
	PPTN_STRING,
	PPTN_INDIRECTOBJ,
	PPTN_INDIRECTREF,
	PPTN_DICTIONARY,
	PPTN_ARRAY,
	PPTN_STREAM,
	PPTN_TRAILER,
	PPTN_XREF,
	PPTN_COMMAND_PARSER
} PPClassType;


// PPTN_XXXX : 'XXXX'는 클래스명(대문자) 
// 각 클래스 마다 정의되는 Class Type Name


//static const char	*PPTN_BASE = "PPBase";


// 모든 클래스의 기반(Root) 클래스
// 주위 : '클래스'라는 한글 단어 바로 아래 'class' 문이 오면 컴파일 에러 발생
class PPBase {
public:
	PPBase *_clone;
public:
	PPBase() {_clone = NULL;}
    virtual string Description(); // 클래스에 대한 설명을 리턴
    virtual string TypeName();   // class type 을 string으로 변환해서 리턴 
    virtual PPClassType ClassType();  // return PPTN_BASE;

	// 모든 하위 클래스들이 반드시 구현해 줘야 하는 함수.
	// C++ 특성상 크래스 타입에 대한 조작이 유연하지 않아서 필요한 함수.
	// 타 언어에서는 스트링(클래스명)을 클래스 객체를 변환할 수 있으며
	// 클래스 객체로 인스탄스를 생성할 수 있으나, C++에서는 이것이 불가능 함. 
	virtual PPBase *Create() {return new PPBase();}

	 // 객체를 복사하기위한 기반함수. 상속할 팔요는 없음
	virtual PPBase *Copy(); 

	// 객체를 복사하기 위한 실제 함수
	// 객체 복사에 필요한 멤버 변수들을 복사하는 기능을 구현해야 함.
	virtual void CopyMembersTo(PPBase *obj); 
};




#endif /* defined(__PDFPlusLib__PPBase__) */
