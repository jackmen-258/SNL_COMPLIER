#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include<iostream>
#include<list>
#include<string>
#include<algorithm>
#include<iomanip>

using namespace std;

//������Ϣ��
#define SEM_Reservedword  "�����֣���������Ϣ"
#define SEM_sDelimiter    "���ֽ������������Ϣ"
#define SEM_dDelimiter    "˫�ֽ������������Ϣ"
#define SEM_array         "�����޽������������Ϣ"
#define SEM_EOF           "�ļ�����������������Ϣ"
#define SEM_DOT           "�������������������Ϣ"

//������Ϣ�б�
#define Error_Annotate  "�����ļ�β�����ҵ���ע�ͽ�����\"}\""
#define Error_CharC     "�����ļ�β�����ҵ����ַ�����������\"'\""
#define Error_Undefine  "����ʶ���ַ�"


typedef struct node
{
    int LineNum;               //�к�
	string LexStr;  //�ʷ���Ϣ�ַ����������ɼ���
	string SemStr;  //������Ϣ������Ϊ�ַ�����
}TokenNode;
list <TokenNode*>TokenList;

int CntLexError=0;


string LexStr[]//�����ֱ�
{
    "program" , "procedure" , "type" , "var" , "if" , "then" , "else" , "fi" ,
    "while" , "do", "endwh" , "begin" , "end" , "read" , "write" , "array" ,
    "of" , "record" , "return" , "integer" , "char" ,"$"
};

bool isChar(char ch);//�Ƿ�Ϊ�ַ�
bool isDigit(char ch);//�Ƿ�Ϊ����
bool isDelimiter(char ch);//�Ƿ�Ϊ�ֽ��
bool isLexStr(string ch);//�Ƿ�Ϊ������

int getNextChar(FILE * file);//��ȡ��һ�ַ�
int ungetNextChar(FILE * file);//����һ���ַ�

void AddNode(int cntline,string lexstr,string semstr);//Token�������һ���ڵ�

void GetID(FILE*file,int CntLine);//�����֡���ʶ��
void GetINTC(FILE* file,int CntLine);//�޷�������
void GetDelimiter(FILE* file,int CntLine);//��˫�ֽ��
void Filter(FILE* file,int &CntLine);//����ע��
void GetCharC(FILE* file,int &CntLine);//�ַ�������
void EndFile(FILE* file,int CntLine);//�ļ�����

void GetTokenList();//ɨ���ļ������Token����

void PrintTokenList();//��ӡToken����

bool isChar(char ch)//��ʶ��
{
	if(ch>='a'&&ch<='z'||ch>='A'&&ch<='Z')
		return true;
	return false;
}
bool isDigit(char ch)//����
{
	if(ch>='0'&&ch<='9')
		return true;
	return false;
}
bool isDelimiter(char ch)//�ֽ��
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

int getNextChar(FILE * file)//��ȡ�ļ���һ�ַ�
{
	return fgetc(file);
}
int ungetNextChar(FILE * file)//����һ�ַ�
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

bool isLexStr(string ch)//�ж��Ƿ�Ϊ������
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

    char ch=getNextChar(file);//��ȡ�ַ����������ֻ��ʶ����
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
            AddNode(cntline,"lex_error",Error_Annotate);//�����ļ�β�����ҵ���ע�ͽ�����\"}\"";
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
	char ch=getNextChar(file);//��ȡ��һ���ַ�
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
    fprintf(file,"%s","\n       ��Դ������дʷ��������õ�TOKEN��Ϣ���£�\n\n\n");
    fprintf(file,"%20s %30s %40s","�к�","�ʷ���Ϣ","������Ϣ\n\n\n");
    list<TokenNode*>::iterator it;
    for(it=TokenList.begin(); it!=TokenList.end();it++)
    {
        fprintf(file,"%20d %30s %40s",(*it)->LineNum,(*it)->LexStr.c_str(),(*it)->SemStr.c_str());
        fprintf(file,"%s","\n");
    }

    fclose(file);




    /*����̨���*/
//    cout<<"��Դ������дʷ��������õ�TOKEN��Ϣ���£�"<<endl<<endl;
//    cout<<left<<setw(18)<<"�к�";
//    cout<<left<<setw(18)<<"�ʷ���Ϣ";
//    cout<<left<<setw(18)<<"������Ϣ"<<endl<<endl;
//    list<TokenNode*>::iterator it;
//    for(it=TokenList.begin(); it!=TokenList.end();it++)
//    {
//        cout<<left<<setw(15)<<(*it)->LineNum<<" ";
//        cout<<left<<setw(15)<<(*it)->LexStr<<" ";
//        cout<<left<<setw(15)<<(*it)->SemStr<<" "<<endl;
//    }

}



#endif // LEXER_H_INCLUDED




