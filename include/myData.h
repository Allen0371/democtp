#pragma once
#include "DataCollect.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
//
#include <iconv.h>  
#include <cstring> 
#include <string>
#include <iostream>
#include <memory>
#include <map>
#include <unordered_map>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"//for basic file logger
#include <sys/time.h>
//#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  //以上三个是open的头文件
#include<unistd.h>//close   unix的标准头文件
#include<stdlib.h>//exit
#include<stdio.h>//perror
#include <signal.h>
#include <locale>

#include <codecvt>
#include <limits>


#include <regex>
#include<fstream>
using namespace std;

#define My_Max_Double numeric_limits<double>::infinity()
struct AccInfo
{
    string jyAdd;
    string hqAdd;
    string brkId;
    string userId;
    string psd;
    string inId;
    string prodInfo;
    string auCode;
    string appId;
    /* data */
};

struct appStatus
{
    bool mdAppStatus;
    bool tdAppStatus;
    bool clAppStatus;
    bool ccAppStatus;
    bool zbAppStatus;
    /* data */
};
struct tdUserPosInfo
{
     vector<string> SEN_ORD; //下单数据
};
enum kpdk_opt{OR_KD,OR_KK,OR_PD,OR_PK};
struct orderData{
    string ord_symbol;
    double ord_price;
    int ord_lots = 1;
    kpdk_opt ord_kpdk;

};
struct orderActionData{ //使用FrontID+SessionID+OrderRef撤单
    string ordact_OrderRef;
    int ordact_FrontID;
    int ordact_SessionID;
       

};
struct tdDataInfo{
    vector<CThostFtdcInstrumentField> InstrumentData;///品种数据
    vector<pair<timeval,orderData>>                 SendOrderSimple;///简单下单数据
    vector<pair<timeval,CThostFtdcInputOrderField>> SendOrderData;///下单数据
    vector<pair<timeval,CThostFtdcOrderField> >     SendOrderRtn;///报单返回
    vector<pair<timeval,CThostFtdcTradeField> >     SendOrderTrade;///成交返回
    vector<pair<timeval,CThostFtdcRspInfoField>>    SendOrderErr;///错误信息

    vector<pair<timeval,string>>                 sendOrderActionSimple;///简单下单数据
    vector<pair<timeval,CThostFtdcInputOrderActionField>> SendOrderAction;///撤单
   
    vector<pair<timeval,CThostFtdcOrderActionField> >     ErrOrderAction;///撤单错误


};

struct zbOutDataInfo{



};
typedef unordered_map<string,vector<CThostFtdcDepthMarketDataField>> TickMarkets;
typedef shared_ptr<appStatus> appState;
typedef shared_ptr<AccInfo> Acc_ptr;
typedef shared_ptr<tdDataInfo> ptrTdDataInfo;

unsigned int myGetSysTime();

inline unsigned int myGetSysTime() // inline 与函数定义体放在一起
{
   timeval t1;
   gettimeofday(&t1,0);
   //cout << t1.tv_sec << endl;
   return (t1.tv_sec+8*60*60)%(24*60*60);//北京时间
}

unsigned int myGetLocalTimeToSec(unsigned int myhour,unsigned int myminute=0,unsigned int mysec =0);

inline unsigned int myGetLocalTimeToSec(unsigned int myhour,unsigned int myminute,unsigned int mysec) // inline 与函数定义体放在一起
{
   
   return myhour%24*60*60+myminute%60*60+mysec%60;//北京时间
}

class strategyBase{

 public:
    virtual ~strategyBase();
    virtual string getSrategyName() = 0;
    virtual string getSrategyVersion() = 0;

};
inline string getTimeStr(){
    timeval _t;
    gettimeofday(&_t,0);
    tm _t2;
    localtime_r(&_t.tv_sec,&_t2);
    
   
    char myStr[255]="\0";  
    strftime(myStr,sizeof(myStr),"%Y-%m-%d_%H:%M:%S_",&_t2);
    return string(myStr) + to_string(_t.tv_usec);

}

// std::wstring convertEncoding(const std::string &gb2312Str) {
//     std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
//     return converter.from_bytes(gb2312Str);
// }

// 定义 GB2312 到 UTF-8 的转换函数
std::string GB2312_to_UTF8(const std::string& gb2312_str) {
    // 创建 iconv 转换句柄
    iconv_t cd = iconv_open("UTF-8", "GB2312");

    if (cd == (iconv_t)-1) {
        std::cerr << "Error opening iconv handle" << std::endl;
        return "";
    }

    // 分配输入缓冲区的空间
    const char* in_buf = gb2312_str.c_str(); // 将 std::string 转换为 const char*

    // 分配输出缓冲区的空间
    size_t out_buf_size = gb2312_str.size() * 4;
    char* out_buf = new char[out_buf_size];

    // 进行编码转换
    size_t in_bytes = gb2312_str.size();
    size_t out_bytes = iconv(cd, &in_buf, &in_bytes, &out_buf[0], out_buf_size);

    // 关闭 iconv 转换句柄
    iconv_close(cd);

    // 如果转换成功，构建并返回 UTF-8 字符串
    if (out_bytes > 0) {
        std::string utf8_str(out_buf, out_bytes);
        delete[] out_buf;
        return utf8_str;
    } else {
        // 转换失败时，返回空字符串
        delete[] out_buf;
        return "";
    }
}