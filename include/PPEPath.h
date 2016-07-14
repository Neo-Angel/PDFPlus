
#ifndef __PDFPlusLib__PPEPath__
#define __PDFPlusLib__PPEPath__

#include <PPElement.h>
#include <PPPath.h>

class PPGState;
//
//      PPEPath
//
///////////////////////////////////////////////////////////////
class PPEPath : public PPElement {
public:
	PPPath *_path;

    PPEP_StrokeType _strokeType;
    PPEP_ClipType _clipType;
    PPEP_FillType _fillType;

	int _paintingType;
    
public:
	PPEPath(float x, float y, float w, float h, PPContext *gcontext);
	PPEPath(float x, float y, float w, float h);
    PPEPath(PPPath *path, PPContext *gcontext);
    PPEPath(PPContext *gcontext);
	PPEPath(PPGState *gstate):PPElement(gstate){};
	PPEPath(){}
	
	PPBase *Create(){return new PPEPath;}
	void CopyMembersTo(PPBase *obj);
    string MakeCommandString();
    string XMLString(int level);
    
    void Stroke();
    void Fill();
    void EOFill();
    void Clip();
    void EOClip();
//	void SetNoPaint() {_paintingType = PPC_NoPaint;}

	inline bool IsFilled() { return (_fillType != PPEP_NonFill);}
	inline bool IsStroked() { return (_strokeType != PPEP_NonStroke);}
	inline bool IsClipped() { return (_clipType != PPEP_NonClip);}


	void SetPathStroke(bool flag) {_strokeType = flag ? PPEP_Stroke : PPEP_NonStroke;}
	void SetPathFill(bool flag) {_fillType = flag ? PPEP_EOFill : PPEP_NonFill;}
	PPPath *Path(){return _path;}
	void SetPathClip(bool flag) {_clipType = flag ? PPEP_EOClip : PPEP_NonClip;}

    void SetPaintingType(int type);
	PPElementType Type() {return PPET_PATH;}
	PPRect BBox() {return _path->BBox();}

	void AddRect(float x, float y, float w, float h);
	void MoveTo(float x, float y) { _path->MoveTo(x, y);}
	void LineTo(float x, float y) { _path->LineTo(x, y);}
	void CurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y) {
		_path->CurveTo(cx1, cy1, cx2, cy2, x, y);
	}
	void Close() { _path->Close(); }



};


#endif