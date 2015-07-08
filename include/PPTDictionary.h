


#ifndef __PDFPlusLib__PPTTDictionary__
#define __PDFPlusLib__PPTTDictionary__

#include <PPToken.h>



///////////////////////////////////////// PPTDictionary
class PPTIndirectObj;
class PPTName;

class PPTDictionary : public PPToken {

public:
    map <string, PPToken *> _dict;

    PPTDictionary(PPParser *parser);
	PPTDictionary(){}
    ~PPTDictionary();
    void setTokenAndKey(PPToken *token, string key);
	void SetTokenAndKey(PPToken *token, string key){setTokenAndKey(token, key);}
	void SetTokenAndKey(string name, string key);
	void SetTokenAndKey(int num, string key);
	PPTIndirectObj *SetRefTokenAndKey(PPToken *token, string key, int obj_num);
    PPToken *objectForKey(string &keyname);
    PPToken *objectForKey(const char *keyname);
	PPToken *ObjectForKey(const char *keyname){return objectForKey(keyname);}
	PPTName *nameForKey(const char *keyname);
    PPToken *indirectObjectForKey(string &keyname);
    PPToken *valueObjectForKey(string &keyname);
	PPToken *ValueObjectForKey(string &keyname){return valueObjectForKey(keyname);}
	PPToken *ValueObjectForKey(const char *keyname){string key(keyname); return ValueObjectForKey(key);}
    PPToken *indirectObjectForKey(const char *keyname){string str(keyname);return indirectObjectForKey(str);}
    PPToken *valueObjectForKey(const char *keyname){string str(keyname);return valueObjectForKey(str);}
    string description();
    string xmlString(int level);
    string internalXmlString(int level);
    string pdfString();
    
	PPBase *Create() {return new PPTDictionary();}
	void CopyMembersTo(PPBase *obj) ;
	void SetParser(PPParser *parser);


    inline const char *classType(){return PPTN_DICTIONARY;};
};
/////////////////////////////////////////

#endif
