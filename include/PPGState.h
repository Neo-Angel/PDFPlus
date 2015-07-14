

#ifndef __PDFPlusLib__PPGState__
#define __PDFPlusLib__PPGState__

#include <PPBase.h>
#include <PPDash.h>
#include <PPMatrix.h>
#include <PPColor.h>

//
//      PPGState
//
///////////////////////////////////////////////////////////////

enum PPGStateFlag {
    PPGF_NONE               = 0x00000000,
    PPGF_LINEWIDTH          = 0x00000001,
    PPGF_INTENT             = 0x00000002,
    PPGF_FLATNESS           = 0x00000004,
    PPGF_MITERLIMIT         = 0x00000008,
    PPGF_LINECAP            = 0x00000010,
    PPGF_LINEJOIN           = 0x00000020,
    PPGF_DICTNAME           = 0x00000040,
    PPGF_DASH               = 0x00000080,
    PPGF_MATRIX             = 0x00000100,
    PPGF_STROKECOLORSPC     = 0x00000200,
    PPGF_FILLCOLORSPC       = 0x00000400,
    PPGF_STROKECOLOR        = 0x00000800,
    PPGF_FILLCOLOR          = 0x00001000,
    PPGF_CLIPPING           = 0x00002000,
	PPGF_COLORSPACE			= 0x00004000,
    PPGF_SETFILLCOLOR       = 0x00008000,
    PPGF_SETSTROKECOLOR     = 0x00010000,
    PPGF_SETFILLCOLORN      = 0x00020000,
    PPGF_SETSTROKECOLORN    = 0x00040000
};


class PPGState : public PPBase {
public:
    float       _lineWidth;
    string       _intent;
    float       _flatness;
    float       _miterLimit;
	float		_stroke_opacity;
	float		_fill_opacity;
    byte        _lineCap;
    byte        _lineJoin;
    string      _dictName;


    PPDash      _dash;
    PPMatrix    _matrix;
    PPColor     _strokeColor;
    PPColor     _fillColor;
    
    unsigned long _gflag;
public:
    PPGState();
	PPBase *Create() {return new PPGState();}

	void CopyMembersTo(PPBase *obj);

    void setLineWidth(float v);
	void SetLineWidth(float v) {setLineWidth(v);}
    float lineWidth(){return _lineWidth;}
    void setIntent(string v);
    string intent(){return _intent;};
    void setFlatness(float v);
    float flatness(){return _flatness;}
    void setMiterLimit(float v);
    float miterLimit(){return _miterLimit;}
	void setStrokeOpacity(float v) {_stroke_opacity = v;}
	float strokeOpacity(){return _stroke_opacity;}
	void setFillOpacity(float v) {_fill_opacity = v;}
	float fillOpacity(){return _fill_opacity;}
	float GetFillOpacity() {return _fill_opacity;}
    void setLineCap(byte v);
    byte lineCap(){return _lineCap;}
    void setLineJoin(byte v);
    byte lineJoin(){return _lineJoin;}
    void setDictName(string s);
    string dictName(){return _dictName;}
    void setDash(PPDash d);
    PPDash dash(){return _dash;};
    void setMatrix(PPMatrix m);
    PPMatrix matrix(){return _matrix;}
	PPMatrix GetCTM(){return _matrix;}
	PPMatrix *getCTM(){return &_matrix;}
	PPMatrix *ptMatrix(){return &_matrix;}
    
    int numberOfStrokeColorCoponents();
    int numberOfNonStrokeColorCoponents();
    void setStrokeColorSpace(string name);
	void SetUserStrokeColorSpace(string name);
    string strokeColorSpace(){return _strokeColor._colorSpaceName;}
	void setStrokeColor(PPColor c);  void SetStrokeColor(PPColor c) {setStrokeColor(c);}
    void setStrokeColor(float c1, float c2 = -1, float c3 = -1, float c4 = -1);

    PPColor getStrokeColor(){return _strokeColor;}
    PPColor GetStrokeColor(){return _strokeColor;}
	PPColor *strokeColor(){return &_strokeColor;}
    
    void setFillColorSpace(string name);
	void SetUserFillColorSpace(string name);
    string fillColorSpace(){return _fillColor._colorSpaceName;};
	void setFillColor(PPColor c); void SetFillColor(PPColor c) {setFillColor(c);}
    void setFillColor(float c1, float c2 = -1, float c3 = -1, float c4 = -1);

	PPColor getFillColor(){return _fillColor;}
    PPColor *fillColor(){return &_fillColor;}
    
    void clearGFlags(){_gflag = PPGF_NONE;}
    unsigned long gFlags(){return _gflag;}
    void setGFlags(unsigned long flag){_gflag = flag;}
    
    string makeCommandString();
    string xmlString(int level);
	void setTransform(PPMatrix mtx){_matrix = mtx;}

	void SetStrokeColor(vector<PPToken *> &_operands);
	void SetStrokeColorN(vector<PPToken *> &_operands);
	void SetFillColor(vector<PPToken *> &_operands);
	void SetFillColorN(vector<PPToken *> &_operands);
};


#endif