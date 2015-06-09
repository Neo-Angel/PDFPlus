//
//  PPCoord.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 10. 19..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//



#include "PPCoord.h"
#include "PPToken.h"
#include "PPTNumber.h"

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

bool PPIntersectsLine(double v_x, double v_y1, double v_y2, double h_y, double h_x1, double h_x2) 
{
	if(v_x >= h_x1 && v_x <= h_x2) {
		if(h_y >= v_y1 && h_y <= v_y2) {
			return true;
		}
	}
	return false;
}

bool PPIntersectsRect(PPRect &rect1, PPRect &rect2) 
{
	// rect2의 네개의 꼭지점 중에 rect1 안에 들어가 있는 게 있는지 체크
	if(rect1.GetX1() <= rect2.GetX1() && rect1.GetX2() >= rect2.GetX1()) {
		if(rect1.GetY1() <= rect2.GetY1() && rect1.GetY2() >= rect2.GetY1()) {
			return true;
		}
		if(rect1.GetY1() <= rect2.GetY2() && rect1.GetY2() >= rect2.GetY2()) {
			return true;
		}
	}
	if(rect1.GetX1() <= rect2.GetX2() && rect1.GetX2() >= rect2.GetX2()) {
		if(rect1.GetY1() <= rect2.GetY1() && rect1.GetY2() >= rect2.GetY1()) {
			return true;
		}
		if(rect1.GetY1() <= rect2.GetY2() && rect1.GetY2() >= rect2.GetY2()) {
			return true;
		}
	}
	if(PPIntersectsLine(rect1.GetX1(), rect1.GetY1(), rect1.GetY2(), rect2.GetY1(), rect2.GetX1(), rect2.GetX2()))
		return true;
	if(PPIntersectsLine(rect1.GetX1(), rect1.GetY1(), rect1.GetY2(), rect2.GetY2(), rect2.GetX1(), rect2.GetX2()))
		return true;
	if(PPIntersectsLine(rect1.GetX2(), rect1.GetY1(), rect1.GetY2(), rect2.GetY1(), rect2.GetX1(), rect2.GetX2()))
		return true;
	if(PPIntersectsLine(rect1.GetX2(), rect1.GetY1(), rect1.GetY2(), rect2.GetY2(), rect2.GetX1(), rect2.GetX2()))
		return true;

	if(PPIntersectsLine(rect2.GetX1(), rect2.GetY1(), rect2.GetY2(), rect1.GetY1(), rect1.GetX1(), rect1.GetX2()))
		return true;
	if(PPIntersectsLine(rect2.GetX1(), rect2.GetY1(), rect2.GetY2(), rect1.GetY2(), rect1.GetX1(), rect1.GetX2()))
		return true;
	if(PPIntersectsLine(rect2.GetX2(), rect2.GetY1(), rect2.GetY2(), rect1.GetY1(), rect1.GetX1(), rect1.GetX2()))
		return true;
	if(PPIntersectsLine(rect2.GetX2(), rect2.GetY1(), rect2.GetY2(), rect1.GetY2(), rect1.GetX1(), rect1.GetX2()))
		return true;


	return false;
}



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
		_origin._x = ptrect.x1;
		_origin._y = ptrect.y1;
		_size._width = abs(ptrect.x2 - ptrect.x1);
		_size._height = abs(ptrect.y2 - ptrect.y1);
}

PPRect rectFromArray(PPTArray *array)
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

void setRectToArray(PPRect rect, PPTArray *array)
{
	PPTNumber *num;
	num = new PPTNumber(array->_parser, rect.x1());
	array->_array.push_back(num);
	num = new PPTNumber(array->_parser, rect.y1());
	array->_array.push_back(num);
	num = new PPTNumber(array->_parser, rect.width());
	array->_array.push_back(num);
	num = new PPTNumber(array->_parser, rect.height());
	array->_array.push_back(num);
}

/*
bool PPRect::IntersectsRect(PPRect rect)
{
	if(PPIntersectsRect(*this, rect)) {
		return true;
	}
	// 순서를 바꿔서 또 한번 체크함 (rect1가 rect2에 완전히 포함되어 있는 경우)
	return PPIntersectsRect(rect, *this);
}
*/

PPRect PPRect::IntersectRect(PPRect rect)
{
	PPRect ret_rect;
	if(x1() < rect.x1())
		ret_rect.setX1(rect.x1());
	else
		ret_rect.setX1(x1());

	if(x2() > rect.x2())
		ret_rect.setX2(rect.x2());
	else 
		ret_rect.setX2(x2());

	if(y1() < rect.y1())
		ret_rect.setY1(rect.y1());
	else
		ret_rect.setY1(y1());

	if(y2() > rect.y2())
		ret_rect.setY2(rect.y2());
	else 
		ret_rect.setY2(y2());

	return ret_rect;
}

bool PPRect::IntersectRect(PPRect rect1, PPRect rect2)
{
	if(rect1.x1() < rect2.x1())
		setX1(rect2.x1());
	else
		setX1(rect1.x1());

	if(rect1.x2() > rect2.x2())
		setX2(rect2.x2());
	else 
		setX2(rect1.x2());

	if(rect1.y1() < rect2.y1())
		setY1(rect2.y1());
	else
		setY1(rect1.y1());

	if(rect1.y2() > rect2.y2())
		setY2(rect2.y2());
	else 
		setY2(rect1.y2());

	if(width() > 0 && height() > 0)
		return true;
	return false;
}

bool PPRect::IntersectsRect(PPRect rect)
{
	PPRect res_rect = IntersectRect(rect);
	if(res_rect.width() > 0 && res_rect.height() > 0)
		return true;
	return false;
}