#include "parse.h"
#include<cstring> 

#define SPACESTR "                    "
#define SLEN strlen(SPACESTR)

string dash_format(string s);
string space_format(int layer);
void NodeOut(FILE* file,ParseNode* p,int layer,bool feed);//单节点输出 

class Recursive
{
	private:
		TokenNode* current;//正在处理的Token结点指针
		list<TokenNode*> ErrorList;//错误信息暂存链表
		int ErrorNum;//错误数
		list<TokenNode*>::iterator it;//迭代器
		
	public:
		ParseNode* root;//语法树根节点 
		
	private:
		//判断一个终极符是否是在某产生式的Predict集中
		bool Check(string Lex,int *Pre);
		
		//判断终极符是否匹配成功
		bool Match(ParseNode* &ptr,int Sym);
		
		//出错处理函数 ,返回值为Symbol=-1的ParseNode指针，以表示出错信息
		ParseNode* Error(string err);
		
		//获取下一个TokenNode结点
		void GetNextToken();
		
	public:
		Recursive():current(NULL),ErrorNum(0) {};
		~Recursive() {};
		
		//返回语法树根节点 
		void ParseScanner();
		
		//输出错误信息 
		void ErrorOut(const char* filename);
		
		//语法树输出 
		void TreeOut(const char* savefile);
	
	private:
		//产生式对应的处理函数
		ParseNode*	 f1(),*f2(),*f3(),*f4(),*f5(),*f6(),*f7(),*f8(),*f9(),*f10(),*f11(),
					*f12(),*f13(),*f14(),*f15(),*f16(),*f17(),*f18(),*f19(),*f20(),*f21(),
					*f22(),*f23(),*f24(),*f25(),*f26(),*f27(),*f28(),*f29(),*f30(),*f31(),
					*f32(),*f33(),*f34(),*f35(),*f36(),*f37(),*f38(),*f39(),*f40(),*f41(),
					*f42(),*f43(),*f44(),*f45(),*f46(),*f47(),*f48(),*f49(),*f50(),*f51(),
					*f52(),*f53(),*f54(),*f55(),*f56(),*f57(),*f58(),*f59(),*f60(),*f61(),
			        *f62(),*f63(),*f64(),*f65(),*f66(),*f67(),*f68(),*f69(),*f70(),*f71(),
					*f72(),*f73(),*f74(),*f75(),*f76(),*f77(),*f78(),*f79(),*f80(),*f81(),
					*f82(),*f83(),*f84(),*f85(),*f86(),*f87(),*f88(),*f89(),*f90(),*f91(),
					*f92(),*f93(),*f94(),*f95(),*f96(),*f97(),*f98(),*f99(),*f100(),*f101(),
					*f102(),*f103(),*f104();
};
  
string dash_format(string s)
{
	string str=s;
	for(int i=s.length();i<SLEN;i++)
		str+='-';
	str[SLEN]=0;
	return str;
}

string space_format(int layer)
{
	string str;
	for(int i=0;i<layer;i++)
		str+=SPACESTR;
	return str;
}

//单节点输出 
void NodeOut(FILE* file,ParseNode* p,int layer,bool feed)
{
	if(feed && p->Symbol!=-1)
		fprintf(file,"\nline %d :%s",p->LineNum,space_format(layer).c_str());
	if(p->Symbol!=-1 && p->child && p->child->Symbol!=-1)
		fprintf(file,"%s",dash_format(p->ParseStr).c_str());
	else if(p->Symbol!=-1)
		fprintf(file,"%s",p->ParseStr.c_str());
	if(p->child!=NULL)
		NodeOut(file,p->child,layer+1,0);
	if(p->brother!=NULL)
		NodeOut(file,p->brother,layer,1);
}

//语法树输出 
void Recursive::TreeOut(const char* savefile)
{
	FILE* file=fopen(savefile,"w+");
	if(file)
	{
		if(root)
			NodeOut(file,root,0,1);
		else
			cerr<<"语法树不存在"<<endl;
		fclose(file);
	}
	else
		cerr<<"文件打开失败"<<endl;
}

//输出错误信息 
void Recursive::ErrorOut(const char* filename)
{
	FILE* file=fopen(filename,"w+");
	if(file)
	{
		list<TokenNode*>::iterator it = ErrorList.begin();
		while(it != --ErrorList.end())
			fprintf(file,"%s\n",(*it++)->SemStr.c_str());
		fclose(file);
	}
	else
		cerr<<"文件打开失败"<<endl;
}

//出错处理函数 ,返回值为Symbol=-1的ParseNode指针，以表示出错信息
ParseNode* Recursive::Error(string err)
{
	string ErrorStr;
	ErrorNum++;
	ErrorStr+="Error:"+to_string(ErrorNum)+"  Line:"+to_string(current->LineNum)+"  syntax Error:"+err;
	TokenNode* ptr=GetTokenNode(0,MAP[107],ErrorStr);
	ErrorList.push_back(ptr);
	return GetParseNode(-1,current->LineNum,"");
}

