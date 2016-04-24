
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
    retstr += "IndirectRef("+to_string((_Longlong)_objNum)+","+to_string((_Longlong)_genNum)+")";
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

PPTIndirectObj *PPTIndirectRef::targetObject()
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

// �ٸ� ��ť��Ʈ���� ����Ǿ� �Ѿ�ö� ó��
// ��ü�ѹ��� �ٽ� ������.
void PPTIndirectRef::MoveInto(PPDocument *doc)
{
//	PPDocument *src_doc = (PPDocument *)_parser->_owner;
	PPTIndirectObj *copied_obj = NULL;
	int src_id = _document->_docID << 24;
	src_id += _objNum;

	if(_document) {
		// ������ ó���� ���� �ִ� �� Ȯ��
		copied_obj = doc->_srcIndirectObjs[src_id];
	}
	if(!copied_obj) {
		PPTIndirectObj *obj = (PPTIndirectObj *)_document->ObjectForNumber(_objNum);
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
	copied_obj->AddRefObj(this);
}
