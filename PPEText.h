
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

public:
	PPEText(){};
    PPEText(PPContext *gcontext) : PPElement(gcontext){}
	PPBase *Create(){return new PPEText;}

	void CopyMembers(PPBase *);
	void SetParser(PPParser *parser);


    string makeCommandString();
    string xmlString(int level);

    void addCommand(PPTCommand *cmd);
	PPElementType getType() {return PPET_TEXT;}
};





#endif