
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

	void SetFont(char *font_name, float font_size);

	string commandString();
	string makeCommandString();
    string XMLString(int level);

	void SetGContext(PPContext *gcontext);
    void addCommand(PPTCommand *cmd);
	PPElementType getType() {return PPET_TEXT;}


	bool HasResource();
	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);

	void DrawText(char *text);
};





#endif