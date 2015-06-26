
#include <sstream>
#include "PPMatrix.h"


string PPMatrix::pdfString()
{
    ostringstream ostr;
    string retstr;
    
    ostr << _a << " " << _b << " " << _c << " " << _d << " " << _x << " " << _y;
    retstr = ostr.str();
    return retstr;
}

void PPMatrix::multiply(float &x, float &y)
{
	x = _a * x + _c * y + _x;
	y = _b * x + _d * y + _y;
}

//  [ _a, _b, 0]     [ _a, _b, 0]
//  [ _c, _d, 0]  X  [ _c, _d, 0]
//  [ _x, _y, 1]     [ _x, _y, 1]



void PPMatrix::multiply(PPMatrix mtx)
{
	float a = _a * mtx._a + _b * mtx._c + 0 * mtx._x;
	float b = _a * mtx._b + _b * mtx._d + 0 * mtx._y;
	float c = _c * mtx._a + _d * mtx._c + 0 * mtx._x;
	float d = _c * mtx._b + _d * mtx._d + 0 * mtx._y;
	float x = _x * mtx._a + _y * mtx._c + 1 * mtx._x;
	float y = _x * mtx._b + _y * mtx._d + 1 * mtx._y;

	_a = a;
	_b = b;
	_c = c;
	_d = d;
	_x = x;
	_y = y;
}