// PPContentReplacerTest.cpp : 기본 프로젝트 파일입니다.



#include <PDFPlus.h>
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{
	int ret = 0;
	string input_path =  "..\\..\\TestFiles\\";
	string output_path = "..\\..\\TestFiles\\Output\\";

	PDFPlusInit();
	PPDocument doc(input_path + "BusinessCardTemplate.pdf");
//	PPDocument doc(input_path + "BusinessCard.pdf");
	
	PPPage *page = doc.GetPage(1);

	//page->AddImage(page->MediaBox(), input_path + "peppers.jpg");

	page->ReplaceString("NAME", "John Smith");
	page->ReplaceString("QUALIFICATIONS", "Philosophy Doctor"); 
	page->ReplaceString("JOB_TITLE", "Software Developer"); 
	page->ReplaceString("ADDRESS_LINE1", "#100 123 Software Rd"); 
	page->ReplaceString("ADDRESS_LINE2", "Vancouver, BC"); 
	page->ReplaceString("PHONE_OFFICE", "604-730-8989"); 
	page->ReplaceString("PHONE_MOBILE", "604-765-4321"); 
	page->ReplaceString("EMAIL", "info@pdftron.com"); 
	page->ReplaceString("WEBSITE_URL", "http://www.pdftron.com"); 

	doc.Save(output_path + "BusinessCard.pdf");
	return ret;
}
