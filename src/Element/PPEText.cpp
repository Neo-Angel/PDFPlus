
#include <sstream>
#include "PPEText.h"
#include "PPGState.h"



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
}

void PPEText::SetParser(PPParser *parser)
{
	size_t i, icnt = _cmdList.size();
	for(i=0;i<icnt;i++) {
		PPTCommand *acmd = _cmdList.at(i);
		if(acmd)
			acmd->_parser = parser;
	}
}

string PPEText::makeCommandString()
{
    string retstr;
    
	retstr += "BT\xa";
    size_t i, icnt = _cmdList.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _cmdList.at(i);
        retstr += cmd->pdfString();
    }
	retstr += "ET\xa";
    return retstr;
}

void PPEText::addCommand(PPTCommand *cmd)
{
    _cmdList.push_back(cmd);
}

string PPEText::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr << tapStr(level) << "<Element type='Text'>" << PP_ENDL;
    ostr << _gstate->xmlString(level+1);
    ostr << tapStr(level+1) << "<Text>" << PP_ENDL;
    size_t i, icnt = _cmdList.size();
    for (i=0; i<icnt; i++) {
        PPTCommand *cmd = _cmdList.at(i);
        ostr << cmd->pdfString();
    }
    ostr << tapStr(level+1) << "</Text>" << PP_ENDL;
    ostr << tapStr(level) << "</Element>" << PP_ENDL;
    retstr = ostr.str();
    return retstr;
}

string PPEText::commandString()
{
	return PPElement::commandString();

}