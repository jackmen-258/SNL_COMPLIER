#ifndef PARSE_H_INCLUDED
#define PARSE_H_INCLUDED

#include "lexer.h"
#include<iostream>

//语法树结点
struct ParseNode
{
	int Symbol;//终极符与非终极符编号，LEX,N_Symbol
	int LineNum;//行号
	string ParseStr;// 语法描述信息
	ParseNode* brother;
	ParseNode* child;
};

ParseNode* GetParseNode(int symbol, int linenum, string str)
{
	ParseNode* parsenode=new ParseNode;
	parsenode->Symbol=symbol;
	parsenode->LineNum=linenum;
	parsenode->ParseStr=str;
	parsenode->brother=NULL;
	parsenode->child=NULL;
	return parsenode;
}

string MAP[]=
{
     /*非终极符*/
	 "Program", "ProgramHead", "ProgramName", "DeclarePart", "TypeDecpart",//0-4

	 "TypeDec", "TypeDecList", "TypeDecMore", "TypeId", "TypeDef",//5-9

	 "BaseType", "StructureType", "ArrayType", "Low", "Top",//10-14

     "RecType", "FieldDecList", "FieldDecMore", "IdList", "IdMore",//15-19

     "VarDecpart", "VarDec", "VarDecList", "VarDecMore", "VarIdList",//20-24

     "VarIdMore", "ProcDecpart", "ProcDec", "ProcDecMore", "ProcName",//25-29

     "ParamList", "ParamDecList", "ParamMore", "Param", "FormList",//30-34

     "FidMore", "ProcDecPart", "ProcBody", "ProgramBody", "StmList",//35-39

     "StmMore", "Stm", "AssCall", "AssignmentRest", "ConditionalStm",//40-44

     "LoopStm", "InputStm", "Invar", "OutputStm", "ReturnStm",//45-49

     "CallStmRest", "ActParamList", "ActParamMore", "RelExp", "OtherRelE",//50-54

     "Exp", "OtherTerm", "Term", "OtherFactor", "Factor",//55-59

     "Variable", "VariMore", "FieldVar", "FieldVarMore", "CmpOp",//60-64

     "AddOp", "MultOp",//65,66

     /*终极符*/
     "program", "procedure", "type", //67-69

     "var", "if", "then", "else", "fi",//70-74

     "while", "do", "endwh", "begin", "end",//75-79

     "read" , "write" ,"array" , "of" ,"record", //80-84

     "return", "integer", "char", "ID", "INTC",//85-89

     "CHARC", ":=", "=", "<", "+", //90-94

     "-", "*", "/", "(", ")", //95-99

     ".", ";", ",", "[", "]","..",//100-105

     "lex_error", "syn_error"//106-107
};

