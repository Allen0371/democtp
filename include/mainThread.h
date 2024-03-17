#pragma once

#include <iostream>
#include "inifile.h"
#include "MdSpi.h"
#include "TdSpi.h"
#include "Celue.h"
#include "Cunc.h"
#include "Zhib.h"
#include "Fenk.h"
#include "myData.h"
#include <string>
// #include "MdSpi.h"
using namespace std;

class mainThread{

public:

    mainThread();
    ~mainThread();
    void mainStart();
    MdSpi* md;
    TdSpi* td;
    shared_ptr<Celue> cl;
    shared_ptr<Cunc> cc;
    shared_ptr<Zhib> zb;
    shared_ptr<Fenk> fk;
    shared_ptr<AccInfo> m_Acc;
private:
    void getConfig(shared_ptr<AccInfo> &m_mAcc);
    

};