#include "semantic.h"
#include<string.h>

//�ַ�������
string TypeKindStr[]={"intTy","charTy","boolTy","arrayTy","recordTy"};
string AccessKindStr[]={"dir","indir"};
string IdKindStr[]={"varkind","typekind","prockind"};

//����ȫ�ֱ����������������
typeIR* IntTy,*CharTy,*BoolTy;

void Semantic::OutSemError()
{
    FILE *file=fopen("C:\\Users\\HP\\Desktop\\design\\ComplierDesign\\SNL_COMPLIER\\output\\sem_error.txt","w");
    vector<SemErrorNode*>::iterator it;
    for(it=TokenSemError.begin(); it!=TokenSemError.end();it++)
    {
        fprintf(file,"error%d in line%d : %s\n",(*it)->ErrorNum,(*it)->LineNum,(*it)->ErrorStr.c_str());
    }
    fclose(file);
}

void SymTable::insertNode(SymTableNode* node)//����һ�����ű���
{
	if(node==NULL)
		return ;
	if(node->attrIR.kind!=prockind) 
		node->END=0;//ֻ������һ�㿪ʼʱ���Ž�����Ϊ����,�����̳���
	node->next=0;
	if(this->front==NULL)
	{
		rear=node;
		front=rear;
	}
	else
	{
		rear->next=node;
		rear=rear->next;
	}

}

void SymTable::insertMid(SymTableNode* pre,SymTableNode* next)
//�Բ��������Ĺ��̽������⴦��
{
	if(pre->next==NULL)
		pre->next=next;
	else
	{
		SymTableNode* p=pre->next;
		pre->next=next;//�ض�λ
		next->next=p;  //�ض�λ
	}
}

//�ӷ��ű�ջ�в����Ѿ������ı�ʶ��
SymTableNode* Scope::FindID(string idname,bool ntype)
{
	if(scope.empty())
		return NULL;
	ScopeNode* node=this->scope.top();
	while(node)
	{
		SymTableNode* t=node->front;
		while(t)//�ӵ�ǰ���ң����������ű�β����ѭ��
		{
			if(t->name==idname)
				return t;
			if(t->END)//���ﱾ��ĩβ
				break;
			t=t->next;
		}
		if(ntype)
			node=node->parent;//����һ��
		else
			break;
	}
	return NULL;
}
//�õ�������ű��β��
SymTableNode* Scope::GetRear()
{
	SymTableNode* p=this->scope.top()->front;
	while(p)
	{
		if(p->END||p->next==NULL)
			return p;
		p=p->next;
	}
	return 0;
}

void Scope::newLayer(SymTableNode* node)
{
	ScopeNode* t=new ScopeNode;
	t->front=node;
	if(scope.empty())//�ҵ�����ָ��
		t->parent=NULL;
	else
		t->parent=scope.top();
	scope.push(t);

}
void Scope::dropLayer()
{
	if(scope.empty())
		return ;
	ScopeNode* t=scope.top();
	scope.pop();
	delete t;
	return ;
}

void Semantic::initTy()
{
	IntTy=new typeIR;
	CharTy=new typeIR;
	BoolTy=new typeIR;


	IntTy->kind=intTy;
	IntTy->size=INTSIZE;

	CharTy->kind=charTy;
	CharTy->size=CHARSIZE;

	BoolTy->kind=boolTy;
	BoolTy->size=BOOLSIZE;
}

void Semantic::SemError(string str,int line)
{
	CntSemError++;
	SemErrorNode* node=new SemErrorNode;
	node->LineNum=line;
	node->ErrorNum=CntSemError;
	node->ErrorStr=str;
	TokenSemError.push_back(node);
}

ParseNode* Semantic::FindChild(ParseNode*node,string str)
{
    if(!node) return NULL;
    ParseNode* p=node->child;
    while(p)
    {
        if(p->ParseStr==str)
            return p;
        p=p->child;
    }
    return NULL;
}
ParseNode* Semantic::FindBrother(ParseNode*node,string str)
{
    if(!node) return NULL;
    ParseNode* p=node->brother;
    while(p)
    {
        if(p->ParseStr==str)
            return p;
        p=p->brother;
    }
    return NULL;
}


void Semantic::SemScanner()//�����������
{
	if(this->parseTree==NULL)
    {
        cout<<"�﷨��Ϊ�գ��޷������������"<<endl;
        return;
    }
	initTy();
	this->myScope.newLayer(0);
	CntSemError=0;
	SemProgram(parseTree);
	return ;
}

