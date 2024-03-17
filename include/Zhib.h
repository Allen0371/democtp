#pragma once
#include "DataCollect.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include <iostream>
//#include <memory>
#include "myData.h"
using namespace std;
class Zhib
{
private:
    /* data */
public:
    Zhib(shared_ptr<TickMarkets> & w_zbTick, shared_ptr<zbOutDataInfo>& w_zbOut);
    ~Zhib();
    shared_ptr<TickMarkets> zb_Tick;
    shared_ptr<zbOutDataInfo> zb_Out;
};

