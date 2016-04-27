
#include <sstream>
#include "PPGState.h"
#include "PPDash.h"
#include "PPColor.h"
#include "PPMatrix.h"
#include "PPCommandParser.h"
#include "PPTName.h"

extern PPCommandInfo PPCommandList[];
string PPTabStr(int cnt);



PPGState::PPGState()
{
    _gflag = 0;
}

void PPGState::setLineWidth(float v)
{
    _lineWidth = v;
    _gflag |= PPGF_LINEWIDTH;
}
void PPGState::setIntent(string v)
{
    _intent = v;
    _gflag |= PPGF_INTENT;
}
void PPGState::setFlatness(float v)
{
    _flatness = v;
    _gflag |= PPGF_FLATNESS;
}
void PPGState::setMiterLimit(float v)
{
    _miterLimit = v;
    _gflag |= PPGF_MITERLIMIT;
}
void PPGState::setLineCap(byte v)
{
    _lineCap = v;
    _gflag |= PPGF_LINECAP;
}

void PPGState::setLineJoin(byte v)
{
    _lineJoin = v;
    _gflag |= PPGF_LINEJOIN;
}
void PPGState::setDictName(string s)
{
    _dictName = s;
    _gflag |= PPGF_DICTNAME;
}

void PPGState::setDash(PPDash d)
{
    _dash = d;
    _gflag |= PPGF_DASH;
}
void PPGState::setMatrix(PPMatrix m)
{
    _matrix = m;
    _gflag |= PPGF_MATRIX;
}




int PPGState::numberOfStrokeColorCoponents()
{
    bool is_string;
    return _strokeColor.numberOfCoponents(is_string);
}

int PPGState::numberOfNonStrokeColorCoponents()
{
    bool is_string;
    return _fillColor.numberOfCoponents(is_string);
}

void PPGState::setStrokeColor(PPColor c)
{
    _strokeColor = c;
    _gflag |= PPGF_STROKECOLOR;
//    _gflag |= PPGF_FILLCOLORSPC;
}

void PPGState::setStrokeColor(float c1, float c2, float c3, float c4)
{
    _strokeColor._c1 = c1;
    _strokeColor._c2 = c2;
    _strokeColor._c3 = c3;
    _strokeColor._c4 = c4;
	_gflag |= PPGF_STROKECOLOR;
}

void PPGState::setFillColor(PPColor c)
{
    _fillColor = c;
    _gflag |= PPGF_FILLCOLOR;
//    _gflag |= PPGF_FILLCOLORSPC;
}

void PPGState::setFillColor(float c1, float c2, float c3, float c4)
{
    _fillColor._c1 = c1;
    _fillColor._c2 = c2;
    _fillColor._c3 = c3;
    _fillColor._c4 = c4;
    _gflag |= PPGF_FILLCOLOR;
}

void PPGState::setStrokeColorSpace(string name)
{
    _strokeColor.SetColorSpaceName(name);
}

void PPGState::setFillColorSpace(string name)
{
    _fillColor.SetColorSpaceName(name);
}

void PPGState::SetUserStrokeColorSpace(string name, PPTArray *arr)
{
    _strokeColor.SetUserColorSpaceName(name);
	PPTName *cs_name = (PPTName *)arr->_array[0];
	_strokeColor.SetColorSpaceName(*cs_name->_name);
	_strokeColor._colorInfo = arr;
    _gflag |= PPGF_STROKECOLORSPC;
	_gflag |= PPGF_COLORSPACE;
}

void PPGState::SetUserFillColorSpace(string name, PPTArray *arr)
{
    _fillColor.SetUserColorSpaceName(name);
	PPTName *cs_name = (PPTName *)arr->_array[0];
	_fillColor.SetColorSpaceName(*cs_name->_name);
	_fillColor._colorInfo = arr;
    _gflag |= PPGF_FILLCOLORSPC;
	_gflag |= PPGF_COLORSPACE;
}

