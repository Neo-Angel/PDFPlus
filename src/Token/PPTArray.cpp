
#include "PPTArray.h"
#include "PPTNumber.h"


// PPTArray //////////////////////////////////

PPTArray::PPTArray(PPDocument *doc, vector<PPToken *> token_list) : PPToken(doc)
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

string PPTArray::XMLString(int level)
{
    string retstr;
    retstr += PPTabStr(level) + "<Array>\xa";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        retstr += token->XMLString(level+1);
    }
    retstr += PPTabStr(level) + "</Array>\xa";

    return retstr;
}


string PPTArray::PDFString()
{
    string retstr = "[";
    int i, icnt = (int)_array.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _array.at(i);
        if (i > 0) {
            retstr += " ";
        }
        retstr += token->PDFString();
    }
    retstr += "]";
    return retstr;
}


void PPTArray::AddToken(int num) 
{
	AddToken((PPToken *)new PPTNumber(_document, num));
}


void PPTArray::CopyMembersTo(PPBase *obj) 
{
	PPToken::CopyMembersTo(obj);

	PPTArray *tar_arr =(PPTArray *)obj;
	tar_arr->_array.clear();
	int i, icnt = Size();
	for(i=0;i<icnt;i++) {
		PPToken *org_token = ObjectAtIndex(i);
		PPToken *new_token = (PPToken *)org_token->Copy();
		tar_arr->AddToken(new_token);
	}
}

void PPTArray::SetDocument(PPDocument *doc)
{
	PPToken::SetDocument(doc);

	int i, icnt = Size();
	for(i=0;i<icnt;i++) {
		PPToken *token = ObjectAtIndex(i);
		if(doc != token->_document)
			token->SetDocument(doc);
	}
}


void PPTArray::MoveInto(PPDocument *doc)
{
	int i, icnt = Size();
	for(i=0;i<icnt;i++) {
		PPToken *token = ObjectAtIndex(i);
		token->MoveInto(doc);
	}
}

void PPTArray::RemoveTokenAtIndex(int idx)
{
	_array.erase(_array.begin() + idx);
}

void PPTArray::Reorder(int from_idx, int to_idx)
{
	PPToken *token = _array[from_idx];
	RemoveTokenAtIndex(from_idx);
	_array.insert(_array.begin()+to_idx, token);
}