//产生式
int P1[]={0,1,3,38,-1};
/**程序头**/
int P2[]={1,67,2,-1};
int P3[]={2,88,-1};
/**程序声明**/
int P4[]={3,4,20,26,-1};
/**类型声明**/
int P5[]={4,-1};
int P6[]={4,5,-1};
int P7[]={5,69,6,-1};
int P8[]={6,8,92,9,101,7,-1};
int P9[]={7,-1};
int P10[]={7,6,-1};
int P11[]={8,88,-1};
/**类型**/
int P12[]={9,10,-1};
int P13[]={9,11,-1};
int P14[]={9,88,-1};
int P15[]={10,86,-1};
int P16[]={10,87,-1};
int P17[]={11,12,-1};
int P18[]={11,15,-1};
int P19[]={12,82,103,13,105,14,104,83,10,-1};
int P20[]={13,89,-1};
int P21[]={14,89,-1};
int P22[]={15,84,16,79,-1};
int P23[]={16,10,18,101,17,-1};
int P24[]={16,12,18,101,17,-1};
int P25[]={17,-1};
int P26[]={17,16,-1};
int P27[]={18,88,19,-1};
int P28[]={19,-1};
int P29[]={19,102,18,-1};
/**变量声明**/
int P30[]={20,-1};
int P31[]={20,21,-1};
int P32[]={21,70,22,-1};
int P33[]={22,9,24,101,23,-1};
int P34[]={23,-1};
int P35[]={23,22,-1};
int P36[]={24,88,25,-1};
int P37[]={25,-1};
int P38[]={25,102,24,-1};
/**过程声明**/
int P39[]={26,-1};
int P40[]={26,27,-1};
int P41[]={27,68,29,98,30,99,101,36,37,28,-1};
int P42[]={28,-1};
int P43[]={28,27,-1};
int P44[]={29,88,-1};
/**参数声明**/
int P45[]={30,-1};
int P46[]={30,31,-1};
int P47[]={31,33,32,-1};
int P48[]={32,-1};
int P49[]={32,101,31,-1};
int P50[]={33,9,34,-1};
int P51[]={33,70,9,34,-1};
int P52[]={34,88,35,-1};
int P53[]={35,-1};
int P54[]={35,102,34,-1};
/**过程中的声明部分**/
int P55[]={36,3,-1};
/**过程体**/
int P56[]={37,38,-1};
/**主程序体**/
int P57[]={38,78,39,79,-1};
/**语句序列**/
int P58[]={39,41,40,-1};
int P59[]={40,-1};
int P60[]={40,101,39,-1};
/**语句**/
int P61[]={41,44,-1};
int P62[]={41,45,-1};
int P63[]={41,46,-1};
int P64[]={41,48,-1};
int P65[]={41,49,-1};
int P66[]={41,88,42,-1};
int P67[]={42,43,-1};
int P68[]={42,50,-1};
/**赋值语句**/
int P69[]={43,61,91,55,-1};
/**条件语句**/
int P70[]={44,71,53,72,39,73,39,74,-1};
/**循环语句**/
int P71[]={45,75,53,76,39,77,-1};
/**输入语句**/
int P72[]={46,80,98,47,99,-1};
int P73[]={47,88,-1};
/**输出语句**/
int P74[]={48,81,98,55,99,-1};
/**返回语句**/
int P75[]={49,85,-1};
/**过程调用语句**/
int P76[]={50,98,51,99,-1};
int P77[]={51,-1};
int P78[]={51,55,52,-1};
int P79[]={52,-1};
int P80[]={52,102,51,-1};
/**条件表达式**/
int P81[]={53,55,54,-1};
int P82[]={54,64,55,-1};
/**算式表达式**/
int P83[]={55,57,56,-1};
int P84[]={56,-1};
int P85[]={56,65,55,-1};
/**项**/
int P86[]={57,59,58,-1};
int P87[]={58,-1};
int P88[]={58,66,57,-1};
/**因子**/
int P89[]={59,98,55,99,-1};
int P90[]={59,89,-1};
int P91[]={59,60,-1};
int P92[]={60,88,61,-1};
int P93[]={61,-1};
int P94[]={61,103,55,104,-1};
int P95[]={61,100,62,-1};
int P96[]={62,88,63,-1};
int P97[]={63,-1};
int P98[]={63,103,55,104,-1};
int P99[]={64,93,-1};
int P100[]={64,92,-1};
int P101[]={65,94,-1};
int P102[]={65,95,-1};
int P103[]={66,96,-1};
int P104[]={66,97,-1};

//产生式总集，方便进行对应处理
int *P[]=
{
	0,P1,P2,P3,P4,P5,P6,P7,P8,P9,
	P10,P11,P12,P13,P14,P15,P16,P17,P18,P19,
	P20,P21,P22,P23,P24,P25,P26,P27,P28,P29,
	P30,P31,P32,P33,P34,P35,P36,P37,P38,P39,
	P40,P41,P42,P43,P44,P45,P46,P47,P48,P49,
	P50,P51,P52,P53,P54,P55,P56,P57,P58,P59,
	P60,P61,P62,P63,P64,P65,P66,P67,P68,P69,
	P70,P71,P72,P73,P74,P75,P76,P77,P78,P79,
	P80,P81,P82,P83,P84,P85,P86,P87,P88,P89,
	P90,P91,P92,P93,P94,P95,P96,P97,P98,P99,
	P100,P101,P102,P103,P104
};

