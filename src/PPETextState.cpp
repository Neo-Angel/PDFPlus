
#include <sstream>
#include "PPETextState.h"
#include "PPGState.h"
#include "PPContext.h"
#include "PPTName.h"


//  Text
//
///////////////////////////////////////////////////////

void PPETextState::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPETextState *tar_obj = (PPETextState *)obj;

	size_t i, icnt = _cmdList.size();
	for(i=0;i<icnt;i++) {
		PPTCommand *src_obj = _cmdList.at(i);
		PPTCommand *new_obj = (PPTCommand *)src_obj->Copy();
		tar_obj->_cmdList.push_back(new_obj);
	}

	icnt = _fontKeyList.size();
	for(i=0;i<icnt;i++) {
		string *src_fkey = _fontKeyList.at(i);
		string *new_fkey = new string(*src_fkey);
		tar_obj->_fontKeyList.push_back(new_fkey);

		PPDocument *tar_doc = tar_obj->_parentForm->_document;
		PPDocument *this_doc = this->_parentForm->_document;
		if(tar_doc !=  this_doc && new_fkey->length() > 0) {
			PPTIndirectObj *rsc_obj = _parentForm->ResourceObjForName(*new_fkey, "Font");
			if(rsc_obj) {
				// this->Document() 에 있는 rsc_obj를 tar_doc에 복사해 넣음
				// 복사할 때 rsc_obj의 objNum를 tar_doc에 맞춰서 변경함.
				PPTIndirectObj *new_obj = tar_doc->MoveObjFrom(rsc_obj, this->Document());
				PPTIndirectRef *new_ref = tar_obj->_parentForm->AddResourceRef(new_obj->_objNum, *new_fkey, "Font");
				if(new_ref) {
					new_obj->AddRefObj(new_ref);
				}
			}
		}

	}

}

void PPETextState::SetDocument(PPDocument *doc)
{
	size_t i, icnt = _cmdList.size();
	for(i=0;i<icnt;i++) {
		PPTCommand *acmd = _cmdList.at(i);
		if(acmd)
			acmd->_document = doc;
	}
}

string PPETextState::CommandString()
{
    string cmd_str;
    
	cmd_str += PPElement::CommandString();
    return cmd_str;
}

string PPETextState::MakeCommandString()
{
    string retstr;

    size_t i, icnt = _cmdList.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _cmdList.at(i);
        retstr += cmd->PDFString();
    }

    return retstr;
}

void PPETextState::AddCommand(PPTCommand *cmd)
{
    _cmdList.push_back(cmd);
	if(cmd->_cmdInfo->code == PPC_FontAndSize) {
		PPTName *font_name = (PPTName *)cmd->_operands[0];
		_fontKeyList.push_back(font_name->_name);
	}
}

void PPETextState::SetGContext(PPContext *gcontext)
{
	_gstate = gcontext->NewGState();
	_gstate->_parent = this;
}

string PPETextState::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='TextState'>" << PP_ENDL;
    ostr << _gstate->XMLString(level+1);
    ostr << PPTabStr(level+1) << "<State>" << PP_ENDL;
    size_t i, icnt = _cmdList.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _cmdList.at(i);
        ostr << cmd->PDFString();
    }
    ostr << PPTabStr(level+1) << "</State>" << PP_ENDL;
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}


bool PPETextState::HasResource()
{
	if(_fontKeyList.size() > 0) {
		return true;
	}
	return PPElement::HasResource();
}
/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPETextState::ResourceTypeList()
{
	vector <const char *> rsc_list = PPElement::ResourceTypeList();
	if(_fontKeyList.size() > 0) {
		rsc_list.push_back(PPRT_FONT);
	}
	return rsc_list;
}

string PPETextState::ResourceKeyFor(const char *rsc_type)
{
	string key = PPElement::ResourceKeyFor(rsc_type);
	if(key.length() > 0) {
		return key;
	}
	string ret_key;
	if(rsc_type == PPRT_FONT) {
		if(_fontKeyList.size() > 0) {
			ret_key = *_fontKeyList[0];
		}
	}
	return ret_key;
}

