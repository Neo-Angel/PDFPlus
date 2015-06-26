
#include "PPPathItem.h"
#include <sstream>

extern PPCommandInfo PPCommandList[];
//////////////////////////////////////////////////////////
//
//  PPPathItem def
//  Path Item
//
//////////////////////////////////////////////////////////

void PPPathItem::CopyMembers(PPBase *obj)
{
	PPBase::CopyMembers(obj);
	PPPathItem *new_obj = (PPPathItem *) obj;
	new_obj->_op = _op;
	new_obj->_pt1 = _pt1;
	new_obj->_pt2 = _pt2;
	new_obj->_pt3 = _pt3;
}


string PPPathItem::makeCommand()
{
    ostringstream ostr;
    switch (_op) {
        case PPC_MoveTo:
            ostr << _pt1._x << " " << _pt1._y << " m" << PP_ENDL;
            break;
        case PPC_LineTo:
            ostr << _pt1._x << " " << _pt1._y << " l" << PP_ENDL;
            break;
        case PPC_CurveTo:
            ostr << _pt1._x << " " << _pt1._y << " " ;
            ostr << _pt2._x << " " << _pt2._y << " " ;
            ostr << _pt3._x << " " << _pt3._y << " c" << PP_ENDL;
            break;
        case PPC_CurveTo1:
            ostr << _pt1._x << " " << _pt1._y << " " ;
            ostr << _pt2._x << " " << _pt2._y << " v" << PP_ENDL;
            break;
        case PPC_CurveTo2:
            ostr << _pt1._x << " " << _pt1._y << " " ;
            ostr << _pt2._x << " " << _pt2._y << " y" << PP_ENDL;
            break;
            
        default:
            ostr << PPCommandList[_op].code << PP_ENDL;
            break;
    }
    
    
    return ostr.str();
}

string PPPathItem::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    switch (_op) {
        case PPC_MoveTo:
            ostr << tapStr(level) << "<MoveTo X=" << _pt1._x <<" Y=" << _pt1._y << ">" << PP_ENDL;
            break;
        case PPC_LineTo:
            ostr << tapStr(level) << "<LineTo X=" << _pt1._x <<" Y=" << _pt1._y << ">" << PP_ENDL;
            break;
        case PPC_CurveTo1:
            ostr << tapStr(level) << "<CurveTo1 X1=" << _pt1._x <<" Y1=" << _pt1._y
            << " X2=" << _pt2._x <<" Y2=" << _pt2._y << ">" << PP_ENDL;
            break;
        case PPC_CurveTo2:
            ostr << tapStr(level) << "<CurveTo2 X1=" << _pt1._x <<" Y1=" << _pt1._y
            << " X2=" << _pt2._x <<" Y2=" << _pt2._y << ">" << PP_ENDL;
            break;
        case PPC_CurveTo:
            ostr << tapStr(level) << "<CurveTo X1=" << _pt1._x <<" Y1=" << _pt1._y
            << " X2=" << _pt2._x <<" Y2=" << _pt2._y << " X3=" << _pt3._x <<" Y3=" << _pt3._y <<">" << PP_ENDL;
            break;
        case PPC_Rectangle:
            ostr << tapStr(level) << "<Rectangle X1=" << _pt1._x <<" Y1=" << _pt1._y
            << " X2=" << _pt2._x <<" Y2=" << _pt2._y << ">" << PP_ENDL;
            break;
        case PPC_ClosePath:
            ostr << tapStr(level) << "<ClosePath/>" << PP_ENDL;
            break;

        default:
            break;
    }
    retstr = ostr.str();
    return retstr;
}