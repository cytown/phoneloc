/************************************************************
手机号归属地数据导入及查询工具源代码（C++）
  Author: rssn
  Email : rssn@163.com
  QQ    : 126027268
  Blog  : http://blog.csdn.net/rssn_net/
 ************************************************************/

#include <stdio.h>
#include <string.h>
#include "Global.h"

StringNode::StringNode(const char * val,unsigned short cityCode)
{
	this->length=strlen(val);
	this->value=new char[this->length+1];
	strcpy(this->value,val);
    this->cityCode = cityCode;
	this->next=NULL;
}
StringNode::StringNode()
{
	value=NULL;
	length=0;
	next=NULL;
}
StringNode::~StringNode()
{
	if(value) delete[] value;
}

IndexNode::IndexNode()
{
	NumStart=NumEnd=0;
	Address=NULL;
	next=NULL;
}
IndexNode::IndexNode(int ns, int ne, StringNode * ad)
{
	NumStart=ns; NumEnd=ne; Address=ad;
	next=NULL;
}

char * ChangeFileExt(const char * fn, const char * fext)
{
	int l=strlen(fn);
	int le=strlen(fext);
    int i;
	for( i=l-1; fn[i]!='.' && fn[i]!='\\' && fn[i]!='/' && fn[i]!=':' && i>=0; i--);
	char * fnext;
	//如果没扩展名
	if(i<=0||fn[i]=='\\'||fn[i]=='/'||fn[i]==':')
	{
		fnext=new char[l+le+2];
		strcpy(fnext,fn);
		fnext[l]='.';
		l++;
		strcpy(fnext+l,fext);
	}
	else
	{
		l=i+1;
		fnext=new char[l+le+1];
		strcpy(fnext,fn);
		strcpy(fnext+l,fext);
	}
	//申请新文件名的存储空间
	return fnext;
}

bool IsNumeric(const char * val)
{
	for(int i=0;val[i]!='\0';i++)
	{
		if(val[i]<'0'||val[i]>'9')
			return false;
	}
	return true;
}