void Semantic::SemProgram(ParseNode* node)//�ܳ���
{
	ParseNode* p;
	if((p=FindBrother(node->child,"DeclarePart")))//����
	{
		SemDeclarePart(p,0);
	}

	if((p=FindBrother(node->child,"ProgramBody")))//������
	{
		SemProgramBody(p);
	}


}

void Semantic::SemDeclarePart(ParseNode* node,int layer)//��������
{
	ParseNode* p=node->child;
	if(p->ParseStr=="TypeDecpart")//��������
		SemTypeDecpart(p,layer);

	if((p=FindBrother(p,"VarDecpart")))//��������
        SemVarDecpart(p,layer);

    if((p=FindBrother(p,"ProcDecpart")))//��������
    {
		SemProcDecpart(p,layer);
    }
}

/**************************��������***********************/
void Semantic::SemTypeDecpart(ParseNode* node,int layer)//��������
{
	ParseNode* p=node->child;
	if(p->Symbol==-1)//����������
		return ;
	if((p=FindBrother(p->child,"TypeDecList")))//����������
		SemTypeDecList(p,layer);
}

void Semantic::SemTypeDecList(ParseNode* node,int layer)//���������б�
{
	ParseNode* p=node->child;
	if(p->ParseStr=="TypeId")
	{
		if(myScope.FindID(p->child->ParseStr,0)!=NULL) //�������Ƿ��Ѿ�����
		{
			char str[256];
			sprintf(str,SERROR_TYPE_MULTIDEFINE,p->child->ParseStr.c_str());
			string s(str);
			SemError(s,p->child->LineNum);
		}
		else
		{
			SymTableNode* s=new SymTableNode;
			s->name=p->child->ParseStr;
			s->attrIR.kind=typekind;
			s->attrIR.level=layer;
			s->next=NULL;

			if((p=FindBrother(p,"TypeDef")))
				SemTypeDef(p,s->attrIR.idtype,layer);
			if(s->attrIR.idtype)//��������������Ϣ
			{
				if(myScope.scope.top()->front==NULL)
					myScope.scope.top()->front=s;//Ϊ����ĳ�ʼ���ű���
				
				this->symTable.insertNode(s);
			}
		}	
	}
	ParseNode* q;
	if((q=FindBrother(node->child,"TypeDecMore")))//�Ƿ��и������������
		if(q->child->ParseStr=="TypeDecList")
			SemTypeDecList(q->child,layer);

}

void Semantic::SemTypeDef(ParseNode* node ,typeIR*& tIR,int layer)//������������
{
    ParseNode* p=node->child;
	if(p==NULL)
	{
		tIR=NULL;
		return ;
	}
	if(p->ParseStr=="BaseType")//��������
	{
		if(p->child->ParseStr=="integer")
			tIR=IntTy;
		else if(p->child->ParseStr=="char")
			tIR=CharTy;
	}
	else if(p->ParseStr=="StructureType")//�Զ�������
		SemStructureType(p,tIR,layer);
	else if(p->Symbol==88)//���ͱ�ʶ��
	{
		SymTableNode* s=myScope.FindID(p->ParseStr,1);//�������ͱ�ʶ��
		if(s==NULL)//�Ҳ���
		{
			char str[256];
			sprintf(str,SERROR_TYPEID_UNDEFINE,p->ParseStr.c_str());
			string t(str);
			SemError(t,node->LineNum);
			tIR=NULL;
		}
		else//�ҵ���
		{
			if(s->attrIR.kind!=typekind)
			{
				char str[256];
				sprintf(str,SERROR_ISNOT_TYPEID,p->ParseStr.c_str());
				string t(str);
				SemError(t,node->LineNum);
				tIR=NULL;
			}
			else
				tIR=s->attrIR.idtype;
		}
	}
}
void Semantic::SemStructureType(ParseNode* node ,typeIR*& tIR,int layer)//�Զ�����������
{
	tIR=new typeIR;//�ǻ������ͣ�������һ���ռ�
	ParseNode* p=node->child;

	if(p->ParseStr=="ArrayType")//����
		SemArrayType(p,tIR,layer);

	else if(p->ParseStr=="RecType")//��¼
		SemRecType(p,tIR,layer);

	else
	{
		delete tIR;
		tIR=NULL;
	}
}

