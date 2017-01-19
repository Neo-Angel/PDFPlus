

#include <PDFPlus.h>
#include <iostream>

int main(int argc, char* argv[])
{
	int ret = 0;
	string input_path =  "C:\\Work\\PDFPlus\\Lib\\PPSamples\\TestFiles\\"; //"..\\..\\..\\TestFiles\\";
	string output_path = "C:\\Work\\PDFPlus\\Lib\\PPSamples\\TestFiles\\Output\\";

	PDFPlusInit();
	PPDocument doc;

	string src_pdf_path1 = input_path + "test_0.pdf";
	string src_pdf_path2 = input_path + "test_2.pdf";
	
	PPDocument src_pdf1(src_pdf_path1);

	int i, icnt = (int)src_pdf1.NumberOfPages();
	for(i=0;i<icnt;i++) {
		PPPage *page = src_pdf1.PageAtIndex(i);
		doc.AddPage(page);
	}

	PPDocument src_pdf2(src_pdf_path2);

	icnt = (int)src_pdf2.NumberOfPages();
	for(i=0;i<icnt;i++) {
		PPPage *page = src_pdf2.PageAtIndex(i);
		doc.AddPage(page);
	}
	
/*
	PPRect page_rect(0, 0, 595, 842); // A4
	PPPage *page = doc.NewPage(page_rect);
	*/

	string pdf_path = output_path + "new_page.pdf";
	doc.Save(pdf_path);
	return 0;
}
