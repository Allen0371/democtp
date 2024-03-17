#pragma once
#include "myData.h"
#include "DataCollect.h"
#include "ThostFtdcUserApiDataType.h"
#include "ThostFtdcUserApiStruct.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include <iostream>

class Cunc
{
private:
    /* data */
    shared_ptr<TickMarkets> cc_hqptr;
    ptrTdDataInfo cc_tdInfo;
public:
    Cunc(shared_ptr<TickMarkets> w_hqptr,  ptrTdDataInfo w_tdInfo);
    ~Cunc();
    void cc_CunStart();
};
