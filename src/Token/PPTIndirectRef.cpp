
#include <sstream>

#include "PPDocument.h"
#include "PPTIndirectRef.h"

//  ////////////////////////////////// PPTIndirectRef
#include "PPTIndirectObj.h"
#include "PPParser.h"

PPTIndirectRef::PPTIndirectRef(PPDocument *doc, int num1, int num2):PPToken(doc)
{
    _objNum = num1;
    _genNum = num2;
    _document->_ref_list.push_back(this);
}

string PPTIndirectRef::Description()
{
    string retstr = "";
#ifdef _WIN32
    retstr += "IndirectRef("+to_string((_Longlong)_objNum)+","+to_string((_Longlong)_genNum)+")";
#else
    retstr += "IndirectRef("+to_string((long long)_objNum)+","+to_string((long long)_genNum)+")";
#endif
    return retstr;
}

string PPTIndirectRef::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<PPTabStr(level)<<"<Ref ObjID='"<<_objNum<<"' Gen='"<<_genNum<< "'/>\xa";
    retstr = ostr.str();
    return retstr;
}

string PPTIndirectRef::PDFString()
{
    string retstr;
    ostringstream ostr;
    ostr << _objNum << " " << _genNum << " R";
    retstr = ostr.str();
    return retstr;
}

PPToken *PPTIndirectRef::ValueObject()
{
    PPTIndirectObj *tar = TargetObject();
    if (tar) {
        return tar->NumberOfTokens() > 0 ? tar->TokenAtIndex(0) : NULL;
    }
    return NULL;
    
}

PPTIndirectObj *PPTIndirectRef::TargetObject()
{
    PPTIndirectObj *ret = (PPTIndirectObj *)_document->ObjectForNumber(_objNum);
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

//	PPDocument *src_doc = (PPDocument *)_parser->_owner;
	PPTIndirectObj *copied_obj = NULL;
	int src_id = _document->_docID << 24;
	src_id += _objNum;

//	if(_document) {
		// 이전에 처리된 적이 있는 지 확인
		copied_obj = doc->_srcIndirectObjs[src_id];
//	}
	if(!copied_obj) {
		PPTIndirectObj *org_obj = (PPTIndirectObj *)_document->ObjectForNumber(_objNum);
		if(!org_obj)
			return;
		PPToken *clone_token = (PPToken *)_clone;
		PPTIndirectObj *clone_parent = clone_token->_parentObj;
		if(clone_parent != NULL && org_obj == clone_parent->_parentObj) {
			PPTIndirectObj *pObj = _parentObj->GetParentObj();
			if(pObj) {
				_objNum = pObj->_objNum;
			}
			return;
		}
		copied_obj = (PPTIndirectObj *)org_obj->Copy();
		copied_obj->MoveInto(doc);

		int new_obj_num = doc->NewObjNum();
		doc->PushObj(copied_obj, new_obj_num);
		copied_obj->_objNum = new_obj_num;
		doc->_srcIndirectObjs[src_id] = copied_obj;
	}
	_objNum = copied_obj->_objNum;
	copied_obj->AddRefObj(this);

	PPToken::MoveInto(doc);

}
