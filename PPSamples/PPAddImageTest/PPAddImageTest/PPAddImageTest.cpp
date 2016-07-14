// PPAddImageTest.cpp : 기본 프로젝트 파일입니다.



#include <PDFPlus.h>
#include <iostream>

using namespace std;


int main(int argc, char *argv[])
{
	int ret = 0;
	string input_path =  "..\\..\\TestFiles\\";
	string output_path = "..\\..\\TestFiles\\Output\\";

	PDFPlusInit();
	PPDocument doc;

	PPRect page_rect(0, 0, 595, 842); // A4
	PPPage *page = doc.NewPage(page_rect);

	string image_path = input_path + "peppers.jpg";
	PPEImage *ie = new PPEImage(image_path, page->ContextRef());
	ie->SetTransform(200, 0, 0, 300, 50, 200);
	page->WriteElement(ie);
	string pdf_path = output_path + "addimage.pdf";
	doc.Save(pdf_path);

	return ret;
}
