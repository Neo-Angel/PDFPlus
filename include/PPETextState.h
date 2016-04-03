
#ifndef __PDFPlusLib__PPETextState__
#define __PDFPlusLib__PPETextState__

#include <PPElement.h>




//
//      PPETextState
//
///////////////////////////////////////////////////////////////
class PPTCommand;

class PPETextState : public PPElement {
    vector<PPTCommand *> _cmdList;
	vector<string *> _fontKeyList;

public:
	PPETextState(){};
    PPETextState(PPContext *gcontext) : PPElement(gcontext){}
	PPBase *Create(){return new PPETextState;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);


	string commandString();
	string makeCommandString();
    string XMLString(int level);

	void SetGContext(PPContext *gcontext);
    void addCommand(PPTCommand *cmd);
	PPElementType getType() {return PPET_TEXT_STATE;}


	bool HasResource();
	vector <const char *> ResourceTypeList();
	string ResourceKeyFor(const char *rsc_type);


};





#endif