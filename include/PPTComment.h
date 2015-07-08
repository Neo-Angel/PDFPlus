

#ifndef __PDFPlusLib__PPTComment__
#define __PDFPlusLib__PPTComment__

#include <PPToken.h>




///////////////////////////////////////// PPTComments
class PPTComment : public PPToken {
    string *_comment;
    
public:
    PPTComment(PPParser *parser, string *str);
	PPTComment() {_comment = NULL;}
    ~PPTComment();
    string xmlString(int level);
    string pdfString();

	PPBase *Create() {return new PPTComment();}
	void CopyMembersTo(PPBase *obj) ;

};
/////////////////////////////////////////


#endif
