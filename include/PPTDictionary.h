


#ifndef __PDFPlusLib__PPTTDictionary__
#define __PDFPlusLib__PPTTDictionary__

#include <PPToken.h>



///////////////////////////////////////// PPTDictionary
class PPTIndirectObj;
class PPTName;

class PPTDictionary : public PPToken {

public:
    map <string, PPToken *> _dict;

    PPTDictionary(PPDocument *doc);
	PPTDictionary(){}
    ~PPTDictionary();
    void setTokenAndKey(PPToken *token, string key);
	void SetTokenAndKey(PPToken *token, string key){setTokenAndKey(token, key);}
	void SetTokenAndKey(string name, string key);
	void SetTokenAndKey(int num, string key);
	void SetStringAndKey(string name, string key);
	void SetNameAndKey(string name, string key);
	PPTIndirectObj *SetRefTokenAndKey(PPToken *token, string key, int obj_num);
    PPToken *ObjectForKey(string &keyname);
    PPToken *ObjectForKey(const char *keyname);
	PPTName *NameForKey(const char *keyname);
    PPTIndirectObj *IndirectObjectForKey(string &keyname);
    PPTIndirectObj *IndirectObjectForKey(const char *keyname){string str(keyname);return IndirectObjectForKey(str);}
    PPToken *ValueObjectForKey(string &keyname);
	PPToken *ValueObjectForKey(const char *keyname){string key(keyname); return ValueObjectForKey(key);}

    string Description();
    string XMLString(int level);
    string pdfString();
    
	PPBase *Create() {return new PPTDictionary();}
	void CopyMembersTo(PPBase *obj) ;
	void SetDocument(PPDocument *doc);
	void MoveInto(PPDocument *doc);

    inline const char *ClassType(){return PPTN_DICTIONARY;};
};
/////////////////////////////////////////

#endif
