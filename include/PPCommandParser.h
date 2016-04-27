//
//  PPCommandParser.h
//  PDFPlusLib
//
//  Created by Neo Angel on 2014. 11. 11..
//  Copyright (c) 2014년 #CORP#. All rights reserved.
//

#ifndef __PDFPlusLib__PPCommandParser__
#define __PDFPlusLib__PPCommandParser__

#include <stdio.h>
#include "PPParser.h"
#include "PPToken.h"

// 그래픽 명령어들이 알파벳 한,두 글자로 되어 있어 그 의미를 쉽게 알 수 없다.
// PPCommandCode는 알기 쉽지않은 그래픽 명령어와 1:1 매핑되어 있어 코딩을 쉽게한다.
////////////////////////////////////////////////////////////////////////////////////
enum PPCommandCode {
    PPC_LineWidth, PPC_LineCap,PPC_LineJoin, PPC_MiterLimit, PPC_Dash, PPC_Intent, PPC_Flatness, PPC_DictName,
    PPC_SaveGState, PPC_RestoreGState, PPC_Matrix,
    PPC_MoveTo, PPC_LineTo, PPC_CurveTo, PPC_CurveTo1, PPC_CurveTo2, PPC_ClosePath, PPC_Rectangle,
    PPC_Stroke, PPC_CloseStroke, PPC_Fill, PPC_Fill2, PPC_EOFill, PPC_FillStroke, PPC_EOFillStroke,
    PPC_CloseFillStroke, PPC_CloseEOFillStroke, PPC_NoPaint,
    PPC_Clip, PPC_EOClip,
    PPC_BeginText,PPC_EndText,
    PPC_CharSpace, PPC_WordSpace,PPC_Scale,PPC_Leading,PPC_FontAndSize,PPC_Render,PPC_Rise,
    PPC_NextLine,PPC_NextLine2,PPC_TextMatrix,PPC_NextLine3,
    PPC_ShowText,PPC_ShowMultiText,PPC_ShowTextNextLine, PPC_ShowTextNextLine2,
    PPC_GlyphWidth,PPC_GlyphWidthAndBox,
    PPC_StrokeColorSpace, PPC_NonStrokeColorSpace,PPC_SetColor, PPC_SetColorN, PPC_SetNonStrokeColor, PPC_SetNonStrokeColorN,
    PPC_DeviceGray, PPC_NonStrokeDeviceGray, PPC_DeviceRGB, PPC_NonStrokeDeviceRGB, PPC_DeviceCMYK, PPC_NonStrokeDeviceCMYK,
    PPC_Shade,
    PPC_BeginInlineImage,PPC_InlineImage,PPC_EndInlineImage,
    PPC_XObject,
    PPC_MarkedContent,PPC_MarkedContentP,PPC_BeginMarkedContent, PPC_BeginMarkedContentP, PPC_EndMarkedContent,
    PPC_BeginCompatibility, PPC_EndCompatibility
};

// 그래픽 명령들을 특성별로 그룹짓고 Element 객체로 만들어 다루기 쉽게한다.
////////////////////////////////////////////////////////////////////////////////////
enum PPCommandGroup {
    PPCG_GState,
    PPCG_SaveGState,
    PPCG_RestoreGState,
    PPCG_DrawPath,
    PPCG_FinishPath,
    PPCG_Clipping,
    PPCG_Shading,
    PPCG_BeginText,
    PPCG_EndText,
    PPCG_Text,
	PPCG_TextState,
    PPCG_BeginInlineImage,
    PPCG_InlineImage,
    PPCG_EndInlineImage,
    PPCG_XObject,
    PPCG_BeginMarkedContent,
    PPCG_MarkedContent,
    PPCG_EndMarkedContent,
    PPCG_BeginCompatibility,
    PPCG_EndCompatibility
};

struct PPCommandInfo {
    const char *command;	// 실질적으로 PDF에 쓰여지는 그래픽 명령어 문자열		
    PPCommandCode code;		// command 문자열과 1:1로 대응하는 code
    PPCommandGroup group;	// command가 속해있는 명령어 그룹
    char numOfOperands;		// 필요한 오퍼랜드 수 
    const char *name;		// 이름 문자열
    
};

class PPDash;
class PPContext;
class PPPath;
class PPEPath;

// 한 개의 단위 명령에 대한 정보를 담고필요한 기능들을 제공한다.
////////////////////////////////////////////////////////////////////////////
class PPTCommand : public PPToken {
public:
    vector<PPToken *> _operands;
    PPCommandInfo *_cmdInfo;
    
public:
    ~PPTCommand();
    float FloatValueAt(int idx);
    int IntValueAt(int idx);
    void GetDash(PPDash *dash);
    string StringValueAt(int idx);
	string *StringPtAt(int idx);
    PPToken *TokenValueAt(int idx);

    string PDFString();
	PPBase *Create() {return new PPTCommand();}
	void CopyMembersTo(PPBase *obj) ;

    string XMLString(int level);
};

static const char  *PPTN_COMMAND_PARSER = "PPCommandParser";

// 페이지나 폼내 스트림으로 저장되어있는 그래픽 명령어들을 파싱한다.
// PPParser에게 스트림을 공급하고 기본적인 파싱의 범주에 벗어나는 문자열들을
// 파싱해 준다.
////////////////////////////////////////////////////////////////////////////
class PPCommandParser : public PPParserSource 
{
public:
	// 포인터 형임에 주의. 외부에서 지정해 주면 파싱하면서 채워짐.
    vector <PPToken *> *_tokens;

	// 파서에 의해 파싱되는 파리미터 값들을 임시로 저장함.
    vector <PPToken *> _operands;
    char *_streamData;
    unsigned long _index;
    unsigned long _streamSize;
    

public:
    // overriding methods of PPParserSource
    bool canParseString(string str);
    PPToken *parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void get(char &ch);
    bool eof();
    size_t tellg();
    void seekg(size_t pos);
    void read(char *buf, size_t size);
    void getline(char *buf, size_t size);
    const char *ClassType();

public:
    PPCommandParser(vector <PPToken *> *tokens);
    bool ParseStream(PPTStream &stream);

};

#endif /* defined(__PDFPlusLib__PPCommandParser__) */
