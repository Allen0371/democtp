#include "mainThread.h"



auto mainLogger = spdlog::basic_logger_mt("main_log", "./logs/main_log.txt");

mainThread::mainThread(){
    mainLogger->info("mainThread创建成功");
    mainLogger->flush();
    cout << "mainThread start" << endl;
    mainStart();
}


mainThread::~mainThread(){
    mainLogger->info("mainThread析构");
    mainLogger->flush();
cout << "mainThread break" << endl;
}

void mainThread::mainStart(){
    cout << ">>> 1、开始>>>" << endl;
//cout << myGetSysTime() << endl;

//      CFFEX CZCE DCE INE SHFE 
// 20:55:00 　 集合竞价报单 集合竞价报单 集合竞价报单 集合竞价报单 
// 20:59:00 　 开盘前 集合竞价撮合 集合竞价撮合 集合竞价撮合 
// 21:00:00 　 连续交易 连续交易 连续交易 连续交易 
// 8:55:00 　 集合竞价报单 集合竞价报单 　 集合竞价报单 
// 8:59:00 　 开盘前 非交易 　 非交易 
// 9:00:00 　 连续交易 连续交易 连续交易 连续交易 
// 9:10:00 集合竞价报单 　 　 　 　 
// 9:14:00 集合竞价撮合 　 　 　 　 
// 9:15:00 连续交易 　 　 　 　 
// 9:25:00 集合竞价报单 　 　 　 　 
// 9:29:00 集合竞价撮合 　 　 　 　 
// 9:30:00 连续交易 　 　 　 　 
// 10:15:00 　 开盘前 非交易 非交易 非交易 
// 10:30:00 　 连续交易 连续交易 连续交易 连续交易 
// 11:30:00 非交易 开盘前 非交易 非交易 非交易 
// 13:00:00 连续交易 连续交易 连续交易 连续交易 连续交易 
// 15:00:00 收盘 收盘 收盘 收盘 收盘 
// 15:15:00 收盘 　 　 　 　 

    shared_ptr<appStatus> appS = make_shared<appStatus>();
    appS->ccAppStatus = 0;
    appS->clAppStatus = 0;
    appS->mdAppStatus = 0;
    appS->tdAppStatus = 0;
    appS->zbAppStatus = 0;


    cout << ">>> 2、配置>>>" << endl;
    shared_ptr<AccInfo> m_Acc = make_shared<AccInfo>();
    getConfig(m_Acc);
    
    ///////////////////////////////////////////////////
    cout << ">>> 3、行情>>>" << endl;
    shared_ptr<TickMarkets> hqptr = make_shared<TickMarkets>();
    md = new MdSpi(hqptr,m_Acc,appS);
   
   ///////////////////////////////////////////////////////
    cout << ">>> 4、交易>>>" << endl;
   
    ptrTdDataInfo m_tdInfo = make_shared<tdDataInfo>();
    td = new TdSpi(m_Acc,m_tdInfo);
    ////////////////////////////////////////
    cout << ">>> 5、风控>>>" << endl;
    fk = make_shared<Fenk>();


    /////////////////////////////////////////////////////
    cout << ">>> 6、指标>>>" << endl;
    shared_ptr<zbOutDataInfo> _Out = make_shared<zbOutDataInfo>();
    zb = make_shared<Zhib>(hqptr,_Out);


////////////////////////////////////////////
    cout << ">>> 7、策略>>>" << endl;
    cl = make_shared<Celue>(hqptr,td);



    cout << ">>> 8、存储>>>" << endl;
    cc = make_shared<Cunc>(hqptr,m_tdInfo);

    /////////////////////////////////////
    
    while(1){
        int menu_int(0);
        menu_int = getchar();
        switch (menu_int)
        {
        case 1:
            /* code */
            break;
        case 2:
            /* code */
            break;
        case 3:
            /* code */
            break;
        default:
        cout << menu_int << endl;
            break;
        }
    }
    

}


void mainThread::getConfig(shared_ptr<AccInfo> &m_mAcc){
    string jyAdd,hqAdd,Brk,Uid,Psw,Iid,Uinfo,AuCode,AppId;
    inifile::IniFile ini;
    int res = ini.Load("./simnow.ini");
    string configName("simnow");
    if( res == 0)
    {
        
       ini.GetStringValue(configName,"FrontAddr",&jyAdd);
       ini.GetStringValue(configName,"FrontMdAddr",&hqAdd);
       ini.GetStringValue(configName,"BrokerID",&Brk);
       ini.GetStringValue(configName,"UserID",&Uid);
       ini.GetStringValue(configName,"Password",&Psw);
       ini.GetStringValue(configName,"InvestorID",&Iid);
       ini.GetStringValue(configName,"UserProductInfo",&Uinfo);
       ini.GetStringValue(configName,"AuthCode",&AuCode);
       ini.GetStringValue(configName,"AppID",&AppId);
       
        cout << "行情地址：" << hqAdd << endl;
        cout <<  "交易地址：" << jyAdd << endl;
        cout <<  "用户代码：" << Uid << endl;
       
        mainLogger->info("加载config信息，交易地址{}，行情地址{}",jyAdd,hqAdd);
        mainLogger->flush();
        m_mAcc->jyAdd = jyAdd;
        m_mAcc->hqAdd =  hqAdd;
        m_mAcc->brkId = Brk;
        m_mAcc->userId = Uid;
        m_mAcc->psd = Psw;
        m_mAcc->inId = Iid;
        m_mAcc->prodInfo = Uinfo;
        m_mAcc->auCode = AuCode;
        m_mAcc->appId= AppId;
        
    }else{
        cout << " 加载config信息错误"<< endl;
        mainLogger->warn("config错误");
        mainLogger->flush();
    }
    
 

}