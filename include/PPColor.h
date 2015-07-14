
#ifndef __PDFPlusLib__PPColor__
#define __PDFPlusLib__PPColor__

#include <PPBase.h>
#include <PPToken.h>
//
//      PPColor
//
///////////////////////////////////////////////////////////////


extern const char *PPCSN_DeviceGray;
extern const char *PPCSN_DeviceRGB;
extern const char *PPCSN_DeviceCMYK;
extern const char *PPCSN_CalGray;
extern const char *PPCSN_CalRGB;
extern const char *PPCSN_Lab;
extern const char *PPCSN_ICCBased;
extern const char *PPCSN_Indexed;
extern const char *PPCSN_Separation;
extern const char *PPCSN_DeviceN;
extern const char *PPCSN_Pattern ;



class PPColor : public PPBase {
    
public:
	string _userColorSpaceName;
    string _colorSpaceName;
    string _alterColorSpaceName;
    float _c1, _c2, _c3, _c4;
    string _colorName;
	byte		_numofcolors;

	PPColor(){_numofcolors = 0; _c1=_c2=_c3=_c4=-1;}
    void SetColorSpaceName(string name);
	void SetUserColorSpaceName(string name);
	void SetColorName(string name) {_colorName = name;}
	void SetAlterColorSpaceName(string name) {_alterColorSpaceName = name;}
	void SetCMYKColor(float c, float m, float y, float k) {_c1 = c; _c2 = m; _c3 = y; _c4 = k;}
	void SetRGBColor(float r, float g, float b, float a);
	void SetAlpha(float a) {_c4 = a;}
    string colorSpaceName(){return _colorSpaceName;}
    string ColorSpaceName(){return _colorSpaceName;}
	string UserColorSpaceName(){return _userColorSpaceName;}
    int numberOfCoponents(bool &is_string);
	string colorName(){return _colorName;}
	void SetColos(vector<PPToken *> &colors);
	string StringOfColors();

};


PPColor *PPNewRGBColor(float r, float g, float b, float a = 1.0);
PPColor *PPNewCMYKColor(float c, float m, float y, float k);





#endif