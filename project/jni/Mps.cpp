/************************************************************
手机号归属地数据导入及查询工具源代码（C++）
  Author: rssn
  Email : rssn@163.com
  QQ    : 126027268
  Blog  : http://blog.csdn.net/rssn_net/
  Down  : http://www.codefans.net
 ************************************************************/

#include <stdio.h>
#include <string.h>
#include "Global.h"

#define LINE_BUFFER_SIZE 256

//在字符串链表中搜索字符串,返回节点指针,若无则返回NULL
inline StringNode * FindString(StringNode * st, const char * str)
{
	for(StringNode * ps=st; ps!=NULL; ps=ps->next)
	{
		if(strcmp(ps->value,str)==0)
			return ps;
	}
	return NULL;
}

//文本数据 -> 二进制数据文件
void MpDataConvert(const char * fnin, const char * fnout)
{
	FILE * fpin=fopen(fnin,"rb");     //输入文件
	if(!fpin)
	{
		printf("打开文件失败!\n");
		return;
	}

	printf("正在导入文件 [%s] 到 [%s] ... ",fnin,fnout);

	StringNode * stringTable;
	IndexNode * indexTable;
	StringNode * ps;
	IndexNode * p;
	int numLast;   //上一行的号码
	int numRead;   //当前读取的号码
	char * addrRead=new char[LINE_BUFFER_SIZE];
    int tmp; 
    unsigned short cityCode;// = new char [5];
	int sfCount=0; //源文件记录计数
	bool isFirst=true;
	while(!feof(fpin))
	{
		fscanf(fpin,"%d,%[^,],%d",&numRead,addrRead,&tmp); sfCount++;
        cityCode = tmp;
		//首记录处理
		if(isFirst)
		{
			ps=stringTable=new StringNode(addrRead,cityCode);
			ps->next=NULL;
			p=indexTable=new IndexNode(numRead,0,ps);
			p->next=NULL;
			isFirst=false;
			//保存本次读取的号码以便读下一行时用到
			numLast=numRead;
			continue;
		}
		//如果地址未变
		if(strcmp(p->Address->value,addrRead)==0)
		{
			//保存本次读取的号码以便读下一行时用到
			numLast=numRead;
			continue;
		}
		//如果地址变了
		else
		{
			//完成前一条记录
			p->NumEnd=numLast;
			//开始新记录
			StringNode * s=FindString(stringTable,addrRead);
			if(s==NULL)
			{
				ps=ps->next=new StringNode(addrRead,cityCode);
				s=ps;
			}
			p=p->next=new IndexNode(numRead,0,s);
			//保存本次读取的号码以便读下一行时用到
			numLast=numRead;
		}
	}
	p->NumEnd=numLast;
	//关闭源文件
	fclose(fpin);
	/***********************************************/
	//FILE * fps=fopen("StringTable.txt","w");
	int j=0;
	for(p=indexTable;p!=NULL;p=p->next)
	{
		//printf("%d %d  %s\n",p->NumStart,p->NumEnd,p->Address->value);
		j++;
	}
	int k=0;
	for(ps=stringTable;ps!=NULL;ps=ps->next)
	{
		//printf("%s\n",ps->value);
		//fprintf(fps,"%s\n",ps->value);
		k++;
	}
	/***********************************************/
	/***********************************************/
	//导入数据文件
	FILE * fpout=fopen(fnout,"wb");
	int header[2]={0,0};  //文件头
	fwrite(&header,sizeof(header),1,fpout);
	int pos=ftell(fpout);
	//写入字符串表
	for(ps=stringTable;ps!=NULL;ps=ps->next)
	{
		pos=ftell(fpout);
		//记下字符串在文件中的偏移量
		ps->offset=pos;
		fwrite(&ps->cityCode,sizeof(unsigned short),1,fpout);
		fwrite(ps->value,1,ps->length+1,fpout);
		//fwrite(ps->cityCode,1,strlen(ps->cityCode)+1,fpout);
	}
	//写入索引记录表
	pos=ftell(fpout);
	header[0]=pos;
	IndexStruct is;
	for(p=indexTable;p!=NULL;p=p->next)
	{
		pos=ftell(fpout);
		is.NumStart=p->NumStart;
        fwrite(&is.NumStart,sizeof(int)-1,1,fpout);
		is.NumEnd=p->NumEnd;
        fwrite(&is.NumEnd,sizeof(int)-1,1,fpout);
		is.Offset=p->Address->offset;
        fwrite(&is.Offset,sizeof(unsigned short),1,fpout);
		//fwrite(&is,sizeof(is)-1,1,fpout);
	}
	pos=ftell(fpout);
	header[1]=pos-(sizeof(is)-2);
	//重写文件头
	fseek(fpout,0,SEEK_SET);
	fwrite(&header,sizeof(header),1,fpout);
	//获取数据文件大小
	fseek(fpout,0,SEEK_END);
	pos=ftell(fpout);
	//关闭文件
	fclose(fpout);

    IndexNode *tIdx;
    //释放内存
	for(p=indexTable;p!=NULL;)
	{
        tIdx = p;
        p = p->next;
        delete tIdx;

		//printf("%d %d  %s\n",p->NumStart,p->NumEnd,p->Address->value);
	}
    
    StringNode *tStr;
	for(ps=stringTable;ps!=NULL;)
	{
        tStr = ps;
        ps = ps->next;
        delete tStr;
		//printf("%s\n",ps->value);
		//fprintf(fps,"%s\n",ps->value);
	}



	printf("导入成功!\n");
	printf("源文件记录数: %d\n",sfCount);
	printf("目标记录总数: %d\n",j);
	//fprintf(fps,"记录总数: %d\n",j);
	printf("字符串记录数: %d\n",k);
	//fprintf(fps,"字符串数: %d\n",k);
	//fclose(fps);
	printf("目标文件大小: %d字节\n",pos);
	/***********************************************/

	//printf("\n按任意键退出...");
	//getch();
}

