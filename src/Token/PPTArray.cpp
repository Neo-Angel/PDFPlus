
#include "PPTArray.h"
#include "PPTNumber.h"


// PPTArray //////////////////////////////////

PPTArray::PPTArray(PPParser *parser, vector<PPToken *> token_list) : PPToken(parser)
{
    _array = token_list;
}

PPTArray::PPTArray()
{

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


void PPTArray::CopyMembersTo(PPBase *obj) 
{
	PPToken::CopyMembersTo(obj);


	PPTArray *tar_arr =(PPTArray *)obj;
	tar_arr->_array.clear();
	int i, icnt = size();
	for(i=0;i<icnt;i++) {
		PPToken *org_token = objectAtIndex(i);
		PPToken *new_token = (PPToken *)org_token->Copy();
		tar_arr->AddToken(new_token);
	}
}

void PPTArray::SetParser(PPParser *parser)
{
	PPToken::SetParser(parser);

	int i, icnt = size();
	for(i=0;i<icnt;i++) {
		PPToken *token = objectAtIndex(i);
		if(parser != token->_parser)
			token->SetParser(parser);
	}
}


void PPTArray::MoveInto(PPDocument *doc)
{
	int i, icnt = size();
	for(i=0;i<icnt;i++) {
		PPToken *token = objectAtIndex(i);
		token->MoveInto(doc);
	}
}