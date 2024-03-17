#include "Cunc.h"
// Cunc::Cunc(/* args */)
// {
//     cout << "存储线程" << endl;
// }

Cunc::Cunc(shared_ptr<TickMarkets> w_hqptr, ptrTdDataInfo w_tdInfo):cc_hqptr(w_hqptr),cc_tdInfo(w_tdInfo)
{
    cout << "创建存储线程" << endl;
}

Cunc::~Cunc()
{
}

void Cunc::cc_CunStart()
{
    cout << "存一下，就存一下" << endl;
}
