


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
    ~PPTDictionary();
    void setTokenAndKey(PPToken *token, string key);
	void SetTokenAndKey(PPToken *token, string key){setTokenAndKey(token, key);}
	void SetTokenAndKey(string name, string key);
	void SetTokenAndKey(int num, string key);
	PPTIndirectObj *SetRefTokenAndKey(PPToken *token, string key, int obj_num);
    PPToken *objectForKey(string &keyname);
    PPToken *objectForKey(const char *keyname);
	PPTName *nameForKey(const char *keyname);
    PPToken *indirectObjectForKey(string &keyname);
    PPToken *valueObjectForKey(string &keyname);
    PPToken *indirectObjectForKey(const char *keyname){string str(keyname);return indirectObjectForKey(str);}
    PPToken *valueObjectForKey(const char *keyname){string str(keyname);return valueObjectForKey(str);}
    string description();
    string xmlString(int level);
    string internalXmlString(int level);
    string pdfString();
    
    inline const char *classType(){return PPTN_DICTIONARY;};
};
/////////////////////////////////////////

#endif