void Semantic::SemArrayType(ParseNode* node ,typeIR*& tIR,int layer)//��������
{
	tIR->kind=arrayTy; //Ϊ��������
	ParseNode* p=FindBrother(node->child,"Low");
	if(p)//�½� Low
		tIR->More.ArrayAttr.low=atoi(p->child->ParseStr.c_str());
	if((p=FindBrother(p,"Top")))//�Ͻ� Top
	    tIR->More.ArrayAttr.top=atoi(p->child->ParseStr.c_str());

	p=p->brother->brother->brother;
	if(p->child->ParseStr=="integer")//Ԫ������
        tIR->More.ArrayAttr.elemTy=IntTy;
    else tIR->More.ArrayAttr.elemTy=CharTy;

	if(tIR->More.ArrayAttr.top<=tIR->More.ArrayAttr.low)//�ж��Ƿ��������½����
	{
		SemError(SERROR_ARRAY_TOPLTLOW,node->LineNum);
		return;
	}

	tIR->size=(tIR->More.ArrayAttr.top-tIR->More.ArrayAttr.low+1)//���ʹ�С
		*(tIR->More.ArrayAttr.elemTy->size);
}

void Semantic::SemRecType(ParseNode* node ,typeIR*& tIR,int layer)  //��¼����
{
	tIR->kind=recordTy;
	ParseNode* p=node->child;
	if(p->brother->ParseStr=="FieldDecList")
		SemFieldDecList(p->brother,tIR->More.body,tIR->size,layer);

}
void Semantic::SemFieldDecList(ParseNode* node ,
                               fieldChain*& body,int &size,int layer)//�õ���Ա����
{
	ParseNode* p=node->child;

	if(p->ParseStr=="BaseType")//��������
	{
	    ParseNode*t=FindBrother(p,"FieldDecMore");
		if(p->child->ParseStr=="integer")
			SemIdList(p->brother,t,body,layer,size,IntTy);
			//t����FieldDecMore�ж�
		else if(p->child->ParseStr=="char")
			SemIdList(p->brother,t,body,layer,size,CharTy);
	}

	else if(p->ParseStr=="ArrayType")//��������
	{
		typeIR* tIR=new typeIR;
		SemArrayType(p,tIR,layer);//���γ�����������Ϣ
		ParseNode*t=FindBrother(p,"FieldDecMore");
		SemIdList(p->brother,t,body,layer,size,tIR);
	}
}

void Semantic::SemIdList(ParseNode* node,ParseNode*isMore,
                         fieldChain*& body,int layer,int &size,typeIR* tIR)
{
	ParseNode* p=node->child;

	body=new fieldChain;
	size=0;
	body->idname=p->ParseStr;
	body->unitType=tIR;
	body->next=NULL;
	size+=tIR->size;

	if(p->brother->child->Symbol!=-1)//��������ͬ���ͱ���
    {
        p=p->brother->child->brother;
        if(p->ParseStr=="IdList");
        SemIdList(p,isMore,body->next,layer,size,tIR);
    }
	else if(isMore&&isMore->child->ParseStr=="FieldDecList")//�м�¼�ı��������б�
		SemFieldDecList(isMore->child,body->next,size,layer);
}

/**************************��������***********************/
void Semantic::SemVarDecpart (ParseNode* node,int layer)//��������
{
	ParseNode* p=node->child;
	if(p->Symbol==-1)//�ޱ�������
		return;
	if((p=FindBrother(p->child,"VarDecList")))//�б�������
		SemVarDecList(p,layer);
}

