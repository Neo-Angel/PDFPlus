

#ifndef __PDFPlusLib__PPTComment__
#define __PDFPlusLib__PPTComment__

#include <PPToken.h>




///////////////////////////////////////// PPTComments
class PPTComment : public PPToken {
    string *_comment;
    
public:
    PPTComment(PPDocument *doc, string *str);
	PPTComment() {_comment = NULL;}
    ~PPTComment();
    string XMLString(int level);
    string pdfString();

	PPBase *Create() {return new PPTComment();}
	void CopyMembersTo(PPBase *obj) ;

};
/////////////////////////////////////////


#endif
