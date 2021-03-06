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

/* PDF Specification에 정의되어 있는 그래픽 명령어들 */

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


// 모든 그래픽 명령어들을 사용하기 좋게 정리해 놓는다.
//////////////////////////////////////////////////////////
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
    
    {"Tc",PPC_CharSpace, PPCG_TextState, 1,"CharSpace"},
    {"Tw",PPC_WordSpace, PPCG_TextState, 1,"WordSpace"},
    {"Tz",PPC_Scale, PPCG_TextState, 1,"Scale"},
    {"TL",PPC_Leading, PPCG_TextState, 1,"Leading"},
    {"Tf",PPC_FontAndSize, PPCG_TextState, 2,"FontAndSize"},
    {"Tr",PPC_Render, PPCG_TextState, 1,"Render"},
    {"Ts",PPC_Rise, PPCG_TextState, 1,"Rise"},
    
    {"Td",PPC_NextLine, PPCG_Text, 2,"NextLine"},
    {"TD",PPC_NextLine2, PPCG_Text, 2,"NextLine2"},
    {"Tm",PPC_TextMatrix, PPCG_TextState, 6,"TextMatrix"},
    {"T*",PPC_NextLine3, PPCG_Text, 0,"NextLine3"},
    
    {"Tj",PPC_ShowText, PPCG_Text, 1,"ShowText"},
    {"TJ",PPC_ShowMultiText, PPCG_Text, 1,"ShowMultiText"},
    {"'",PPC_ShowTextNextLine, PPCG_Text, 3,"ShowTextNextLine"},
    {"\"",PPC_ShowTextNextLine2, PPCG_Text, 1,"ShowTextNextLine2"},
    
    {"d0",PPC_GlyphWidth, PPCG_TextState, 2,"GlyphWidth"},
    {"d1",PPC_GlyphWidthAndBox, PPCG_TextState, 6,"GlyphWidthAndBox"},
    
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

// 위에 정리된 명령어 리스트들이 들어갈 해쉬 사전
map <string, PPCommandInfo *> CommandDict;

// cnt만큼의 탭문자들을 리턴한다.
string PPTabStr(int cnt);


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// PDF Plus Initialize
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// PDF Plus 라이브러리를 사용하기 위해선 프로그램이 시작할 때
// 반드시 호출해 줘야하는 초기화 함수.

bool _PP_Initialized = false;
void PDFPlusInit()
{
	// CommandDict에 PPCommandList의 모든 내용들을 담는다.
    int i;
    for (i=0; i<PP_NUM_OF_OPS; i++) {
        PPCommandInfo *cmdinfo = &PPCommandList[i];
        CommandDict[cmdinfo->command] = cmdinfo; // PPCommandInfo->command를 키로 한다.
    }
	_PP_Initialized = true;
}


PPDash::~PPDash()
{
}

////////////////////////////////////////////////////////////////
//  PPCommandParser
////////////////////////////////////////////////////////////////
PPTCommand::~PPTCommand()
{
    int i, icnt = (int)_operands.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _operands.at(i);
        delete token;
    }
    _operands.clear();
}

// 선택함수
string PPTCommand::XMLString(int level)
{
    string retstr;
//    PPCommandInfo &cmdinfo = PPCommandList[_type];
    if (_operands.size() == 0) {
        retstr += PPTabStr(level) + "<"+_cmdInfo->name+"/>\xa";
    }
    else {
        retstr += PPTabStr(level) + "<"+_cmdInfo->name+">\xa";
        int i, icnt = (int)_operands.size();
        for (i=0; i<icnt; i++) {
            PPToken *token = _operands.at(i);
            retstr += token->XMLString(level+1);
        }
        retstr += PPTabStr(level) + "</"+_cmdInfo->name+">\xa";
    }
    
    return retstr;
}

// idx 번째 있는 오퍼랜드(인수값)을 리턴한다.
// 타입에 따른 함수를 실행시켜준다.
float PPTCommand::FloatValueAt(int idx)
{
    PPToken *token = _operands[idx];
    if (token->ClassType() != PPTN_NUMBER) {
        cout << "Error : Parameter Type Error.\xa";
        return 0;
    }
    float retvalue = ((PPTNumber *)token)->floatValue();
    
    return retvalue;
}

int PPTCommand::IntValueAt(int idx)
{
    PPToken *token = _operands[idx];
    if (token->ClassType() != PPTN_NUMBER) {
        cout << "Error : Parameter Type Error.\xa";
        return 0;
    }
    int retvalue = ((PPTNumber *)token)->intValue();
    
    return retvalue;
}

