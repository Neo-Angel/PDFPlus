

#include <sstream>
#include <string>
#include "PPColor.h"

#include "PPTNumber.h"
#include "PPTName.h"

using namespace std;


// Color Space Names
const char *PPCSN_DeviceGray = "DeviceGray";
const char *PPCSN_DeviceRGB = "DeviceRGB";
const char *PPCSN_DeviceCMYK = "DeviceCMYK";
const char *PPCSN_CalGray = "CalGray";
const char *PPCSN_CalRGB = "CalRGB";
const char *PPCSN_Lab = "Lab";
const char *PPCSN_ICCBased = "ICCBased";
const char *PPCSN_Indexed = "Indexed";
const char *PPCSN_Separation = "Separation";
const char *PPCSN_DeviceN = "DeviceN";
const char *PPCSN_Pattern = "Pattern";


void PPColor::SetColorSpaceName(string name)
{
    _c1 = _c2 = _c3 = _c4 = 0;
    _colorName = "";
    if (name == PPCSN_DeviceCMYK) {
        _c4 = 1.0;
    }
    else if (name == PPCSN_Separation || name == PPCSN_DeviceN) {
        _c1 = _c2 = _c3 = _c4 = 1.0;
    }
    _colorSpaceName = name;
	_alterColorSpaceName = "";
	_colorName = "";
}

void PPColor::SetUserColorSpaceName(string name)
{
	_userColorSpaceName = name;
}

void PPColor::SetRGBColor(float r, float g, float b, float a)
{
	SetColorSpaceName(PPCSN_DeviceRGB);
	_c1 = r; _c2 = g; _c3 = b; _c4 = a;
}

int PPColor::numberOfCoponents(bool &is_string)
{
    is_string = false;
    if(_colorSpaceName == PPCSN_DeviceGray || _colorSpaceName == PPCSN_CalGray || _colorSpaceName == PPCSN_Indexed) {
        return 1;
    }
	else if(_colorSpaceName == PPCSN_CalRGB || _colorSpaceName == PPCSN_DeviceRGB) {
		return 4;
	}
	else if(_colorSpaceName == PPCSN_Lab) {
        return 3;
    }
    else if(_colorSpaceName == PPCSN_DeviceCMYK) {
        return 4;
    }
    else if(_colorSpaceName == PPCSN_Pattern) {
        is_string = true;
        return 1;
    }
    else {
//        PPCSN_Lab;
//        PPCSN_ICCBased;
//        PPCSN_Separation;
//        PPCSN_DeviceN;
        return -1;
    }
    
}

void PPColor::SetColos(vector<PPToken *> &colors) // &vector<PPToken *> _operands
{
	_numofcolors = colors.size();
	if(_numofcolors >= 1) {
		PPTNumber *number = (PPTNumber *)colors[0];
		_c1 = number->floatValue();
	}
	if(_numofcolors >= 2) {
		_c2 = ((PPTNumber *)colors[1])->floatValue();
	}
	if(_numofcolors >= 3) {
		_c3 = ((PPTNumber *)colors[2])->floatValue();
	}
	if(_numofcolors >= 4) {
		_c4 = ((PPTNumber *)colors[3])->floatValue();
	}

}	


string PPColor::StringOfColors()
{
	ostringstream ostr;
	string retstr = "";
	if(_numofcolors >= 1) {
		ostr << _c1;
	}
	if(_numofcolors >= 2) {
		ostr << " " << _c2;
	}
	if(_numofcolors >= 3) {
		ostr << " " << _c3;
	}
	if(_numofcolors >= 4) {
		ostr << " " << _c4;
	}

    retstr = ostr.str();
	return retstr;
}
PPColor *PPNewRGBColor(float r, float g, float b, float a)
{
	PPColor *ret_color = new PPColor;
	ret_color->_colorSpaceName = PPCSN_DeviceRGB;
	ret_color->_c1 = r;
	ret_color->_c2 = g;
	ret_color->_c3 = b;
	ret_color->_c4 = a;

	return ret_color;
}

PPColor *PPNewCMYKColor(float c, float m, float y, float k)
{
	PPColor *ret_color = new PPColor;
	ret_color->_colorSpaceName = PPCSN_DeviceCMYK;
	ret_color->_c1 = c;
	ret_color->_c2 = m;
	ret_color->_c3 = y;
	ret_color->_c4 = k;

	return ret_color;
}


string PPColor::UserColorName()
{
	string ret_str;
	if(_colorInfo) {
		PPTName *name = (PPTName *)_colorInfo->_array[1];
		ret_str = *name->_name;
	}
	return ret_str;
}
