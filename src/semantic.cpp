#include "semantic.h"
#include<string.h>

//字符串数组
string TypeKindStr[]={"intTy","charTy","boolTy","arrayTy","recordTy"};
string AccessKindStr[]={"dir","indir"};
string IdKindStr[]={"varkind","typekind","prockind"};

//声明全局变量，定义基本类型
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

void SymTable::insertNode(SymTableNode* node)//插入一个符号表项
{
	if(node==NULL)
		return ;
	if(node->attrIR.kind!=prockind) 
		node->END=0;//只有在另一层开始时，才将其置为结束,但过程除外
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
//对并行声明的过程进行特殊处理
{
	if(pre->next==NULL)
		pre->next=next;
	else
	{
		SymTableNode* p=pre->next;
		pre->next=next;//重定位
		next->next=p;  //重定位
	}
}

//从符号表栈中查找已经声明的标识符
SymTableNode* Scope::FindID(string idname,bool ntype)
{
	if(scope.empty())
		return NULL;
	ScopeNode* node=this->scope.top();
	while(node)
	{
		SymTableNode* t=node->front;
		while(t)//从当前层找，如果到达符号表尾结束循环
		{
			if(t->name==idname)
				return t;
			if(t->END)//到达本层末尾
				break;
			t=t->next;
		}
		if(ntype)
			node=node->parent;//找上一层
		else
			break;
	}
	return NULL;
}
//得到本层符号表的尾项
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
	if(scope.empty())//找到父层指针
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


void Semantic::SemScanner()//进行语义分析
{
	if(this->parseTree==NULL)
    {
        cout<<"语法树为空，无法进行语义分析"<<endl;
        return;
    }
	initTy();
	this->myScope.newLayer(0);
	CntSemError=0;
	SemProgram(parseTree);
	return ;
}

void Semantic::SemProgram(ParseNode* node)//总程序
{
	ParseNode* p;
	if((p=FindBrother(node->child,"DeclarePart")))//声明
	{
		SemDeclarePart(p,0);
	}

	if((p=FindBrother(node->child,"ProgramBody")))//程序体
	{
		SemProgramBody(p);
	}


}

void Semantic::SemDeclarePart(ParseNode* node,int layer)//程序声明
{
	ParseNode* p=node->child;
	if(p->ParseStr=="TypeDecpart")//类型声明
		SemTypeDecpart(p,layer);

	if((p=FindBrother(p,"VarDecpart")))//变量声明
        SemVarDecpart(p,layer);

    if((p=FindBrother(p,"ProcDecpart")))//过程声明
    {
		SemProcDecpart(p,layer);
    }
}

/**************************类型声明***********************/
void Semantic::SemTypeDecpart(ParseNode* node,int layer)//类型声明
{
	ParseNode* p=node->child;
	if(p->Symbol==-1)//无类型声明
		return ;
	if((p=FindBrother(p->child,"TypeDecList")))//有类型声明
		SemTypeDecList(p,layer);
}

void Semantic::SemTypeDecList(ParseNode* node,int layer)//类型声明列表
{
	ParseNode* p=node->child;
	if(p->ParseStr=="TypeId")
	{
		if(myScope.FindID(p->child->ParseStr,0)!=NULL) //本层中是否已经存在
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
			if(s->attrIR.idtype)//生成类型声明信息
			{
				if(myScope.scope.top()->front==NULL)
					myScope.scope.top()->front=s;//为本层的初始符号表项
				
				this->symTable.insertNode(s);
			}
		}	
	}
	ParseNode* q;
	if((q=FindBrother(node->child,"TypeDecMore")))//是否有更多的类型声明
		if(q->child->ParseStr=="TypeDecList")
			SemTypeDecList(q->child,layer);

}

