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

/////////////////////////////////////////////
//
// 좌표와 관련된 유틸리티 클래스등의 모음
//
/////////////////////////////////////////////

//    PPPoint Class
//
////////////////////////////
class PPPoint : PPBase {
public:
    float _x;
    float _y;
    PPPoint();
    PPPoint(float x, float y);
};


//    PPSize Class
//
////////////////////////////
class PPSize : PPBase {
public:
    float _width;
    float _height;
    PPSize();
    PPSize(float w, float h);
};



//    PPRect Class
//
////////////////////////////
class PTRect; // 바로 아래 선언되어 있음.

class PPRect : PPBase {  // origin, size 등의 하위 클래스로 구성된 2중형 구조
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
	inline float X1(){return _origin._x;}
	inline float Y1(){return _origin._y;}
	inline float X2(){return (_origin._x + _size._width);}
	inline float Y2(){return (_origin._y + _size._height);}

	inline void SetX1(float x) {
		_size._width = X2() - x;
		_origin._x = x;
	}
	inline void SetY1(float y) {
		_size._height = Y2() - y;
		_origin._y = y;
	}
	inline void SetX2(float x) {_size._width = (x - _origin._x);}
	inline void SetY2(float y) {_size._height = (y - _origin._y);}

	inline float Width(){return _size._width;}
	inline float Height(){return _size._height;}

	// this(PPRect)와 파라미터 rect가 겹치는 부분이 있으면 true 리턴
	bool IntersectsRect(PPRect rect);

	// 클래스 메소드로 rect1와 rect2가 겹치는 부분이 있으면 true 리턴
	bool PPRect::IntersectsRect(PPRect rect1, PPRect rect2);

	// this(PPRect)와 파라미터 rect가 겹치는 영역을 PPRect로 리턴
	PPRect IntersectRect(PPRect rect);
};

//    PTRect Class
//
////////////////////////////
class PTRect : PPBase {  // 네개의 좌표값만으로 구성된 단층형 구조

public:
	float _x1;
	float _y1;
	float _x2;
	float _y2;

	inline PTRect(float x1, float y1, float x2, float y2) {
		_x1 = x1; _y1 = y1; _x2 = x2; _y2 = y2; 
	}
	inline PTRect(PPRect pprect) {
		_x1 = pprect.X1(); 
		_y1 = pprect.Y1();
		_x2 = pprect.X2();
		_y2 = pprect.Y2();
	}

};
class PPTArray;

// {x, y, w, h} 순으로 배열에서 가져와 PPRect를 만들어 리턴
PPRect RectFromArray(PPTArray *array);

//rect를  {x, y, w, h} 순으로 배열에 저장함
void SetRectToArray(PPRect rect, PPTArray *array);


#endif /* defined(__PDFPlusLib__PPCoord__) */
