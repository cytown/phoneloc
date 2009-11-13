/************************************************************
  原作者信息:
  Author: rssn
  Email : rssn@163.com
  QQ    : 126027268
  Blog  : http://blog.csdn.net/rssn_net/
  Modify History:
   1) Geesun 2009/10/09 增加城市区号
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

