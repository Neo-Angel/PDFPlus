
#ifndef __PDFPlusLib__PPTArray__
#define __PDFPlusLib__PPTArray__

#include <PPToken.h>


// PDF 스펙 중 배열을 담당하는 클래스 
///////////////////////////////////////// PPTArray
class PPTArray : public PPToken {
public:
	// _array에 등록된 토큰들은 이 객체 외부에서 할당된 메모리를 사용하나
	// 메모리 해제는 이 객체가 소멸될 때 같이 해제한다.
    vector <PPToken *> _array; // 
    
	PPTArray(PPDocument *doc) : PPToken(doc) {};
    PPTArray(PPDocument *doc, vector<PPToken *> token_list);
	PPTArray();
    ~PPTArray();

	// PPToken에서 계승된 함수들
    string XMLString(int level);
    string PDFString(); // 실제 PDF에 기록될 문자열을 리턴한다.

	// PPBase에서 계승된 함수들
    inline const char *ClassType() {return PPTN_ARRAY;}
	PPBase *Create() {return new PPTArray;} 
	void CopyMembersTo(PPBase *obj) ;

	void SetDocument(PPDocument *doc);
	void MoveInto(PPDocument *doc); // doc의 소속으로 들어가기 위한 작업을 함.

	// Manipulate _array
    inline size_t Size() {return _array.size();}
    inline PPToken *ObjectAtIndex(int idx) {return _array.at(idx);}
	void AddToken(PPToken *token) {_array.push_back(token);}
	void AddToken(int num); // num을 PPTNumber으로 변환해서 _array에 push한다.
	void Reorder(int from_idx, int to_idx); // from_idx에 있는 토큰을 to_idx 위치로 옮긴다.
	void RemoveTokenAtIndex(int idx);
};
/////////////////////////////////////////

#endif