void Semantic::SemVarDecList(ParseNode* node,int layer)//���������б�
{
	ParseNode* p;
	if((p=FindChild(node,"TypeDef")))//����
	{
		typeIR* tIR;
		SemTypeDef(p,tIR,layer);
		if(tIR)
			SemVarIdList(p->brother,layer,tIR);
	}
	if((p=FindBrother(node->child,"VarDecMore")))//�Ƿ��и���ı�������
		if((p=FindChild(p,"VarDecList")))
			SemVarDecList(p,layer);

}
void Semantic::SemVarIdList(ParseNode* node,int layer,typeIR* tIR)
{
	SymTableNode* p=new SymTableNode;
	p->name=node->child->ParseStr;
	p->next=NULL;
	p->attrIR.kind=varkind;
	p->attrIR.idtype=tIR;
	p->attrIR.level=layer;
	p->attrIR.More.access=dir;

	if(myScope.FindID(p->name,0)!=NULL) //�������Ƿ��Ѿ�����
	{
		char str[256];
		sprintf(str,SERROR_VAR_MULTIDEFINE,p->name.c_str());
		string s(str);
		SemError(s,node->LineNum);
	}
	else
	{
		if(p->attrIR.idtype)//��������γ�δ����
		{
			if(myScope.scope.top()->front==NULL)
				myScope.scope.top()->front=p;//Ϊ����ĳ�ʼ���ű���

			this->symTable.insertNode(p);
		}
		else
			return ;//�����򷵻أ�������Ϣ�Ѿ���SemTypeDef()�����ɣ��ʲ����ظ�
	}
	ParseNode* q;
	if((q=FindBrother(node->child,"VarIdMore")))//�Ƿ��и���ı�������
		if((q=FindBrother(q->child,"VarIdList")))
			SemVarIdList(q,layer,tIR);
}

/**************************��������***********************/

void Semantic::SemProcDecpart(ParseNode* ptr,int layer)//������������
{	
	ParseNode* p;
	if((p=FindChild(ptr,"ProcDec")))//�Ƿ���ڹ�������
	{
		SemProcDec(p,layer+1,0);
		if((p=FindBrother(p->child,"ProcDecMore")))//�Ƿ��и���Ĺ�������
			if((p=FindChild(p,"ProcDec")))
				SemProcDec(p,layer+1,1);
	}
}

void Semantic::SemProcDec(ParseNode* ptr,int layer,bool mid)
{
	ParseNode* p;
	SymTableNode* st=new SymTableNode;//�½����ű���
	
	p=ptr->child->brother->child;

	if(myScope.FindID(p->ParseStr,1)!=NULL) //�������Ƿ��Ѿ�����
	{
		char str[256];
		sprintf(str,SERROR_PROCEDURE_MULTIDEFINE,p->ParseStr.c_str());
		string s(str);
		SemError(s,p->LineNum);
	}
	
	st->name=p->ParseStr;
	st->next=0;
	st->attrIR.idtype=0;//�޷���ֵ 
	st->attrIR.kind=prockind;
	st->attrIR.level=layer;
	st->attrIR.More.param=0;//�����б��ʼ��Ϊ��
	st->END=true;
	
	if(myScope.scope.top()->front==NULL)
		myScope.scope.top()->front=st;//Ϊ����ĳ�ʼ���ű���

	if(!mid)
		symTable.insertNode(st);//���뵽���ű���
	else
	{
		SymTableNode* s0=myScope.GetRear();//���й���
		s0->END=0;
		symTable.insertMid(s0,st);
	}
	
	myScope.newLayer(0);
	
	p=ptr->child->brother->brother->brother;
	SemParamList(p,layer,st->attrIR.More.param);
	
	p=p->brother->brother->brother;
	SemDeclarePart(p->child,layer);
	
	SemProgramBody(p->brother->child);
	
	myScope.dropLayer();
}

void Semantic::SemParamList(ParseNode* ptr,int layer,ParamTable* &param)//�����б�
{
	ParseNode* p;
	if((p=FindChild(ptr,"ParamDecList")))//�Ƿ���ڲ����б�
		SemParamDecList(p,layer,param);
}

void Semantic::SemParamDecList(ParseNode* ptr,int layer,ParamTable* &param)//�γɷ��ű�
{
	ParseNode* p;
	if(ptr->child&&ptr->child->ParseStr=="Param")//�в���(�����޲�)
	{
		typeIR* tIR=0;
		if((p=FindChild(ptr->child,"TypeDef")))
		{
			SemTypeDef(p,tIR,layer);
			if(tIR&&(p=FindBrother(p,"FormList")))
				SemFormList(p,layer,param,tIR,0);
		}
		else if((p=FindChild(ptr->child,"var")))
		{
			SemTypeDef(p->brother,tIR,layer);
			if(tIR&&(p=FindBrother(p->brother,"FormList")))
				SemFormList(p,layer,param,tIR,1);
		}
	}
	
	if((p=FindBrother(ptr->child,"ParamMore")))//�Ƿ��и����������͵Ĳ�����
		if((p=FindBrother(p->child,"ParamDecList")))
		{
			if(param)//param����һ�η����б��޸�
			{
				ParamTable* pr=param;
				while(pr->next) 
					pr=pr->next;//�ҵ���ĩ�� 
				SemParamDecList(p,layer,pr->next);
			}
			else 
				SemParamDecList(p,layer,param);
		}
}

