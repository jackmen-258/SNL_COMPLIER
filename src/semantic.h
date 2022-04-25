#ifndef SEMANTIC_H_INCLUDED
#define SEMANTIC_H_INCLUDED

#include"parse.h"
#include"recursive.h" 
#include<stack> 
#include<cstring>
#include<vector>


enum TypeKind	{intTy,charTy,boolTy,arrayTy,recordTy};//����
enum IdKind		{varkind,typekind,prockind};//��ʶ��
enum AccessKind	{dir,indir};//��������

#define INTSIZE  1
#define CHARSIZE 1
#define BOOLSIZE 1


//������Ϣ�б�
#define SERROR_VAR_MULTIDEFINE		"����%s�ظ�����!"
#define SERROR_TYPE_MULTIDEFINE		"����%s�ظ�����!"
#define SERROR_PROCEDURE_MULTIDEFINE	"����%s�ظ�����!"
#define SERROR_PARAMID_MULTIDEFINE	"������ʶ��%s�ظ�����!"

#define SERROR_VARID_UNDEFINE		"������ʶ��%sδ����!"
#define SERROR_PROCID_UNDEFINE		"���̱�ʶ��%sδ����!"
#define SERROR_TYPEID_UNDEFINE		"���ͱ�ʶ��%sδ����!"

#define SERROR_ARRAY_TOPLTLOW		"�����Ͻ�Ӧ�ô����½�!"

#define SERROR_ISNOT_TYPEID			"%s�����ͱ�ʶ��!"
#define SERROR_ID_ISNOT_VARID		"��ʶ��%sӦΪ��������!"
#define SERROR_VAR_NOT_PROC			"%s���ǹ��̱�ʶ��������%s����!"
#define SERROR_VAR_NOT_FIELD_MEM	"����%s�Ǽ�¼%s��Ա����!"

#define SERROR_NOT_ENOUGH_PARAM		"���̵���ʵ����Ŀ������!"
#define SERROR_TOO_MUCH_PARAM		"���̵���ʵ����Ŀ����!"


#define SERROR_TYPE_NOT_MATCH1		"����Ӧ��Ϊ����,����Ӧ����%s!"
#define SERROR_TYPE_NOT_MATCH2		"��ʶ��%s����Ӧ��Ϊ%s!"
#define SERROR_TYPE_NOT_MATCH3		"��ʶ��%s����Ԫ������Ӧ��Ϊ%s!"
#define SERROR_TYPE_NOT_MATCH4      "��¼%s��Ա������ʶ��%s���Ͳ�����������!"


//����
struct fieldChain;
typedef struct typeIR
{
	int size;
	TypeKind kind;
	union
	{
		struct//����
		{
			struct typeIR* elemTy;
			int low,top;
		}ArrayAttr;
		fieldChain *body;//��¼
	}More;
}TypeIR;
struct fieldChain
{
	string idname;
	struct typeIR* unitType; //���б���������ָ��
	fieldChain* next;
};

//��ʶ��
struct ParamTable;
struct AttributeIR
{
	struct typeIR* idtype;//���ͱ�ʶ��
	IdKind kind;
	int level;
	union
	{
        AccessKind access;//����
        ParamTable* param;//����
	}More;
};
struct ParamTable//�����б�
{
	typeIR* type;
	ParamTable *next;
};

//���ű�
struct SymTableNode
{
	string name;
	AttributeIR attrIR;
	bool END; //�Ƿ�Ϊ������ű�Ľ���������Ϊtrue
	SymTableNode* next;
};

class SymTable
{
    public:
        SymTableNode* front;//����
        SymTableNode* rear; //ĩ��
        void insertNode(SymTableNode* node);
		void insertMid(SymTableNode* pre,SymTableNode* next);
		SymTable():front(NULL),rear(NULL) {};
};

//scopeջ
struct ScopeNode
{
	SymTableNode* front; //������ű��ͷָ��
	ScopeNode* parent;	 //�����ָ��
	ScopeNode():front(NULL),parent(NULL) {};
};

class Scope
{
    public:
        stack<ScopeNode*> scope;
        SymTableNode* FindID(string idname,bool ntype);
        //ntype==1ʱ�������в�
        //ntype==0ʱ�������Լ����ڲ�
        SymTableNode* GetRear();//�õ���ǰ�����ĩβ�ķ��ű���
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
        vector<SemErrorNode*>TokenSemError;     //�����������
        int CntSemError;                        //�������������
	
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
