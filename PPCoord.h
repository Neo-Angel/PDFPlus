//
//  PPCoord.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 10. 19..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPCoord__
#define __PDFPlusLib__PPCoord__

#include <stdio.h>

#include "PPBase.h"
#include "PPCoord.h"

class PPPoint : PPBase {
public:
    float _x;
    float _y;
    PPPoint();
    PPPoint(float x, float y);
};

class PPSize : PPBase {
public:
    float _width;
    float _height;
    PPSize();
    PPSize(float w, float h);
};


class PTRect;

class PPRect : PPBase {
public:
    PPPoint _origin;
    PPSize _size;
    PPRect();
	PPRect(PTRect ptrect);
    PPRect(float x, float y, float w, float h);
	inline void Set(float x, float y, float w, float h) {
		_origin._x = x;
		_origin._y = y; 
		_size._width = w; 
		_size._height = h;
	}
	inline float getX1(){return _origin._x;}
	inline float getY1(){return _origin._y;}
	inline float getX2(){return (_origin._x + _size._width);}
	inline float getY2(){return (_origin._y + _size._height);}
	inline float GetX1(){return _origin._x;}
	inline float GetY1(){return _origin._y;}
	inline float GetX2(){return (_origin._x + _size._width);}
	inline float GetY2(){return (_origin._y + _size._height);}
	inline float x1(){return getX1();}
	inline float y1(){return getY1();}
	inline float x2(){return getX2();}
	inline float y2(){return getY2();}

	inline void setX1(float x) {
		_size._width = getX2() - x;
		_origin._x = x;
	}
	inline void setY1(float y) {
		_size._height = getY2() - y;
		_origin._y = y;
	}
	inline void setX2(float x) {_size._width = (x - _origin._x);}
	inline void setY2(float y) {_size._height = (y - _origin._y);}

	inline void SetX1(float x) {setX1(x);}
	inline void SetY1(float y) {setY1(y);}
	inline void SetX2(float x) {setX2(x);}
	inline void SetY2(float y) {setY2(y);}

	inline float width(){return _size._width;}
	inline float height(){return _size._height;}
	inline float Width(){return _size._width;}
	inline float Height(){return _size._height;}

	bool IntersectsRect(PPRect rect);
	bool PPRect::IntersectRect(PPRect rect1, PPRect rect2);
	PPRect IntersectRect(PPRect rect);
};

class PTRect : PPBase {

public:
	float x1;
	float y1;
	float x2;
	float y2;

	inline PTRect(float _x1, float _y1, float _x2, float _y2) {
		x1 = _x1; y1 = _y1; x2 = _x2; y2 = _y2; 
	}
	inline PTRect(PPRect pprect) {
		x1 = pprect.GetX1(); 
		y1 = pprect.GetY1();
		x2 = pprect.GetX2();
		y2 = pprect.GetY2();
	}

};
class PPTArray;

PPRect rectFromArray(PPTArray *array);
void setRectToArray(PPRect rect, PPTArray *array);


#endif /* defined(__PDFPlusLib__PPCoord__) */
