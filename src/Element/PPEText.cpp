
#include <sstream>
#include "PPEText.h"
#include "PPGState.h"
#include "PPContext.h"
#include "PPTName.h"

/* Example
/F0 120 Tf
BT
0.707123 0.70709 -0.70709 0.707123 56.5731 197.989 Tm
(A text layer!) Tj
ET
*/

//  Text
//
///////////////////////////////////////////////////////

void PPEText::CopyMembersTo(PPBase *obj)
{
	PPElement::CopyMembersTo(obj);
	PPEText *tar_obj = (PPEText *)obj;

	size_t i, icnt = _cmdList.size();
	for(i=0;i<icnt;i++) {
		PPTCommand *src_obj = _cmdList.at(i);
		PPTCommand *new_obj = (PPTCommand *)src_obj->Copy();
		tar_obj->_cmdList.push_back(new_obj);
	}
	
	icnt = _textList.size();
	for(i=0;i<icnt;i++) {
		string *src_obj = _textList.at(i);
		string *new_obj = new string(*src_obj);
		tar_obj->_textList.push_back(new_obj);
	}

	icnt = _fontKeyList.size();
	for(i=0;i<icnt;i++) {
		string *src_obj = _fontKeyList.at(i);
		string *new_obj = new string(*src_obj);
		tar_obj->_fontKeyList.push_back(new_obj);
	}

}

void PPEText::SetDocument(PPDocument *doc)
{
	size_t i, icnt = _cmdList.size();
	for(i=0;i<icnt;i++) {
		PPTCommand *acmd = _cmdList.at(i);
		if(acmd)
			acmd->_document = doc;
	}
}

string PPEText::commandString()
{
    string cmd_str;
    
	cmd_str += "BT\xa";
	cmd_str += PPElement::commandString();
	cmd_str += "ET\xa";
    return cmd_str;
}

string PPEText::makeCommandString()
{
    string retstr;

    size_t i, icnt = _cmdList.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _cmdList.at(i);
        retstr += cmd->pdfString();
    }

    return retstr;
}

void PPEText::addCommand(PPTCommand *cmd)
{
    _cmdList.push_back(cmd);
	if(cmd->_cmdInfo->type == PPC_FontAndSize) {
		PPTName *font_name = (PPTName *)cmd->_operands[0];
		_fontKeyList.push_back(font_name->_name);
	}
}

void PPEText::SetGContext(PPContext *gcontext)
{
	_gstate = gcontext->newGState();
}

string PPEText::XMLString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << PPTabStr(level) << "<Element type='Text'>" << PP_ENDL;
    ostr << _gstate->XMLString(level+1);
    ostr << PPTabStr(level+1) << "<Text>" << PP_ENDL;
    size_t i, icnt = _cmdList.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _cmdList.at(i);
        ostr << cmd->pdfString();
    }
    ostr << PPTabStr(level+1) << "</Text>" << PP_ENDL;
    ostr << PPTabStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}


bool PPEText::HasResource()
{
	if(_fontKeyList.size() > 0) {
		return true;
	}
	return PPElement::HasResource();
}
/////////////////////////////////////////////////////  Multi Resource Handling
vector <const char *> PPEText::ResourceTypeList()
{
	vector <const char *> rsc_list = PPElement::ResourceTypeList();
	if(_fontKeyList.size() > 0) {
		rsc_list.push_back(PPRT_FONT);
	}
	return rsc_list;
}

string PPEText::ResourceKeyFor(const char *rsc_type)
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


void PPEText::SetFont(char *font_name, float font_size)
{

}

void PPEText::DrawText(char *text)
{

}