int Predict1[]={67,-1};
int Predict2[]={67,-1};
int Predict3[]={88,-1};
int Predict4[]={69,70,68,78,-1};
int Predict5[]={70,68,78,-1};
int Predict6[]={69,-1};
int Predict7[]={69,-1};
int Predict8[]={88,-1};
int Predict9[]={70,68,78,-1};
int Predict10[]={88,-1};
int Predict11[]={88,-1};
int Predict12[]={86,87,-1};
int Predict13[]={82,84,-1};
int Predict14[]={88,-1};
int Predict15[]={86,-1};
int Predict16[]={87,-1};
int Predict17[]={82,-1};
int Predict18[]={84,-1};
int Predict19[]={82,-1};
int Predict20[]={89,-1};
int Predict21[]={89,-1};
int Predict22[]={84,-1};
int Predict23[]={86,87,-1};
int Predict24[]={82,-1};
int Predict25[]={79,-1};
int Predict26[]={86,87,82,-1};
int Predict27[]={88,-1};
int Predict28[]={101,-1};
int Predict29[]={102,-1};
int Predict30[]={68,78,-1};
int Predict31[]={70,-1};
int Predict32[]={70,-1};
int Predict33[]={86,87,82,84,88,-1};
int Predict34[]={68,78,-1};
int Predict35[]={86,87,82,84,88,-1};
int Predict36[]={88,-1};
int Predict37[]={101,-1};
int Predict38[]={102,-1};
int Predict39[]={78,-1};
int Predict40[]={68,-1};
int Predict41[]={68,-1};
int Predict42[]={78,-1};
int Predict43[]={68,-1};
int Predict44[]={88,-1};
int Predict45[]={99,-1};
int Predict46[]={86,87,82,84,88,70,-1};
int Predict47[]={86,87,82,84,88,70,-1};
int Predict48[]={99,-1};//注意这里  书中疑为有错，书中为"("
int Predict49[]={101,-1};
int Predict50[]={86,87,82,84,88,-1};
int Predict51[]={70,-1};
int Predict52[]={88,-1};
int Predict53[]={101,99,-1};
int Predict54[]={102,-1};
int Predict55[]={69,70,68,78,-1};
int Predict56[]={78,-1};
int Predict57[]={78,-1};
int Predict58[]={88,71,75,85,80,81,79,101,-1};//注意这里
int Predict59[]={73,74,79,77,-1};
int Predict60[]={101,-1};
int Predict61[]={71,-1};
int Predict62[]={75,-1};
int Predict63[]={80,-1};
int Predict64[]={81,-1};
int Predict65[]={85,-1};
int Predict66[]={88,-1};
int Predict67[]={103,100,91,-1};
int Predict68[]={98,-1};
int Predict69[]={103,100,91,-1};
int Predict70[]={71,-1};
int Predict71[]={75,-1};
int Predict72[]={80,-1};
int Predict73[]={88,-1};
int Predict74[]={81,-1};
int Predict75[]={85,-1};
int Predict76[]={98,-1};
int Predict77[]={99,-1};
int Predict78[]={98,89,88,-1};
int Predict79[]={99,-1};
int Predict80[]={102,-1};
int Predict81[]={98,89,88,-1};
int Predict82[]={93,92,-1};
int Predict83[]={98,89,88,-1};
int Predict84[]={93,92,104,72,73,74,76,77,99,79,101,102,-1};
int Predict85[]={94,95,-1};
int Predict86[]={98,89,88,-1};
int Predict87[]={94,95,93,92,104,72,73,74,76,77,99,79,101,102,-1};
int Predict88[]={96,97,-1};
int Predict89[]={98,-1};
int Predict90[]={89,-1};
int Predict91[]={88,-1};
int Predict92[]={88,-1};
int Predict93[]={91,96,97,94,95,93,92,104,72,73,74,76,77,99,79,101,102,-1};
int Predict94[]={103,-1};
int Predict95[]={100,-1};
int Predict96[]={88,-1};
int Predict97[]={91,96,97,94,95,93,92,104,72,73,74,76,77,99,79,101,102,-1};
int Predict98[]={103,-1};
int Predict99[]={93,-1};
int Predict100[]={92,-1};
int Predict101[]={94,-1};
int Predict102[]={95,-1};
int Predict103[]={96,-1};
int Predict104[]={97,-1};


