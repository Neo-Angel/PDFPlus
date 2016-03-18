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
	PPET_TEXT_STATE,
    PPET_FORM,
    PPET_SHADING,
	PPET_BEGIN_MARKED_CONTENT,
	PPET_MARKED_CONTENT,
	PPET_END_MARKED_CONTENT,
	PPET_BEGIN_COMPATIBILITY,
	PPET_END_COMPATIBILITY//    PPET_FORMSTART,
//    PPET_FORMEND
};

// ExtGState, ColorSpace, Pattern, Shading, XObject, Font, ProcSet, Properties 
extern const char *PPRT_EXTSTATE;
extern const char *PPRT_COLORSPC;
extern const char *PPRT_PATTERN;
extern const char *PPRT_SHADING;
extern const char *PPRT_XOBJECT;
extern const char *PPRT_FONT;
extern const char *PPRT_PROSET;
extern const char *PPRT_PROPERTIES;

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
	virtual string XMLString(int level) {return "";}
	virtual PPElementType getType() {return PPET_NONE;}
	virtual PPRect getBBox() {return _bounds;}
	virtual void SetParser(PPParser *parser) {};

	virtual bool HasResource();
	virtual vector <const char *> ResourceTypeList();
	virtual string ResourceKeyFor(const char *rsc_type);
	virtual int ResourceObjNum(const char *rsc_type);
	virtual PPToken *GetResource(const char *rsc_type);
	//virtual PPToken *GetRealResource(string rsc_type, string rsc_key);
	virtual string commandString();

	PPBase *Create() {return new PPElement();}
	void CopyMembersTo(PPBase *obj);

	PPGState *getGState(){return _gstate;}
	PPGState *GetGState(){return _gstate;}
	PPMatrix *getCTM() {return _gstate->getCTM();}
	PPElementType GetType() {return getType();}
	PPRect GetBBox() {return getBBox();}
	PPDocument *GetDocument();

	void SetLineWidth(float v) {_gstate->setLineWidth(v);}
    float LineWidth(){return _gstate->_lineWidth;}
	void SetIntent(string v) {_gstate->setIntent(v);}
    string Intent(){return _gstate->_intent;};
    void SetFlatness(float v) {_gstate->setFlatness(v);}
    float Flatness(){return _gstate->_flatness;}
    void SetMiterLimit(float v){_gstate->setMiterLimit(v);}
    float MiterLimit(){return _gstate->_miterLimit;}
	void SetStrokeOpacity(float v) {_gstate->_stroke_opacity = v;}
	float StrokeOpacity(){return _gstate->_stroke_opacity;}
	void SetFillOpacity(float v) {_gstate->_fill_opacity = v;}
	float FillOpacity(){return _gstate->_fill_opacity;}
    void SetLineCap(byte v){_gstate->setLineCap(v);}
    byte LineCap(){return _gstate->_lineCap;}
    void SetLineJoin(byte v){_gstate->setLineJoin(v);}
    byte LineJoin(){return _gstate->_lineJoin;}
    void SetDash(PPDash d){_gstate->setDash(d);}
    PPDash Dash(){return _gstate->_dash;};

	void SetStrokeColor(PPColor c) {_gstate->setStrokeColor(c);}
	void SetFillColor(PPColor c) {_gstate->setFillColor(c);}

	void SetTransform(float a, float b, float c, float d, float x, float y);
	void SetTransform(PPMatrix *mtx);
};



#endif /* defined(__PDFPlusLib__PPElement__) */