void Semantic::SemTypeDef(ParseNode* node ,typeIR*& tIR,int layer)//类型声明部分
{
    ParseNode* p=node->child;
	if(p==NULL)
	{
		tIR=NULL;
		return ;
	}
	if(p->ParseStr=="BaseType")//基本类型
	{
		if(p->child->ParseStr=="integer")
			tIR=IntTy;
		else if(p->child->ParseStr=="char")
			tIR=CharTy;
	}
	else if(p->ParseStr=="StructureType")//自定义类型
		SemStructureType(p,tIR,layer);
	else if(p->Symbol==88)//类型标识符
	{
		SymTableNode* s=myScope.FindID(p->ParseStr,1);//查找类型标识符
		if(s==NULL)//找不到
		{
			char str[256];
			sprintf(str,SERROR_TYPEID_UNDEFINE,p->ParseStr.c_str());
			string t(str);
			SemError(t,node->LineNum);
			tIR=NULL;
		}
		else//找到了
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
void Semantic::SemStructureType(ParseNode* node ,typeIR*& tIR,int layer)//自定义类型声明
{
	tIR=new typeIR;//非基本类型，新申请一个空间
	ParseNode* p=node->child;

	if(p->ParseStr=="ArrayType")//数组
		SemArrayType(p,tIR,layer);

	else if(p->ParseStr=="RecType")//纪录
		SemRecType(p,tIR,layer);

	else
	{
		delete tIR;
		tIR=NULL;
	}
}

void Semantic::SemArrayType(ParseNode* node ,typeIR*& tIR,int layer)//数组类型
{
	tIR->kind=arrayTy; //为数组类型
	ParseNode* p=FindBrother(node->child,"Low");
	if(p)//下界 Low
		tIR->More.ArrayAttr.low=atoi(p->child->ParseStr.c_str());
	if((p=FindBrother(p,"Top")))//上界 Top
	    tIR->More.ArrayAttr.top=atoi(p->child->ParseStr.c_str());

	p=p->brother->brother->brother;
	if(p->child->ParseStr=="integer")//元素类型
        tIR->More.ArrayAttr.elemTy=IntTy;
    else tIR->More.ArrayAttr.elemTy=CharTy;

	if(tIR->More.ArrayAttr.top<=tIR->More.ArrayAttr.low)//判断是否数组上下界出错
	{
		SemError(SERROR_ARRAY_TOPLTLOW,node->LineNum);
		return;
	}

	tIR->size=(tIR->More.ArrayAttr.top-tIR->More.ArrayAttr.low+1)//类型大小
		*(tIR->More.ArrayAttr.elemTy->size);
}

void Semantic::SemRecType(ParseNode* node ,typeIR*& tIR,int layer)  //记录类型
{
	tIR->kind=recordTy;
	ParseNode* p=node->child;
	if(p->brother->ParseStr=="FieldDecList")
		SemFieldDecList(p->brother,tIR->More.body,tIR->size,layer);

}
void Semantic::SemFieldDecList(ParseNode* node ,
                               fieldChain*& body,int &size,int layer)//得到成员变量
{
	ParseNode* p=node->child;

	if(p->ParseStr=="BaseType")//基本类型
	{
	    ParseNode*t=FindBrother(p,"FieldDecMore");
		if(p->child->ParseStr=="integer")
			SemIdList(p->brother,t,body,layer,size,IntTy);
			//t用于FieldDecMore判断
		else if(p->child->ParseStr=="char")
			SemIdList(p->brother,t,body,layer,size,CharTy);
	}

	else if(p->ParseStr=="ArrayType")//数组类型
	{
		typeIR* tIR=new typeIR;
		SemArrayType(p,tIR,layer);//先形成数组类型信息
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

	if(p->brother->child->Symbol!=-1)//仍有其它同类型变量
    {
        p=p->brother->child->brother;
        if(p->ParseStr=="IdList");
        SemIdList(p,isMore,body->next,layer,size,tIR);
    }
	else if(isMore&&isMore->child->ParseStr=="FieldDecList")//有记录的变量定义列表
		SemFieldDecList(isMore->child,body->next,size,layer);
}

/**************************变量声明***********************/
void Semantic::SemVarDecpart (ParseNode* node,int layer)//变量声明
{
	ParseNode* p=node->child;
	if(p->Symbol==-1)//无变量声明
		return;
	if((p=FindBrother(p->child,"VarDecList")))//有变量声明
		SemVarDecList(p,layer);
}

void Semantic::SemVarDecList(ParseNode* node,int layer)//变量声明列表
{
	ParseNode* p;
	if((p=FindChild(node,"TypeDef")))//类型
	{
		typeIR* tIR;
		SemTypeDef(p,tIR,layer);
		if(tIR)
			SemVarIdList(p->brother,layer,tIR);
	}
	if((p=FindBrother(node->child,"VarDecMore")))//是否还有更多的变量声明
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

	if(myScope.FindID(p->name,0)!=NULL) //本层中是否已经存在
	{
		char str[256];
		sprintf(str,SERROR_VAR_MULTIDEFINE,p->name.c_str());
		string s(str);
		SemError(s,node->LineNum);
	}
	else
	{
		if(p->attrIR.idtype)//如果类型形成未出错
		{
			if(myScope.scope.top()->front==NULL)
				myScope.scope.top()->front=p;//为本层的初始符号表项

			this->symTable.insertNode(p);
		}
		else
			return ;//出错则返回，出错信息已经在SemTypeDef()中生成，故不再重复
	}
	ParseNode* q;
	if((q=FindBrother(node->child,"VarIdMore")))//是否有更多的变量声明
		if((q=FindBrother(q->child,"VarIdList")))
			SemVarIdList(q,layer,tIR);
}

/**************************过程声明***********************/

void Semantic::SemProcDecpart(ParseNode* ptr,int layer)//过程声明部分
{	
	ParseNode* p;
	if((p=FindChild(ptr,"ProcDec")))//是否存在过程声明
	{
		SemProcDec(p,layer+1,0);
		if((p=FindBrother(p->child,"ProcDecMore")))//是否有更多的过程声明
			if((p=FindChild(p,"ProcDec")))
				SemProcDec(p,layer+1,1);
	}
}

void Semantic::SemProcDec(ParseNode* ptr,int layer,bool mid)
{
	ParseNode* p;
	SymTableNode* st=new SymTableNode;//新建符号表项
	
	p=ptr->child->brother->child;

	if(myScope.FindID(p->ParseStr,1)!=NULL) //本层中是否已经存在
	{
		char str[256];
		sprintf(str,SERROR_PROCEDURE_MULTIDEFINE,p->ParseStr.c_str());
		string s(str);
		SemError(s,p->LineNum);
	}
	
	st->name=p->ParseStr;
	st->next=0;
	st->attrIR.idtype=0;//无返回值 
	st->attrIR.kind=prockind;
	st->attrIR.level=layer;
	st->attrIR.More.param=0;//参数列表初始化为空
	st->END=true;
	
	if(myScope.scope.top()->front==NULL)
		myScope.scope.top()->front=st;//为本层的初始符号表项

	if(!mid)
		symTable.insertNode(st);//插入到符号表中
	else
	{
		SymTableNode* s0=myScope.GetRear();//并行过程
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

void Semantic::SemParamList(ParseNode* ptr,int layer,ParamTable* &param)//参数列表
{
	ParseNode* p;
	if((p=FindChild(ptr,"ParamDecList")))//是否存在参数列表
		SemParamDecList(p,layer,param);
}

void Semantic::SemParamDecList(ParseNode* ptr,int layer,ParamTable* &param)//形成符号表
{
	ParseNode* p;
	if(ptr->child&&ptr->child->ParseStr=="Param")//有参数(允许无参)
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
	
	if((p=FindBrother(ptr->child,"ParamMore")))//是否有更多其它类型的参数项
		if((p=FindBrother(p->child,"ParamDecList")))
		{
			if(param)//param在上一次分析中被修改
			{
				ParamTable* pr=param;
				while(pr->next) 
					pr=pr->next;//找到最末项 
				SemParamDecList(p,layer,pr->next);
			}
			else 
				SemParamDecList(p,layer,param);
		}
}

void Semantic::SemFormList(ParseNode* ptr,int layer,ParamTable* &param,typeIR* tIR,bool ntype)//形参项
{
	ParseNode* p;
	SymTableNode* st=new SymTableNode;
	
	st->name=ptr->child->ParseStr;
	st->next=0;
	st->attrIR.idtype=tIR;
	st->attrIR.kind=varkind;
	st->attrIR.More.access=AccessKind(ntype);
	st->attrIR.level=layer;
	
	if(myScope.FindID(st->name,0))//是否参数已经声明过
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
		
		param=new ParamTable;//新建立一个形参结点
		param->type=tIR;
		param->next=0;
		
		if(myScope.scope.top()->front==NULL)//是否为本层首符号表项
			myScope.scope.top()->front=st;

		p =	ptr->child->brother->child;
		if(p->Symbol!=-1&&p->brother->ParseStr=="FormList")
			SemFormList(p->brother,layer,param->next,tIR,ntype);
	}
}

/**************************过程体***********************/

void Semantic::SemProgramBody(ParseNode* node)//程序体部分
{
	ParseNode* p=node->child;
	if(p->brother->ParseStr=="StmList")//得到语句序列语法树结点
		SemStmList(p->brother);
}

void Semantic::SemStmList(ParseNode* node)//语句序列的语义分析
{
	ParseNode* p=node->child->child;
	if(p->ParseStr=="ConditionalStm")//条件语句
		SemConditionalStm(p);
	else if(p->ParseStr=="LoopStm")//循环语句
		SemLoopStm(p);
	else if(p->ParseStr=="InputStm")//输入语句
		SemInputStm(p);
	else if(p->ParseStr=="OutputStm")//输出语句
		SemOutputStm(p);
	else if(p->ParseStr=="ReturnStm")//返回语句
		SemReturnStm(p);
	else if(p->Symbol==88)//其它语句：函数调用，赋值语句的分析
		SemOtherStm(p);
	if((p=FindBrother(node->child,"StmMore")))//是否有更多的语句序列，
		if((p=FindBrother(p->child,"StmList")))//如是继续进行分析
			SemStmList(p);
}

void Semantic::SemConditionalStm(ParseNode* node)//条件语句
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

void Semantic::SemLoopStm(ParseNode* ptr)//循环语句
{
	ParseNode* p;
	if((p=FindBrother(ptr->child,"RelExp")))//条件判断
		SemRelExp(p);
	if((p=FindBrother(ptr->child,"StmList")))//do部分语句
		SemStmList(p);
}

void Semantic::SemInputStm(ParseNode* ptr)//输入语句
{
	ParseNode* p;
	if((p=FindBrother(ptr->child,"Invar")))
		SemInvar(p);
}

void Semantic::SemOutputStm(ParseNode* ptr)//输出语句
{
	ParseNode* p;
	typeIR* tIR=0;
	int ntype=0;
	if((p=FindBrother(ptr->child,"Exp")))
		SemExp(p,tIR,ntype);
}

void Semantic::SemReturnStm(ParseNode* ptr)//返回语句
{
	
}

void Semantic::SemOtherStm(ParseNode* ptr)//赋值语句及过程调用语句
{
	ParseNode* p;
	SymTableNode* st=myScope.FindID(ptr->ParseStr,1);//找到标识符的符号表位置
	
	if(st)
	{
		if((p=FindChild(ptr->brother,"AssignmentRest")))//赋值语句
			SemAssignmentRest(p,st);
		else if((p=FindChild(ptr->brother,"CallStmRest")))//过程调用语句
			SemCallStmRest(p,st);
	}
	else //未找标识符名称
	{	
		char str[256];
		
		if((p=FindChild(ptr->brother,"AssignmentRest")))//变量标识符 
			sprintf(str,SERROR_VARID_UNDEFINE,ptr->ParseStr.c_str());
		else //过程标识符
			sprintf(str,SERROR_PROCID_UNDEFINE,ptr->ParseStr.c_str());
		string s(str);	
		SemError(s,ptr->LineNum);
	}
}

void Semantic::SemAssignmentRest(ParseNode* ptr,SymTableNode* st)//赋值语句
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

void Semantic::SemCallStmRest(ParseNode* ptr,SymTableNode* st)//过程调用语句
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

void Semantic::SemActParamList(ParseNode* ptr,ParamTable* param)//形参实参结合 
{
	ParseNode* p;
	int ntype=1;
	
	if((p=FindChild(ptr,"Exp")))
	{
		if(param==NULL)//无参 
		{
			SemError(SERROR_TOO_MUCH_PARAM,ptr->LineNum);
			return;
		}
		
		SemExp(p,param->type,ntype);
		
		if((p=FindBrother(p,"ActParamMore")))
		{
			if((p=FindBrother(p->child,"ActParamList")))
				SemActParamList(p,param->next);
			else if(param->next)//仍有形参
				SemError(SERROR_NOT_ENOUGH_PARAM,ptr->LineNum);
		}
	}
	else if(param)//找不到exp
		SemError(SERROR_NOT_ENOUGH_PARAM,ptr->LineNum);
}

void Semantic::SemRelExp(ParseNode* node)//条件表达式
{
	typeIR* tIR=NULL;
	ParseNode* p=node->child;
	int ntype=0;
	if(p->ParseStr=="Exp")
	{
		SemExp(p,tIR,ntype);//第一个表达式
		p=p->brother->child->brother;
		if(p->ParseStr=="Exp")
			SemExp(p,tIR,ntype=1);//其他表达式
	}
}

void Semantic::SemInvar(ParseNode* ptr)
{
	if(ptr->child&&ptr->child->Symbol==88)
	{
		SymTableNode* sym=myScope.FindID(ptr->child->ParseStr,1);
		if(sym==NULL)//未找到
		{
			char str[256];
			sprintf(str,SERROR_VARID_UNDEFINE,ptr->child->ParseStr.c_str());
			string s(str);
			SemError(s,ptr->child->LineNum);
		}
		else if(sym->attrIR.kind!=varkind)//不是变量
		{
			char str[256];
			sprintf(str,SERROR_ID_ISNOT_VARID,ptr->child->ParseStr.c_str());
			string s(str);
			SemError(s,ptr->child->LineNum);
		}
	}
}

void Semantic::SemExp(ParseNode* node,typeIR* &tIR,int &ntype)
	//当ntype==0时，第一个表达式的第一项
	//当ntype==1时，其他表达项
	//当ntype==2时，数组下标
{
	ParseNode* p=node->child;
	if(p->ParseStr=="Term")//算术表达式的第一项
		SemTerm(p,tIR,ntype);
	if(p->brother->ParseStr=="OtherTerm")//算术表达式的其它项
		SemOtherTerm(p->brother,tIR,ntype);
}

void Semantic::SemVariMore(ParseNode* ptr,SymTableNode* sym,typeIR* &tIR)//记录成员或者数组索引 
{
	ParseNode* p;
	int ntype=2;
	
	if((p=FindBrother(ptr->child,"Exp")))//数组索引
	{
		tIR=IntTy;
		SemExp(p,tIR,ntype);
	}
	else if((p=FindBrother(ptr->child,"FieldVar")))//记录成员 
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
			while(body)//在纪录体内找，是否包含此成员变量
			{
				if(body->idname==p->child->ParseStr)
					break;
				body=body->next;
			}
			if(body)//找到
			{
				if((p=FindBrother(p->child,"FieldVarMore")))
					if((p=FindBrother(p->child,"Exp")))
					{
						tIR=IntTy;
						SemExp(p,tIR,ntype);
					}
			}
			else //未找到，形成出错信息
			{
				char str[256];
				sprintf(str,SERROR_VAR_NOT_FIELD_MEM,p->child->ParseStr.c_str(),sym->name.c_str());
				string s(str);
				SemError(s,p->child->LineNum);
			}
		}
	}
}

void Semantic::SemTerm(ParseNode* node,typeIR* &tIR,int &ntype)//算术表达式第一项
{
	ParseNode* p=node->child;
	if(p->ParseStr=="Factor")//因子
	{
	    ParseNode*q;
		if((q=FindBrother(p->child,"Exp")))//表达式
			SemExp(q,tIR,ntype);
		else if(p->child->ParseStr=="Variable")//当为数组为纪录时
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
		if(t->ParseStr=="OtherFactor")//其它因子
			if(t->child->Symbol!=-1)
				SemTerm(t->child->brother,tIR,ntype);
	}
}
void Semantic::SemOtherTerm(ParseNode* node,typeIR* &tIR,int &ntype)//算术表达式中的其它项
{
	ParseNode* p=node->child;
	if(p->Symbol!=-1&&p->brother->ParseStr=="Exp")
		SemExp(p->brother,tIR,ntype);
}

void Semantic::SemVariable(ParseNode* node,typeIR* &tIR,int &ntype)//变量
{
	ParseNode* p=node->child,*q,*t;
	int type=2;
	
	if(p->Symbol==88)//标识符
	{
		SymTableNode* sym=myScope.FindID(p->ParseStr,1);//在本层逐次向上找
		if(sym)//找到
		{

			if(sym->attrIR.kind!=varkind)//不是变量类型
			{
				char str[256];
				sprintf(str,SERROR_ID_ISNOT_VARID,p->ParseStr.c_str());
				string s(str);
				SemError(s,p->LineNum);
		
			}
			else
			{
				if(p->brother->child->Symbol!=-1)//如果存在VaiMore
				{
					if((q=FindBrother(p->brother->child,"Exp")))
					{
						if(sym->attrIR.idtype->kind!=arrayTy)//本标识符并非数组类型
						{
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),"arrayTy");
							string s(str);
							SemError(s,p->LineNum);
							return;
						}
						if(ntype && sym->attrIR.idtype->More.ArrayAttr.elemTy!=tIR)//类形不匹配
						{
							if(tIR==NULL)
								return;
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH3,p->ParseStr.c_str(),TypeKindStr[tIR->kind].c_str());
							string s(str);
							SemError(s,p->LineNum);
							return;
						}
						else//对数组下标算术表达式进行语义分析
						{
							typeIR* tIR0=IntTy;
							SemExp(q,tIR0,type);//q=exp  对q进行深入的语义分析
							if(ntype==0)//如果是第一项
							{
								ntype=1;
								tIR=sym->attrIR.idtype->More.ArrayAttr.elemTy;//暂存类型，用于后续比较
							}
						}
					}
					else if((q=FindBrother(p->brother->child,"FieldVar")))
					{
						if(sym->attrIR.idtype->kind!=recordTy)//本标识符并非记录类型
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
								{//------------------------------------------为数组类型
									if(body->unitType->kind!=arrayTy)//判断记录此成员变量是否为数组类型
									{
										char str[256];
										sprintf(str,SERROR_TYPE_NOT_MATCH4,p->ParseStr.c_str(),body->idname.c_str());
										string s(str);
										SemError(s,q->LineNum);
										
										return;
									}
									if(ntype&& body->unitType->More.ArrayAttr.elemTy!=tIR)//类型不匹配
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
										SemExp(t,tIR0,type);//q=exp  对q进行深入的语义分析
										if(ntype==0)
										{
											ntype=1;
											tIR=body->unitType->More.ArrayAttr.elemTy;//修改要匹配的类型
										}
									}
								}
								else
								{//---------------------------------为成员变量类型
									if(ntype&& body->unitType!=tIR)//匹配不成功
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

					}/////////////////////////////////////////////////////////////////////////////记录结束
				}///////////////////////////////////////////////////////////////////////////////VariMore结束
				else
				{//---------------------------------------------------------------仅仅有Variable:=ID
					if(ntype==0)//如果是首项
					{
						tIR=sym->attrIR.idtype;
						ntype=1;
					}
					else if(ntype==1)//如果是1，则对其进行类型审查
					{
						if(sym->attrIR.idtype!=tIR)//检测类型
						{
							if(tIR==NULL)
								return;
							char str[256];
							sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),TypeKindStr[tIR->kind].c_str());
							string s(str);
							SemError(s,p->LineNum);
						}
					}
					else if(ntype==2&&sym->attrIR.idtype!=IntTy)//检测是否为数组下标类型
					{
						char str[256];
						sprintf(str,SERROR_TYPE_NOT_MATCH2,p->ParseStr.c_str(),"IntTy");
						string s(str);
						SemError(s,p->LineNum);
					}
				}
			}
		}
		else //局部化符号表中未找到此标识符
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
