

#ifndef __PDFPlusLib__PPTComment__
#define __PDFPlusLib__PPTComment__

#include <PPToken.h>




// PPTComments
////////////////////////////////////////////////////////////////////////////
// PDF 동작엔 영향을 미치지 않는 설명이나 해설등 문자열 표현하기 위한 규격
// 주로 헤더 부분에 사용 되어지고 때때로 중요한 정보를 포함하는 경우도 있다.
//////////////////////////////////////////////////////////////////////////// 
class PPTComment : public PPToken {

protected: // delete 문제 때문에 직접 접근을 차단한다.
    string *_comment;
    
public:
    PPTComment(PPDocument *doc, string *str);
	PPTComment() {_comment = NULL;}
    ~PPTComment(); // _comment 를 delete 한다.
    string XMLString(int level);
    string PDFString();

	PPBase *Create() {return new PPTComment();}
	void CopyMembersTo(PPBase *obj) ;

	// _comment가 protected이기 때문에 아래와 같은 함수를 제공.
	void SetComment(string *str);
	string *Comment() {return _comment;}

};
/////////////////////////////////////////


#endif
