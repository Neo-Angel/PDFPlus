


#ifndef __PDFPlusLib__PPTTDictionary__
#define __PDFPlusLib__PPTTDictionary__

#include <PPToken.h>



///////////////////////////////////////// PPTDictionary
class PPTIndirectObj;
class PPTName;
class PPTNumber;

// hash map�� �̿��� ���� ������ �ڷ� �����̴�.
// key/value ���� �����Ǿ� �ִ�.
// key �� string ������ ���ڿ��̰�, 
// value�� PPToken�� ��� Ŭ������� �����̵� �� �� �ִ�.
//
class PPTDictionary : public PPToken {

private:
	// _dict�� ��ϵ� ��ū���� �� ��ü �ܺο��� �Ҵ�� �޸𸮸� ����ϳ�
	// �޸� ������ �� ��ü�� �Ҹ�� �� ���� �����Ѵ�.
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
