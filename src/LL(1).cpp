#include"parse.h"
#include<string.h>
#include<stack>

#define SPACESTR "              "
#define SLEN strlen(SPACESTR)
#define VT 67

stack<ParseNode*>ParseStack;


int CntParseError=0;
vector<string>TokenParseError;
ParseNode* ParseTree;

int cur=0;

//语法分析表
int AT[70][40];
void AnalysisTable()
{
    AT[0][0]=1;       AT[1][0]=2;       AT[2][0]=3;       AT[3][1]=4;       AT[3][2]=4;
    AT[3][3]=4;       AT[3][11]=4;      AT[4][1]=5;       AT[4][2]=6;       AT[4][3]=5;
    AT[4][11]=5;	  AT[5][2]=7;       AT[6][21]=8;      AT[7][1]=9;       AT[7][3]=9;
    AT[7][11]=9;      AT[7][21]=10;     AT[8][21]=11;     AT[9][15]=13;     AT[9][17]=13;  AT[9][19]=12;
    AT[9][21]=14;     AT[9][23]=12;     AT[10][19]=15;    AT[10][23]=16;    AT[11][15]=17;
    AT[11][17]=18;    AT[12][15]=19;    AT[13][22]=20;    AT[14][22]=21;    AT[15][17]=22;
    AT[16][15]=24;    AT[16][19]=23;    AT[16][23]=23;    AT[17][12]=25;    AT[17][15]=26;
    AT[17][19]=26;    AT[17][20]=26;    AT[18][21]=27;    AT[19][34]=28;    AT[19][35]=29;
    AT[20][1]=30;     AT[20][3]=31;     AT[20][11]=30;    AT[21][3]=32;     AT[22][15]=33;
    AT[22][17]=33;    AT[22][19]=33;    AT[22][20]=33;    AT[22][21]=33;    AT[23][1]=34;
    AT[23][11]=34;    AT[23][15]=35;    AT[23][17]=35;    AT[23][19]=35;    AT[23][20]=35;
    AT[23][21]=35;    AT[24][21]=36;    AT[25][34]=28;    AT[25][35]=29;    AT[26][1]=40;
    AT[26][11]=39;    AT[27][1]=41;     AT[28][1]=43;     AT[28][11]=42;    AT[29][21]=44;
    AT[30][3]=46;     AT[30][15]=46;    AT[30][17]=46;    AT[30][19]=46;    AT[30][20]=46;
    AT[30][21]=46;    AT[30][32]=45;    AT[31][3]=47;     AT[31][15]=47;    AT[31][17]=47;
    AT[31][19]=47;    AT[31][20]=47;    AT[30][21]=47;    AT[32][32]=48;    AT[32][34]=49;
    AT[33][3]=51;     AT[33][15]=50;    AT[33][17]=50;    AT[33][19]=50;    AT[33][20]=50;
    AT[33][21]=50;    AT[34][21]=52;    AT[35][32]=53;    AT[35][34]=53;    AT[35][35]=54;
    AT[36][1]=55;     AT[36][2]=55;     AT[36][3]=55;     AT[36][11]=55;    AT[37][11]=56;
    AT[38][11]=57;    AT[39][4]=58;     AT[39][8]=58;     AT[39][12]=58;    AT[39][13]=58;
    AT[39][14]=58;    AT[39][18]=58;    AT[39][21]=58;    AT[39][34]=58;    AT[40][6]=59;
    AT[40][7]=59;     AT[40][10]=59;    AT[40][12]=59;    AT[40][34]=60;    AT[41][4]=61;
    AT[41][8]=62;     AT[41][13]=63;    AT[41][14]=64;    AT[41][18]=65;    AT[41][21]=66;
    AT[42][24]=67;    AT[42][31]=68;    AT[42][33]=67;    AT[42][36]=67;    AT[42][24]=67;
    AT[43][24]=69;    AT[43][33]=69;    AT[43][36]=69;    AT[44][4]=70;     AT[45][8]=71;
    AT[46][13]=72;    AT[47][21]=73;    AT[48][14]=74;    AT[49][18]=75;    AT[50][31]=76;
    AT[51][21]=78;    AT[51][22]=78;    AT[51][31]=78;    AT[51][32]=77;    AT[52][32]=79;
    AT[52][35]=80;    AT[53][21]=81;    AT[53][22]=81;    AT[53][31]=81;    AT[54][25]=82;
    AT[54][26]=82;    AT[55][21]=83;    AT[55][22]=83;    AT[55][31]=83;    AT[56][5]=84;
    AT[56][6]=84;     AT[56][7]=84;     AT[56][9]=84;     AT[56][10]=84;    AT[56][12]=84;
    AT[56][25]=84;    AT[56][26]=84;    AT[56][27]=85;    AT[56][28]=85;    AT[56][32]=84;
    AT[56][34]=84;    AT[56][35]=84;    AT[56][37]=84;    AT[57][21]=86;    AT[57][22]=86;
    AT[57][31]=86;    AT[58][5]=87;     AT[58][6]=87;     AT[58][7]=87;     AT[58][9]=87;
    AT[58][12]=87;    AT[58][12]=87;    AT[58][12]=87;    AT[58][12]=87;    AT[58][12]=87;
    AT[58][25]=87;    AT[58][26]=87;    AT[58][27]=87;    AT[58][28]=87;    AT[58][29]=88;
    AT[58][30]=88;    AT[58][34]=87;    AT[58][37]=87;    AT[58][12]=87;    AT[58][12]=87;
    AT[59][21]=91;    AT[59][22]=90;    AT[59][31]=89;    AT[60][21]=92;    AT[61][5]=93;
    AT[61][6]=93;     AT[61][7]=93;     AT[61][9]=93;     AT[61][10]=93;    AT[61][12]=93;
    AT[61][24]=93;    AT[61][25]=93;    AT[61][26]=93;    AT[61][27]=93;    AT[61][28]=93;
    AT[61][29]=93;    AT[61][30]=93;    AT[61][32]=93;    AT[61][33]=95;    AT[61][34]=93;
    AT[61][35]=93;    AT[61][36]=94;    AT[61][37]=93;    AT[61][24]=93;    AT[61][24]=93;
    AT[62][21]=96;    AT[63][5]=97;     AT[63][6]=97;     AT[63][7]=97;     AT[63][9]=97;
    AT[63][10]=97;    AT[63][12]=97;    AT[63][24]=97;    AT[63][25]=97;    AT[63][26]=97;
    AT[63][27]=97;    AT[63][28]=97;    AT[63][29]=97;    AT[63][30]=97;    AT[63][32]=97;
    AT[63][34]=97;    AT[63][35]=97;    AT[63][36]=98;    AT[63][37]=97;    AT[64][25]=100;
    AT[64][26]=99;    AT[65][27]=101;   AT[65][28]=102;   AT[66][29]=103;   AT[66][30]=104;


}

