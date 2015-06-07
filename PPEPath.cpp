
#include <sstream>
#include "PPEPath.h"
#include "PPGState.h"


class PPContext;


//////////////////////////////////////////////////////////
//
//  PPEPath (Path Element)
//
//////////////////////////////////////////////////////////

PPEPath::PPEPath(PPPath *path, PPContext *gcontext):PPElement(gcontext)
{
    _path = path;
}

PPEPath::PPEPath(PPContext *gcontext):PPElement(gcontext)
{
    _path = new PPPath;
}

void PPEPath::CopyMembers(PPBase *obj)
{
	PPElement::CopyMembers(obj);
	PPEPath *tar_obj = (PPEPath *)obj;
	tar_obj->_path = (PPPath *)_path->Copy();
	tar_obj->_strokeType = _strokeType;
	tar_obj->_strokeType = _strokeType;
	tar_obj->_fillType = _fillType;


}

void PPEPath::stroke()
{
    _strokeType = PPEP_Stroke;
}

void PPEPath::fill()
{
    _fillType = PPEP_Fill;
}

void PPEPath::eofill()
{
    _fillType = PPEP_EOFill;
}

void PPEPath::clip()
{
    _clipType = PPEP_Clip;
}

void PPEPath::eoclip()
{
    _clipType = PPEP_EOClip;
}

//PPC_Stroke, PPC_CloseStroke, PPC_Fill, PPC_Fill2, PPC_EOFill, PPC_FillStroke, PPC_EOFillStroke,
//PPC_CloseFillStroke, PPC_CloseEOFillStroke, PPC_EndPath,
//PPC_Clip, PPC_EOClip,

void PPEPath::setPaintingType(int type)
{
    switch (type) {
        case PPC_Stroke:
            stroke();
            break;
        case PPC_CloseStroke:
            _path->close();
            stroke();
            break;
        case PPC_Fill:
            fill();
            break;
        case PPC_Fill2:
            fill();
            break;
        case PPC_EOFill:
            eofill();
            break;
        case PPC_FillStroke:
            fill();
            stroke();
            break;
        case PPC_EOFillStroke:
            eofill();
            stroke();
            break;
        case PPC_CloseFillStroke:
            _path->close();
            fill();
            stroke();
            break;
        case PPC_CloseEOFillStroke:
            _path->close();
            fill();
            stroke();
            break;
        case PPC_EndPath:
            _strokeType = PPEP_NonStroke;
            _fillType = PPEP_NonFill;
            break;
        case PPC_Clip:
            clip();
            break;
        case PPC_EOClip:
            eoclip();
            break;
            
        default:
            break;
    }
}

string PPEPath::makeCommandString()
{
    _gstate->makeCommandString();
    string retstr;
    size_t i, icnt = _path->_itemList.size();
    for (i=0; i<icnt; i++) {
        PPPathItem *path_item = _path->_itemList.at(i);
        retstr += path_item->makeCommand();
    }
    return retstr;
}


string PPEPath::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    const char *stroke_state =(_fillType != PPEP_NonFill ? "yes" : "no") ;
    const char *fill_state = (_fillType != PPEP_NonFill ? "yes" : "no");
    const char *clipping_state = (_clipType > 0 ? "yes" : "no");
    
    ostr << tapStr(level) << "<Element  type='Path' stroke='"<< stroke_state;
    ostr << "' fill='" << fill_state;
    ostr << "' clipping= '" << clipping_state << "'>" << PP_ENDL;

    ostr << _gstate->xmlString(level+1);
    ostr << _path->xmlString(level+1);
   
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

