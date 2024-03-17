 #include "MdSpi.h"
 #define VECTER_TICK_RELESE_NUM 6*60*60*2*3
 auto MdLogger = spdlog::basic_logger_mt("md_log", "./logs/md_log.txt");
    //trace debug info warn err critical off
  static int req_num{0}; 
 MdSpi::MdSpi(shared_ptr<TickMarkets> & hqmdsc,shared_ptr<AccInfo> & m_Acc,appState & m_apps):tick(hqmdsc),md_Acc(m_Acc),m_appState(m_apps)
 {
    //tick = hqmdsc;
    MdLogger->set_level(spdlog::level::trace);
    MdLogger->info("行情线程创建成功！");
    connect();
 }

 MdSpi::~MdSpi()
 {

 }
void MdSpi::OnFrontConnected()
{
		MdLogger->info("登陆前置回复");
		/*strcpy_s(g_chBrokerID, getConfig("config", "BrokerID").c_str());
		strcpy_s(g_chUserID, getConfig("config", "UserID").c_str());
		strcpy_s(g_chPassword, getConfig("config", "Password").c_str());*/
		MdLogger->info("准备登陆");
		MdLogger->flush();
		ReqUserLogin();
	}

	

	// 当客户端与交易托管系统通信连接断开时，该方法被调用
	void MdSpi::OnFrontDisconnected(int nReason)
	{
		// 当发生这个情况后，API会自动重新连接，客户端可不做处理
		MdLogger->info("<OnFrontDisconnected>");
		// MdLogger->info("nReason= = [%d]{}", nReason);
		MdLogger->warn("登陆前回置错误。。。");
		MdLogger->flush();
		MdLogger->info("</OnFrontDisconnected>");
	}
    void MdSpi::OnHeartBeatWarning(int nTimeLapse)
    {
    }
    void MdSpi::ReqUserLogin()
    {
		MdLogger->info("登陆");
		
		
		//int req_num = 0 ;
        CThostFtdcReqUserLoginField req; 
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, md_Acc->brkId.c_str());
		strcpy(req.UserID, md_Acc->userId.c_str());
		strcpy(req.Password, md_Acc->psd.c_str());
		int ret = m_mdApi->ReqUserLogin(&req, ++req_num);
		if(ret == 0){
			MdLogger->info("登陆请示发送成功 {}", ret);
		}else{MdLogger->info("登陆请示发送失败 {}", ret);}
	
	    
		MdLogger->flush();
	}
	// 当客户端发出登录请求之后，该方法会被调用，通知客户端登录是否成功
    void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		MdLogger->info("OnRspUserLogin:");
		MdLogger->info("ErrorCode={}, ErrorMsg={}", pRspInfo->ErrorID,
			pRspInfo->ErrorMsg);
		MdLogger->info("RequestID={}, Chain={}", nRequestID, bIsLast);
		if (pRspInfo->ErrorID != 0) {
			// 端登失败，客户端需进行错误处理
			MdLogger->info("回复-登陆失败Failed to login, errorcode={} errormsg={} requestid={} chain = {}",
				pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
			exit(-1);
		}else{
			MdLogger->info("回复-登陆成功 RequestID={}, Chain={}，登陆行情", nRequestID, bIsLast);
			MdLogger->flush();
			SubscribeMarketData();//订阅行情
		}
				
		//SubscribeForQuoteRsp();//询价请求
	}
