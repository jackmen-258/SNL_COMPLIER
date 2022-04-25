#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include<iostream>
#include<list>
#include<string>
#include<algorithm>
#include<iomanip>

using namespace std;

//语义信息表
#define SEM_Reservedword  "保留字，无语义信息"
#define SEM_sDelimiter    "单分界符，无语义信息"
#define SEM_dDelimiter    "双分界符，无语义信息"
#define SEM_array         "数组限界符，无语义信息"
#define SEM_EOF           "文件结束符，无语义信息"
#define SEM_DOT           "程序结束符，无语义信息"

//出错信息列表
#define Error_Annotate  "到达文件尾，仍找到不注释结束符\"}\""
#define Error_CharC     "到达文件尾，仍找到不字符常量结束符\"'\""
#define Error_Undefine  "不可识别字符"


typedef struct node
{
    int LineNum;               //行号
	string LexStr;  //词法信息字符串描述（可见）
	string SemStr;  //词义信息（类型为字符串）
}TokenNode;
list <TokenNode*>TokenList;

int CntLexError=0;


string LexStr[]//保留字表
{
    "program" , "procedure" , "type" , "var" , "if" , "then" , "else" , "fi" ,
    "while" , "do", "endwh" , "begin" , "end" , "read" , "write" , "array" ,
    "of" , "record" , "return" , "integer" , "char" ,"$"
};

bool isChar(char ch);//是否为字符
bool isDigit(char ch);//是否为数字
bool isDelimiter(char ch);//是否为分界符
bool isLexStr(string ch);//是否为保留字

int getNextChar(FILE * file);//读取下一字符
int ungetNextChar(FILE * file);//倒退一个字符

void AddNode(int cntline,string lexstr,string semstr);//Token序列添加一个节点

void GetID(FILE*file,int CntLine);//保留字、标识符
void GetINTC(FILE* file,int CntLine);//无符号整数
void GetDelimiter(FILE* file,int CntLine);//单双分界符
void Filter(FILE* file,int &CntLine);//过滤注释
void GetCharC(FILE* file,int &CntLine);//字符串常量
void EndFile(FILE* file,int CntLine);//文件结束

void GetTokenList();//扫描文件，获得Token序列

void PrintTokenList();//打印Token序列

