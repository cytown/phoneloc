/************************************************************
手机号归属地数据导入及查询工具源代码（C++）
  Author: rssn
  Email : rssn@163.com
  QQ    : 126027268
  Blog  : http://blog.csdn.net/rssn_net/
   Modify History:
   1) Geesun 2009/10/09 增加城市区号
   2) Geesun 2009/10/11 更改号码为24位,以节约空间200k
 ************************************************************/

#ifndef _MPGLOBAL_INCLUDED_
#define _MPGLOBAL_INCLUDED_
#pragma pack (1)

//链表节点类
class StringNode
{
public:
    unsigned short cityCode;
	char * value;
	int length;
	unsigned short offset;
	StringNode * next;

	StringNode(const char * val, unsigned short cityCode);
	StringNode();
	~StringNode();
};


//索引表节点类
class IndexNode
{
public:
	int NumStart;
	int NumEnd;
	StringNode * Address;
	IndexNode * next;

	IndexNode();
	IndexNode(int ns, int ne, StringNode * ad=NULL);
};

//索引记录结构体
typedef struct _IndexStruct
{
	int NumStart;
	int NumEnd;
	unsigned short Offset;
} IndexStruct;

//手机归属地结构体类型
typedef struct _MpLocation
{
	int NumStart;
	int NumEnd;
	char Location[48];
    int locationCode;
} MpLocation;

#endif
