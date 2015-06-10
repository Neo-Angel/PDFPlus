
#include "PPTArray.h"
#include "PPTNumber.h"


// PPTArray //////////////////////////////////

PPTArray::PPTArray(PPParser *parser, vector<PPToken *> token_list) : PPToken(parser)
{
    _array = token_list;
}

PPTArray::~PPTArray()
{
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        delete token;
    }
}

string PPTArray::xmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Array>\xa";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->xmlString(level+1);
    }
    retstr += tapStr(level) + "</Array>\xa";

    return retstr;
}

string PPTArray::internalXmlString(int level)
{
    string retstr;
    retstr += tapStr(level) + "<Array>\xa";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->internalXmlString(level+1);
    }
    retstr += tapStr(level) + "</Array>\xa";
    
    return retstr;
}

string PPTArray::pdfString()
{
    string retstr = "[";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        if (i > 0) {
            retstr += " ";
        }
        retstr += token->pdfString();
    }
    retstr += "]";
    return retstr;
}


void PPTArray::AddToken(int num) 
{
	AddToken((PPToken *)new PPTNumber(_parser, num));
}
