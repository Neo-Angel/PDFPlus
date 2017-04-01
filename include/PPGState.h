

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

// 수정된 상태 정보 타입들의 bit mask 정의
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

class PPElement;
class PPFormBase;


// Graphic Context의 상태정보를 담고있다.
class PPGState : public PPBase {
public:
	PPElement *	_parent;

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
    
    unsigned long _gflag; // 수정된 상태 정보들을 bit mask 형태로 보관한다.
	unsigned long _gflag_save;

public:
    PPGState();
	PPBase *Create() {return new PPGState();}

	PPBase *Copy();
	PPBase *Copy(PPElement *tar_element);
	void CopyMembersTo(PPBase *obj);
	void MoveColorTo(PPColor *color, PPFormBase *tar_form);

	void SetLineWidth(float v);
    float LineWidth(){return _lineWidth;}
    void SetIntent(string v);
    string Intent(){return _intent;};
    void SetFlatness(float v);
    float Flatness(){return _flatness;}
    void SetMiterLimit(float v);
    float MiterLimit(){return _miterLimit;}
	void SetStrokeOpacity(float v) {_stroke_opacity = v;}
	float StrokeOpacity(){return _stroke_opacity;}
	void SetFillOpacity(float v) {_fill_opacity = v;}
	float FillOpacity(){return _fill_opacity;}
    void SetLineCap(byte v);
    byte LineCap(){return _lineCap;}
    void SetLineJoin(byte v);
    byte LineJoin(){return _lineJoin;}
    void SetDictName(string s);
    string DictName(){return _dictName;}
    void SetDash(PPDash d);
    PPDash Dash(){return _dash;};
    void SetMatrix(PPMatrix m);
    PPMatrix Matrix(){return _matrix;}
	PPMatrix CTM(){return _matrix;}
	PPMatrix *CTMRef(){return &_matrix;}
	PPMatrix *MatrixRef(){return &_matrix;}
    
    int NumberOfStrokeColorComponents();
    int NumberOfNonStrokeColorComponents();
    void SetStrokeColorSpace(string name);
	void SetUserStrokeColorSpace(string name, PPTArray *arr);
    string StrokeColorSpace(){return _strokeColor._colorSpaceName;}
	void SetStrokeColor(PPColor c);  
    void SetStrokeColor(float c1, float c2 = -1, float c3 = -1, float c4 = -1);

    PPColor StrokeColor(){return _strokeColor;}
	PPColor *StrokeColorRef(){return &_strokeColor;}
    
    void SetFillColorSpace(string name);
	void SetUserFillColorSpace(string name, PPTArray *arr);
    string FillColorSpace(){return _fillColor._colorSpaceName;};
	void SetFillColor(PPColor c); 
    void SetFillColor(float c1, float c2 = -1, float c3 = -1, float c4 = -1);

	PPColor FillColor(){return _fillColor;}
    PPColor *FillColorRef(){return &_fillColor;}
    
    void ClearGFlags(){_gflag = PPGF_NONE;}
    unsigned long GFlags(){return _gflag;}
    void SetGFlags(unsigned long flag){_gflag = flag;}
    void RestoreGFlags(){_gflag = _gflag_save;}
    
    string MakeCommandString();
    string XMLString(int level);
	void SetTransform(PPMatrix mtx){SetMatrix(mtx);}

	void SetStrokeColor(vector<PPToken *> &_operands);
	void SetStrokeColorN(vector<PPToken *> &_operands);
	void SetFillColor(vector<PPToken *> &_operands);
	void SetFillColorN(vector<PPToken *> &_operands);
};


#endif


