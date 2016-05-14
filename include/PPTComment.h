

#ifndef __PDFPlusLib__PPTComment__
#define __PDFPlusLib__PPTComment__

#include <PPToken.h>




// PPTComments
////////////////////////////////////////////////////////////////////////////
// PDF ���ۿ� ������ ��ġ�� �ʴ� �����̳� �ؼ��� ���ڿ� ǥ���ϱ� ���� �԰�
// �ַ� ��� �κп� ��� �Ǿ����� ������ �߿��� ������ �����ϴ� ��쵵 �ִ�.
//////////////////////////////////////////////////////////////////////////// 
class PPTComment : public PPToken {

protected: // delete ���� ������ ���� ������ �����Ѵ�.
    string *_comment;
    
public:
    PPTComment(PPDocument *doc, string *str);
	PPTComment() {_comment = NULL;}
    ~PPTComment(); // _comment �� delete �Ѵ�.
    string XMLString(int level);
    string PDFString();

	PPBase *Create() {return new PPTComment();}
	void CopyMembersTo(PPBase *obj) ;

	// _comment�� protected�̱� ������ �Ʒ��� ���� �Լ��� ����.
	void SetComment(string *str);
	string *Comment() {return _comment;}

};
/////////////////////////////////////////


#endif
