//
//  PPCommandParser.cpp
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 11. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#include "PPBase.h"
#include "PPCommandParser.h"
#include "PPTStream.h"
#include "PPElement.h"
#include "PPContext.h"
#include "PPPath.h"
#include "PPTNumber.h"
#include "PPTString.h"
#include "PPTName.h"

#define PP_NUM_OF_OPS           73

// General graphics state        : w,J,j,M,d,ri, i,gs
// Special graphics state        : q,Q,cm
// Path construction             : m,l,c,v,y,h,re
// Path painting                 : S,s,f,F,f*,B,B*,b,b*,n
// Clipping paths                : W,W*
// Text objects                  : BT,ET
// Text state                    : Tc, Tw, Tz, TL, Tf, Tr, Ts
// Text positioning              : Td, TD, Tm, T*
// Text showing                  : Tj,TJ,',"
// Type 3 fonts                  : d0, d1
// Color                         : CS,cs,SC,SCN,sc,scn,G,g,RG,rg,K,k
// Shading patterns              : sh
// Inline images                 : BI, ID, EI
// XObjects                      : Do
// Marked content                : MP, DP, BMC, BDC, EMC
// Compatibility                 : BX, EX



//enum PPCommandGroup {
//      PPCG_GState,
//    PPCG_SaveGState,
//    PPCG_RestoreGState,
//    PPCG_DrawPath,
//    PPCG_FinishPath,
//    PPCG_Clipping,
//    PPCG_Shading,
//    PPCG_BeginText,
//    PPCG_EndText,
//    PPCG_Text,
//};

