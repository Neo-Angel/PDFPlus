
#ifndef __PDFPlusLib__PPTArray__
#define __PDFPlusLib__PPTArray__

#include <PPToken.h>


// PDF ���� �� �迭�� ����ϴ� Ŭ���� 
///////////////////////////////////////// PPTArray
class PPTArray : public PPToken {
private:
	// _array�� ��ϵ� ��ū���� �� ��ü �ܺο��� �Ҵ�� �޸𸮸� ����ϳ�
	// �޸� ������ �� ��ü�� �Ҹ�� �� ���� �����Ѵ�.
    vector <PPToken *> _array; // 

public:
    
	PPTArray(PPDocument *doc) : PPToken(doc) {};
    PPTArray(PPDocument *doc, vector<PPToken *> token_list);
	PPTArray();
    ~PPTArray();

	// PPToken���� ��µ� �Լ���
    string XMLString(int level);
    string PDFString(); // ���� PDF�� ��ϵ� ���ڿ��� �����Ѵ�.

	// PPBase���� ��µ� �Լ���
    inline PPClassType ClassType() {return PPTN_ARRAY;}
	PPBase *Create() {return new PPTArray;} 
	void CopyMembersTo(PPBase *obj) ;

	void SetDocument(PPDocument *doc);
	void MoveInto(PPDocument *doc); // doc�� �Ҽ����� ���� ���� �۾��� ��.

	// Manipulate _array
    inline size_t Size() {return _array.size();}
	size_t NumberOfTokens() {return _array.size();}
    inline PPToken *TokenAtIndex(uint idx) {return _array.at(idx);}
	void AddToken(PPToken *token);
	void AddToken(int num); // num�� PPTNumber���� ��ȯ�ؼ� _array�� push�Ѵ�.
	void Reorder(int from_idx, int to_idx); // from_idx�� �ִ� ��ū�� to_idx ��ġ�� �ű��.
	void RemoveTokenAtIndex(uint idx);
	void Clear();
};
/////////////////////////////////////////

#endif