void MdSpi::ReqUserLogout()
    {
		CThostFtdcUserLogoutField a = { 0 };
        m_mdApi->ReqUserLogout(&a, ++req_num);
		MdLogger->info("行情退出  登陆请求 》》》 ");
		MdLogger->flush();
    }
    void MdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {

    }

    void MdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {

    }

 

    void MdSpi::SubscribeMarketData() // 订阅行情
    {
	   cout << "准备订阅行情"<< endl;
	   string path = "./somefile/symbol.txt";
	   auto _ve = myReadSymbol(path);
	   cout << _ve.size() << "  个品种 将被订阅";
	   int bs { static_cast<int>(_ve.size())} ;
	   char **ppInstrumentID =new char*[bs];
	   
	   
	   for(int num_sym = 0;num_sym < bs; num_sym++){
		
		ppInstrumentID[num_sym] = const_cast<char*>(_ve.at(num_sym).c_str());
		cout << "\nnum_sym " << num_sym  << " " << _ve.at(num_sym) << endl;
		
	   }
	   int result = m_mdApi->SubscribeMarketData(ppInstrumentID, bs);
		MdLogger->info((result == 0) ? "订阅行情请求1......发送成功" : "订阅行情请求1......发送失败，错误序号=[%d]", result);
		MdLogger->flush();


		// int md_num = 0;
		// char **ppInstrumentID = new char*[5000];
		// for (int count1 = 0; count1 <= md_InstrumentID.size() / 500; count1++)
		// {
		// 	if (count1 < md_InstrumentID.size() / 500)
		// 	{
		// 		int a = 0;
		// 		for (a; a < 500; a++)
		// 		{
		// 			ppInstrumentID[a] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
		// 			md_num++;
		// 		}
		// 		int result = m_mdApi->SubscribeMarketData(ppInstrumentID, a);
		// 		MdLogger->info((result == 0) ? "订阅行情请求1......发送成功" : "订阅行情请求1......发送失败，错误序号=[%d]", result);
		// 	}
		// 	else if (count1 = md_InstrumentID.size() / 500)
		// 	{
		// 		int count2 = 0;
		// 		for (count2; count2 < md_InstrumentID.size() % 500; count2++)
		// 		{
		// 			ppInstrumentID[count2] = const_cast<char *>(md_InstrumentID.at(md_num).c_str());
		// 			md_num++;
		// 		}
		// 		int result = m_mdApi->SubscribeMarketData(ppInstrumentID, count2);
		// 		MdLogger->info((result == 0) ? "订阅行情请求2......发送成功" : "订阅行情请求2......发送失败，错误序号=[%d]", result);
		// 	}
		// }
		

		//int result = m_mdApi->SubscribeMarketData(ppInstrumentID, 1);
	}

	///订阅行情应答
	void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
	{
		MdLogger->info("<OnRspSubMarketData>");
		if (pSpecificInstrument)
		{
			MdLogger->info("InstrumentID = [{}]", pSpecificInstrument->InstrumentID);
		}
		if (pRspInfo)
		{
			MdLogger->info("ErrorMsg = [{}]", pRspInfo->ErrorMsg);
			MdLogger->info("ErrorID = [{}]", pRspInfo->ErrorID);
		}
		MdLogger->info("nRequestID = [{}]", nRequestID);
		MdLogger->info("bIsLast = [{}]", bIsLast);
		MdLogger->info("</OnRspSubMarketData>");
	}

    void MdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
    }

    ///深度行情通知
	void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
	{
		
		// MdLogger->info("<OnRtnDepthMarketData>");
		if (pDepthMarketData)
		{
			// MdLogger->info("InstrumentID = [{}]", pDepthMarketData->InstrumentID);
			// MdLogger->info("ExchangeID = [{}]", pDepthMarketData->ExchangeID);
			// MdLogger->info("LastPrice = [{}]", pDepthMarketData->LastPrice);
			// MdLogger->info("PreSettlementPrice = [{}]", pDepthMarketData->PreSettlementPrice);
			// MdLogger->info("OpenPrice = [{}]", pDepthMarketData->OpenPrice);
			// MdLogger->info("Volume = [{}]", pDepthMarketData->Volume);
			// MdLogger->info("Turnover = [{}]", pDepthMarketData->Turnover);
			// MdLogger->info("OpenInterest = [{}]", pDepthMarketData->OpenInterest);
			static int abc(0);
			if(abc%100 == 0)
			cout << abc <<"品种:" << pDepthMarketData->InstrumentID << "      价格:" << pDepthMarketData->LastPrice<< endl;
			++abc;
			
			auto itr = tick->find((string)pDepthMarketData->InstrumentID);
			if(itr != tick->end()){
				if(tick->at((string)pDepthMarketData->InstrumentID).back().Volume > pDepthMarketData->Volume)return;
				tick->at((string)pDepthMarketData->InstrumentID).push_back(*pDepthMarketData);


			}else{
				vector<CThostFtdcDepthMarketDataField> _vec;
				_vec.reserve(VECTER_TICK_RELESE_NUM);
				tick->insert(make_pair((string)pDepthMarketData->InstrumentID,_vec));
				tick->at((string)pDepthMarketData->InstrumentID).push_back(*pDepthMarketData);

			}
			
		}
		//int raise(1);
		// MdLogger->info("</OnRtnDepthMarketData>");
	}

	///订阅询价请求
	void MdSpi::SubscribeForQuoteRsp()
	{
		// MdLogger->info("行情中订阅询价请求");
		// char **ppInstrumentID = new char*[50];
		// string g_chInstrumentID = getConfig("config", "InstrumentID");
		// ppInstrumentID[0] = const_cast<char *>(g_chInstrumentID.c_str());
		// int result = m_mdApi->SubscribeForQuoteRsp(ppInstrumentID, 1);
	}

	///订阅询价应答
	void MdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
	{
		// MdLogger->info("<OnRspSubForQuoteRsp>");
		// if (pSpecificInstrument)
		// {
		// 	MdLogger->info("InstrumentID = [%s]", pSpecificInstrument->InstrumentID);
		// }
		// if (pRspInfo)
		// {
		// 	MdLogger->info("ErrorMsg = [%s]", pRspInfo->ErrorMsg);
		// 	MdLogger->info("ErrorID = [%d]", pRspInfo->ErrorID);
		// }
		// MdLogger->info("nRequestID = [%d]", nRequestID);
		// MdLogger->info("bIsLast = [%d]", bIsLast);
		// MdLogger->info("</OnRspSubForQuoteRsp>");
		
	}

    void MdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
    {
    }

    ///询价通知
	void MdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
	{
		// MdLogger->info("<OnRtnForQuoteRsp>");
		// if (pForQuoteRsp)
		// {
		// 	MdLogger->info("TradingDay = [%s]", pForQuoteRsp->TradingDay);
		// 	MdLogger->info("InstrumentID = [%s]", pForQuoteRsp->InstrumentID);
		// 	MdLogger->info("ForQuoteSysID = [%s]", pForQuoteRsp->ForQuoteSysID);
		// 	MdLogger->info("ForQuoteTime = [%s]", pForQuoteRsp->ForQuoteTime);
		// 	MdLogger->info("ActionDay = [%s]", pForQuoteRsp->ActionDay);
		// 	MdLogger->info("ExchangeID = [%s]", pForQuoteRsp->ExchangeID);
		// }
		// MdLogger->info("</OnRtnForQuoteRsp>");
		
	}
 

 void MdSpi::MdSpi::connect()
 {
    //创建并初始化API
    MdLogger->info("初始化行情！！！");
	//第二个参数描述是否使用 UDP 传输模式，true 表示使用 UDP 模式，false 表示使用 TCP 模式。
//如果开发者要使用组播行情，则需要使用该函数的第三个参数 bIsMulticast。在第二个参数和第三个参数赋值
//均为 true 时，行情将以组播的形式传输。
   m_mdApi = CThostFtdcMdApi::CreateFtdcMdApi("./flow/mdfile/", 1, 1);
   MdLogger->info("注册Spi！！！，UPD模式");
   m_mdApi->RegisterSpi(this);
   MdLogger->info("行情地址：{}！！！","tcp://192.169.1.0:41413");
   //cout << "智能指针" << md_Acc->hqAdd << endl;
   m_mdApi->RegisterFront(const_cast<char*>((md_Acc->hqAdd).c_str()));
   MdLogger->info("脱离主线程！！！");
   m_mdApi->Init();
 }

 vector<string> MdSpi::myReadSymbol(string &path)
 {
	vector<string> _strSymbol;
    ifstream readFile;
	readFile.open(path, ios::in);
 
	if (readFile.is_open())
	{
		cout << "文件打开成功！" << endl;
		char buff[1000] = { 0 };
		while (readFile >>buff)
		{
			_strSymbol.push_back(buff);
			cout << buff << endl;
		}
	}
	else
	{
		cout << "文件打开失败！" << endl;
	}
 
	readFile.close();
	cout << "读取的订阅元素 "<<_strSymbol.size() << endl;
   return _strSymbol;
 }
