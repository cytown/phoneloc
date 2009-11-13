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


bool IsNumeric(const char * val)
{
	for(int i=0;val[i]!='\0';i++)
	{
		if(val[i]<'0'||val[i]>'9')
			return false;
	}
	return true;
}
