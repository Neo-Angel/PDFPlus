
#include <sstream>
#include "PPTIndirectRef.h"

//  ////////////////////////////////// PPTIndirectRef
#include "PPTIndirectObj.h"
#include "PPParser.h"

PPTIndirectRef::PPTIndirectRef(PPParser *parser, int num1, int num2):PPToken(parser)
{
    _objNum = num1;
    _genNum = num2;
    parser->_ref_list.push_back(this);
}

string PPTIndirectRef::description()
{
    string retstr = "";
    retstr += "IndirectRef("+to_string((_Longlong)_objNum)+","+to_string((_Longlong)_genNum)+")";
    return retstr;
}

string PPTIndirectRef::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<tapStr(level)<<"<Ref ObjID='"<<_objNum<<"' Gen='"<<_genNum<< "'/>\xa";
    retstr = ostr.str();
    return retstr;
}

string PPTIndirectRef::pdfString()
{
    string retstr;
    ostringstream ostr;
    ostr << _objNum << " " << _genNum << " R";
    retstr = ostr.str();
    return retstr;
}

PPToken *PPTIndirectRef::valueObject()
{
    PPTIndirectObj *tar = targetObject();
    if (tar) {
        return tar->_array.size() > 0 ? tar->_array[0] : NULL;
    }
    return NULL;
    
}

string PPTIndirectRef::internalXmlString(int level)
{
    string retstr;
    PPToken *obj = valueObject();
//    if (obj->classType() == PPTN_DICTIONARY) {
//        retstr += tapStr(level) + "<Dict>\xa";
//        retstr += obj->internalXmlString(level+1);
//        retstr += tapStr(level) + "</Dict>\xa";
//    }
//    else {
        retstr += obj->internalXmlString(level);
//    }
    return retstr;
}

PPTIndirectObj *PPTIndirectRef::targetObject()
{
    PPTIndirectObj *ret = (PPTIndirectObj *)_parser->objectByID(_objNum);
    return ret;
}
