
#ifndef __PDFPlusLib__PPEText__
#define __PDFPlusLib__PPEText__

#include <PPElement.h>

//
//      PPEText
//
///////////////////////////////////////////////////////////////
class PPTCommand;

class PPEText : public PPElement {
    vector<PPTCommand *> _cmdList;
    vector<string *> _textList;
	vector<string *> _fontKeyList;

public:
	PPEText(){};
    PPEText(PPContext *gcontext) : PPElement(gcontext){}
	PPBase *Create(){return new PPEText;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);

	string CommandString();
	string MakeCommandString();
    string XMLString(int level);

	bool HasResource();
	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);

	void SetGContext(PPContext *gcontext);
    void AddCommand(PPTCommand *cmd);
	PPElementType Type() {return PPET_TEXT;}

	void SetString(string str);
	string String();
	void SetFont(string font_name, float font_size);

};





#endif