string PPTCommand::StringValueAt(int idx)
{
    string *ret = NULL;
    PPToken *token = _operands[idx];
    if (token->ClassType() == PPTN_NAME) {
        ret = ((PPTName *)token)->_name;
        return *ret;
    }
    else if (token->ClassType() == PPTN_STRING) {
        ret = ((PPTString *)token)->_string;
        return *ret;
    }
    return *ret;
}

string *PPTCommand::StringPtAt(int idx)
{
    string *ret = NULL;
    PPToken *token = _operands[idx];
    if (token->ClassType() == PPTN_NAME) {
        ret = ((PPTName *)token)->_name;
        return ret;
    }
    else if (token->ClassType() == PPTN_STRING) {
        ret = ((PPTString *)token)->_string;
        return ret;
    }
    return ret;
}

PPToken *PPTCommand::TokenValueAt(int idx)
{
    PPToken *token = _operands[idx];
    return token;
}

void PPTCommand::GetDash(PPDash *dash)
{
    dash->_array = (PPTArray *)_operands[0];
    dash->_phase = (PPTNumber *)_operands[1];
}

// Instance Variable 들을 이용해 PDF에 들어갈 문자열을 만든다.
string PPTCommand::PDFString()
{
    string retstr;
    size_t i, icnt = _operands.size();
    for (i=0; i<icnt; i++) {
        PPToken *token = _operands.at(i);
        retstr += token->PDFString();
        retstr += " ";
    }
    retstr += _cmdInfo->command;
	retstr += "\xa";
    return retstr;
}

// 복사를 위한 함수
void PPTCommand::CopyMembersTo(PPBase *obj)
{
	PPTCommand *tar_cmd = (PPTCommand *)obj;
	size_t i, icnt = _operands.size();
	for(i=0;i<icnt;i++) {
		PPToken *token = (PPToken *)_operands[i]->Copy();
		tar_cmd->_operands.push_back(token);
	}

	tar_cmd->_cmdInfo = _cmdInfo;
}

////////////////////////////////////////////////////////////////
//  PPCommandParser
////////////////////////////////////////////////////////////////

PPCommandParser::PPCommandParser(vector <PPToken *> *tokens)
{
    _tokens = tokens;
}

bool PPCommandParser::ParseStream(PPTStream &stream)
{
    PPParser parser;
    _index = 0;
	if(stream._decoded == false) {
		stream.FlateDecodeStream();
	}
    _streamSize = stream._streamSize;
    _streamData = stream._streamData;

	// parser에 operands들을 담을 리스트를 넘겨준다.
	// parser는 주로 operand들을 파싱하고 명령어들은 
	// 이(this) 클래스(PPCommandParser)에서 한다.
    return parser.ParseSource(*this, _operands);
}

PPClassType PPCommandParser::ClassType()
{
    return PPTN_COMMAND_PARSER;
}


// Overriding Part
////////////////////////////////////////////////
bool PPCommandParser::canParseString(string str)
{
	// str이 코멘드 중에 하나면 파싱을 한다.
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
	// parser가 파싱하지 못한 문자열(str)을 파싱한다.
	// tokens에는 operands가 담겨져 있다.
    PPCommandInfo *cinfo = CommandDict[str];
    if (cinfo) {
        if (cinfo->code == PPC_BeginInlineImage) {
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
			// 코멘드 토큰을 생성해서 코멘드 정보(cinfo)를 담고
            PPTCommand *cmd = new PPTCommand;
            cmd->_cmdInfo = cinfo;
            size_t i, icnt = cinfo->numOfOperands;
			if(icnt == -1) { // -1 은 오퍼랜드 갯수가 정해지지 않았음을 의미.
				icnt = tokens.size();
			}
			else if (icnt != tokens.size()) {
                cout << "Error : Graphic Parsing Error (" << str << ")\xa";
                return NULL;
            }
			// 파싱된 오퍼랜드들을 가지고 온다.
            for (i=0; i<icnt; i++) {
                PPToken *token = tokens.at(i);
                cmd->_operands.push_back(token);
            }
            tokens.clear();
			// 코멘드 토큰을 _tokens에 저장한다.
            _tokens->push_back(cmd);
            return cmd;
        }
        
    }
    return NULL;
}

// Stream Data를 파서에 공급하기 위한 함수들
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
    _index = (ulong)pos;
}

void PPCommandParser::read(char *buf,size_t size)
{
    memcpy(buf, _streamData+_index, size);
    _index += (ulong)size;
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
