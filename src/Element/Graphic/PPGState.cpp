
#include <sstream>
#include "PPGState.h"
#include "PPDash.h"
#include "PPColor.h"
#include "PPMatrix.h"
#include "PPCommandParser.h"
#include "PPTName.h"
#include "PPElement.h"

extern PPCommandInfo PPCommandList[];
string PPTabStr(int cnt);



PPGState::PPGState()
{
    _gflag = 0;
	_gflag_save = 0;
}

void PPGState::SetLineWidth(float v)
{
    _lineWidth = v;
    _gflag |= PPGF_LINEWIDTH;
    _gflag_save |= PPGF_LINEWIDTH;
}
void PPGState::SetIntent(string v)
{
    _intent = v;
    _gflag |= PPGF_INTENT;
    _gflag_save |= PPGF_INTENT;
}
void PPGState::SetFlatness(float v)
{
    _flatness = v;
    _gflag |= PPGF_FLATNESS;
    _gflag_save |= PPGF_FLATNESS;
}
void PPGState::SetMiterLimit(float v)
{
    _miterLimit = v;
    _gflag |= PPGF_MITERLIMIT;
    _gflag_save |= PPGF_MITERLIMIT;
}
void PPGState::SetLineCap(byte v)
{
    _lineCap = v;
    _gflag |= PPGF_LINECAP;
    _gflag_save |= PPGF_LINECAP;
}

void PPGState::SetLineJoin(byte v)
{
    _lineJoin = v;
    _gflag |= PPGF_LINEJOIN;
    _gflag_save |= PPGF_LINEJOIN;
}

void PPGState::SetDictName(string s)
{
    _dictName = s;
    _gflag |= PPGF_DICTNAME;
    _gflag_save |= PPGF_DICTNAME;	
}

void PPGState::SetDash(PPDash d)
{
    _dash = d;
    _gflag |= PPGF_DASH;
    _gflag_save |= PPGF_DASH;
}
void PPGState::SetMatrix(PPMatrix m)
{
    _matrix = m;
    _gflag |= PPGF_MATRIX;
    _gflag_save |= PPGF_MATRIX;
}

void PPGState::SetFillColorFlag(uint mask) 
{
	_gflag &= ~(PPGF_FILLCOLOR | PPGF_SETFILLCOLOR | PPGF_SETFILLCOLORN);  // clear fill color flags
	_gflag |= mask;
	_gflag_save &= ~(PPGF_FILLCOLOR | PPGF_SETFILLCOLOR | PPGF_SETFILLCOLORN);
	_gflag_save |= mask;
}

void PPGState::SetStrokeColorFlag(uint mask)
{
	_gflag &= ~(PPGF_STROKECOLOR | PPGF_SETSTROKECOLOR | PPGF_SETSTROKECOLORN);  // clear fill color flags
	_gflag |= mask;
	_gflag_save &= ~(PPGF_STROKECOLOR | PPGF_SETSTROKECOLOR | PPGF_SETSTROKECOLORN);
	_gflag_save |= mask;
}

int PPGState::NumberOfStrokeColorComponents()
{
    bool is_string;
    return _strokeColor.NumberOfComponents(is_string);
}

int PPGState::NumberOfNonStrokeColorComponents()
{
    bool is_string;
    return _fillColor.NumberOfComponents(is_string);
}

void PPGState::SetStrokeColor(PPColor c)
{
    _strokeColor = c;
	this->SetStrokeColorFlag(PPGF_STROKECOLOR);
}

void PPGState::SetStrokeColor(float c1, float c2, float c3, float c4)
{
    _strokeColor._c1 = c1;
    _strokeColor._c2 = c2;
    _strokeColor._c3 = c3;
    _strokeColor._c4 = c4;
	this->SetStrokeColorFlag(PPGF_STROKECOLOR);
}

void PPGState::SetFillColor(PPColor c)
{
    _fillColor = c;
	this->SetFillColorFlag(PPGF_FILLCOLOR);
}

void PPGState::SetFillColor(float c1, float c2, float c3, float c4)
{
    _fillColor._c1 = c1;
    _fillColor._c2 = c2;
    _fillColor._c3 = c3;
    _fillColor._c4 = c4;
	this->SetFillColorFlag(PPGF_FILLCOLOR);
}

void PPGState::SetStrokeColorSpace(string name)
{
    _strokeColor.SetColorSpaceName(name);
}

void PPGState::SetFillColorSpace(string name)
{
    _fillColor.SetColorSpaceName(name);
}

void PPGState::SetUserStrokeColorSpace(string name, PPTArray *arr)
{
    _strokeColor.SetUserColorSpaceName(name);
	PPTName *cs_name = (PPTName *)arr->TokenAtIndex(0);
	_strokeColor.SetColorSpaceName(*cs_name->_name);
	_strokeColor._colorInfo = arr;
    _gflag |= PPGF_STROKECOLORSPC;
	_gflag |= PPGF_COLORSPACE;
    _gflag_save |= PPGF_STROKECOLORSPC;
	_gflag_save |= PPGF_COLORSPACE;
}

