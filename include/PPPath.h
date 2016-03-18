
#ifndef __PDFPlusLib__PPPath__
#define __PDFPlusLib__PPPath__

#include <PPBase.h>
#include <PPCommandParser.h>
#include <PPCoord.h> // for PPPoint


enum PPEP_StrokeType {
    PPEP_NonStroke,
    PPEP_Stroke
};

enum PPEP_FillType {
    PPEP_NonFill,
    PPEP_Fill,
    PPEP_EOFill
};

enum PPEP_ClipType {
    PPEP_NonClip,
    PPEP_Clip,
    PPEP_EOClip
};


//
//      PPPath
//
///////////////////////////////////////////////////////////////
class PPPathItem;

class PPPath : public PPBase {
    
public:
    vector<PPPathItem *> _itemList;
    PPRect _bounds;

public:
    PPPath(){}
	PPBase *Create() {return new PPPath;}

	void CopyMembersTo(PPBase *obj);

	void checkBounds(float x, float y);
	void moveTo(float x, float y); void MoveTo(float x, float y) {moveTo(x, y);}
	void lineTo(float x, float y); void LineTo(float x, float y) {lineTo(x, y);}
    void curveTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
    void CurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y)
	{ curveTo(cx1, cy1, cx2, cy2, x, y);}
    void curveTo1(float cx2, float cy2, float x, float y);
    void curveTo2(float cx1, float cy1, float x, float y);
    void rectangle(float x, float y, float w, float h);
	void close(); void ClosePath() {close();}
    string XMLString(int level);

	size_t itemCount() {return _itemList.size();}
	PPPathItem *itemAtIndex(size_t idx) {return _itemList.at(idx);}
	PPRect getBBox(){return _bounds;}
};


#endif