int *Predicts[]=
{
	0,Predict1,Predict2,Predict3,Predict4,Predict5,Predict6,Predict7,Predict8,Predict9,
	Predict10,Predict11,Predict12,Predict13,Predict14,Predict15,Predict16,Predict17,Predict18,Predict19,
	Predict20,Predict21,Predict22,Predict23,Predict24,Predict25,Predict26,Predict27,Predict28,Predict29,
	Predict30,Predict31,Predict32,Predict33,Predict34,Predict35,Predict36,Predict37,Predict38,Predict39,
	Predict40,Predict41,Predict42,Predict43,Predict44,Predict45,Predict46,Predict47,Predict48,Predict49,
	Predict50,Predict51,Predict52,Predict53,Predict54,Predict55,Predict56,Predict57,Predict58,Predict59,
	Predict60,Predict61,Predict62,Predict63,Predict64,Predict65,Predict66,Predict67,Predict68,Predict69,
	Predict70,Predict71,Predict72,Predict73,Predict74,Predict75,Predict76,Predict77,Predict78,Predict79,
	Predict80,Predict81,Predict82,Predict83,Predict84,Predict85,Predict86,Predict87,Predict88,Predict89,
	Predict90,Predict91,Predict92,Predict93,Predict94,Predict95,Predict96,Predict97,Predict98,Predict99,
	Predict100,Predict101,Predict102,Predict103,Predict104
};


string SyntaxError[]=
{
		"程序头出错，缺少保留字:program!",
		/**程序头**/
		"程序头出错，缺少保留字:program!","程序头出错，程序名称应该为标识符!",
		/**程序声明**/
		"程序声明部分出错!","程序类型声明部分出错!",
		/**类型声明**/
		"类型声明出错!","类型声明列表出错!","类型声明列表出错!","无此类型标识符，类型ID错误!",
		/**类型***/
		"类型定义出错!","基本类型出错!","结构体类型出错!","数组类型出错!","数组下界应该为无符号整数!",
		"数组上界应该为无符号整数!","记录类型出错!",
		"域声明出错!","域声明列表出错!","应该为标识符!","应该为标识符!",
		/**变量声明**/
		"变量声明出错!","变量声明出错!","变量声明列表出错!","变量声明列表出错!",
		"变量标识符列表出错，应为标识符!","变量标识符列表出错!",
		/**过程声明**/
		"过程声明出错!","过程声明出错!","过程声明列表出错!","过程名称出错，应该标识符",
		/**参数声明**/
		"过程参数列表出错!","过程参数声明列表出错!","过程参数声明列表出错!",
		"过程参数列表出错!","过程参数变量列表出错，应该为标识符!","过程参数变量列表出错!",
		/**过程中的声明部分**/
		"过程的声明部分出错!",
		/**过程体**/
		"过程体出错!",
		/**主程序体**/
		"主程序体出错!",
		/**语句序列**/
		"语句序列出错!","语句序列出错!",
		/**语句**/
		"非法语句!","函数调用语句或者赋值语句出错!",
		/**赋值语句**/
		"赋值语句出错!",
		/**条件语句**/
		"条件语句出错!",
		/**循环语句**/
		"循环语句出错!",
		/**输入语句**/
		"输入语句出错!","输入变量应该为标识符!",
		/**输出语句**/
		"输出语句出错!",
		/**返回语句**/
		"返回语句出错!",
		/**过程调用语句**/
		"过程调用语句出错!","过程调用参数出错!","过程调用参数列表出错!",
		/**条件表达式**/
		"条件表达式出错!","比较运算符出错!",
		/**算术表达式**/
		"算术表达式出错!","算术表达式出错!",
		/**项**/
		"算术表达式项有误!","算术表达式项有误!",
		/**因子**/
		"因子出错，应为无符号整数或者变量标识符!","算术表达式变量出错,应为标识符!",
		"非法的算术因子!","域变量出错，应为标识符!","域变量出错，应为标识符!",
		"比较运算符出错!","加减运算符出错!","乘除运算符出错!"
};


#endif // PARSE_H_INCLUDED