PPCommandInfo PPCommandList[PP_NUM_OF_OPS] = {
    {"w",PPC_LineWidth, PPCG_GState,1,"LineWidth"},
    {"J",PPC_LineCap, PPCG_GState,1,"LineCap"},
    {"j",PPC_LineJoin, PPCG_GState,1,"LineJoin"},
    {"M",PPC_MiterLimit, PPCG_GState,1,"MiterLimit"},
    {"d",PPC_Dash, PPCG_GState,2,"Dash"},
    {"ri",PPC_Intent, PPCG_GState,1,"Intent"},
    {"i",PPC_Flatness, PPCG_GState,1,"Flatness"},
    {"gs",PPC_DictName, PPCG_GState,1,"DictName"},
    
    {"q",PPC_SaveGState, PPCG_SaveGState,0,"SaveGState"},
    {"Q",PPC_RestoreGState, PPCG_RestoreGState,0,"RestoreGState"},
    {"cm",PPC_Matrix, PPCG_GState,6,"Matrix"},
    
    {"m",PPC_MoveTo, PPCG_DrawPath, 2,"MoveTo"},
    {"l",PPC_LineTo, PPCG_DrawPath,2,"LineTo"},
    {"c",PPC_CurveTo, PPCG_DrawPath,6,"CurveTo"},
    {"v",PPC_CurveTo1, PPCG_DrawPath,4,"CurveTo1"},
    {"y",PPC_CurveTo2, PPCG_DrawPath,4,"CurveTo2"},
    {"h",PPC_ClosePath, PPCG_DrawPath, 0,"ClosePath"},
    {"re",PPC_Rectangle, PPCG_DrawPath,4,"Rectangle"},
    
    {"S",PPC_Stroke, PPCG_FinishPath,0,"Stroke"},
    {"s",PPC_CloseStroke, PPCG_FinishPath,0,"CloseStroke"},
    {"f",PPC_Fill, PPCG_FinishPath,0,"Fill"},
    {"F",PPC_Fill2, PPCG_FinishPath,0,"Fill2"},
    {"f*",PPC_EOFill, PPCG_FinishPath,0,"EOFill"},
    {"B",PPC_FillStroke, PPCG_FinishPath,0,"FillStroke"},
    {"B*",PPC_EOFillStroke, PPCG_FinishPath,0,"EOFillStroke"},
    {"b",PPC_CloseFillStroke, PPCG_FinishPath,0,"CloseFillStroke"},
    {"b*",PPC_CloseEOFillStroke, PPCG_FinishPath,0,"EOCloseFillStroke"},
    {"n",PPC_NoPaint, PPCG_FinishPath,0,"EndPath"},  // for clipping path, no stoking, no filling
    
    {"W",PPC_Clip, PPCG_Clipping, 0,"Clip"},
    {"W*",PPC_EOClip, PPCG_Clipping,0,"EOClip"},
    
    {"BT",PPC_BeginText, PPCG_BeginText, 0,"BeginText"},
    {"ET",PPC_EndText, PPCG_EndText, 0,"EndText"},
    
    {"Tc",PPC_CharSpace, PPCG_Text, 1,"CharSpace"},
    {"Tw",PPC_WordSpace, PPCG_Text, 1,"WordSpace"},
    {"Tz",PPC_Scale, PPCG_Text, 1,"Scale"},
    {"TL",PPC_Leading, PPCG_Text, 1,"Leading"},
    {"Tf",PPC_FontSize, PPCG_Text, 2,"FontSize"},
    {"Tr",PPC_Render, PPCG_Text, 1,"Render"},
    {"Ts",PPC_Rise, PPCG_Text, 1,"Rise"},
    
    {"Td",PPC_NextLine, PPCG_Text, 2,"NextLine"},
    {"TD",PPC_NextLine2, PPCG_Text, 2,"NextLine2"},
    {"Tm",PPC_TextMatrix, PPCG_Text, 6,"TextMatrix"},
    {"T*",PPC_NextLine3, PPCG_Text, 0,"extLine3"},
    
    {"Tj",PPC_ShowText, PPCG_Text, 1,"ShowText"},
    {"TJ",PPC_ShowMultiText, PPCG_Text, 1,"ShowMultiText"},
    {"'",PPC_ShowTextNextLine, PPCG_Text, 3,"ShowTextNextLine"},
    {"\"",PPC_ShowTextNextLine2, PPCG_Text, 1,"ShowTextNextLine2"},
    
    {"d0",PPC_GlyphWidth, PPCG_Text, 2,"GlyphWidth"},
    {"d1",PPC_GlyphWidthAndBox, PPCG_Text, 6,"GlyphWidthAndBox"},
    
    {"CS",PPC_StrokeColorSpace, PPCG_GState, 1,"StrokeColorSpace"},
    {"cs",PPC_NonStrokeColorSpace, PPCG_GState, 1,"NonStrokeColorSpace"},
    
    {"SC",PPC_SetColor, PPCG_GState, -1,"SetColor"},
    {"SCN",PPC_SetColorN, PPCG_GState, -1,"SetColorN"},  // PPC_SetColor + Pattern, Separation, DeviceN, ICCBased
    {"sc",PPC_SetNonStrokeColor, PPCG_GState, -1,"NonStrokeSetColor"},
    {"scn",PPC_SetNonStrokeColorN, PPCG_GState, -1,"NonStrokeSetColorN"},
    
    {"G",PPC_DeviceGray, PPCG_GState, 1,"DeviceGray"},
    {"g",PPC_NonStrokeDeviceGray, PPCG_GState, 1,"NonStrokeDeviceGray"},
    {"RG",PPC_DeviceRGB, PPCG_GState, 3,"DeviceRGB"},
    {"rg",PPC_NonStrokeDeviceRGB, PPCG_GState, 3,"NonStrokeDeviceRGB"},
    {"K",PPC_DeviceCMYK, PPCG_GState, 4,"DeviceCMYK"},
    {"k",PPC_NonStrokeDeviceCMYK, PPCG_GState, 4,"NonStrokeDeviceCMYK"},
    
    {"sh",PPC_Shade, PPCG_Shading, 1,"Shade"},
    
    {"BI",PPC_BeginInlineImage, PPCG_BeginInlineImage, 1,"BeginInlineImage"},
    {"ID",PPC_InlineImage, PPCG_InlineImage, 1,"InlineImage"},
    {"EI",PPC_EndInlineImage, PPCG_EndInlineImage, 0,"EndInlineImage"},
    
    {"Do",PPC_XObject, PPCG_XObject, 1,"XObject"},
    
    {"MP",PPC_MarkedContent, PPCG_MarkedContent, 1,"MarkedContent"},
    {"DP",PPC_MarkedContentP, PPCG_MarkedContent, 2,"MarkedContentP"},
    {"BMC",PPC_BeginMarkedContent, PPCG_BeginMarkedContent, 1,"BeginMarkedContent"},
    {"BDC",PPC_BeginMarkedContentP, PPCG_BeginMarkedContent, 2,"BeginMarkedContentP"},
    {"EMC",PPC_EndMarkedContent, PPCG_EndMarkedContent, 0,"EndMarkedContent"},
    
    {"BX",PPC_BeginCompatibility, PPCG_BeginCompatibility, 0,"BeginCompatibility"},
    {"EX",PPC_EndCompatibility, PPCG_EndCompatibility, 0,"EndCompatibility"}
};

