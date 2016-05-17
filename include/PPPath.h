
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

	void CheckBounds(float x, float y); // 주워진 x, y포인트를 포함한 path의 바우드 영역(_bounds)을 계산한다.
	void MoveTo(float x, float y);
	void LineTo(float x, float y);
    void CurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y);
    void CurveTo1(float cx2, float cy2, float x, float y);
    void CurveTo2(float cx1, float cy1, float x, float y);
    void Rectangle(float x, float y, float w, float h);
	void Close();
    string XMLString(int level);

	size_t NumberOfItems() {return _itemList.size();}
	PPPathItem *ItemAtIndex(size_t idx) {return _itemList.at(idx);}
	PPRect BBox(){return _bounds;}
};


#endif