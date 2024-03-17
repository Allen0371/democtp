#include "Zhib.h"


Zhib::Zhib(shared_ptr<TickMarkets> & w_zbTick, shared_ptr<zbOutDataInfo>& w_zbOut):zb_Tick(w_zbTick), zb_Out(w_zbOut)
{

    cout << "指标创建成功" << endl;
}

Zhib::~Zhib()
{
}