map <string, PPCommandInfo *> CommandDict;

string tapStr(int cnt);



void PDFPlusInit()
{
    int i;
    for (i=0; i<PP_NUM_OF_OPS; i++) {
        PPCommandInfo *cmdinfo = &PPCommandList[i];
        CommandDict[cmdinfo->code] = cmdinfo;
    }
}


PPDash::~PPDash()
{
//    size_t i, icnt = _array.size();
//    for (i=0; i<icnt; i++) {
//        float *value = _array[i];
//        delete value;
//    }
}


PPTCommand::~PPTCommand()
{
    int i, icnt = (int)_operands.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _operands.at(i);
        delete token;
    }
    _operands.clear();
}
string PPTCommand::xmlString(int level)
{
    string retstr;
//    PPCommandInfo &cmdinfo = PPCommandList[_type];
    if (_operands.size() == 0) {
        retstr += tapStr(level) + "<"+_cmdInfo->name+"/>\xa";
    }
    else {
        retstr += tapStr(level) + "<"+_cmdInfo->name+">\xa";
        int i, icnt = (int)_operands.size();
        for (i=0; i<icnt; i++) {
            PPToken *token = _operands.at(i);
            retstr += token->xmlString(level+1);
        }
        retstr += tapStr(level) + "</"+_cmdInfo->name+">\xa";
    }
    
    return retstr;
}

float PPTCommand::getFloatValue(int idx)
{
    PPToken *token = _operands[idx];
    if (token->classType() != PPTN_NUMBER) {
        cout << "Error : Parameter Type Error.\xa";
        return 0;
    }
    float retvalue = ((PPTNumber *)token)->floatValue();
    
    return retvalue;
}

int PPTCommand::getIntValue(int idx)
{
    PPToken *token = _operands[idx];
    if (token->classType() != PPTN_NUMBER) {
        cout << "Error : Parameter Type Error.\xa";
        return 0;
    }
    int retvalue = ((PPTNumber *)token)->intValue();
    
    return retvalue;
}

string PPTCommand::getStringValue(int idx)
{
    string *ret = NULL;
    PPToken *token = _operands[idx];
    if (token->classType() == PPTN_NAME) {
        ret = ((PPTName *)token)->_name;
        return *ret;
    }
    else if (token->classType() == PPTN_STRING) {
        ret = ((PPTString *)token)->_string;
        return *ret;
    }
    return *ret;
}

string *PPTCommand::getStringPt(int idx)
{
    string *ret = NULL;
    PPToken *token = _operands[idx];
    if (token->classType() == PPTN_NAME) {
        ret = ((PPTName *)token)->_name;
        return ret;
    }
    else if (token->classType() == PPTN_STRING) {
        ret = ((PPTString *)token)->_string;
        return ret;
    }
    return ret;
}

PPToken *PPTCommand::getTokenValue(int idx)
{
    PPToken *token = _operands[idx];
    return token;
}

//void PPTCommand::getDash(PPDash *dash)
//{
//    dash->_array.clear();
//    PPTArray *array = (PPTArray *)_operands[0];
//    size_t i, icnt = array->size();
//    for (i=0; i<icnt; i++) {
//        PPTNumber *num = (PPTNumber *)array->_array[i];
//        float *float_value = new float(num->floatValue());
//        dash->_array.push_back(float_value);
//        
//    }
//    PPTNumber *phase = (PPTNumber *)_operands[1];
//    dash->_phase = phase->intValue();
//}

void PPTCommand::getDash(PPDash *dash)
{
    dash->_array = (PPTArray *)_operands[0];
    dash->_phase = (PPTNumber *)_operands[1];
}