void Semantic::SemFormList(ParseNode* ptr,int layer,ParamTable* &param,typeIR* tIR,bool ntype)//�β���
{
	ParseNode* p;
	SymTableNode* st=new SymTableNode;
	
	st->name=ptr->child->ParseStr;
	st->next=0;
	st->attrIR.idtype=tIR;
	st->attrIR.kind=varkind;
	st->attrIR.More.access=AccessKind(ntype);
	st->attrIR.level=layer;
	
	if(myScope.FindID(st->name,0))//�Ƿ�����Ѿ�������
	{
		char str[256];
		sprintf(str,SERROR_PARAMID_MULTIDEFINE,st->name.c_str());
		string s(str);
		SemError(s,ptr->child->LineNum);
		delete st;
		
		ParseNode* h=ptr->child->brother->child;
		if(h->Symbol!=-1&&h->brother->ParseStr=="FormList")
			SemFormList(p,layer,param,tIR,ntype);
	}
	else
	{
		symTable.insertNode(st);
		
		param=new ParamTable;//�½���һ���βν��
		param->type=tIR;
		param->next=0;
		
		if(myScope.scope.top()->front==NULL)//�Ƿ�Ϊ�����׷��ű���
			myScope.scope.top()->front=st;

		p =	ptr->child->brother->child;
		if(p->Symbol!=-1&&p->brother->ParseStr=="FormList")
			SemFormList(p->brother,layer,param->next,tIR,ntype);
	}
}

/**************************������***********************/

void Semantic::SemProgramBody(ParseNode* node)//�����岿��
{
	ParseNode* p=node->child;
	if(p->brother->ParseStr=="StmList")//�õ���������﷨�����
		SemStmList(p->brother);
}

void Semantic::SemStmList(ParseNode* node)//������е��������
{
	ParseNode* p=node->child->child;
	if(p->ParseStr=="ConditionalStm")//�������
		SemConditionalStm(p);
	else if(p->ParseStr=="LoopStm")//ѭ�����
		SemLoopStm(p);
	else if(p->ParseStr=="InputStm")//�������
		SemInputStm(p);
	else if(p->ParseStr=="OutputStm")//������
		SemOutputStm(p);
	else if(p->ParseStr=="ReturnStm")//�������
		SemReturnStm(p);
	else if(p->Symbol==88)//������䣺�������ã���ֵ���ķ���
		SemOtherStm(p);
	if((p=FindBrother(node->child,"StmMore")))//�Ƿ��и����������У�
		if((p=FindBrother(p->child,"StmList")))//���Ǽ������з���
			SemStmList(p);
}

void Semantic::SemConditionalStm(ParseNode* node)//�������
{
	ParseNode* p=node->child;
	if((p=FindBrother(p,"RelExp")))
		SemRelExp(p);
	if((p=FindBrother(p,"StmList")))//then
	{
	    SemStmList(p);
		if((p=FindBrother(p,"StmList")))//else
			SemStmList(p);
	}

}

void Semantic::SemLoopStm(ParseNode* ptr)//ѭ�����
{
	ParseNode* p;
	if((p=FindBrother(ptr->child,"RelExp")))//�����ж�
		SemRelExp(p);
	if((p=FindBrother(ptr->child,"StmList")))//do�������
		SemStmList(p);
}

void Semantic::SemInputStm(ParseNode* ptr)//�������
{
	ParseNode* p;
	if((p=FindBrother(ptr->child,"Invar")))
		SemInvar(p);
}

void Semantic::SemOutputStm(ParseNode* ptr)//������
{
	ParseNode* p;
	typeIR* tIR=0;
	int ntype=0;
	if((p=FindBrother(ptr->child,"Exp")))
		SemExp(p,tIR,ntype);
}

void Semantic::SemReturnStm(ParseNode* ptr)//�������
{
	
}

