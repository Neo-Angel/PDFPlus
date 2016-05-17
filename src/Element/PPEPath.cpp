
#include <sstream>
#include "PPEPath.h"
#include "PPGState.h"
#include "PPPathItem.h"

extern PPCommandInfo PPCommandList[];

class PPContext;


//////////////////////////////////////////////////////////
//
//  PPEPath (Path Element)
//
//////////////////////////////////////////////////////////

PPEPath::PPEPath(PPPath *path, PPContext *gcontext):PPElement(gcontext)
{
    _path = path;
	_strokeType = PPEP_NonStroke;
	_clipType = PPEP_NonClip;
	_fillType = PPEP_NonFill;
	_paintingType = PPC_NoPaint;
}

PPEPath::PPEPath(PPContext *gcontext):PPElement(gcontext)
{
    _path = new PPPath;
	_strokeType = PPEP_NonStroke;
	_clipType = PPEP_NonClip;
	_fillType = PPEP_NonFill;

	_paintingType = PPC_NoPaint;
}

PPEPath::PPEPath(float x, float y, float w, float h, PPContext *gcontext):PPElement(gcontext)
{
    _path = new PPPath;
	_strokeType = PPEP_NonStroke;
	_clipType = PPEP_NonClip;
	_fillType = PPEP_NonFill;

	_paintingType = PPC_NoPaint;

	this->AddRect(x, y, w, h);
}

void PPEPath::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEPath *tar_obj = (PPEPath *)obj;
	if(_path)
		tar_obj->_path = (PPPath *)_path->Copy();
	tar_obj->_clipType = _clipType;
	tar_obj->_strokeType = _strokeType;
	tar_obj->_fillType = _fillType;
	tar_obj->_paintingType = _paintingType;

}

void PPEPath::Stroke()
{
    _strokeType = PPEP_Stroke;
}

void PPEPath::Fill()
{
    _fillType = PPEP_Fill;
}

void PPEPath::EOFill()
{
    _fillType = PPEP_EOFill;
}

void PPEPath::Clip()
{
    _clipType = PPEP_Clip;
}

void PPEPath::EOClip()
{
    _clipType = PPEP_EOClip;
}

//PPC_Stroke, PPC_CloseStroke, PPC_Fill, PPC_Fill2, PPC_EOFill, PPC_FillStroke, PPC_EOFillStroke,
//PPC_CloseFillStroke, PPC_CloseEOFillStroke, PPC_NoPaint,
//PPC_Clip, PPC_EOClip,

void PPEPath::SetPaintingType(int type)
{
	_paintingType = type;
    switch (type) {
        case PPC_Stroke:
            Stroke();
            break;
        case PPC_CloseStroke:
            _path->Close();
            Stroke();
            break;
        case PPC_Fill:
            Fill();
            break;
        case PPC_Fill2:
            Fill();
            break;
        case PPC_EOFill:
            EOFill();
            break;
        case PPC_FillStroke:
            Fill();
            Stroke();
            break;
        case PPC_EOFillStroke:
            EOFill();
            Stroke();
            break;
        case PPC_CloseFillStroke:
            _path->Close();
            Fill();
            Stroke();
            break;
        case PPC_CloseEOFillStroke:
            _path->Close();
            Fill();
            Stroke();
            break;
        case PPC_NoPaint:
            _strokeType = PPEP_NonStroke;
            _fillType = PPEP_NonFill;
            break;
        case PPC_Clip:
            Clip();
            break;
        case PPC_EOClip:
            EOClip();
            break;
            
        default:
            break;
    }
}

string PPEPath::MakeCommandString()
{
//    _gstate->makeCommandString();
    string retstr;
    size_t i, icnt = _path->_itemList.size();
    for (i=0; i<icnt; i++) {
        PPPathItem *path_item = _path->_itemList.at(i);
		string path_item_str = path_item->MakeCommand();
        retstr += path_item_str;
    }
	if(_clipType == PPEP_Clip) {
		retstr += "W ";
	}
	else if(_clipType == PPEP_EOClip) {
		retstr += "W* ";
	}
	else {
		if(_strokeType == PPEP_Stroke) {
			if(_fillType == PPEP_Fill) {
				retstr += "B";
			}
			else if(_fillType == PPEP_EOFill) {
				retstr += "B*";
			}
			else {
				retstr += "S";
			}
		}
		else {
			if(_fillType == PPEP_Fill) {
				retstr += "f";
			}
			else if(_fillType == PPEP_EOFill) {
				retstr += "f*";
			}
		}
	}
	retstr += PP_ENDL;
    return retstr;
}


string PPEPath::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    const char *stroke_state =(_fillType != PPEP_NonFill ? "yes" : "no") ;
    const char *fill_state = (_fillType != PPEP_NonFill ? "yes" : "no");
    const char *clipping_state = (_clipType > 0 ? "yes" : "no");
    
    ostr << PPTabStr(level) << "<Element  type='Path' stroke='"<< stroke_state;
    ostr << "' fill='" << fill_state;
    ostr << "' clipping= '" << clipping_state << "'>" << PP_ENDL;

    ostr << _gstate->XMLString(level+1);
    ostr << _path->XMLString(level+1);
   
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

void PPEPath::AddRect(float x, float y, float w, float h)
{
	_path->MoveTo(x, y);
	_path->LineTo(x+w, y);
	_path->LineTo(x+w, y+h);
	_path->LineTo(x, y+h);
	_path->Close();
}

