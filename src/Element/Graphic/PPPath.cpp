
#include <sstream>
#include "PPPathItem.h"
#include "PPPath.h"



//////////////////////////////////////////////////////////
//
//  PPPath Def (pure Path Class)
//
//////////////////////////////////////////////////////////

void PPPath::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPPath *tar_obj = (PPPath *)obj;
	tar_obj->_bounds = _bounds;

	size_t i, icnt = _itemList.size();
	for(i=0;i<icnt;i++) {
		PPPathItem *src_item = _itemList[i];
		PPPathItem *new_item = (PPPathItem *)src_item->Copy();
		tar_obj->_itemList.push_back(new_item);
	}
}

void PPPath::checkBounds(float x, float y)
{
	if (_bounds._origin._x == 0 && _bounds._size._width == 0) 
		_bounds._origin._x = x;
	else if (x < _bounds._origin._x)
		_bounds.setX1(x);

	if (x > _bounds.getX2()) 
		_bounds.setX2(x);

	if (_bounds._origin._y == 0 && _bounds._size._height == 0) 
		_bounds._origin._y = y;
	else if (y < _bounds._origin._y)
		_bounds.setY1(y);

	if (y > _bounds.getY2()) 
		_bounds.setY2(y);



}

void PPPath::moveTo(float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_MoveTo;
    path_item->_pt1._x = x;
    path_item->_pt1._y = y;
    _itemList.push_back(path_item);
	checkBounds(x, y);
}

void PPPath::lineTo(float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_LineTo;
    path_item->_pt1._x = x;
    path_item->_pt1._y = y;
    _itemList.push_back(path_item);
	checkBounds(x, y);
}

void PPPath::curveTo(float cx1, float cy1, float cx2, float cy2, float x, float y)
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
	checkBounds(cx1, cy1);
	checkBounds(cx2, cy2);
	checkBounds(x, y);
}

void PPPath::curveTo1(float cx2, float cy2, float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_CurveTo1;
    path_item->_pt1._x = cx2;
    path_item->_pt1._y = cy2;
    path_item->_pt2._x = x;
    path_item->_pt2._y = y;
    _itemList.push_back(path_item);
	checkBounds(cx2, cy2);
	checkBounds(x, y);
}

void PPPath::curveTo2(float cx1, float cy1, float x, float y)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_CurveTo2;
    path_item->_pt1._x = cx1;
    path_item->_pt1._y = cy1;
    path_item->_pt2._x = x;
    path_item->_pt2._y = y;
    _itemList.push_back(path_item);
}

void PPPath::rectangle(float x, float y, float w, float h)
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = PPPO_Rectangle;
    path_item->_pt1._x = x;
    path_item->_pt1._y = y;
    path_item->_pt2._x = w;
    path_item->_pt2._y = h;
    _itemList.push_back(path_item);
}

void PPPath::close()
{
    PPPathItem *path_item = new PPPathItem;
    path_item->_op = (PPPathOperator)PPC_ClosePath;
    _itemList.push_back(path_item);
}

string PPPath::xmlString(int level)  // for Element
{
    string retstr;
    ostringstream ostr;
    
    ostr << tapStr(level) << "<Path>" << PP_ENDL;
    size_t i, icnt = _itemList.size();
    for (i=0; i<icnt; i++) {
        PPPathItem *pitem = _itemList.at(i);
        ostr << pitem->xmlString(level+1) ;
    }
    
    ostr << tapStr(level) << "</Path>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}