void Semantic::SemOtherStm(ParseNode* ptr)//��ֵ��估���̵������
{
	ParseNode* p;
	SymTableNode* st=myScope.FindID(ptr->ParseStr,1);//�ҵ���ʶ���ķ��ű�λ��
	
	if(st)
	{
		if((p=FindChild(ptr->brother,"AssignmentRest")))//��ֵ���
			SemAssignmentRest(p,st);
		else if((p=FindChild(ptr->brother,"CallStmRest")))//���̵������
			SemCallStmRest(p,st);
	}
	else //δ�ұ�ʶ������
	{	
		char str[256];
		
		if((p=FindChild(ptr->brother,"AssignmentRest")))//������ʶ�� 
			sprintf(str,SERROR_VARID_UNDEFINE,ptr->ParseStr.c_str());
		else //���̱�ʶ��
			sprintf(str,SERROR_PROCID_UNDEFINE,ptr->ParseStr.c_str());
		string s(str);	
		SemError(s,ptr->LineNum);
	}
}

void Semantic::SemAssignmentRest(ParseNode* ptr,SymTableNode* st)//��ֵ���
{
	ParseNode* p;

	typeIR* qIR,*tIR;
	if(st->attrIR.idtype->kind==arrayTy)
		qIR=st->attrIR.idtype->More.ArrayAttr.elemTy;
	else 
		qIR=st->attrIR.idtype;

	int ntype=1;
	
	if((p=FindChild(ptr,"VariMore")))
		SemVariMore(p,st,tIR);
	if((p=FindBrother(ptr->child,"Exp")))
		SemExp(p,qIR,ntype);
}

void Semantic::SemCallStmRest(ParseNode* ptr,SymTableNode* st)//���̵������
{
	ParseNode* p;
	
	if((p=FindBrother(ptr->child,"ActParamList")))
	{	
		if(st->attrIR.kind==prockind)
			SemActParamList(p,st->attrIR.More.param);
		else
		{
			char str[256];
			sprintf(str,SERROR_VAR_NOT_PROC,st->name.c_str(),IdKindStr[st->attrIR.kind].c_str());
			string s(str);
			SemError(s,p->LineNum);
		}
	}
}

void Semantic::SemActParamList(ParseNode* ptr,ParamTable* param)//�β�ʵ�ν�� 
{
	ParseNode* p;
	int ntype=1;
	
	if((p=FindChild(ptr,"Exp")))
	{
		if(param==NULL)//�޲� 
		{
			SemError(SERROR_TOO_MUCH_PARAM,ptr->LineNum);
			return;
		}
		
		SemExp(p,param->type,ntype);
		
		if((p=FindBrother(p,"ActParamMore")))
		{
			if((p=FindBrother(p->child,"ActParamList")))
				SemActParamList(p,param->next);
			else if(param->next)//�����β�
				SemError(SERROR_NOT_ENOUGH_PARAM,ptr->LineNum);
		}
	}
	else if(param)//�Ҳ���exp
		SemError(SERROR_NOT_ENOUGH_PARAM,ptr->LineNum);
}

void Semantic::SemRelExp(ParseNode* node)//�������ʽ
{
	typeIR* tIR=NULL;
	ParseNode* p=node->child;
	int ntype=0;
	if(p->ParseStr=="Exp")
	{
		SemExp(p,tIR,ntype);//��һ�����ʽ
		p=p->brother->child->brother;
		if(p->ParseStr=="Exp")
			SemExp(p,tIR,ntype=1);//�������ʽ
	}
}

void Semantic::SemInvar(ParseNode* ptr)
{
	if(ptr->child&&ptr->child->Symbol==88)
	{
		SymTableNode* sym=myScope.FindID(ptr->child->ParseStr,1);
		if(sym==NULL)//δ�ҵ�
		{
			char str[256];
			sprintf(str,SERROR_VARID_UNDEFINE,ptr->child->ParseStr.c_str());
			string s(str);
			SemError(s,ptr->child->LineNum);
		}
		else if(sym->attrIR.kind!=varkind)//���Ǳ���
		{
			char str[256];
			sprintf(str,SERROR_ID_ISNOT_VARID,ptr->child->ParseStr.c_str());
			string s(str);
			SemError(s,ptr->child->LineNum);
		}
	}
}

void Semantic::SemExp(ParseNode* node,typeIR* &tIR,int &ntype)
	//��ntype==0ʱ����һ�����ʽ�ĵ�һ��
	//��ntype==1ʱ�����������
	//��ntype==2ʱ�������±�
{
	ParseNode* p=node->child;
	if(p->ParseStr=="Term")//�������ʽ�ĵ�һ��
		SemTerm(p,tIR,ntype);
	if(p->brother->ParseStr=="OtherTerm")//�������ʽ��������
		SemOtherTerm(p->brother,tIR,ntype);
}

