//
//  PPCoord.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 10. 19..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//


#include <math.h>
#include "PPCoord.h"
#include "PPToken.h"
#include "PPTNumber.h"
#include "PPTArray.h"


////////////////////////////
//
//    Utility Functions
//
////////////////////////////

// {x, y, w, h} 순으로 배열에서 가져와 PPRect를 만들어 리턴
PPRect RectFromArray(PPTArray *array)
{
    PPRect ret;
    if (array->_array.size() == 4) {
        PPTNumber *num = (PPTNumber *)array->_array.at(0);
        ret._origin._x = num->floatValue();
        num = (PPTNumber *)array->_array.at(1);
        ret._origin._y = num->floatValue();
        num = (PPTNumber *)array->_array.at(2);
        ret._size._width = num->floatValue();
        num = (PPTNumber *)array->_array.at(3);
        ret._size._height = num->floatValue();
    }
    
    return ret;
}

//rect를  {x, y, w, h} 순으로 배열에 저장함
void SetRectToArray(PPRect rect, PPTArray *array)
{
	PPTNumber *num;
	num = new PPTNumber(array->_document, rect.X1());
	array->_array.push_back(num);
	num = new PPTNumber(array->_document, rect.Y1());
	array->_array.push_back(num);
	num = new PPTNumber(array->_document, rect.Width());
	array->_array.push_back(num);
	num = new PPTNumber(array->_document, rect.Height());
	array->_array.push_back(num);
}

//    PPPoint Class
//
////////////////////////////

PPPoint::PPPoint()
{
    _x = 0;
    _y = 0;
}

PPPoint::PPPoint(float x, float y)
{
    _x = x;
    _y = y;
}

//    PPSize Class
//
////////////////////////////
PPSize::PPSize()
{
    _width = 0;
    _height = 0;
}

PPSize::PPSize(float w, float h)
{
    _width = w;
    _height = h;
}

//    PPRect Class
//
////////////////////////////
PPRect::PPRect()
{
    _origin._x = 0;
    _origin._y = 0;
    _size._width = 0;
    _size._height = 0;

}

PPRect::PPRect(float x, float y, float w, float h)
{
    _origin._x = x;
    _origin._y = y;
    _size._width = w;
    _size._height = h;
}


PPRect::PPRect(PTRect ptrect) 
{
		_origin._x = ptrect._x1;
		_origin._y = ptrect._y1;
		_size._width = fabs(ptrect._x2 - ptrect._x1);
		_size._height = fabs(ptrect._y2 - ptrect._y1);
}

float PPRect::Left()
{
	if(_size._width > 0)
		return _origin._x;
	return (_origin._x+_size._width);
}

float PPRect::Right()
{
	if(_size._width > 0)
		return (_origin._x + _size._width);
	return _origin._x;
}

float PPRect::Top()
{
	if(_size._height > 0) 
		return _origin._y;
	return (_origin._y + _size._height);
}

float PPRect::Bottom()
{
	if(_size._height > 0) 
		return (_origin._y + _size._height);
	return _origin._y;
}

// this(PPRect)와 파라미터 rect가 겹치는 영역을 PPRect로 리턴
PPRect PPRect::IntersectRect(PPRect rect)
{
	PPRect ret_rect;
	if(X1() < rect.X1())
		ret_rect.SetX1(rect.X1());
	else
		ret_rect.SetX1(X1());

	if(X2() > rect.X2())
		ret_rect.SetX2(rect.X2());
	else 
		ret_rect.SetX2(X2());

	if(Y1() < rect.Y1())
		ret_rect.SetY1(rect.Y1());
	else
		ret_rect.SetY1(Y1());

	if(Y2() > rect.Y2())
		ret_rect.SetY2(rect.Y2());
	else 
		ret_rect.SetY2(Y2());

	return ret_rect;
}

// 클래스 메소드로 rect1와 rect2가 겹치는 부분이 있으면 true 리턴
bool PPRect::IntersectsRect(PPRect rect1, PPRect rect2)
{
	if(rect1.X1() < rect2.X1())
		SetX1(rect2.X1());
	else
		SetX1(rect1.X1());

	if(rect1.X2() > rect2.X2())
		SetX2(rect2.X2());
	else 
		SetX2(rect1.X2());

	if(rect1.Y1() < rect2.Y1())
		SetY1(rect2.Y1());
	else
		SetY1(rect1.Y1());

	if(rect1.Y2() > rect2.Y2())
		SetY2(rect2.Y2());
	else 
		SetY2(rect1.Y2());

	if(Width() > 0 && Height() > 0)
		return true;
	return false;
}

// this(PPRect)와 파라미터 rect가 겹치는 부분이 있으면 true 리턴
bool PPRect::IntersectsRect(PPRect rect)
{
	PPRect res_rect = IntersectRect(rect);
	if(res_rect.Width() > 0 && res_rect.Height() > 0)
		return true;
	return false;
}


bool PPRect::ContainsRect(PPRect rect)
{
	if(Left() <= rect.Left() && Right() >= rect.Right()
		&& Top() <= rect.Top() && Bottom() >= rect.Bottom()) {
			return true;
	}
	return false;
}
