#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include"parse.h"
#include"recursive.h" 
#include<stack> 
#include<cstring>
#include<vector>


enum TypeKind	{intTy,charTy,boolTy,arrayTy,recordTy};//类型
enum IdKind		{varkind,typekind,prockind};//标识符
enum AccessKind	{dir,indir};//变量类型

#define INTSIZE  1
#define CHARSIZE 1
#define BOOLSIZE 1


//错误信息列表
#define SERROR_VAR_MULTIDEFINE		"变量%s重复定义!"
#define SERROR_TYPE_MULTIDEFINE		"类型%s重复定义!"
#define SERROR_PROCEDURE_MULTIDEFINE	"过程%s重复定义!"
#define SERROR_PARAMID_MULTIDEFINE	"参数标识符%s重复定义!"

#define SERROR_VARID_UNDEFINE		"变量标识符%s未定义!"
#define SERROR_PROCID_UNDEFINE		"过程标识符%s未定义!"
#define SERROR_TYPEID_UNDEFINE		"类型标识符%s未定义!"

#define SERROR_ARRAY_TOPLTLOW		"数组上界应该大于下界!"

#define SERROR_ISNOT_TYPEID			"%s非类型标识符!"
#define SERROR_ID_ISNOT_VARID		"标识符%s应为变量类型!"
#define SERROR_VAR_NOT_PROC			"%s并非过程标识符，而是%s类型!"
#define SERROR_VAR_NOT_FIELD_MEM	"变量%s非记录%s成员变量!"

#define SERROR_NOT_ENOUGH_PARAM		"过程调用实参数目不完整!"
#define SERROR_TOO_MUCH_PARAM		"过程调用实参数目过多!"


#define SERROR_TYPE_NOT_MATCH1		"类型应该为整型,而不应该是%s!"
#define SERROR_TYPE_NOT_MATCH2		"标识符%s类型应该为%s!"
#define SERROR_TYPE_NOT_MATCH3		"标识符%s数组元素类型应该为%s!"
#define SERROR_TYPE_NOT_MATCH4      "记录%s成员变量标识符%s类型并非数组类型!"


//类型
struct fieldChain;
typedef struct typeIR
{
	int size;
	TypeKind kind;
	union
	{
		struct//数组
		{
			struct typeIR* elemTy;
			int low,top;
		}ArrayAttr;
		fieldChain *body;//记录
	}More;
}TypeIR;
struct fieldChain
{
	string idname;
	struct typeIR* unitType; //域中变量的类型指针
	fieldChain* next;
};

//标识符
struct ParamTable;
struct AttributeIR
{
	struct typeIR* idtype;//类型标识符
	IdKind kind;
	int level;
	union
	{
        AccessKind access;//变量
        ParamTable* param;//过程
	}More;
};
struct ParamTable//参数列表
{
	typeIR* type;
	ParamTable *next;
};

//符号表
struct SymTableNode
{
	string name;
	AttributeIR attrIR;
	bool END; //是否为本层符号表的结束？如是为true
	SymTableNode* next;
};

class SymTable
{
    public:
        SymTableNode* front;//首项
        SymTableNode* rear; //末项
        void insertNode(SymTableNode* node);
		void insertMid(SymTableNode* pre,SymTableNode* next);
		SymTable():front(NULL),rear(NULL) {};
};

//scope栈
struct ScopeNode
{
	SymTableNode* front; //本层符号表的头指针
	ScopeNode* parent;	 //父层的指针
	ScopeNode():front(NULL),parent(NULL) {};
};

class Scope
{
    public:
        stack<ScopeNode*> scope;
        SymTableNode* FindID(string idname,bool ntype);
        //ntype==1时遍历所有层
        //ntype==0时仅遍历自己所在层
        SymTableNode* GetRear();//得到当前层的最末尾的符号表项
        void newLayer(SymTableNode* node);
        void dropLayer();
};

typedef struct snode
{
    int LineNum;
    int ErrorNum;
    string ErrorStr;
}SemErrorNode;


class Semantic
{
    private:
        SymTable symTable;
        Scope myScope;
        vector<SemErrorNode*>TokenSemError;     //语义分析错误
        int CntSemError;                        //语义分析错误数
	
	public:
		Recursive rec;
		ParseNode* parseTree;
		
    public:
        void initTy();
        void SemError(string str,int line);
        void OutSemError();
        void SemScanner();
        ParseNode* FindChild(ParseNode*node,string str);
        ParseNode* FindBrother(ParseNode*node,string str);

    private:
        void SemProgram(ParseNode* node);
        void SemDeclarePart(ParseNode* node,int layer);
        void SemTypeDecpart(ParseNode* node,int layer);
        void SemTypeDecList(ParseNode* node,int layer);
        void SemTypeDef(ParseNode* node ,typeIR*& tIR,int layer);
        void SemStructureType(ParseNode* node ,typeIR*& tIR,int layer);
        void SemArrayType(ParseNode* node ,typeIR*& tIR,int layer);
        void SemRecType(ParseNode* node ,typeIR*& tIR,int layer);
        void SemFieldDecList(ParseNode* node ,fieldChain*& body,int &size,int layer);
        void SemIdList(ParseNode* node,ParseNode*isMore,
                         fieldChain*& body,int layer,int &size,typeIR* tIR);
        void SemVarDecpart (ParseNode* node,int layer);
        void SemVarDecList(ParseNode* node,int layer);
        void SemVarIdList(ParseNode* node,int layer,typeIR* tIR);
		void SemProcDecpart(ParseNode* ptr,int layer);
		void SemProcDec(ParseNode* ptr,int layer,bool mid);
		void SemParamList(ParseNode* ptr,int layer,ParamTable* &param);
		void SemParamDecList(ParseNode* ptr,int layer,ParamTable* &param);
		void SemFormList(ParseNode* ptr,int layer,ParamTable* &param,
							typeIR* tIR,bool ntype);
		void SemProgramBody(ParseNode* node);
		void SemStmList(ParseNode* node);
		void SemConditionalStm(ParseNode* node);
		void SemLoopStm(ParseNode* ptr);
		void SemInputStm(ParseNode* ptr);
		void SemOutputStm(ParseNode* ptr);
		void SemReturnStm(ParseNode* ptr);
		void SemOtherStm(ParseNode* ptr);
		void SemAssignmentRest(ParseNode* ptr,SymTableNode* st);
		void SemCallStmRest(ParseNode* ptr,SymTableNode* st);
		void SemActParamList(ParseNode* ptr,ParamTable* param);
		void SemRelExp(ParseNode* node);
		void SemInvar(ParseNode* ptr);
		void SemExp(ParseNode* node,typeIR* &tIR,int &ntype);
		void SemVariMore(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR);
		void SemTerm(ParseNode* node,typeIR* &tIR,int &ntype);
		void SemOtherTerm(ParseNode* node,typeIR* &tIR,int &ntype);
		void SemVariable(ParseNode* node,typeIR* &tIR,int &ntype);
};
























#endif // SEMANTIC_H_INCLUDED