int getSymbol(string str)//获取编号
{
    for(int i=0;i<=106;i++)
    {
        if(str==MAP[i])
            return i;
    }
    return -1;
}
void ParseError(string str)
{
	char* error;
	CntParseError++;
	sprintf(error,"Error %d Line %d : %s",CntParseError,TokenList[cur]->LineNum,str.c_str());
	string s=(string)error;
	TokenParseError.push_back(s);
}


void LL1_VT(ParseNode* node)
{
	if(node->Symbol==getSymbol(TokenList[cur]->LexStr))
	{
	    cur++;
	    if(node->Symbol==getSymbol("INTC")||node->Symbol==getSymbol("CHARC")||node->Symbol==getSymbol("ID"))
            node->ParseStr=TokenList[cur]->SemStr;
	}
	else
	{
		string str="缺少";
		str+=MAP[node->Symbol];
		ParseError(str);
	}
}


void LL1_VN(ParseNode* node)
{
	int *p=P[AT[node->Symbol][getSymbol(TokenList[cur]->LexStr)-VT]];
	//int *p=P[cnt];//获得产生式

	if(!p)
	{
	    ParseError(SyntaxError[node->Symbol]);//未找到产生式，则产生一个出错信息
	    return;
	}

    if(p[1]==-1) return ;
    ParseNode* t=GetParseNode(p[1],TokenList[cur]->LineNum,MAP[p[1]]);
    node->child=t;
    stack<ParseNode*> s;//利用栈，将产生式倒序
    s.push(t);
    for(int i=2;p[i]!=-1;i++)//将产生式构成一个子树
    {
        t->brother=GetParseNode(p[i],TokenList[cur]->LineNum,MAP[p[i]]);
        t=t->brother;
        s.push(t);
    }
    while(!s.empty())
    {
        ParseStack.push(s.top());
        s.pop();
    }

}

void ParseScanner()
{
    ParseNode*pnode=GetParseNode(getSymbol("Program"), TokenList[cur]->LineNum, "Program");
    ParseTree=pnode;
    ParseStack.push(pnode);
    while(!ParseStack.empty())
    {
        pnode=ParseStack.top();
		ParseStack.pop();
		if(pnode->Symbol>=VT)
			LL1_VT(pnode);
		else
			LL1_VN(pnode);		
    }

}

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

void NodeOut(FILE* file,ParseNode* p,int layer,bool feed)
{
	if(feed&&p->Symbol!=-1)
		fprintf(file,"\nline %d :%s",p->LineNum,space_format(layer).c_str());
	if(p->Symbol!=-1)
		fprintf(file,"%s",dash_format(p->ParseStr).c_str());
	if(p->child!=NULL)
		NodeOut(file,p->child,layer+1,0);
	if(p->brother!=NULL)
		NodeOut(file,p->brother,layer,1);
}

void TreeOut()
{
	FILE* file=fopen("D:\\Code\\Curriculum design\\Compilers\\parse_result.txt","w+");
	if(file)
	{
		if(ParseTree)
			NodeOut(file,ParseTree,0,1);
		else
			cerr<<"语法树不存在"<<endl;
		fclose(file);
	}
	else
		cerr<<"文件打开失败"<<endl;
}



int main()
{
    GetTokenList();
    ParseScanner();
    TreeOut();

    return 0;
}













