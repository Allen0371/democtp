#pragma once
#include "myData.h"
#include "DataCollect.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include <iostream>
#include "TdSpi.h"
class Celue
{
private:
    /* data */
public:
    Celue(shared_ptr<TickMarkets> & w_hqTick,TdSpi *w_tdapi);
    ~Celue();
    void clStart();
    shared_ptr<TickMarkets>  cl_Tick;
    TdSpi *cl_tdapi;

    unordered_map<string,strategyBase*> Baselib;
    
    int clGetFile(vector<string> & vecfile);
    int getSymbolExchange(string & symbolName);

    double getSymbolNewPrice(string &symName,int canshu = 0);
};
