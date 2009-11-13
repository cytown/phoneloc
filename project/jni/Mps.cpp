/************************************************************
  手机号归属地数据导入及查询工具源代码（C++）
  Author: rssn
  Email : rssn@163.com
  QQ    : 126027268
  Blog  : http://blog.csdn.net/rssn_net/
  Down  : http://www.codefans.net
  Modify History:
    1) Geesun 2009/10/09 修正二分法查找可能找不到记录问题   
    2) Geesun 2009/10/11 更改号码为24位，以节约空间200k。
    3）Geesun 2009/11/13 清除部分不用代码,分开生成数据程序，使Jni 更小
 ************************************************************/

#include <stdio.h>
#include <string.h>
#include "Global.h"

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

#ifdef __cplusplus
}
#endif
