
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
    
public:
    PPEPath(PPPath *path, PPContext *gcontext);
    PPEPath(PPContext *gcontext);
	PPEPath(PPGState *gstate):PPElement(gstate){};
	PPEPath(){}
	PPBase *Create(){return new PPEPath;}

	void CopyMembers(PPBase *obj);
    string makeCommandString();
    string xmlString(int level);
    
    void stroke();
    void fill();
    void eofill();
    void clip();
    void eoclip();

	inline bool isFilled() { return (_fillType != PPEP_NonFill);}
	inline bool IsFilled() { return (_fillType != PPEP_NonFill);}
	inline bool isStroked() { return (_strokeType != PPEP_NonStroke);}
	inline bool IsStroked() { return (_strokeType != PPEP_NonStroke);}
	inline bool isClipped() { return (_clipType != PPEP_NonClip);}
	inline bool IsClipped() { return (_clipType != PPEP_NonClip);}
	inline bool IsClippingPath() { return (_clipType != PPEP_NonClip);}


	void setPathStroke(bool flag) {_strokeType = flag ? PPEP_Stroke : PPEP_NonStroke;}
	void SetPathStroke(bool flag) {_strokeType = flag ? PPEP_Stroke : PPEP_NonStroke;}
	void setPathFill(bool flag) {_fillType = flag ? PPEP_EOFill : PPEP_NonFill;}
	void SetPathFill(bool flag) {_fillType = flag ? PPEP_EOFill : PPEP_NonFill;}
	PPPath *path(){return _path;}
	void SetPathClip(bool flag) {_clipType = flag ? PPEP_EOClip : PPEP_NonClip;}

    void setPaintingType(int type);
	PPElementType getType() {return PPET_PATH;}
	PPPath *getPath() {return _path;}
	PPRect getBBox() {return _path->getBBox();}

};


#endif