void PPTCommand::setValueToGState(PPContext &gcontext)
{
    
    switch (_cmdInfo->type) {
        case PPC_LineWidth:
            gcontext.setLineWidth(getFloatValue(0));
            break;
        
        case PPC_LineCap:
            gcontext.setLineCap(getIntValue(0));
            break;
        
        case PPC_LineJoin:
            gcontext.setLineJoin(getIntValue(0));
            break;
        
        case PPC_MiterLimit:
            gcontext.setMiterLimit(getFloatValue(0));
            break;
        
        case PPC_Dash:
        {
            PPDash dash;
            getDash(&dash);
            gcontext.setDash(dash);
            break;
        }
        
        case PPC_Intent:
            gcontext.setIntent(getStringValue(0));
            break;
        
        case PPC_Flatness:
            gcontext.setFlatness(getFloatValue(0));
            break;
        
        case PPC_DictName:
            gcontext.setDictName(getStringValue(0));
            break;
        
        case PPC_Matrix:
        {
            PPMatrix mtx;
            mtx._a = getFloatValue(0);
            mtx._b = getFloatValue(1);
            mtx._c = getFloatValue(2);
            mtx._d = getFloatValue(3);
            mtx._x = getFloatValue(4);
            mtx._y = getFloatValue(5);
            gcontext.setMatrix(mtx);
            break;
        }
        
        case PPC_StrokeColorSpace:
            gcontext.setStrokeColorSpace(getStringValue(0));
            break;
        
        case PPC_NonStrokeColorSpace:
            gcontext.setFillColorSpace(getStringValue(0));
            break;
        
        case PPC_SetColor:
        case PPC_SetColorN:
       {
            int n = gcontext.numberOfStrokeColorCoponents();
            if (n == 1) {
                gcontext.setStrokeColor(getFloatValue(0));
            }
            else if(n == 3) {
                gcontext.setStrokeColor(getFloatValue(0),getFloatValue(1),getFloatValue(2));
            }
            else if(n == 4) {
                gcontext.setStrokeColor(getFloatValue(0),getFloatValue(1),getFloatValue(2), getFloatValue(3));
            }
            break;
        }
        case PPC_SetNonStrokeColor:
        case PPC_SetNonStrokeColorN:
        {
            int n = gcontext.numberOfNonStrokeColorCoponents();
            if (n == 1) {
                gcontext.setFillColor(getFloatValue(0));
            }
            else if(n == 3) {
                gcontext.setFillColor(getFloatValue(0),getFloatValue(1),getFloatValue(2));
            }
            else if(n == 4) {
                gcontext.setFillColor(getFloatValue(0),getFloatValue(1),getFloatValue(2), getFloatValue(3));
            }
            break;
        }
        
        case PPC_DeviceGray:
            gcontext.setStrokeColorSpace(PPCSN_DeviceGray);
            gcontext.setStrokeColor(getFloatValue(0));
            break;
        
        case PPC_DeviceRGB:
            gcontext.setStrokeColorSpace(PPCSN_DeviceRGB);
            gcontext.setStrokeColor(getFloatValue(0),getFloatValue(1),getFloatValue(2));
            break;
        
        case PPC_DeviceCMYK:
            gcontext.setStrokeColorSpace(PPCSN_DeviceRGB);
            gcontext.setStrokeColor(getFloatValue(0),getFloatValue(1),getFloatValue(2), getFloatValue(3));
            break;
            
        case PPC_NonStrokeDeviceGray:
            gcontext.setFillColorSpace(PPCSN_DeviceGray);
            gcontext.setFillColor(getFloatValue(0));
            break;
        
        case PPC_NonStrokeDeviceRGB:
            gcontext.setFillColorSpace(PPCSN_DeviceRGB);
            gcontext.setFillColor(getFloatValue(0),getFloatValue(1),getFloatValue(2));
            break;
        
        case PPC_NonStrokeDeviceCMYK:
            gcontext.setFillColorSpace(PPCSN_DeviceCMYK);
            gcontext.setFillColor(getFloatValue(0),getFloatValue(1),getFloatValue(2), getFloatValue(3));
            break;
        
        default:
            break;
    }
}

void PPTCommand::addCommandToPath(PPPath *path)
{
    switch (_cmdInfo->type) {
        case PPC_MoveTo:
            path->moveTo(getFloatValue(0), getFloatValue(1));
            break;
        case PPC_LineTo:
            path->lineTo(getFloatValue(0), getFloatValue(1));
            break;
        case PPC_CurveTo:
            path->curveTo(getFloatValue(0), getFloatValue(1), getFloatValue(2), getFloatValue(3), getFloatValue(4), getFloatValue(5));
            break;
        case PPC_CurveTo1:
            path->curveTo1(getFloatValue(0), getFloatValue(1), getFloatValue(2), getFloatValue(3));
            break;
        case PPC_CurveTo2:
            path->curveTo2(getFloatValue(0), getFloatValue(1), getFloatValue(2), getFloatValue(3));
            break;
        case PPC_ClosePath:
            path->close();
            break;
        case PPC_Rectangle:
            path->rectangle(getFloatValue(0), getFloatValue(1), getFloatValue(2), getFloatValue(3));
            break;
        default:
            break;
    }
}

