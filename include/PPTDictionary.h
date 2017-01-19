


#ifndef __PDFPlusLib__PPTTDictionary__
#define __PDFPlusLib__PPTTDictionary__

#include <PPToken.h>



///////////////////////////////////////// PPTDictionary
class PPTIndirectObj;
class PPTName;
class PPTNumber;

// hash map을 이용한 사전 형식의 자료 구조이다.
// key/value 페어로 구성되어 있다.
// key 는 string 형식의 문자열이고, 
// value는 PPToken의 계승 클래스라면 무엇이든 올 수 있다.
//
class PPTDictionary : public PPToken {

private:
	// _dict에 등록된 토큰들은 이 객체 외부에서 할당된 메모리를 사용하나
	// 메모리 해제는 이 객체가 소멸될 때 같이 해제한다.
    map <string, PPToken *> _dict;

public:
    PPTDictionary(PPDocument *doc);
	PPTDictionary(){}
    ~PPTDictionary();

	// Inherited from super class (PPBase)
	map <string, PPToken *> ::iterator Begin() {return _dict.begin();}
	map <string, PPToken *> ::iterator End() {return _dict.end();}

    string Description();
    inline PPClassType ClassType(){return PPTN_DICTIONARY;};
	PPBase *Create() {return new PPTDictionary();}
	void CopyMembersTo(PPBase *obj) ;

	// Inherited from super class (PPToken)
    string XMLString(int level);
    string PDFString();
	void SetDocument(PPDocument *doc);
	void MoveInto(PPDocument *doc);
	void Merge(PPTDictionary *other_dict);

	// local methods
    void SetTokenAndKey(PPToken *token, string key);
	void SetTokenAndKey(string name, string key);
	void SetTokenAndKey(int num, string key);
	void SetStringAndKey(string str, string key);
	void SetNameAndKey(string name, string key);
	PPTIndirectObj *SetRefTokenAndKey(PPToken *token, string key, int obj_num);
    PPToken *ObjectForKey(string &keyname);
    PPToken *ObjectForKey(const char *keyname);
	PPTIndirectObj *IndirectObjectForKey(string &keyname);
    PPTIndirectObj *IndirectObjectForKey(const char *keyname){string str(keyname);return IndirectObjectForKey(str);}
    PPToken *ValueObjectForKey(string &keyname);
	PPToken *ValueObjectForKey(const char *keyname){string key(keyname); return ValueObjectForKey(key);}
	PPTName *NameForKey(const char *keyname);
	PPTNumber *NumberForKey(string &keyname);
	float FloatForKey(string keyname);    
};
/////////////////////////////////////////

#endif
