
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

    void SetBeginImageObject(PPTCommand *cmd){}
    void SetBeginImageData(PPTCommand *cmd){}
    string MakeCommandString();
    string XMLString(int level);
	PPToken *XObject();
	PPElementType Type() {return PPET_INLINE_IMAGE;}

};


#endif