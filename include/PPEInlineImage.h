
#ifndef __PDFPlusLib__PPEInlineImage__
#define __PDFPlusLib__PPEInlineImage__

#include <PPElement.h>




//
//      PPEInlineImage
//
///////////////////////////////////////////////////////////////
class PPEInlineImage : public PPElement {
    map <string, PPToken *> _dict;  // ???
    char * _data;  //  ???
    
public:
    PPEInlineImage(PPContext *gcontext) : PPElement(gcontext){}
	PPEInlineImage(){}
	PPBase *Create(){return new PPEInlineImage;}

	void CopyMembersTo(PPBase *);
	void SetDocument(PPDocument *doc);

    void setBeginImageObject(PPTCommand *cmd){}
    void setBeginImageData(PPTCommand *cmd){}
    string makeCommandString();
    string XMLString(int level);
	PPToken *getXObject();
	PPElementType getType() {return PPET_INLINE_IMAGE;}

};


#endif