bool isChar(char ch)//标识符
{
	if(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z')
		return true;
	return false;
}
bool isDigit(char ch)//数字
{
	if(ch>='0'&&ch<='9')
		return true;
	return false;
}
bool isDelimiter(char ch)//分界符
{
	char str[]={
		':' , '=' , '<' , '+' , '-' ,
		'*' , '/' , '(' , ')' , '.' ,
		';' , ',' , '[' , ']' ,'$'
	};
	for(int i=0;str[i]!='$';i++)
	{
		if(ch==str[i])
			return true;
	}
	return false;
}

int getNextChar(FILE * file)//读取文件下一字符
{
	return fgetc(file);
}
int ungetNextChar(FILE * file)//回退一字符
{
	return fseek(file,-1,1);//1 current  2 endof file   0 beginof
}


void AddNode(int cntline,string lexstr,string semstr)
{
    TokenNode *t=new TokenNode;
    t->LineNum=cntline;
    t->LexStr=lexstr;
    t->SemStr=semstr;
    TokenList.push_back(t);
}

TokenNode* GetTokenNode(int cntline,string lexstr,string semstr)
{
    TokenNode *t=new TokenNode;
    t->LineNum=cntline;
    t->LexStr=lexstr;
    t->SemStr=semstr;
    return t;
}

bool isLexStr(string ch)//判断是否为保留字
{
	for(int i=0;LexStr[i]!="$";i++)
	{
		if(LexStr[i]==ch)
			return true;
	}
	return false;
}
void GetID(FILE*file,int CntLine)
{
    string str;

    char ch=getNextChar(file);//读取字符串（保留字或标识符）
    while(isChar(ch)||isDigit(ch))
    {
        str=str+ch;
        ch=getNextChar(file);
    }
    if(ch!=EOF) ungetNextChar(file);

    if(isLexStr(str))
        AddNode(CntLine,str,SEM_Reservedword);
    else
        AddNode(CntLine,"ID",str);
}

void GetINTC(FILE* file,int CntLine)
{
    string str;
	char ch=getNextChar(file);
	while(isDigit(ch))
	{
		str+=ch;
		ch=getNextChar(file);
	}
	if(ch!=EOF)
		ungetNextChar(file);
	AddNode(CntLine,"INTC",str);
}

void GetDelimiter(FILE* file,int CntLine)
{
	char ch=getNextChar(file),chh;
	if(ch==':')
	{
		if(getNextChar(file)=='=')
		{
			AddNode(CntLine,":=",SEM_dDelimiter);
			return ;
		}
		else
			ungetNextChar(file);
	}
	if(ch=='.')
	{
		chh=getNextChar(file);
		if(chh=='.')
		{
			AddNode(CntLine,"..",SEM_array);
			return ;
		}
		if(chh!=EOF)
			ungetNextChar(file);
		AddNode(CntLine,".",SEM_DOT);
		return ;
	}
	string str;
	str+=ch;
	AddNode(CntLine,str,SEM_sDelimiter);
}
void Filter(FILE* file,int &CntLine)
{
	char ch=getNextChar(file);
	int cntline=CntLine;
	while(ch!='}')
	{
		if(ch=='\n') CntLine++;
		else if(ch==EOF)
		{
            AddNode(cntline,"lex_error",Error_Annotate);//到达文件尾，仍找到不注释结束符\"}\"";
			CntLexError++;
			break;
		}
		ch=getNextChar(file);
	}
}
void GetCharC(FILE* file,int &CntLine)
{
	string str;
	char ch=getNextChar(file);
	int cntline=CntLine;
	while(ch!='\'')
	{
		if(ch=='\n')
			CntLine++;
		else if(ch==EOF)
		{
			AddNode(cntline,"lex_error",Error_CharC);
			CntLexError++;
			return ;
		}
		else str+=ch;
		ch=getNextChar(file);
	}
	AddNode(cntline,"CHARC",str);

}
void EndFile(FILE* file,int CntLine)
{
	AddNode(CntLine,"EOF",SEM_EOF);
	fclose(file);
}

void GetTokenList()
{
    string str1="C:\\Users\\HP\\Desktop\\design\\ComplierDesign\\SNL_COMPLIER\\data\\",str2;
    cin>>str2;
    string SourceFile=str1+str2;
    FILE* file=fopen(SourceFile.c_str(),"r");
	if(file==NULL)
	{
		cerr<<"Error: cannot open the sourcefile!"<<endl;
		return ;
	}

	int CntLine = 0;
	char ch=getNextChar(file);//获取下一个字符
	while(ch!=EOF)
    {
        if(isChar(ch))
        {
            ungetNextChar(file);
			GetID(file,CntLine);
        }else if(isDigit(ch))
        {
            ungetNextChar(file);
			GetINTC(file,CntLine);
        }else if(isDelimiter(ch))
        {
            ungetNextChar(file);
			GetDelimiter(file,CntLine);
        }else if(ch=='{')
        {
            ungetNextChar(file);
			Filter(file,CntLine);
        }else if(ch=='\'')
        {
            ungetNextChar(file);
			GetCharC(file,CntLine);
        }else if(ch==' ');
        else if(ch=='\n')
        {
            CntLine++;
        }else if(ch==EOF)
        {
            EndFile(file,CntLine);
        }else
        {
            AddNode(CntLine,"lex_error",Error_Undefine);
            CntLexError++;
        }
        ch=getNextChar(file);
    }

}

void PrintTokenList()
{

    FILE *file=fopen("C:\\Users\\HP\\Desktop\\design\\ComplierDesign\\SNL_COMPLIER\\output\\lex.txt","w");
    fprintf(file,"%s","\n       对源代码进行词法分析，得到TOKEN信息如下：\n\n\n");
    fprintf(file,"%20s %30s %40s","行号","词法信息","语义信息\n\n\n");
    list<TokenNode*>::iterator it;
    for(it=TokenList.begin(); it!=TokenList.end();it++)
    {
        fprintf(file,"%20d %30s %40s",(*it)->LineNum,(*it)->LexStr.c_str(),(*it)->SemStr.c_str());
        fprintf(file,"%s","\n");
    }

    fclose(file);




    /*控制台输出*/
//    cout<<"对源代码进行词法分析，得到TOKEN信息如下："<<endl<<endl;
//    cout<<left<<setw(18)<<"行号";
//    cout<<left<<setw(18)<<"词法信息";
//    cout<<left<<setw(18)<<"语义信息"<<endl<<endl;
//    list<TokenNode*>::iterator it;
//    for(it=TokenList.begin(); it!=TokenList.end();it++)
//    {
//        cout<<left<<setw(15)<<(*it)->LineNum<<" ";
//        cout<<left<<setw(15)<<(*it)->LexStr<<" ";
//        cout<<left<<setw(15)<<(*it)->SemStr<<" "<<endl;
//    }

}



#endif // LEXER_H_INCLUDED