//获取下一个TokenNode结点
void Recursive::GetNextToken()
{
	do {
		if(it != --TokenList.end())
			current=*(++it);
	} while(current->LexStr==MAP[106]);
}

//锟判断一个终极符是否是在某产生式的Predict集中
bool Recursive::Check(string Lex,int *Pre)
{
	for(int i=0;Pre[i]!=-1;i++)
	{
		if(Lex==MAP[Pre[i]])
			return true;
	}
	return 0;
}

//判断终极符是否匹配成功
bool Recursive::Match(ParseNode* &ptr,int Sym)
{
	if(current->LexStr==MAP[Sym])
	{
		ptr=GetParseNode(Sym,current->LineNum,MAP[Sym]);
		GetNextToken();
		return true;
	}
	else
	{
		string err="缺少："+MAP[Sym];
		ptr=Error(err);
		return false;
	}
}

//返回语法树根节点 
void Recursive::ParseScanner()
{
	it = TokenList.begin(); 
	current=*it;
	while(current->LexStr==MAP[106])
		GetNextToken();
	root=f1();
}

//Program
ParseNode* Recursive::f1()
{
	ParseNode* ptr=GetParseNode(0,current->LineNum,MAP[0]);
	if(Check(current->LexStr,Predicts[1]))
	{
		ptr->child=f2();
		ptr->child->brother=f4();
		ptr->child->brother->brother=f57();
		Match(ptr->child->brother->brother->brother,100);
	}
	else
		return Error(SyntaxError[0]);
	return ptr;
}
ParseNode* Recursive::f2()
{
	ParseNode* ptr=GetParseNode(1,current->LineNum,	MAP[1]);
	if(Check(current->LexStr,Predicts[2]))
	{
		Match(ptr->child,67);
		ptr->child->brother=f3();
	}
	else
		return Error(SyntaxError[1]);
	return ptr;
}
//ID
ParseNode* Recursive::f3()
{
	ParseNode* ptr=GetParseNode(2,current->LineNum,MAP[2]);
	if(Check(current->LexStr,Predicts[3]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
	}
	else
		return Error(SyntaxError[2]);
	return ptr;
}
//DeclarePart
ParseNode* Recursive::f4()
{
	ParseNode* ptr=GetParseNode(3,current->LineNum,MAP[3]);
	if(Check(current->LexStr,Predicts[4]))
	{
		ptr->child=f5();
		ptr->child->brother=f30();
		ptr->child->brother->brother=f39();
	}
	else
		return Error(SyntaxError[3]);
	return ptr;
}
//TypeDecpart
ParseNode* Recursive::f5()
{
	if(Check(current->LexStr,Predicts[5])){
		ParseNode* ptr=GetParseNode(4,current->LineNum,MAP[4]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[6]))
		return f6();
	else
		return Error(SyntaxError[4]);
}
//TypeDecpart
ParseNode* Recursive::f6()
{
	ParseNode* ptr=GetParseNode(4,current->LineNum,MAP[4]);
	ptr->child=f7();
	return ptr;
}
//TypeDec
ParseNode* Recursive::f7()
{
	ParseNode* ptr=GetParseNode(5,current->LineNum,MAP[5]);
	if(Check(current->LexStr,Predicts[7]))
	{
		Match(ptr->child,69);
		ptr->child->brother=f8();
	}
	else
		return Error(SyntaxError[5]);
	return ptr;
}
//TypeDecList
ParseNode* Recursive::f8()
{
	ParseNode* ptr=GetParseNode(6,current->LineNum,MAP[6]);
	if(Check(current->LexStr,Predicts[8]))
	{
		ptr->child=f11();
		Match(ptr->child->brother,92);
		ptr->child->brother->brother=f12();
		Match(ptr->child->brother->brother->brother,101);
		ptr->child->brother->brother->brother->brother=f9();
	}
	else
		return Error(SyntaxError[6]);
	return ptr;
}
//TypeDecMore
ParseNode* Recursive::f9()
{
	if(Check(current->LexStr,Predicts[9])){
		ParseNode* ptr=GetParseNode(7,current->LineNum,MAP[7]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[10]))
		return f10();
	else 
		return Error(SyntaxError[7]);
}
//TypeDecMore
ParseNode* Recursive::f10()
{
	ParseNode* ptr=GetParseNode(7,current->LineNum,MAP[7]);
	ptr->child=f8();
	return ptr;
}
//TypeId
ParseNode* Recursive::f11()
{
	ParseNode* ptr=GetParseNode(8,current->LineNum,MAP[8]);
	if(Check(current->LexStr,Predicts[11]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
	}
	else
		return Error(SyntaxError[8]);
	return ptr;
}
//TypeDef
ParseNode* Recursive::f12()
{
	if(Check(current->LexStr,Predicts[12]))
	{
		ParseNode* ptr=GetParseNode(9,current->LineNum,MAP[9]);
		ptr->child=f15();
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[13]))
		return f13();
	else if(Check(current->LexStr,Predicts[14]))
		return f14();
	else
		return Error(SyntaxError[9]);
}
//TypeDef
ParseNode* Recursive::f13()
{
	ParseNode* ptr=GetParseNode(9,current->LineNum,MAP[9]);
	ptr->child=f17();
	return ptr;
}
//TypeDef
ParseNode* Recursive::f14()
{
	ParseNode* ptr=GetParseNode(9,current->LineNum,MAP[9]);
	ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
	GetNextToken();
	return ptr;
}
//BaseType
ParseNode* Recursive::f15()
{
	if(Check(current->LexStr,Predicts[15]))
	{
		ParseNode* ptr=GetParseNode(10,current->LineNum,MAP[10]);
		Match(ptr->child,86);
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[16]))
		return f16();
	else
		return Error(SyntaxError[10]);
}
//BaseType
ParseNode* Recursive::f16()
{
	ParseNode* ptr=GetParseNode(10,current->LineNum,MAP[10]);
	Match(ptr->child,87);
	return ptr;
}
//StructureType
ParseNode* Recursive::f17()
{
	if(Check(current->LexStr,Predicts[17]))
	{
		ParseNode* ptr=GetParseNode(11,current->LineNum,MAP[11]);
		ptr->child=f19();
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[18]))
		return f18();
	else
		return Error(SyntaxError[11]);
}
//StructureType
ParseNode* Recursive::f18()
{
	ParseNode* ptr=GetParseNode(11,current->LineNum,MAP[11]);
	ptr->child=f22();
	return ptr;
}
//ArrayType
ParseNode* Recursive::f19()
{
	ParseNode* ptr=GetParseNode(12,current->LineNum,MAP[12]);
	if(Check(current->LexStr,Predicts[19]))
	{
		Match(ptr->child,82);
		Match(ptr->child->brother,103);
		ptr->child->brother->brother=f20();
		Match(ptr->child->brother->brother->brother,105);
		ptr->child->brother->brother->brother->brother=f21();
		Match(ptr->child->brother->brother->brother->brother->brother,104);
		Match(ptr->child->brother->brother->brother->brother->brother->brother,83);
		ptr->child->brother->brother->brother->brother->brother->brother->brother=f15();
	}
	else
		return Error(SyntaxError[12]);
	return ptr;
}
//Low
ParseNode* Recursive::f20()
{
	ParseNode* ptr=GetParseNode(13,current->LineNum,MAP[13]);
	if(Check(current->LexStr,Predicts[20]))
	{
		ptr->child=GetParseNode(89,current->LineNum,current->SemStr);
		GetNextToken();
	}
	else
		return Error(SyntaxError[13]);
	return ptr;
}
//Top
ParseNode* Recursive::f21()
{
	ParseNode* ptr=GetParseNode(14,current->LineNum,MAP[14]);
	if(Check(current->LexStr,Predicts[21]))
	{
		ptr->child=GetParseNode(89,current->LineNum,current->SemStr);
		GetNextToken();
	}
	else
		return Error(SyntaxError[14]);
	return ptr;
}
//RecType
ParseNode* Recursive::f22()
{
	ParseNode* ptr=GetParseNode(15,current->LineNum,MAP[15]);
	if(Check(current->LexStr,Predicts[22]))
	{
		Match(ptr->child,84);
		ptr->child->brother=f23();
		Match(ptr->child->brother->brother,79);
	}
	else
		return Error(SyntaxError[15]);
	return ptr;
}
//FieldDecList
ParseNode* Recursive::f23()
{
	if(Check(current->LexStr,Predicts[23]))
	{
		ParseNode* ptr=GetParseNode(16,current->LineNum,MAP[16]);
		ptr->child=f15();
		ptr->child->brother=f27();
		Match(ptr->child->brother->brother,101);
		ptr->child->brother->brother->brother=f25();
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[24]))
		return f24();
	else
		return 	Error(SyntaxError[16]);
}
//FieldDecList
ParseNode* Recursive::f24()
{
	ParseNode* ptr=GetParseNode(16,current->LineNum,MAP[16]);
	ptr->child=f19();
	ptr->child->brother=f27();
	Match(ptr->child->brother->brother,101);
	ptr->child->brother->brother->brother=f25();
	return ptr;
}
//FieldDecMore
ParseNode* Recursive::f25()
{
	if(Check(current->LexStr,Predicts[25]))
	{
		ParseNode* ptr=GetParseNode(17,current->LineNum,MAP[17]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[26]))
		return f26();
	else
		return 	Error(SyntaxError[17]);
}
//FieldDecMore
ParseNode* Recursive::f26()
{
	ParseNode* ptr=GetParseNode(17,current->LineNum,MAP[17]);
	ptr->child=f23();
	return ptr;
}
//IdList
ParseNode* Recursive::f27()
{
	ParseNode* ptr=GetParseNode(18,current->LineNum,MAP[18]);
	if(Check(current->LexStr,Predicts[27]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
		ptr->child->brother=f28();
	}
	else
		return Error(SyntaxError[18]);
	return ptr;
}
//IdMore
ParseNode* Recursive::f28()
{
	if(Check(current->LexStr,Predicts[28]))
	{
		ParseNode* ptr=GetParseNode(19,current->LineNum,MAP[19]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[29]))
		return f29();
	else
		return Error(SyntaxError[19]);
}
//IdMore
ParseNode* Recursive::f29()
{
	ParseNode* ptr=GetParseNode(19,current->LineNum,MAP[19]);
	Match(ptr->child,102);
	ptr->child->brother=f27();
	return ptr;
}
//VarDecpart
ParseNode* Recursive::f30()
{	
	if(Check(current->LexStr,Predicts[30]))
	{
		ParseNode* ptr=GetParseNode(20,current->LineNum,MAP[20]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[31]))
		return f31();
	else
		return 	Error(SyntaxError[20]);
}
//VarDecpart
ParseNode* Recursive::f31()
{
	ParseNode* ptr=GetParseNode(20,current->LineNum,MAP[20]);
	ptr->child=f32();
	return ptr;
}
//VarDec
ParseNode* Recursive::f32()
{
	ParseNode* ptr = GetParseNode(21, current->LineNum, MAP[21]);
	if(Check(current->LexStr,Predicts[32]))
	{
		Match(ptr->child,70);
		ptr->child->brother = f33();
	}
	else
		return Error(SyntaxError[21]);
	return ptr;
}
//VarDecList
ParseNode* Recursive::f33()
{
	ParseNode* ptr=GetParseNode(22,current->LineNum,MAP[22]);
	if(Check(current->LexStr,Predicts[33]))
	{
		ptr->child=f12();
		ptr->child->brother=f36();
		Match(ptr->child->brother->brother,101);
		ptr->child->brother->brother->brother=f34();
	}
	else
		return Error(SyntaxError[22]);
	return ptr;
}
//VarDecMore
ParseNode* Recursive::f34()
{	
	if(Check(current->LexStr,Predicts[34]))
	{
		ParseNode* ptr=GetParseNode(23,current->LineNum,MAP[23]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[35]))
		return f35();
	else
		return Error(SyntaxError[23]);
}
//VarDecMore
ParseNode* Recursive::f35()
{
	ParseNode* ptr=GetParseNode(23,current->LineNum,MAP[23]);
	ptr->child=f33();
	return ptr;
}
//VarIdList
ParseNode* Recursive::f36()
{
	ParseNode* ptr=GetParseNode(24,current->LineNum,MAP[24]);
	if(Check(current->LexStr,Predicts[36]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
		ptr->child->brother=f37();
	}
	else
		return Error(SyntaxError[24]);
	return ptr;
}
//VarIdMore
ParseNode* Recursive::f37()
{	
	if(Check(current->LexStr,Predicts[37]))
	{
		ParseNode* ptr=GetParseNode(25,current->LineNum,MAP[25]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[38]))
		return f38();
	else
		return 	Error(SyntaxError[25]);
}
//VarIdMore
ParseNode* Recursive::f38()
{
	
	ParseNode* ptr=GetParseNode(25,current->LineNum,MAP[25]);
	Match(ptr->child,102);
	ptr->child->brother=f36();
	return ptr;
}
//ProcDecpart
ParseNode* Recursive::f39()
{
	if(Check(current->LexStr,Predicts[39]))
	{
		ParseNode* ptr=GetParseNode(26,current->LineNum,MAP[26]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[40]))
		return f40();
	else
		return 	Error(SyntaxError[26]);
}
//ProcDecpart
ParseNode* Recursive::f40()
{
	
	ParseNode* ptr=GetParseNode(26,current->LineNum,MAP[26]);
	ptr->child=f41();
	return ptr;
}
//ProcDec
ParseNode* Recursive::f41()
{
	ParseNode* ptr=GetParseNode(27,current->LineNum,MAP[27]);
	if(Check(current->LexStr,Predicts[41]))
	{
		Match(ptr->child,68);
		ptr->child->brother=f44();
		Match(ptr->child->brother->brother,98);
		ptr->child->brother->brother->brother=f45();
		Match(ptr->child->brother->brother->brother->brother,99);
		Match(ptr->child->brother->brother->brother->brother->brother,101);
		ptr->child->brother->brother->brother->brother->brother->brother=f55();
		ptr->child->brother->brother->brother->brother->brother->brother->brother=f56();
		ptr->child->brother->brother->brother->brother->brother->brother->brother->brother=f42();
	}
	else
		Error(SyntaxError[27]);
	return ptr;
}
//ProcDecMore
ParseNode* Recursive::f42()
{	
	if(Check(current->LexStr,Predicts[42]))
	{
		ParseNode* ptr=GetParseNode(28,current->LineNum,MAP[28]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[43]))
		return f43();
	else
		return 	Error(SyntaxError[28]);
}
//ProcDecMore
ParseNode* Recursive::f43()
{
	
	ParseNode* ptr=GetParseNode(28,current->LineNum,MAP[28]);
	ptr->child=f41();
	return ptr;
}
//ProcName
ParseNode* Recursive::f44()
{
	ParseNode* ptr=GetParseNode(29,current->LineNum,MAP[29]);
	if(Check(current->LexStr,Predicts[44]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
	}
	else
		return Error(SyntaxError[29]);
	return ptr;
}
//ParamList
ParseNode* Recursive::f45()
{	
	if(Check(current->LexStr,Predicts[45]))
	{
		ParseNode* ptr=GetParseNode(30,current->LineNum,MAP[30]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[46]))
		return f46();
	else
		return 	Error(SyntaxError[30]);
}
//ParamList
ParseNode* Recursive::f46()
{
	ParseNode* ptr=GetParseNode(30,current->LineNum,MAP[30]);
	ptr->child=f47();
	return ptr;
}
//ParamDecList
ParseNode* Recursive::f47()
{
	ParseNode* ptr=GetParseNode(31,current->LineNum,MAP[31]);
	if(Check(current->LexStr,Predicts[47]))
	{
		ptr->child=f50();
		ptr->child->brother=f48();
	}
	else
		return Error(SyntaxError[31]);
	return ptr;
}
//ParamMore
ParseNode* Recursive::f48()
{	
	if(Check(current->LexStr,Predicts[48]))
	{
		ParseNode* ptr=GetParseNode(32,current->LineNum,MAP[32]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[49]))
		return f49();
	else
		return 	Error(SyntaxError[32]);
}
//ParamMore
ParseNode* Recursive::f49()
{
	ParseNode* ptr=GetParseNode(32,current->LineNum,MAP[32]);
	Match(ptr->child,101);
	ptr->child->brother=f47();
	return ptr;
}
//Param
ParseNode* Recursive::f50()
{	
	if(Check(current->LexStr,Predicts[50]))
	{
		ParseNode* ptr=GetParseNode(33,current->LineNum,MAP[33]);
		ptr->child=f12();
		ptr->child->brother=f52();
		return ptr;
	} 
	else if(Check(current->LexStr,Predicts[51]))
		return f51();
	else
		return 	Error(SyntaxError[33]);
}
//Param
ParseNode* Recursive::f51()
{	
	
	ParseNode* ptr=GetParseNode(33,current->LineNum,MAP[33]);
	Match(ptr->child,70);
	ptr->child->brother=f12();
	ptr->child->brother->brother=f52();
	return ptr;
}
//FormList
ParseNode* Recursive::f52()
{	
	ParseNode* ptr=GetParseNode(34,current->LineNum,MAP[34]);
	if(Check(current->LexStr,Predicts[52]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
		ptr->child->brother=f53();
	}
	else
		return Error(SyntaxError[34]);
	return ptr;
}
//FidMore
ParseNode* Recursive::f53()
{	
	if(Check(current->LexStr,Predicts[53])){
		ParseNode* ptr=GetParseNode(35,current->LineNum,MAP[35]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[54]))
		return f54();
	else
		return 	Error(SyntaxError[35]);
}
//FidMore
ParseNode* Recursive::f54()
{	
	ParseNode* ptr=GetParseNode(35,current->LineNum,MAP[35]);
	Match(ptr->child,102);
	ptr->child->brother=f52();
	return ptr;
}
//ProcDecPart
ParseNode* Recursive::f55()
{
	ParseNode* ptr=GetParseNode(36,current->LineNum,MAP[36]);	
	if(Check(current->LexStr,Predicts[55]))
		ptr->child=f4();
	else
		return Error(SyntaxError[36]);
	return ptr;
}
//ProcBody
ParseNode* Recursive::f56()
{
	ParseNode* ptr=GetParseNode(37,current->LineNum,MAP[37]);	
	if (Check(current->LexStr, Predicts[56]))
		ptr->child=f57();
	else
		return Error(SyntaxError[37]);
	return ptr;
}
//ProgramBody
ParseNode* Recursive::f57()
{
	ParseNode* ptr=GetParseNode(38,current->LineNum,MAP[38]);
	if(Check(current->LexStr,Predicts[57]))
	{
		Match(ptr->child,78);
		ptr->child->brother=f58();
		Match(ptr->child->brother->brother,79);
	}
	else
		return Error(SyntaxError[38]);
	return ptr;
}
//StmList
ParseNode* Recursive::f58()
{
	ParseNode* ptr=GetParseNode(39,current->LineNum,MAP[39]);
	if(Check(current->LexStr,Predicts[58]))
	{
		ptr->child=f61();
		ptr->child->brother=f59();
	}
	else
		return Error(SyntaxError[39]);
	return ptr;
}
//StmMore
ParseNode* Recursive::f59()
{	
	if(Check(current->LexStr,Predicts[59])){
		ParseNode* ptr=GetParseNode(40,current->LineNum,MAP[40]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[60]))
		return f60();
	else
		return 	Error(SyntaxError[40]);
}
//StmMore
ParseNode* Recursive::f60()
{
	ParseNode* ptr=GetParseNode(40,current->LineNum,MAP[40]);	
	Match(ptr->child,101);
	ptr->child->brother=f58();
	return ptr;
}
//Stm
ParseNode* Recursive::f61()
{	
	if(Check(current->LexStr,Predicts[61]))
	{
		ParseNode* ptr=GetParseNode(41,current->LineNum,MAP[41]);
		ptr->child=f70();
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[62]))
		return f62();
	else if(Check(current->LexStr,Predicts[63]))
		return f63();
	else if(Check(current->LexStr,Predicts[64]))
		return f64();
	else if(Check(current->LexStr,Predicts[65]))
		return f65();
	else if(Check(current->LexStr,Predicts[66]))
		return f66();
	else
		return Error(SyntaxError[41]);
}
//Stm
ParseNode* Recursive::f62()
{
	ParseNode* ptr=GetParseNode(41,current->LineNum,MAP[41]);
	ptr->child=f71();
	return ptr;
}
//Stm
ParseNode* Recursive::f63()
{
	ParseNode* ptr=GetParseNode(41,current->LineNum,MAP[41]);
	ptr->child=f72();
	return ptr;
}
//Stm
ParseNode* Recursive::f64()
{
	ParseNode* ptr=GetParseNode(41,current->LineNum,MAP[41]);
	ptr->child=f74();
	return ptr;
}
//Stm
ParseNode* Recursive::f65()
{
	ParseNode* ptr=GetParseNode(41,current->LineNum,MAP[41]);
	ptr->child=f75();
	return ptr;
}
//Stm
ParseNode* Recursive::f66()
{
	ParseNode* ptr=GetParseNode(41,current->LineNum,MAP[41]);
	ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
	GetNextToken();
	ptr->child->brother=f67();
	return ptr;
}
//AssCall
ParseNode* Recursive::f67()
{
	if(Check(current->LexStr,Predicts[67]))
	{
		ParseNode* ptr=GetParseNode(42,current->LineNum,MAP[42]);
		ptr->child=f69();
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[68]))
		return f68();
	else
		return Error(SyntaxError[42]);;
}
//AssCall
ParseNode* Recursive::f68()
{
	ParseNode* ptr=GetParseNode(42,current->LineNum,MAP[42]);	
	ptr->child=f76();
	return ptr;
}
//AssignmentRest
ParseNode* Recursive::f69()
{
	ParseNode* ptr=GetParseNode(43,current->LineNum,MAP[43]);
	if(Check(current->LexStr,Predicts[69]))
	{
		ptr->child=f93();
		Match(ptr->child->brother,91);
		ptr->child->brother->brother=f83();
	} 
	else
		return Error(SyntaxError[43]);
	return ptr;
}
//ConditionalStm
ParseNode* Recursive::f70()
{
	ParseNode* ptr=GetParseNode(44,current->LineNum,MAP[44]);
	if(Check(current->LexStr,Predicts[70]))
	{
		Match(ptr->child,71);
		ptr->child->brother=f81();
		Match(ptr->child->brother->brother,72);
		ptr->child->brother->brother->brother=f58();
		Match(ptr->child->brother->brother->brother->brother,73);
		ptr->child->brother->brother->brother->brother->brother=f58();
		Match(ptr->child->brother->brother->brother->brother->brother->brother,74);
	}
	else
		return Error(SyntaxError[44]);
	return ptr;
}
//LoopStm
ParseNode* Recursive::f71()
{
	ParseNode* ptr=GetParseNode(45,current->LineNum,MAP[45]);
	if(Check(current->LexStr,Predicts[71]))
	{
		Match(ptr->child,75);
		ptr->child->brother=f81();
		Match(ptr->child->brother->brother,76);
		ptr->child->brother->brother->brother=f58();
		Match(ptr->child->brother->brother->brother->brother,77);
	}
	else
		return Error(SyntaxError[45]);
	return ptr;
}
//InputStm
ParseNode* Recursive::f72()
{
	ParseNode* ptr=GetParseNode(46,current->LineNum,MAP[46]);
	if(Check(current->LexStr,Predicts[72]))
	{
		Match(ptr->child,80);
		Match(ptr->child->brother,98);
		ptr->child->brother->brother=f73();
		Match(ptr->child->brother->brother->brother,99);
	}
	else
		return Error(SyntaxError[46]);
	return ptr;
}
//Invar
ParseNode* Recursive::f73()
{
	ParseNode* ptr=GetParseNode(47,current->LineNum,MAP[47]);
	if(Check(current->LexStr,Predicts[73]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
	}
	else
		return Error(SyntaxError[47]);
	return ptr;
}
//OutputStm
ParseNode* Recursive::f74()
{
	ParseNode* ptr=GetParseNode(48,current->LineNum,MAP[48]);
	if(Check(current->LexStr,Predicts[74]))
	{
		Match(ptr->child,81);
		Match(ptr->child->brother,98);
		ptr->child->brother->brother=f83();
		Match(ptr->child->brother->brother->brother,99);
	}
	else
		return Error(SyntaxError[48]);
	return ptr;
}
//ReturnStm
ParseNode* Recursive::f75()
{
	ParseNode* ptr=GetParseNode(49,current->LineNum,MAP[49]);
	if(Check(current->LexStr,Predicts[75]))
		Match(ptr->child,85);
	else
		return Error(SyntaxError[49]);
	return ptr;
}
//CallStmRest
ParseNode* Recursive::f76()
{
	ParseNode* ptr=GetParseNode(50,current->LineNum,MAP[50]);
	if(Check(current->LexStr,Predicts[76]))
	{
		Match(ptr->child,98);
		ptr->child->brother=f77();
		Match(ptr->child->brother->brother,99);
	}
	else
		return Error(SyntaxError[50]);
	return ptr;
}
//ActParamList
ParseNode* Recursive::f77()
{
	if(Check(current->LexStr,Predicts[77])){
		ParseNode* ptr=GetParseNode(51,current->LineNum,MAP[51]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[78]))
		return f78();
	else
		return 	Error(SyntaxError[51]);
}
//ActParamList
ParseNode* Recursive::f78()
{
	ParseNode* ptr=GetParseNode(51,current->LineNum,MAP[51]);
	ptr->child=f83();
	ptr->child->brother=f79();
	return ptr;
}
//ActParamMore
ParseNode* Recursive::f79()
{
	if(Check(current->LexStr,Predicts[79])){
		ParseNode* ptr=GetParseNode(52,current->LineNum,MAP[52]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[80]))
		return f80();
	else
		return 	Error(SyntaxError[52]);
}
//ActParamMore
ParseNode* Recursive::f80()
{
	ParseNode* ptr=GetParseNode(52,current->LineNum,MAP[52]);
	Match(ptr->child,102);
	ptr->child->brother=f77();
	return ptr;
}
//RelExp
ParseNode* Recursive::f81()
{
	ParseNode* ptr=GetParseNode(53,current->LineNum,MAP[53]);
	if(Check(current->LexStr,Predicts[81]))
	{
		ptr->child=f83();
		ptr->child->brother=f82();
	}
	else
		return Error(SyntaxError[53]);
	return ptr;
}
//OtherRelE
ParseNode* Recursive::f82()
{
	ParseNode* ptr=GetParseNode(54,current->LineNum,MAP[54]);
	if(Check(current->LexStr,Predicts[82]))
	{
		ptr->child=f99();
		ptr->child->brother=f83();
	}
	else
		return Error(SyntaxError[54]);
	return ptr;
}
//Exp
ParseNode* Recursive::f83()
{
	ParseNode* ptr=GetParseNode(55,current->LineNum,MAP[55]);
	if(Check(current->LexStr,Predicts[83]))
	{
		ptr->child=f86();
		ptr->child->brother=f84();
	}
	else
		return Error(SyntaxError[55]);
	return ptr;
}
//OtherTerm
ParseNode* Recursive::f84()
{
	if(Check(current->LexStr,Predicts[84])){
		ParseNode* ptr=GetParseNode(56,current->LineNum,MAP[56]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[85]))
		return f85();
	else
		return Error(SyntaxError[56]);
}
//OtherTerm
ParseNode* Recursive::f85()
{
	ParseNode* ptr=GetParseNode(56,current->LineNum,MAP[56]);
	ptr->child=f101();
	ptr->child->brother=f83();
	return ptr;
}
//Term
ParseNode* Recursive::f86()
{
	ParseNode* ptr=GetParseNode(57,current->LineNum,MAP[57]);
	if(Check(current->LexStr,Predicts[86]))
	{
		ptr->child=f89();
		ptr->child->brother=f87();
	}
	else
		return Error(SyntaxError[57]);
	return ptr;
}
//OtherFactor
ParseNode* Recursive::f87()
{
	if(Check(current->LexStr,Predicts[87])){
		ParseNode* ptr=GetParseNode(58,current->LineNum,MAP[58]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[88]))
		return f88();
	else
		return 	Error(SyntaxError[58]);
}
//OtherFactor
ParseNode* Recursive::f88()
{
	ParseNode* ptr=GetParseNode(58,current->LineNum,MAP[58]);
	ptr->child=f103();
	ptr->child->brother=f86();
	return ptr;
}
//Factor
ParseNode* Recursive::f89()
{
	
	if(Check(current->LexStr,Predicts[89]))
	{
		ParseNode* ptr=GetParseNode(59,current->LineNum,MAP[59]);
		Match(ptr->child,98);
		ptr->child->brother=f83();
		Match(ptr->child->brother->brother,99);
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[90]))
		return f90();
	else if(Check(current->LexStr,Predicts[91]))
		return f91();
	else
		return Error(SyntaxError[59]);
}
//Factor
ParseNode* Recursive::f90()
{
	ParseNode* ptr=GetParseNode(59,current->LineNum,MAP[59]);
	ptr->child=GetParseNode(89,current->LineNum,current->SemStr);
	GetNextToken();
	return ptr;
}
//Factor
ParseNode* Recursive::f91()
{
	ParseNode* ptr=GetParseNode(59,current->LineNum,MAP[59]);
	ptr->child=f92();
	return ptr;
}
//Variable
ParseNode* Recursive::f92()
{
	ParseNode* ptr=GetParseNode(60,current->LineNum,MAP[60]);
	if(Check(current->LexStr,Predicts[92]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
		ptr->child->brother=f93();
	}
	else
		return Error(SyntaxError[60]);
	return ptr;
}
//VariMore
ParseNode* Recursive::f93()
{
	if(Check(current->LexStr,Predicts[93])){
		ParseNode* ptr=GetParseNode(61,current->LineNum,MAP[61]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[94]))
		return f94();
	else if(Check(current->LexStr,Predicts[95]))
		return f95();
	else
		return Error(SyntaxError[61]);
}
//VariMore
ParseNode* Recursive::f94()
{
    ParseNode* ptr=GetParseNode(61,current->LineNum,MAP[61]);
	Match(ptr->child,103);
	ptr->child->brother=f83();
	Match(ptr->child->brother->brother,104);
	return ptr;
}
//VariMore
ParseNode* Recursive::f95()
{
	ParseNode* ptr=GetParseNode(61,current->LineNum,MAP[61]);
	Match(ptr->child,100);
	ptr->child->brother=f96();
	return ptr;
}
//FieldVar
ParseNode* Recursive::f96()
{
	ParseNode* ptr=GetParseNode(62,current->LineNum,MAP[62]);
	if(Check(current->LexStr,Predicts[96]))
	{
		ptr->child=GetParseNode(88,current->LineNum,current->SemStr);
		GetNextToken();
		ptr->child->brother=f97();
	}
	else
		return Error(SyntaxError[62]);
	return ptr;
}
//FieldVarMore
ParseNode* Recursive::f97()
{
	if(Check(current->LexStr,Predicts[97])){
		ParseNode* ptr=GetParseNode(63,current->LineNum,MAP[63]);
		ptr->child=GetParseNode(-1,current->LineNum,"");
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[98]))
		return f98();
	else
		return 	Error(SyntaxError[63]);
}
//FieldVarMore
ParseNode* Recursive::f98()
{
	ParseNode* ptr=GetParseNode(63,current->LineNum,MAP[63]);
	Match(ptr->child,103);
	ptr->child->brother=f83();
	Match(ptr->child->brother->brother,104);
	return ptr;
}
//CmpOp
ParseNode* Recursive::f99()
{
	if(Check(current->LexStr,Predicts[99]))
	{
		ParseNode* ptr=GetParseNode(64,current->LineNum,MAP[64]);
		Match(ptr->child,93);
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[100]))
		return f100();
	else 
		return Error(SyntaxError[64]);
}
//CmpOp
ParseNode* Recursive::f100()
{
	ParseNode* ptr=GetParseNode(64,current->LineNum,MAP[64]);
	Match(ptr->child,92);
	return ptr;
}
//AddOp
ParseNode* Recursive::f101()
{
	if(Check(current->LexStr,Predicts[101]))
	{
		ParseNode* ptr=GetParseNode(65,current->LineNum,MAP[65]);
		Match(ptr->child,94);
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[102]))
		return f102();
	else
		return Error(SyntaxError[65]);
}
//AddOp
ParseNode* Recursive::f102()
{
	ParseNode* ptr=GetParseNode(65,current->LineNum,MAP[65]);
	Match(ptr->child,95);
	return ptr;
}
//MultOp
ParseNode* Recursive::f103()
{
	if(Check(current->LexStr,Predicts[103]))
	{
		ParseNode* ptr=GetParseNode(66,current->LineNum,MAP[66]);
		Match(ptr->child,96);
		return ptr;
	}
	else if(Check(current->LexStr,Predicts[104]))
		return f104();
	else
		return Error(SyntaxError[66]);
}
//MultOp
ParseNode* Recursive::f104()
{
	ParseNode* ptr=GetParseNode(66,current->LineNum,MAP[66]);
	Match(ptr->child,97);
	return ptr;
}