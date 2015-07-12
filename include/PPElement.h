//
//  PPElement.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 9. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPElement__
#define __PDFPlusLib__PPElement__

#include <iostream>
#include <stack>
#include <vector>

#include <PPCoord.h>
#include <PPCommandParser.h>
#include <PPDocument.h>
#include <PPGState.h>

using namespace std;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  Element Start
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
enum PPElementType {
	PPET_NONE,
    PPET_GSAVE,
    PPET_GRESTORE,
    PPET_PATH,
    PPET_IMAGE,
	PPET_INLINE_IMAGE,
    PPET_TEXT,
    PPET_FORM,
    PPET_SHADING,
	PPET_BEGIN_MARKED_CONTENT,
	PPET_MARKED_CONTENT,
	PPET_END_MARKED_CONTENT,
	PPET_BEGIN_COMPATIBILITY,
	PPET_END_COMPATIBILITY//    PPET_FORMSTART,
//    PPET_FORMEND
};

//
//      PPElement
//
///////////////////////////////////////////////////////////////
class PPFormBase;
class PPContext;


enum PPGStateFlag;

class PPElement : public PPBase {
    
public:  
	// ref:
    PPFormBase *_parentForm;

	// attr:
    PPGState *_gstate;
    PPGStateFlag _gflag;
	PPRect _bounds;
    
    PPElement(PPContext *gcontext);
	PPElement(PPGState *gstate);
	PPElement();
	
    ~PPElement();
    virtual void willAddToParent(PPFormBase *form);
	virtual string makeCommandString() {return "";}
	virtual string xmlString(int level) {return "";}
	virtual PPElementType getType() {return PPET_NONE;}
	virtual PPRect getBBox() {return _bounds;}
	virtual void SetParser(PPParser *parser) {};
	virtual bool HasResource();
	virtual string ResourceType();
	virtual string ResourceKey();
	virtual PPToken *GetResource();

	PPBase *Create() {return new PPElement();}
	void CopyMembersTo(PPBase *obj);

	
    string commandString();
	PPGState *getGState(){return _gstate;}
	PPGState *GetGState(){return _gstate;}
	PPMatrix *getCTM() {return _gstate->getCTM();}
	PPElementType GetType() {return getType();}
	PPRect GetBBox() {return getBBox();}


};



#endif /* defined(__PDFPlusLib__PPElement__) */
