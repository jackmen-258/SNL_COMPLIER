#include "recursive.h"

int main()
{
	GetTokenList();
	PrintTokenList();
	
	Recursive rec;
	rec.ParseScanner();
	
	rec.TreeOut("C:\\Users\\HP\\Desktop\\design\\ComplierDesign\\SNL_COMPLIER\\output\\parse.txt");
	//rec.ErrorOut("C:\\Users\\HP\\Desktop\\design\\ComplierDesign\\SNL_COMPLIER\\output\\parseError.txt");
	
	
}
