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
#include "PPTArray.h"


////////////////////////////
//
//    Utility Functions
//
////////////////////////////

// {x, y, w, h} 순으로 배열에서 가져와 PPRect를 만들어 리턴
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

//rect를  {x, y, w, h} 순으로 배열에 저장함
void SetRectToArray(PPRect rect, PPTArray *array)
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
		_origin._x = ptrect.x1;
		_origin._y = ptrect.y1;
		_size._width = abs(ptrect.x2 - ptrect.x1);
		_size._height = abs(ptrect.y2 - ptrect.y1);
}

// this(PPRect)와 파라미터 rect가 겹치는 영역을 PPRect로 리턴
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

// 클래스 메소드로 rect1와 rect2가 겹치는 부분이 있으면 true 리턴
bool PPRect::IntersectsRect(PPRect rect1, PPRect rect2)
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

// this(PPRect)와 파라미터 rect가 겹치는 부분이 있으면 true 리턴
bool PPRect::IntersectsRect(PPRect rect)
{
	PPRect res_rect = IntersectRect(rect);
	if(res_rect.width() > 0 && res_rect.height() > 0)
		return true;
	return false;
}