string PPGState::makeCommandString()
{
    string retstr;
    ostringstream ostr;
    PPCommandInfo *cinfo;
    if(_gflag & PPGF_LINEWIDTH) {
        cinfo = &PPCommandList[PPC_LineWidth];
        ostr << _lineWidth << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_INTENT) {
        cinfo = &PPCommandList[PPC_Intent];
        ostr << "/" << _intent << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_FLATNESS) {
        cinfo = &PPCommandList[PPC_Flatness];
        ostr << _flatness << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_MITERLIMIT) {
        cinfo = &PPCommandList[PPC_MiterLimit];
        ostr << _miterLimit << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_LINECAP) {
        cinfo = &PPCommandList[PPC_LineCap];
        ostr << _lineCap << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_LINEJOIN) {
        cinfo = &PPCommandList[PPC_LineJoin];
        ostr << _lineJoin << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_DICTNAME) {
        cinfo = &PPCommandList[PPC_DictName];
        ostr << "/" << _dictName << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_DASH) {
        cinfo = &PPCommandList[PPC_Dash];
        ostr << _dash.pdfString() << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_MATRIX) {
        cinfo = &PPCommandList[PPC_Matrix];
        ostr << _matrix.pdfString() << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_STROKECOLOR) {
        if (_strokeColor._colorSpaceName == PPCSN_DeviceGray) {
            cinfo = &PPCommandList[PPC_DeviceGray];
            ostr << _strokeColor._c1 << " " << cinfo->command << PP_ENDL;
        }
        else if (_strokeColor._colorSpaceName == PPCSN_DeviceRGB) {
            cinfo = &PPCommandList[PPC_DeviceRGB];
            ostr << _strokeColor._c1 << " " << _strokeColor._c2 << " " << _strokeColor._c3 << " " << cinfo->command << PP_ENDL;
        }
        else if (_strokeColor._colorSpaceName == PPCSN_DeviceCMYK) {
            cinfo = &PPCommandList[PPC_DeviceCMYK];
            ostr << _strokeColor._c1 << " " << _strokeColor._c2 << " " << _strokeColor._c3 << " " << _strokeColor._c4 << " " << cinfo->command << PP_ENDL;
        }
        else {
            cinfo = &PPCommandList[PPC_StrokeColorSpace];
            ostr << _strokeColor._colorSpaceName << " " << cinfo->command << PP_ENDL;
            if (_strokeColor._colorSpaceName == PPCSN_CalGray
                || _strokeColor._colorSpaceName == PPCSN_CalRGB
                || _strokeColor._colorSpaceName == PPCSN_Indexed
                || _strokeColor._colorSpaceName == PPCSN_Lab
                ) {
                cinfo = &PPCommandList[PPC_SetColor];
                if (_strokeColor._colorSpaceName == PPCSN_CalGray || _strokeColor._colorSpaceName == PPCSN_Indexed) {
                    ostr << _strokeColor._c1 << " " << cinfo->command << PP_ENDL;
                }
                else if (_strokeColor._colorSpaceName == PPCSN_CalRGB || _strokeColor._colorSpaceName == PPCSN_Lab) {
                    ostr << _strokeColor._c1 << " " << _strokeColor._c2 << " " << _strokeColor._c3 << " " << cinfo->command << PP_ENDL;
                }
            }
            else { // Separation, DeviceN, ICCBased
                cinfo = &PPCommandList[PPC_SetColorN];
                if (_strokeColor._colorSpaceName == PPCSN_Separation){
                    if (_strokeColor._alterColorSpaceName == PPCSN_ICCBased
                        || _strokeColor._alterColorSpaceName == PPCSN_DeviceGray) {
                        ostr << _strokeColor._c1 << " " << cinfo->command << PP_ENDL;
                    }
                    else if (_strokeColor._alterColorSpaceName == PPCSN_DeviceCMYK) {
                        ostr << _strokeColor._c1 << " " << _strokeColor._c2 << " " << _strokeColor._c3 << " " << _strokeColor._c4 << " " << cinfo->command << PP_ENDL;
                    }
                    else if (_strokeColor._alterColorSpaceName == PPCSN_DeviceRGB) {
                        ostr << _strokeColor._c1 << " " << _strokeColor._c2 << " " << _strokeColor._c3 << " " << cinfo->command << PP_ENDL;
                    }
                }
            }
        }
    }
    
    if(_gflag & PPGF_FILLCOLOR) {
        if (_fillColor._colorSpaceName == PPCSN_DeviceGray) {
            cinfo = &PPCommandList[PPC_NonStrokeDeviceGray];
            ostr << _fillColor._c1 << " " << cinfo->command << PP_ENDL;
        }
        else if (_fillColor._colorSpaceName == PPCSN_DeviceRGB) {
            cinfo = &PPCommandList[PPC_NonStrokeDeviceRGB];
            ostr << _fillColor._c1 << " " << _fillColor._c2 << " " << _fillColor._c3 << " " << cinfo->command << PP_ENDL;
        }
        else if (_fillColor._colorSpaceName == PPCSN_DeviceCMYK) {
            cinfo = &PPCommandList[PPC_NonStrokeDeviceCMYK];
            ostr << _fillColor._c1 << " " << _fillColor._c2 << " " << _fillColor._c3 << " " << _fillColor._c4 << " " << cinfo->command << PP_ENDL;
        }
        else {
            cinfo = &PPCommandList[PPC_NonStrokeColorSpace];
            ostr << _fillColor._colorSpaceName << " " << cinfo->command << PP_ENDL;
            if (_fillColor._colorSpaceName == PPCSN_CalGray
                || _fillColor._colorSpaceName == PPCSN_CalRGB
                || _fillColor._colorSpaceName == PPCSN_Indexed
                || _fillColor._colorSpaceName == PPCSN_Lab
                ) {
                cinfo = &PPCommandList[PPC_SetNonStrokeColor];
                if (_fillColor._colorSpaceName == PPCSN_CalGray || _fillColor._colorSpaceName == PPCSN_Indexed) {
                    ostr << _fillColor._c1 << " " << cinfo->command << PP_ENDL;
                }
                else if (_fillColor._colorSpaceName == PPCSN_CalRGB || _fillColor._colorSpaceName == PPCSN_Lab) {
                    ostr << _fillColor._c1 << " " << _fillColor._c2 << " " << _fillColor._c3 << " " << cinfo->command << PP_ENDL;
                }
            }
            else { // Separation, DeviceN, ICCBased
                cinfo = &PPCommandList[PPC_SetNonStrokeColorN];
                if (_fillColor._colorSpaceName == PPCSN_Separation){
                    if (_fillColor._alterColorSpaceName == PPCSN_ICCBased
                        || _fillColor._alterColorSpaceName == PPCSN_DeviceGray) {
                        ostr << _fillColor._c1 << " " << cinfo->command << PP_ENDL;
                    }
                    else if (_fillColor._alterColorSpaceName == PPCSN_DeviceCMYK) {
                        ostr << _fillColor._c1 << " " << _fillColor._c2 << " " << _fillColor._c3 << " " << _fillColor._c4 << " " << cinfo->command << PP_ENDL;
                    }
                    else if (_fillColor._alterColorSpaceName == PPCSN_DeviceRGB) {
                        ostr << _fillColor._c1 << " " << _fillColor._c2 << " " << _fillColor._c3 << " " << cinfo->command << PP_ENDL;
                    }
                }
            }
        }
    }
	if(_gflag & PPGF_STROKECOLORSPC) {
		cinfo = &PPCommandList[PPC_StrokeColorSpace];
		ostr <<"/" <<  _strokeColor.UserColorSpaceName()  << " " << cinfo->command << PP_ENDL;
	}
	if(_gflag & PPGF_FILLCOLORSPC) {
		cinfo = &PPCommandList[PPC_NonStrokeColorSpace];
		ostr << "/" << _fillColor.UserColorSpaceName()  << " " << cinfo->command << PP_ENDL;
	}

	// Set Color (N)
	if(_gflag & PPGF_SETSTROKECOLOR) {
		cinfo = &PPCommandList[PPC_SetColor];
		ostr << _strokeColor.StringOfColors()  << " " << cinfo->command << PP_ENDL;
	}
	if(_gflag & PPGF_SETSTROKECOLORN) {
		cinfo = &PPCommandList[PPC_SetColorN];
		ostr << _strokeColor.StringOfColors()  << " " << cinfo->command << PP_ENDL;
	}
	if(_gflag & PPGF_SETFILLCOLOR) {
		cinfo = &PPCommandList[PPC_SetNonStrokeColor];
		ostr << _fillColor.StringOfColors()  << " " << cinfo->command << PP_ENDL;
	}
	if(_gflag & PPGF_SETFILLCOLORN) {
		cinfo = &PPCommandList[PPC_SetNonStrokeColorN];
		ostr << _fillColor.StringOfColors()  << " " << cinfo->command << PP_ENDL;
	}

    retstr = ostr.str();
    return retstr;
}
void PPGState::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
    PPGState *ret_gstate = (PPGState *)obj;

    ret_gstate->setLineWidth(_lineWidth);
    ret_gstate->setIntent(_intent);
    ret_gstate->setFlatness(_flatness);
    ret_gstate->setMiterLimit(_miterLimit);
    ret_gstate->setLineCap(_lineCap);
    ret_gstate->setLineJoin(_lineJoin);
    ret_gstate->setDictName(_dictName);
    
    // needs to delete
    ret_gstate->setDash(_dash);
    ret_gstate->setMatrix(_matrix);
    ret_gstate->setStrokeColor(_strokeColor);
    ret_gstate->setFillColor(_fillColor);
    
    ret_gstate->setGFlags(_gflag);
//    clearGFlags();
    
}

string PPGState::XMLString(int level)
{
    string retstr;
    retstr += PPTabStr(level) + "<GState>\xa";
    retstr += makeCommandString() + "\xa";
    retstr += PPTabStr(level) + "</GState>\xa";
    return retstr;
}




void  PPGState::SetStrokeColor(vector<PPToken *> &_operands)
{
	_gflag |= PPGF_SETSTROKECOLOR;
	_strokeColor.SetColos(_operands);
}

void  PPGState::SetStrokeColorN(vector<PPToken *> &_operands)
{
	_gflag |= PPGF_SETSTROKECOLORN;
	_strokeColor.SetColos(_operands);
}

void  PPGState::SetFillColor(vector<PPToken *> &_operands)
{
	_gflag |= PPGF_SETFILLCOLOR;
	_fillColor.SetColos(_operands);
}

void  PPGState::SetFillColorN(vector<PPToken *> &_operands)
{
	_gflag |= PPGF_SETFILLCOLORN;
	_fillColor.SetColos(_operands);
}
