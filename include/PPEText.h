
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
	void SetParser(PPParser *parser);


	string commandString();
	string makeCommandString();
    string xmlString(int level);

	void SetGContext(PPContext *gcontext);
    void addCommand(PPTCommand *cmd);
	PPElementType getType() {return PPET_TEXT;}


	bool HasResource();
	vector <const char *> ResourceList();
	string ResourceKeyFor(const char *rsc_type);


};





#endif