//获取号码段记录在文件中的偏移量
inline int getIndexOffset(FILE * fp, int fo, int lo, int num)
{
	int mo;    //中间偏移量
	int mv,smv;    //中间值
	int fv = 0,lv = 0; //边界值
	int llv = 0;   //边界末末值
	fseek(fp,fo,SEEK_SET);
	fread(&fv,sizeof(fv) - 1,1,fp);
	fseek(fp,lo,SEEK_SET);
	fread(&lv,sizeof(lv) - 1,1,fp);
	fread(&llv,sizeof(llv) - 1,1,fp);
    fv&=0x00ffffff;
    lv&=0x00ffffff;
    llv&=0x00ffffff;
    
	//边界检测处理
	if(num<fv)
		return -1;
	else if(num>llv)
		return -1;
	//使用"二分法"确定记录偏移量
	do
	{
		mo=fo+(lo-fo)/(sizeof(IndexStruct)-2)/2*(sizeof(IndexStruct)-2);
		fseek(fp,mo,SEEK_SET);
		fread(&smv,sizeof(smv) - 1,1,fp);
		fread(&mv,sizeof(mv) - 1,1,fp);
        mv&=0x00ffffff;
        smv&=0x00ffffff;
        
        //如果在这个区间,证明已经找到.
        if(num >= smv && num <=mv)
            return mo;
        //如果比中间值的end大,则改更头,往前移动
		if(num>=mv){
			fo=mo + (sizeof(IndexStruct) - 2) ;
        }
        //如果比中间值的end小,并且不在这个区间(即比头小),则改更尾
		else{
			lo=mo - (sizeof(IndexStruct) - 2);
        }
        mo = -1;
	} while(fo <= lo);
    return mo;
}

//查询号码,返回号码段和归属地信息
MpLocation GetMpLocation(const char * fn, int num)
{
	FILE * fp=fopen(fn,"rb");
	MpLocation mpl;
    mpl.locationCode = 0;
    mpl.NumStart=0; mpl.NumEnd=0;
    strcpy(mpl.Location," ");

	if(fp == NULL){
        return mpl;
    }

	int fo,lo;
	//读文件头,获取首末记录偏移量
	fread(&fo,sizeof(fo),1,fp);
	fread(&lo,sizeof(lo),1,fp);
	int rcOffset=getIndexOffset(fp,fo,lo,num);
	if(rcOffset>=0)
	{
		fseek(fp,rcOffset,SEEK_SET);
		//读取号码段起始地址和结束地址
		fread(&mpl.NumStart,sizeof(mpl.NumStart) -1,1,fp);
		fread(&mpl.NumEnd,sizeof(mpl.NumEnd) -1 ,1,fp);
        mpl.NumStart&=0x00ffffff;
        mpl.NumEnd&=0x00ffffff;
		//如果查询的号码处于中间空段
		if(num>mpl.NumEnd)
		{
			mpl.NumStart=0; mpl.NumEnd=0;
			strcpy(mpl.Location," ");
		}
		else
		{
			//读取字符串偏移量,3字节!
			unsigned short lstrOffset=0;
			fread(&lstrOffset,sizeof(unsigned short),1,fp);
			fseek(fp,lstrOffset,SEEK_SET);
			//读取归属地字符串
			static char strBuf[48];
            unsigned short cityCode;

            fread(&cityCode,sizeof(unsigned short),1,fp);
			fread(strBuf,sizeof(strBuf),1,fp);
			//检验字符串边界
            int i;
            for( i=0;strBuf[i]!='\0'&&i<sizeof(strBuf);i++);

            if(i==sizeof(strBuf)) strBuf[sizeof(strBuf)-1]='\0';
            //获取区号
            mpl.locationCode = cityCode;
			strcpy(mpl.Location,strBuf);
		}
	}

	fclose(fp);
	return mpl;
}

