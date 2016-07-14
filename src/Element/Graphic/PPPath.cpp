
#include <sstream>
#include "PPPathItem.h"
#include "PPPath.h"



//////////////////////////////////////////////////////////
//
//  PPPath Def (pure Path Class)
//
//////////////////////////////////////////////////////////

void PPPath::CopyMembersTo(PPBase *obj)
{
	PPBase::CopyMembersTo(obj);
	PPPath *tar_obj = (PPPath *)obj;
	tar_obj->_bounds = _bounds;

	size_t i, icnt = _itemList.size();
	for(i=0;i<icnt;i++) {
		PPPathItem *src_item = _itemList[i];
		PPPathItem *new_item = (PPPathItem *)src_item->Copy();
		tar_obj->_itemList.push_back(new_item);
	}
}

void PPPath::CheckBounds(float x, float y)
{
	if (_bounds._origin._x == 0 && _bounds._size._width == 0) 
		_bounds._origin._x = x;
	else if (x < _bounds._origin._x)
		_bounds.SetX1(x);

	if (x > _bounds.X2()) 
		_bounds.SetX2(x);

	if (_bounds._origin._y == 0 && _bounds._size._height == 0) 
		_bounds._origin._y = y;
	else if (y < _bounds._origin._y)
		_bounds.SetY1(y);

	if (y > _bounds.Y2()) 
		_bounds.SetY2(y);



}

void PPPath::MoveTo(float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_MoveTo;
    path_item->_pt1._x = x;
    path_item->_pt1._y = y;
    _itemList.push_back(path_item);
	CheckBounds(x, y);
}

void PPPath::LineTo(float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_LineTo;
    path_item->_pt1._x = x;
    path_item->_pt1._y = y;
    _itemList.push_back(path_item);
	CheckBounds(x, y);
}

void PPPath::CurveTo(float cx1, float cy1, float cx2, float cy2, float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_CurveTo;
    path_item->_pt1._x = cx1;
    path_item->_pt1._y = cy1;
    path_item->_pt2._x = cx2;
    path_item->_pt2._y = cy2;
    path_item->_pt3._x = x;
    path_item->_pt3._y = y;
    _itemList.push_back(path_item);
	CheckBounds(cx1, cy1);
	CheckBounds(cx2, cy2);
	CheckBounds(x, y);
}

void PPPath::CurveTo1(float cx2, float cy2, float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_CurveTo1;
    path_item->_pt1._x = cx2;
    path_item->_pt1._y = cy2;
    path_item->_pt2._x = x;
    path_item->_pt2._y = y;
    _itemList.push_back(path_item);
	CheckBounds(cx2, cy2);
	CheckBounds(x, y);
}

void PPPath::CurveTo2(float cx1, float cy1, float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_CurveTo2;
    path_item->_pt1._x = cx1;
    path_item->_pt1._y = cy1;
    path_item->_pt2._x = x;
    path_item->_pt2._y = y;
	CheckBounds(cx1, cy1);
	CheckBounds(x, y);
    _itemList.push_back(path_item);
}

void PPPath::Rectangle(float x, float y, float w, float h)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_Rectangle;
    path_item->_pt1._x = x;
    path_item->_pt1._y = y;
    path_item->_pt2._x = w;
    path_item->_pt2._y = h;
	CheckBounds(x, y);
	CheckBounds(x+w, y+h);
    _itemList.push_back(path_item);
}

void PPPath::Close()
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = (PPPathOperator)PPC_ClosePath;
    _itemList.push_back(path_item);
}

string PPPath::XMLString(int level)  // for Element
{
    string retstr;
    ostringstream ostr;
    
    ostr << PPTabStr(level) << "<Path>" << PP_ENDL;
    size_t i, icnt = _itemList.size();
    for (i=0; i<icnt; i++) {
        PPPathItem *pitem = _itemList.at(i);
        ostr << pitem->XMLString(level+1) ;
    }
    
    ostr << PPTabStr(level) << "</Path>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