void Semantic::SemVariMore(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR)//��¼��Ա������������ 
{
	ParseNode* p;
	int ntype=2;
	
	if((p=FindBrother(ptr->child,"Exp")))//��������
	{
		tIR=IntTy;
		SemExp(p,tIR,ntype);
	}
	else if((p=FindBrother(ptr->child,"FieldVar")))//��¼��Ա 
	{
		if(p->child&&p->child->Symbol==88)
		{
			if(sym->attrIR.idtype->kind!=recordTy) 
			{
				char str[256];
				sprintf(str,SERROR_TYPE_NOT_MATCH2,sym->name.c_str(),"recordTy");
				string s(str);
				SemError(s,p->child->LineNum);
				return;
			}

			fieldChain* body=sym->attrIR.idtype->More.body;
			while(body)//�ڼ�¼�����ң��Ƿ�����˳�Ա����
			{
				if(body->idname==p->child->ParseStr)
					break;
				body=body->next;
			}
			if(body)//�ҵ�
			{
				if((p=FindBrother(p->child,"FieldVarMore")))
					if((p=FindBrother(p->child,"Exp")))
					{
						tIR=IntTy;
						SemExp(p,tIR,ntype);
					}
			}
			else //δ�ҵ����γɳ�����Ϣ
			{
				char str[256];
				sprintf(str,SERROR_VAR_NOT_FIELD_MEM,p->child->ParseStr.c_str(),sym->name.c_str());
				string s(str);
				SemError(s,p->child->LineNum);
			}
		}
	}
}

void Semantic::SemTerm(ParseNode* node,typeIR* &tIR,int &ntype)//�������ʽ��һ��
{
	ParseNode* p=node->child;
	if(p->ParseStr=="Factor")//����
	{
	    ParseNode*q;
		if((q=FindBrother(p->child,"Exp")))//���ʽ
			SemExp(q,tIR,ntype);
		else if(p->child->ParseStr=="Variable")//��Ϊ����Ϊ��¼ʱ
			SemVariable(p->child,tIR,ntype);
		else if(tIR&&p->child->Symbol==89){
			if(tIR!=IntTy){
				if(tIR==NULL)
					return;
				char str[256];
				sprintf(str,SERROR_TYPE_NOT_MATCH1,TypeKindStr[tIR->kind].c_str());
				string t(str);
				SemError(t,p->child->LineNum);
			}
		}
        ParseNode* t=p->brother;
		if(t->ParseStr=="OtherFactor")//��������
			if(t->child->Symbol!=-1)
				SemTerm(t->child->brother,tIR,ntype);
	}
}
void Semantic::SemOtherTerm(ParseNode* node,typeIR* &tIR,int &ntype)//�������ʽ�е�������
{
	ParseNode* p=node->child;
	if(p->Symbol!=-1&&p->brother->ParseStr=="Exp")
		SemExp(p->brother,tIR,ntype);
}