void PPGState::SetUserFillColorSpace(string name, PPTArray *arr)
{
    _fillColor.SetUserColorSpaceName(name);
	PPTName *cs_name = (PPTName *)arr->TokenAtIndex(0);
	_fillColor.SetColorSpaceName(*cs_name->_name);
	_fillColor._colorInfo = arr;
    _gflag |= PPGF_FILLCOLORSPC;
	_gflag |= PPGF_COLORSPACE;
    _gflag_save |= PPGF_FILLCOLORSPC;
	_gflag_save |= PPGF_COLORSPACE;
}

string PPGState::MakeCommandString()
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
        ostr << (int)_lineCap << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_LINEJOIN) {
        cinfo = &PPCommandList[PPC_LineJoin];
        ostr << (int)_lineJoin << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_DICTNAME) {
        cinfo = &PPCommandList[PPC_DictName];
        ostr << "/" << _dictName << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_DASH) {
        cinfo = &PPCommandList[PPC_Dash];
        ostr << _dash.PDFString() << " " << cinfo->command << PP_ENDL;
    }
    if(_gflag & PPGF_MATRIX) {
        cinfo = &PPCommandList[PPC_Matrix];
        ostr << _matrix.PDFString() << " " << cinfo->command << PP_ENDL;
    }

	if(_gflag & PPGF_STROKECOLORSPC) {
		cinfo = &PPCommandList[PPC_StrokeColorSpace];
		ostr <<"/" <<  _strokeColor.UserColorSpaceName()  << " " << cinfo->command << PP_ENDL;
	}
	if(_gflag & PPGF_FILLCOLORSPC) {
		cinfo = &PPCommandList[PPC_NonStrokeColorSpace];
		ostr << "/" << _fillColor.UserColorSpaceName()  << " " << cinfo->command << PP_ENDL;
	}

	if(_gflag & PPGF_SETSTROKECOLOR) {
		cinfo = &PPCommandList[PPC_SetColor];
		ostr << _strokeColor.StringValue()  << " " << cinfo->command << PP_ENDL;
	}
	else if(_gflag & PPGF_SETSTROKECOLORN) {
		cinfo = &PPCommandList[PPC_SetColorN];
		ostr << _strokeColor.StringValue()  << " " << cinfo->command << PP_ENDL;
	}
    else if(_gflag & PPGF_STROKECOLOR) {
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
    
	// Set Color (N)
	if(_gflag & PPGF_SETFILLCOLOR) {
		cinfo = &PPCommandList[PPC_SetNonStrokeColor];
		ostr << _fillColor.StringValue()  << " " << cinfo->command << PP_ENDL;
	}
	else if(_gflag & PPGF_SETFILLCOLORN) {
		cinfo = &PPCommandList[PPC_SetNonStrokeColorN];
		ostr << _fillColor.StringValue()  << " " << cinfo->command << PP_ENDL;
	}
    else if(_gflag & PPGF_FILLCOLOR) {
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


    retstr = ostr.str();
    return retstr;
}

void PPGState::MoveColorSpaceTo(PPColor *color, PPFormBase *tar_form)
{
//	if(color->_colorSpaceName != "DeviceCMYK") {
//		cout << "ColorSpaceName is not DeviceCMYK" << PP_ENDL;
//	}

	PPFormBase *parent_form = this->_parent->_parentForm;
	PPTIndirectObj *rsc_obj = parent_form->ResourceObjForName(color->_userColorSpaceName, "ColorSpace");
	if(rsc_obj) {
		PPDocument *tar_doc = tar_form->_document;
		PPTIndirectObj *new_obj = tar_doc->MoveObjFrom(rsc_obj, parent_form->_document);
		PPTIndirectRef *new_ref = tar_form->AddResourceRef(new_obj->_objNum, color->_userColorSpaceName, "ColorSpace");
		if(new_ref) {
			new_obj->AddRefObj(new_ref);
		}
	}
}

void PPGState::MoveColorPatternTo(PPColor *color, PPFormBase *tar_form)
{
//	if(color->_colorSpaceName != "DeviceCMYK") {
//		cout << "ColorSpaceName is not DeviceCMYK" << PP_ENDL;
//	}

	PPFormBase *parent_form = this->_parent->_parentForm;
	PPTIndirectObj *rsc_obj = parent_form->ResourceObjForName(color->_colorName, "Pattern");
	if(rsc_obj) {
		PPDocument *tar_doc = tar_form->_document;
		PPTIndirectObj *new_obj = tar_doc->MoveObjFrom(rsc_obj, parent_form->_document);
		PPTIndirectRef *new_ref = tar_form->AddResourceRef(new_obj->_objNum, color->_colorName, "Pattern");
		if(new_ref) {
			new_obj->AddRefObj(new_ref);
		}
	}
}

PPBase *PPGState::Copy()
{
	PPGState *new_obj = (PPGState *)this->Create();
	new_obj->_parent = NULL;
	new_obj->_clone = this;
	CopyMembersTo(new_obj);

	return new_obj;
}

PPBase *PPGState::Copy(PPElement *tar_element)
{
	PPGState *new_obj = (PPGState *)this->Create();
	new_obj->_parent = tar_element;
	new_obj->_clone = this;
	CopyMembersTo(new_obj);

	return new_obj;
}

void PPGState::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
    PPGState *ret_gstate = (PPGState *)obj;

    ret_gstate->_lineWidth = _lineWidth; //ret_gstate->SetLineWidth(_lineWidth);
    ret_gstate->_intent = _intent; //SetIntent(_intent);
    ret_gstate->_flatness = _flatness; //SetFlatness(_flatness);
    ret_gstate->_miterLimit = _miterLimit; //SetMiterLimit(_miterLimit);
    ret_gstate->_lineCap = _lineCap; //SetLineCap(_lineCap);
    ret_gstate->_lineJoin = _lineJoin; //SetLineJoin(_lineJoin);
    ret_gstate->_dictName = _dictName; //SetDictName(_dictName);
    
    // needs to delete
    ret_gstate->_dash = _dash; //SetDash(_dash);
    ret_gstate->_matrix = _matrix; //SetMatrix(_matrix);
	if(ret_gstate->_parent != NULL) {
		if(_strokeColor._userColorSpaceName.length() > 0) {
			PPDocument *tar_doc = ret_gstate->_parent->_parentForm->_document;
			PPDocument *this_doc = this->_parent->_parentForm->_document;
			if(tar_doc != this_doc) {
				this->MoveColorSpaceTo(&_strokeColor, ret_gstate->_parent->_parentForm);
			}
		}
		if(_strokeColor._colorName.length() > 0) {
			PPDocument *tar_doc = ret_gstate->_parent->_parentForm->_document;
			PPDocument *this_doc = this->_parent->_parentForm->_document;
			if(tar_doc != this_doc) {
				this->MoveColorPatternTo(&_strokeColor, ret_gstate->_parent->_parentForm);
			}
		}

		if(_fillColor._userColorSpaceName.length() > 0) {
			PPDocument *tar_doc = ret_gstate->_parent->_parentForm->_document;
			PPDocument *this_doc = this->_parent->_parentForm->_document;
			if(tar_doc != this_doc) {
				this->MoveColorSpaceTo(&_fillColor, ret_gstate->_parent->_parentForm);
			}
		}
		if(_fillColor._colorName.length() > 0) {
			PPDocument *tar_doc = ret_gstate->_parent->_parentForm->_document;
			PPDocument *this_doc = this->_parent->_parentForm->_document;
			if(tar_doc != this_doc) {
				this->MoveColorPatternTo(&_fillColor, ret_gstate->_parent->_parentForm);
			}
		}
	}


    ret_gstate->_strokeColor = _strokeColor; //SetStrokeColor(_strokeColor);
    ret_gstate->_fillColor = _fillColor; //(_fillColor);
    
    ret_gstate->_gflag = _gflag; //SetGFlags(_gflag);
    ret_gstate->_gflag_save = _gflag_save; //SetGFlags(_gflag);

//    clearGFlags();
    
}

string PPGState::XMLString(int level)
{
    string retstr;
    retstr += PPTabStr(level) + "<GState>\xa";
    retstr += MakeCommandString() + "\xa";
    retstr += PPTabStr(level) + "</GState>\xa";
    return retstr;
}




void  PPGState::SetStrokeColor(vector<PPToken *> &_operands)
{
	_strokeColor.SetComponents(_operands);
	this->SetStrokeColorFlag(PPGF_SETSTROKECOLOR);
}

void  PPGState::SetStrokeColorN(vector<PPToken *> &_operands)
{
	_strokeColor.SetComponents(_operands);
	this->SetStrokeColorFlag(PPGF_SETSTROKECOLORN);
	if(_strokeColor.ColorName().size() > 0) {
		_strokeColor.SetColorSpaceName(PPCSN_Pattern);
	}
}

void  PPGState::SetFillColor(vector<PPToken *> &_operands)
{
	_fillColor.SetComponents(_operands);
	this->SetFillColorFlag(PPGF_SETFILLCOLOR);
}

void  PPGState::SetFillColorN(vector<PPToken *> &_operands)
{

	_fillColor.SetComponents(_operands);
	this->SetFillColorFlag(PPGF_SETFILLCOLORN);
}
