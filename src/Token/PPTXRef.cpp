
#include <sstream>
#include "PPTXRef.h"
#include "PPParser.h"



//  ////////////////////////////////// PPTXRef
#include "PPTIndirectObj.h"
PPTXRef::PPTXRef(PPParser *parser):PPToken(parser)
{
    
}

PPTXRef::~PPTXRef()
{
    map <unsigned int, XRefItem *>::iterator it_dict;
    for(it_dict = _xreflist.begin(); it_dict != _xreflist.end(); it_dict++) {
        XRefItem *item = it_dict->second;
        delete item;
    }
    _xreflist.clear();
}

void PPTXRef::addXRef(unsigned int objnum, unsigned long long offset, int gennum, char type)
{
    XRefItem *item = new XRefItem;
    item->offset = offset;
    item->generation = gennum;
    item->type = type;
    _xreflist[objnum] = item;
}

string PPTXRef::xmlString(int level)
{
    string retstr;
    ostringstream ostr;
    ostr <<tapStr(level)<< "<CrossRef Pos='" << _filepos << "'>\xa";
    map <unsigned int, XRefItem *> ::iterator it_xref_objs;
    for(it_xref_objs = _xreflist.begin(); it_xref_objs != _xreflist.end(); it_xref_objs++) {
        unsigned int objnum = it_xref_objs->first;
        ostr <<tapStr(level+1)<<"<XRef ObjID='"<<objnum<<"'>\xa";
        
        XRefItem *item = it_xref_objs->second;
        ostr <<tapStr(level+2)<<"<Offset>"<<item->offset<<"</Offset>\xa";
        ostr <<tapStr(level+2)<<"<Generation>"<<item->generation<<"</Generation>\xa";
        ostr <<tapStr(level+2)<<"<Type>"<<item->type<<"</Type>\xa";
        
        ostr <<tapStr(level+1)<<"</XRef>\xa";
    }
    ostr <<tapStr(level)<< "</CrossRef>\xa";
    retstr = ostr.str();
    return retstr;
}

string PPTXRef::pdfString()
{
    string retstr;
    ostringstream ostr;
    ostr << "xref" << PP_ENDL;
    ostr << "0 " << _parser->_last_obj_idx << PP_ENDL;
    int i, icnt = _parser->_last_obj_idx;
    for (i=0; i<icnt; i++) {
        PPTIndirectObj *obj = _parser->_objDict[i];
        char buf[20];
        if (i == 0) {
            sprintf(buf, "%010d %05d %c",0, 65535, 'f' );
        }
        else if (obj == NULL) {
            cout << "Unexpected Error : IndirectObj not exists at " << i << PP_ENDL;
            sprintf(buf, "%010d %05d %c",0, 65535, 'f' );
        }
        else {
            sprintf(buf, "%010d %05d %c",obj->_objNum, obj->_genNum, 'n' );
        }
        ostr << buf << PP_ENDL;
    }
    retstr = ostr.str();
    return retstr;
}

void PPTXRef::write(std::ostream &os)
{
    PPToken::write(os);
    _parser->_filePtDict[_filepos] = this;
}

size_t PPTXRef::numberOfItems()
{
    return _xreflist.size();
}

void PPTXRef::merge(PPTXRef *trailer)
{
    
}
