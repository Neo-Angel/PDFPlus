
#include <sstream>

#include "PPDocument.h"
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
    ostr <<tabStr(level)<<"<Ref ObjID='"<<_objNum<<"' Gen='"<<_genNum<< "'/>\xa";
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
//        retstr += tabStr(level) + "<Dict>\xa";
//        retstr += obj->internalXmlString(level+1);
//        retstr += tabStr(level) + "</Dict>\xa";
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


void PPTIndirectRef::CopyMembersTo(PPBase *obj) 
{
	PPToken::CopyMembersTo(obj);

	PPTIndirectRef *indir_ref = (PPTIndirectRef *)obj;
	indir_ref->_genNum = _genNum;
	indir_ref->_objNum = _objNum;
}

// 다른 도큐먼트에서 복사되어 넘어올때 처리
// 객체넘버를 다시 적용함.
void PPTIndirectRef::MoveInto(PPDocument *doc)
{
	PPDocument *src_doc = (PPDocument *)_parser->_owner;
	PPTIndirectObj *copied_obj = NULL;
	int src_id = src_doc->_docID << 24;
	src_id += _objNum;

	if(src_doc) {
		// 이전에 처리된 적이 있는 지 확인
		copied_obj = doc->_srcIndirectObjs[src_id];
	}
	if(!copied_obj) {
		PPTIndirectObj *obj = (PPTIndirectObj *)_parser->ObjectForNumber(_objNum);
		if(!obj)
			return;
		copied_obj = (PPTIndirectObj *)obj->Copy();
		copied_obj->MoveInto(doc);
		int new_obj_num = doc->NewObjNum();
		doc->PushObj(copied_obj, new_obj_num);
		copied_obj->_objNum = new_obj_num;
		doc->_srcIndirectObjs[src_id] = copied_obj;
	}
	_objNum = copied_obj->_objNum;
	copied_obj->addRefObj(this);
}