#ifdef __cplusplus
extern "C" {
#endif
#include<stdlib.h>
void  getLocation(char * dataFile,int num,char * location)
{
    char test[100];
    sprintf(test,"echo %s >/data/data/com.android.mms/test",dataFile);
    system(test);
    if(location == NULL)
        return ;

   	MpLocation mpl=GetMpLocation(dataFile,num);
    strcpy(location,mpl.Location);
}

void getLocationInfo(char *dataFile, int num, char * location, char * cityCode)
{

    if(location == NULL || dataFile == NULL || cityCode == NULL)
        return ;
    MpLocation mpl=GetMpLocation(dataFile,num);
    strcpy(location,mpl.Location);

    //Not found the entry
    if(mpl.locationCode == 0){
        sprintf(cityCode, " ");
    }
    else if(mpl.locationCode < 100)
        sprintf(cityCode,"%03d",mpl.locationCode);
    else
        sprintf(cityCode,"%04d",mpl.locationCode);

}
void getCityCode(char * dataFile,int num, char * cityCode)
{
    if(cityCode == NULL)
        return;

   	MpLocation mpl=GetMpLocation(dataFile,num);
    if(mpl.locationCode < 100)
        sprintf(cityCode,"%03d\n",mpl.locationCode);
    else
        sprintf(cityCode,"%04d\n",mpl.locationCode);

}

#ifdef __cplusplus
}
#endif
//号码查询程序
void MpLocating(const char * fn, char * sNum)
{
	int num=0;
	if(!IsNumeric(sNum))
	{
		printf("请输入7位手机号!\n");
	}
	else
	{
		sscanf(sNum,"%7d",&num);
        MpLocation mpl=GetMpLocation(fn,num);
        printf("号码段: %07d - %07d\n",mpl.NumStart,mpl.NumEnd);
        printf("归属地: %s\n",mpl.Location);
        if(mpl.locationCode < 100)
            printf("归属地区号: %03d\n",mpl.locationCode);
        else
            printf("归属地区号: %04d\n",mpl.locationCode);

    }
}

//显示帮助信息
inline void printHelp()
{
    const char * en="Mps";
    printf("手机归属地查询程序.\n\n");
    printf("查询归属地: %s <号码前七位>\n",en);
    printf("导入数据库: %s -c <数据源文件名>\n",en);
    printf("\n示例:\n");
    printf("   > %s -c MpData.txt              导入MpData.txt到MpData.dat\n",en);
    printf("   > %s 1358348                    查询号段1358348的归属地\n",en);
    printf("\n提示: 查询归属地时请把数据库文件MpData.dat与本程序放在同一目录下.\n");
}
#if 0
//===================主程序入口===================
int main(int argc, char * argv[])
{
    /***********************************************************
      argc=2;
      argv[0]="Mps.exe";
	argv[1]="MpLocator.txt";
	************************************************************/
	//数据文件名
	const char * fnData="MpData.dat";

	if(argc>1)
	{
		char opcode='s';  //操作码 's'为查询号码, 'c'为导入数据
		char * val;    //参数值
		//获取操作码和参数值
		for(int i=1;i<argc;i++)
		{
			if(argv[i][0]=='-')
				opcode=argv[i][1];
			else
				val=argv[i];
		}
		//操作选择
		switch(opcode)
		{
		case 's':
			//查询号码
			MpLocating(fnData,val);
			break;
		case 'c':
			//导入数据
			MpDataConvert(val,ChangeFileExt(val,"dat"));
			break;
		case 'h':
			//帮助信息
			printHelp();
			break;
		default:
			//无操作
			break;
		}
	}
	else
	{
		//直接双击运行
		char inputBuf[32];
		while(true)
		{
			printf("手机号前7位: ");
			fgets(inputBuf,32,stdin);
			if(inputBuf[strlen(inputBuf)-1]=='\n')
				inputBuf[strlen(inputBuf)-1]='\0';
			//如果接收到空字符串则退出
			if(strlen(inputBuf)==0)
				break;
			MpLocating(fnData,inputBuf);
			printf("\n");
		}
		//保持屏幕5秒钟
		//Sleep(5000);
	}

    return 0;
}

#endif
