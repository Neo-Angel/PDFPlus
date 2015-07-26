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
//#include "PPElement.h"

enum PPCommandType {
    PPC_LineWidth, PPC_LineCap,PPC_LineJoin, PPC_MiterLimit, PPC_Dash, PPC_Intent, PPC_Flatness, PPC_DictName,
    PPC_SaveGState, PPC_RestoreGState, PPC_Matrix,
    PPC_MoveTo, PPC_LineTo, PPC_CurveTo, PPC_CurveTo1, PPC_CurveTo2, PPC_ClosePath, PPC_Rectangle,
    PPC_Stroke, PPC_CloseStroke, PPC_Fill, PPC_Fill2, PPC_EOFill, PPC_FillStroke, PPC_EOFillStroke,
    PPC_CloseFillStroke, PPC_CloseEOFillStroke, PPC_NoPaint,
    PPC_Clip, PPC_EOClip,
    PPC_BeginText,PPC_EndText,
    PPC_CharSpace, PPC_WordSpace,PPC_Scale,PPC_Leading,PPC_FontSize,PPC_Render,PPC_Rise,
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
    const char *code;
    PPCommandType type;
    PPCommandGroup group;
    char numOfOperands;
    const char *name;
    
};


class PPDash;
class PPContext;
class PPPath;
class PPEPath;

class PPTCommand : public PPToken {
public:
    vector<PPToken *> _operands;
    PPCommandInfo *_cmdInfo;
    
    ~PPTCommand();
    string xmlString(int level);
    float getFloatValue(int idx);
    int getIntValue(int idx);
    void getDash(PPDash *dash);
    string getStringValue(int idx);
	string *getStringPt(int idx);
    PPToken *getTokenValue(int idx);
//    void setValueToGState(PPContext &gcontext);
//    void addCommandToPath(PPPath *path);
//    void addCommandToPathElement(PPEPath *pathElement);
    string pdfString();

	PPBase *Create() {return new PPTCommand();}
	void CopyMembersTo(PPBase *obj) ;
};

static const char  *PPTN_GRAPHIC_PARSER = "PPCommandParser";

class PPCommandParser : public PPParserSource
{
public:
    vector <PPToken *> *_tokens;
    vector <PPToken *> _operands;
    char *_streamData;
    unsigned long _index;
    unsigned long _streamSize;
    
    PPCommandParser(vector <PPToken *> *tokens);
//    void setTokenList(vector <PPToken *> &tokens);
    bool parseStream(PPTStream &stream);
    // overriding methods of PPParserSource
    bool canParseString(string str);
    PPToken *parseString(string str, vector <PPToken *> &tokens, PPParser *parser);
    void get(char &ch);
    bool eof();
    size_t tellg();
    void seekg(size_t pos);
    void read(char *buf, size_t size);
    void getline(char *buf, size_t size);
    const char *classType();

};

#endif /* defined(__PDFPlusLib__PPCommandParser__) */