string PPTCommand::pdfString()
{
    string retstr;
    size_t i, icnt = _operands.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _operands.at(i);
        retstr += token->pdfString();
        retstr += " ";
    }
    retstr += _cmdInfo->code;
    return retstr;
}


//  PPCommandParser
//
//
////////////////////////////////////////////////////////////////

PPCommandParser::PPCommandParser(vector <PPToken *> *tokens)
{
    _tokens = tokens;
}

bool PPCommandParser::parseStream(PPTStream &stream)
{
    PPParser parser;
    _index = 0;
    _streamSize = stream._streamSize;
    _streamData = stream._streamData;
    return parser.parseSource(*this, _operands);
}

const char *PPCommandParser::classType()
{
    return PPTN_GRAPHIC_PARSER;
}


// Overriding Part
////////////////////////////////////////////////
bool PPCommandParser::canParseString(string str)
{
    PPCommandInfo *cinfo = CommandDict[str];
    if (cinfo) {
        return true;
    }
    else {
        cout << "Not Command : " << str << PP_ENDL;
    }
    return false;
}

PPToken *PPCommandParser::parseString(string str, vector <PPToken *> &tokens, PPParser *parser)
{
    PPCommandInfo *cinfo = CommandDict[str];
    if (cinfo) {
        if (cinfo->type == PPC_BeginInlineImage) {
            // sorry. I will implement later.
            cout << "Inline Image not yet implemented." << PP_ENDL;
            bool flag = false;
            string *dictstr = new string;
            while (!eof()) {
                char buf[1024];
                getline(buf, 1024);
                size_t slen = strlen(buf);
                if (slen == 2 && buf[0] == 'I' && buf[1] == 'D') {
                    flag = true;
                    break;
                }
                *dictstr += buf;
            }
            if (!flag) {
                return NULL;
            }
            flag = false;
            string *datastr = new string;
            char prevch[2] = {'\0', '\0'};
            while (!eof()) {
                char ch;
                get(ch);
                
                if ((prevch[0] == 0x0a || prevch[0] == 0x0d) && prevch[1] == 'I' && ch == 'D') {
                    get(ch);
                    if (ch == 0x0a || ch == 0x0d) {
                        // OK
                        flag = true;
                        break;
                    }
                }
                *datastr += ch;
                prevch[0] = prevch[1];
                prevch[1] = ch;
            }
            if (!flag) {
                return NULL;
            }
            
            PPTCommand *cmd = new PPTCommand;
            cmd->_cmdInfo = &PPCommandList[PPC_EndInlineImage];
            _tokens->push_back(cmd);
            return cmd;

        }
        else {
            PPTCommand *cmd = new PPTCommand;
            cmd->_cmdInfo = cinfo;
            int i, icnt = cinfo->numOfOperands;
			if(icnt == -1) { // -1 은 오퍼랜드 갯수가 정해지지 않았음을 의미.
				icnt = tokens.size();
			}
			else if (icnt != tokens.size()) {
                cout << "Error : Graphic Parsing Error (" << str << ")\xa";
                return NULL;
            }
            for (i=0; i<icnt; i++) {
                PPToken *token = tokens.at(i);
                cmd->_operands.push_back(token);
            }
            tokens.clear();
            _tokens->push_back(cmd);
            return cmd;
        }
        
    }
    return NULL;
}

void PPCommandParser::get(char &ch)
{
    if (_index < _streamSize) {
        ch = _streamData[_index++];
    }
    else {
        ch = 0;
        _index = _streamSize +1;
    }
}

bool PPCommandParser::eof()
{
    if (_index > _streamSize) {
        return true;
    }
    return false;
}

size_t PPCommandParser::tellg()
{
    return _index;
}

void PPCommandParser::seekg(size_t pos)
{
    _index = pos;
}

void PPCommandParser::read(char *buf,size_t size)
{
    memcpy(buf, _streamData+_index, size);
    _index += size;
}

void PPCommandParser::getline(char *buf, size_t size)
{
    size_t bufidx = 0;
    char ch;
    get(ch);
    while ((ch == 0x0d || ch == 0x0a) && !eof()) {
        get(ch);
    }
    while (ch != 0x0d && ch != 0x0a && !eof()) {
        buf[bufidx++] = ch;
        if (bufidx == size-1) {
            buf[bufidx] = 0;
            return;
        }
        get(ch);
    }
    buf[bufidx] = 0;
    while ((ch == 0x0d || ch == 0x0a)  && !eof()){
        get(ch);
    }
    _index --;
}