void Semantic::SemVariable(ParseNode* node,typeIR* &tIR,int &ntype)//����
{
	ParseNode* p=node->child,*q,*t;
	int type=2;
	
	if(p->Symbol==88)//��ʶ��
	{
		SymTableNode* sym=myScope.FindID(p->ParseStr,1);//�ڱ������������
		if(sym)//�ҵ�
		{

			if(sym->attrIR.kind!=varkind)//���Ǳ�������
			{
				char str[256];
				sprintf(str,SERROR_ID_ISNOT_VARID,p->ParseStr.c_str());
				string s(str);
				SemError(s,p->LineNum);
		
			}
			else
			{
				if(p->brother->child->Symbol!=-1)//�������VaiMore
				{
					if((q=FindBrother(p->brother->child,"Exp")))
					{
						if(sym->attrIR.idtype->kind!=arrayTy)//����ʶ��������������
						{
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),"arrayTy");
							string s(str);
							SemError(s,p->LineNum);
							return;
						}
						if(ntype && sym->attrIR.idtype->More.ArrayAttr.elemTy!=tIR)//���β�ƥ��
						{
							if(tIR==NULL)
								return;
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH3,p->ParseStr.c_str(),TypeKindStr[tIR->kind].c_str());
							string s(str);
							SemError(s,p->LineNum);
							return;
						}
						else//�������±��������ʽ�����������
						{
							typeIR* tIR0=IntTy;
							SemExp(q,tIR0,type);//q=exp  ��q����������������
							if(ntype==0)//����ǵ�һ��
							{
								ntype=1;
								tIR=sym->attrIR.idtype->More.ArrayAttr.elemTy;//�ݴ����ͣ����ں����Ƚ�
							}
						}
					}
					else if((q=FindBrother(p->brother->child,"FieldVar")))
					{
						if(sym->attrIR.idtype->kind!=recordTy)//����ʶ�����Ǽ�¼����
						{
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),"recordTy");
							string s(str);
							SemError(s,p->LineNum);
							return;
						}
						if(q->child->Symbol==88)
						{
							q=q->child;
							string name=q->ParseStr;
							fieldChain* body=sym->attrIR.idtype->More.body;
							while(body)
							{
								if(body->idname==name)
									break;
								body=body->next;
							}
							if(body==NULL)
							{
								char str[256];
								sprintf(str,SERROR_VAR_NOT_FIELD_MEM,q->ParseStr.c_str(),sym->name.c_str());
								string s(str);
								SemError(s,q->LineNum);
								return;
							}
							else
							{
								if(q->brother->child->Symbol!=-1)
								{//------------------------------------------Ϊ��������
									if(body->unitType->kind!=arrayTy)//�жϼ�¼�˳�Ա�����Ƿ�Ϊ��������
									{
										char str[256];
										sprintf(str,SERROR_TYPE_NOT_MATCH4,p->ParseStr.c_str(),body->idname.c_str());
										string s(str);
										SemError(s,q->LineNum);
										
										return;
									}
									if(ntype&& body->unitType->More.ArrayAttr.elemTy!=tIR)//���Ͳ�ƥ��
									{
										if(tIR==NULL)
											return;
										char str[256];
										sprintf(str,SERROR_TYPE_NOT_MATCH3,body->idname.c_str(),TypeKindStr[tIR->kind].c_str());
										string s(str);
										SemError(s,q->LineNum);
										return;
									}
									else
									{
										typeIR* tIR0=IntTy;
										SemExp(t,tIR0,type);//q=exp  ��q����������������
										if(ntype==0)
										{
											ntype=1;
											tIR=body->unitType->More.ArrayAttr.elemTy;//�޸�Ҫƥ�������
										}
									}
								}
								else
								{//---------------------------------Ϊ��Ա��������
									if(ntype&& body->unitType!=tIR)//ƥ�䲻�ɹ�
									{
										if(tIR==NULL)
											return;
										char str[256];
										sprintf(str,SERROR_TYPE_NOT_MATCH2,q->ParseStr.c_str(),TypeKindStr[tIR->kind].c_str());
										string s(str);
										SemError(s,q->LineNum);
										return;
									}
								}
							}
						}

					}/////////////////////////////////////////////////////////////////////////////��¼����
				}///////////////////////////////////////////////////////////////////////////////VariMore����
				else
				{//---------------------------------------------------------------������Variable:=ID
					if(ntype==0)//���������
					{
						tIR=sym->attrIR.idtype;
						ntype=1;
					}
					else if(ntype==1)//�����1�����������������
					{
						if(sym->attrIR.idtype!=tIR)//�������
						{
							if(tIR==NULL)
								return;
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),TypeKindStr[tIR->kind].c_str());
							string s(str);
							SemError(s,p->LineNum);
						}
					}
					else if(ntype==2&&sym->attrIR.idtype!=IntTy)//����Ƿ�Ϊ�����±�����
					{
						char str[256];
						sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),"IntTy");
						string s(str);
						SemError(s,p->LineNum);
					}
				}
			}
		}
		else //�ֲ������ű���δ�ҵ��˱�ʶ��
		{
			char str[256];
			sprintf(str,SERROR_VARID_UNDEFINE,p->ParseStr.c_str());
			string s(str);
			SemError(s,p->LineNum);
			return ;
		}
	}
}

int main()
{
	GetTokenList();
	
	Semantic sem;
	sem.rec.ParseScanner();
	sem.rec.TreeOut("C:\\Users\\HP\\Desktop\\design\\ComplierDesign\\SNL_COMPLIER\\output\\parse.txt");
	sem.parseTree=sem.rec.root;
	
	sem.SemScanner();
	sem.OutSemError();
	
	return 0;
}
