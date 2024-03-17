#include "TdSpi.h"

auto tdLogger = spdlog::basic_logger_mt("td_log", "./logs/td_log.txt");
static int td_reqNum{0};


TdSpi::TdSpi(Acc_ptr& m_acc,ptrTdDataInfo & m_tdInfo):td_acc(m_acc),ptrTdData(m_tdInfo)
{
    cout << "这里是交易线程创建"<< endl;
	sendOrderAllow = 0;
    tdstart();
}

TdSpi::~TdSpi()
{

    cout << "这里是交易线程 Break"<< endl;
}

void TdSpi::xd(orderData &senOrd)
{
	sendOrder(senOrd);
}

 bool TdSpi::getSedordAllow()
{
    return sendOrderAllow;
}
void TdSpi::setSedordAllowTrue(){
	if(!(getSedordAllow())){
		sendOrderAllow = 1;
	}
	
}
void TdSpi::tdstart()
{
	tdLogger->info("创建交易线程㺃");
    td_Api = CThostFtdcTraderApi::CreateFtdcTraderApi("./flow/tdfile/");
    tdLogger->info("注册spi");
    td_Api->RegisterSpi(this);
	tdLogger->info("交易地址：{}",td_acc->jyAdd);
    td_Api->RegisterFront(const_cast<char*>(td_acc->jyAdd.c_str()));
    td_Api->SubscribePrivateTopic(THOST_TERT_QUICK);
    td_Api->SubscribePublicTopic(THOST_TERT_QUICK);
	tdLogger->info("脱离主线程");
    td_Api->Init();
	tdLogger->flush();
	cout << "版本号 "<<td_Api->GetApiVersion()<<endl;
        
   
}



void TdSpi::OnFrontConnected()
{
	static int numon(0);
	cout <<"前置连接" << ++numon << endl;
	tdLogger->info("<OnFrontConnected>");
	tdLogger->info("交易线程登陆前连接返回");
	tdLogger->info("</OnFrontConnected>");
	tdLogger->flush();
	//sleep(3);
	
	ReqAuthenticate();
	//reqUserLogin();
};

void TdSpi::OnFrontDisconnected(int nReason)
{
	tdLogger->info("<OnFrontDisconnected>");
	tdLogger->info("交易线程登陆前连接错误返回  nReason {}",nReason);
	tdLogger->info("");
	tdLogger->info("</OnFrontDisconnected>");
	tdLogger->flush();
}

void TdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	tdLogger->info("<OnHeartBeatWarning>");
	tdLogger->info("网络断开了，才会有nTimeLapse {}",nTimeLapse);
	tdLogger->info("</OnHeartBeatWarning>");
	//tdLogger->info("</OnFrontDisconnected>");
	tdLogger->flush();
}
void TdSpi::ReqAuthenticate()
{

	CThostFtdcReqAuthenticateField a = { 0 };
	

   strcpy(a.BrokerID, td_acc->brkId.c_str());
	strcpy(a.UserID, td_acc->userId.c_str());
	strcpy(a.UserProductInfo, td_acc->prodInfo.c_str());
	strcpy(a.AuthCode, td_acc->auCode.c_str()); 
	strcpy(a.AppID, td_acc->appId.c_str());

	tdLogger->info("认证客户端，USER {}，BrokerID {}",td_acc->userId,td_acc->brkId);tdLogger->flush();
	//cout << "huqou banben " << td_Api->GetApiVersion() << endl;


    int res = td_Api->ReqAuthenticate(&a, ++td_reqNum);
	if(res == 0){
		tdLogger->info("认证客户端请求发送成功");
		tdLogger->flush();
	}else{
		tdLogger->info("认证客户端请求认证失败");
	 	tdLogger->flush();
		sleep(3);
		exit(-1001);
	}
	
}

void TdSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspAuthenticate>");
	if (pRspAuthenticateField)
	{
		tdLogger->info("BrokerID {}",pRspAuthenticateField->BrokerID);
		tdLogger->info("UserID {}",pRspAuthenticateField->UserID);
		tdLogger->info("UserProductInfo {}",pRspAuthenticateField->UserProductInfo);
		tdLogger->info("AppID {}",pRspAuthenticateField->AppID);
		tdLogger->info("AppType {}",pRspAuthenticateField->AppType);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",GB2312_to_UTF8(pRspInfo->ErrorMsg));
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspAuthenticate>");
	tdLogger->info("准备登陆");
	tdLogger->flush();
	sleep(1);
	reqUserLogin();

};
void TdSpi::reqUserLogin()
{
	CThostFtdcReqUserLoginField reqUserLogin;// = { 0 };
	memset(&reqUserLogin, 0, sizeof(reqUserLogin));
   strcpy(reqUserLogin.BrokerID, td_acc->brkId.c_str());
   strcpy(reqUserLogin.UserID, td_acc->userId.c_str());
   strcpy(reqUserLogin.Password, td_acc->psd.c_str()); 
   tdLogger->info("1{}",reqUserLogin.BrokerID);
   tdLogger->info("2{}",reqUserLogin.UserID);
   tdLogger->info("3{}",reqUserLogin.Password);
   cout << "hahahah" << td_Api->GetApiVersion()<< endl;
   int res = td_Api->ReqUserLogin(&reqUserLogin, ++td_reqNum);
   if(res == 0)
   {
		tdLogger->info("交易线程登陆 请求发送成功");
		tdLogger->flush();
		
	}else{
		tdLogger->info("登陆 请求发送失败");
		tdLogger->flush();
		sleep(3);
		exit(-1002);
	}
}



void TdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspUserLogin>");
	if (pRspUserLogin)
	{
		// tdLogger->info("TradingDay {}",pRspUserLogin->TradingDay);
		// tdLogger->info("LoginTime {}",pRspUserLogin->LoginTime);
		// tdLogger->info("BrokerID {}",pRspUserLogin->BrokerID);
		// tdLogger->info("UserID {}",pRspUserLogin->UserID);
		// tdLogger->info("SystemName {}",pRspUserLogin->SystemName);
		// tdLogger->info("MaxOrderRef {}",pRspUserLogin->MaxOrderRef);
		// tdLogger->info("SHFETime {}",pRspUserLogin->SHFETime);
		// tdLogger->info("DCETime {}",pRspUserLogin->DCETime);
		// tdLogger->info("CZCETime {}",pRspUserLogin->CZCETime);
		// tdLogger->info("FFEXTime {}",pRspUserLogin->FFEXTime);
		// tdLogger->info("INETime {}",pRspUserLogin->INETime);
		// tdLogger->info("FrontID {}",pRspUserLogin->FrontID);
		// tdLogger->info("SessionID {}",pRspUserLogin->SessionID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	//登录成功之后，响应函数 OnRspUserLogin 中的参数 pRspUserLogin 中包含了前置编号（FrontID），会话编
//号（SessionID），最大报单编号（MaxOrderRef）。
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspUserLogin>");
	tdLogger->info("登陆认证完成，前置编号（FrontID）{}，会话编号（SessionID）{}，最大报单编号（MaxOrderRef）{}。",pRspUserLogin->FrontID,pRspUserLogin->SessionID,pRspUserLogin->MaxOrderRef);
	tdLogger->flush();
	ReqQryInstrument();
	sleep(2);
	//ReqQrySettlementInfo();
};


void TdSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspUserLogout>");
	if (pUserLogout)
	{
		tdLogger->info("BrokerID {}",pUserLogout->BrokerID);
		tdLogger->info("UserID {}",pUserLogout->UserID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspUserLogout>");
};

void TdSpi::OnRspUserPasswordUpdate(CThostFtdcUserPasswordUpdateField *pUserPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspUserPasswordUpdate>");
	if (pUserPasswordUpdate)
	{
		tdLogger->info("BrokerID {}",pUserPasswordUpdate->BrokerID);
		tdLogger->info("UserID {}",pUserPasswordUpdate->UserID);
		tdLogger->info("OldPassword {}",pUserPasswordUpdate->OldPassword);
		tdLogger->info("NewPassword {}",pUserPasswordUpdate->NewPassword);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspUserPasswordUpdate>");
};

void TdSpi::OnRspTradingAccountPasswordUpdate(CThostFtdcTradingAccountPasswordUpdateField *pTradingAccountPasswordUpdate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspTradingAccountPasswordUpdate>");
	if (pTradingAccountPasswordUpdate)
	{
		tdLogger->info("BrokerID {}",pTradingAccountPasswordUpdate->BrokerID);
		tdLogger->info("AccountID {}",pTradingAccountPasswordUpdate->AccountID);
		tdLogger->info("OldPassword {}",pTradingAccountPasswordUpdate->OldPassword);
		tdLogger->info("NewPassword {}",pTradingAccountPasswordUpdate->NewPassword);
		tdLogger->info("CurrencyID {}",pTradingAccountPasswordUpdate->CurrencyID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspTradingAccountPasswordUpdate>");
};

void TdSpi::OnRspUserAuthMethod(CThostFtdcRspUserAuthMethodField *pRspUserAuthMethod, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspUserAuthMethod>");
	if (pRspUserAuthMethod)
	{
		tdLogger->info("UsableAuthMethod {}",pRspUserAuthMethod->UsableAuthMethod);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspUserAuthMethod>");
};

void TdSpi::OnRspGenUserCaptcha(CThostFtdcRspGenUserCaptchaField *pRspGenUserCaptcha, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspGenUserCaptcha>");
	if (pRspGenUserCaptcha)
	{
		tdLogger->info("BrokerID {}",pRspGenUserCaptcha->BrokerID);
		tdLogger->info("UserID {}",pRspGenUserCaptcha->UserID);
		tdLogger->info("CaptchaInfo {}",pRspGenUserCaptcha->CaptchaInfo);
		tdLogger->info("CaptchaInfoLen {}",pRspGenUserCaptcha->CaptchaInfoLen);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspGenUserCaptcha>");
};

void TdSpi::OnRspGenUserText(CThostFtdcRspGenUserTextField *pRspGenUserText, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspGenUserText>");
	if (pRspGenUserText)
	{
		tdLogger->info("UserTextSeq {}",pRspGenUserText->UserTextSeq);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspGenUserText>");
};
void TdSpi::sendOrder(orderData & senOrd)
{
	//ptrTdData->SendOrderSimple.push_back(senOrd);
	//限价单:
CThostFtdcInputOrderField ord = { 0 };
strcpy(ord.BrokerID, td_acc->brkId.c_str());
strcpy(ord.InvestorID, td_acc->inId.c_str()); 
//strcpy(ord.ExchangeID, “SHFE”);
strcpy(ord.InstrumentID, senOrd.ord_symbol.c_str());
strcpy(ord.UserID,td_acc->userId.c_str());
ord.OrderPriceType = THOST_FTDC_OPT_LimitPrice;//限价
switch(senOrd.ord_kpdk){
    case OR_KD :
	ord.Direction = THOST_FTDC_D_Buy;//买
    ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//开
	break;

	case OR_KK :
    ord.Direction = THOST_FTDC_D_Sell;//卖
    ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//开
	break;

	case OR_PD :
	ord.Direction = THOST_FTDC_D_Buy;//卖
    ord.CombOffsetFlag[0] = THOST_FTDC_OF_Close;//平
	break;

	case OR_PK :
    ord.Direction = THOST_FTDC_D_Sell;//卖
    ord.CombOffsetFlag[0] = THOST_FTDC_OF_Close;//平
	break;



}
//ord.Direction = THOST_FTDC_D_Buy;//买
//ord.CombOffsetFlag[0] = THOST_FTDC_OF_Open;//开
ord.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;//投机
ord.LimitPrice = senOrd.ord_price;
ord.VolumeTotalOriginal = senOrd.ord_lots;
ord.TimeCondition = THOST_FTDC_TC_GFD;///当日有效
ord.VolumeCondition = THOST_FTDC_VC_AV;///任意数量
ord.MinVolume = 1;
ord.ContingentCondition = THOST_FTDC_CC_Immediately;
ord.StopPrice = 0;
ord.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
ord.IsAutoSuspend = 0;
int res = td_Api->ReqOrderInsert(&ord, ++td_reqNum);

if(!res){
	tdLogger->info("下单成功，品种{} ，价格{} ， 手数 {}，",senOrd.ord_symbol,senOrd.ord_price,senOrd.ord_lots);
	tdLogger->flush();
	//ptrTdData->SendOrderData.push_back(ord);
}


}

void TdSpi::checkOrder(string & symbolName,string &ordSysid)
{
//第一种方法，使用OrderSysID撤单（推荐使用）

CThostFtdcInputOrderActionField a = { 0 };
strcpy(a.BrokerID, td_acc->brkId.c_str());
strcpy(a.InvestorID,  td_acc->inId.c_str());
strcpy(a.UserID,  td_acc->userId.c_str());
strcpy(a.OrderSysID, ordSysid.c_str());  //对应要撤报单的OrderSysID
string _exch;
if(!(getSymbolExchange(symbolName,_exch))){
strcpy(a.ExchangeID, _exch.c_str());
}

strcpy(a.InstrumentID, symbolName.c_str());
a.ActionFlag = THOST_FTDC_AF_Delete;
int r = td_Api->ReqOrderAction(&a, ++td_reqNum);
if(r){
	tdLogger->info("撤单申请发送 失败，序号【{}】,品种【{}】，OrderSysID 【{}】",td_reqNum,symbolName,ordSysid);
	tdLogger->flush();
}else{

	tdLogger->info("撤单申请发送 成功，序号【{}】,品种【{}】，OrderSysID 【{}】",td_reqNum,symbolName,ordSysid);
	tdLogger->flush();
}
}

void TdSpi::checkOrder(orderActionData &ordactData)
{

	//第二种方法，使用FrontID+SessionID+OrderRef撤单

// CThostFtdcInputOrderActionField a = { 0 };
// strcpy(a.BrokerID, "9999");
// strcpy(a.InvestorID, "1000001");
// strcpy(a.UserID, "1000001");
// a.FrontID = 1;   //对应要撤报单的FrontID
// a.SessionID = -788541;   //对应要撤报单的sessionid
// strcpy(a.OrderRef, "        3");   //对应要撤报单的OrderRef
// strcpy(a.ExchangeID, "SHFE");
// strcpy(a.InstrumentID, "rb1809");
// ActionFlag = THOST_FTDC_AF_Delete;
// m_pUserApi->ReqOrderAction(&a, nRequestID++);
cout << "暂不启用此方法" << endl;
tdLogger->info("暂不启用此方法");
tdLogger->flush();
}

void TdSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspOrderInsert>");
	if (pInputOrder)
	{
		tdLogger->info("BrokerID {}",pInputOrder->BrokerID);
		tdLogger->info("InvestorID {}",pInputOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pInputOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pInputOrder->OrderRef);
		tdLogger->info("UserID {}",pInputOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pInputOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pInputOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pInputOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pInputOrder->BusinessUnit);
		tdLogger->info("ExchangeID {}",pInputOrder->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pInputOrder->AccountID);
		tdLogger->info("CurrencyID {}",pInputOrder->CurrencyID);
		tdLogger->info("ClientID {}",pInputOrder->ClientID);
		tdLogger->info("IPAddress {}",pInputOrder->IPAddress);
		tdLogger->info("MacAddress {}",pInputOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pInputOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pInputOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pInputOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pInputOrder->RequestID);
		tdLogger->info("UserForceClose {}",pInputOrder->UserForceClose);
		tdLogger->info("IsSwapOrder {}",pInputOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pInputOrder->OrderPriceType);
		tdLogger->info("Direction {}",pInputOrder->Direction);
		tdLogger->info("TimeCondition {}",pInputOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pInputOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pInputOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pInputOrder->ForceCloseReason);
		tdLogger->info("LimitPrice {}",pInputOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pInputOrder->StopPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspOrderInsert>");
};

void TdSpi::OnRspParkedOrderInsert(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspParkedOrderInsert>");
	if (pParkedOrder)
	{
		tdLogger->info("BrokerID {}",pParkedOrder->BrokerID);
		tdLogger->info("InvestorID {}",pParkedOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pParkedOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pParkedOrder->OrderRef);
		tdLogger->info("UserID {}",pParkedOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pParkedOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pParkedOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pParkedOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pParkedOrder->BusinessUnit);
		tdLogger->info("ExchangeID {}",pParkedOrder->ExchangeID);
		tdLogger->info("ParkedOrderID {}",pParkedOrder->ParkedOrderID);
		tdLogger->info("ErrorMsg {}",pParkedOrder->ErrorMsg);
		tdLogger->info("AccountID {}",pParkedOrder->AccountID);
		tdLogger->info("CurrencyID {}",pParkedOrder->CurrencyID);
		tdLogger->info("ClientID {}",pParkedOrder->ClientID);
		tdLogger->info("InvestUnitID {}",pParkedOrder->InvestUnitID);
		tdLogger->info("IPAddress {}",pParkedOrder->IPAddress);
		tdLogger->info("MacAddress {}",pParkedOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pParkedOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pParkedOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pParkedOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pParkedOrder->RequestID);
		tdLogger->info("UserForceClose {}",pParkedOrder->UserForceClose);
		tdLogger->info("ErrorID {}",pParkedOrder->ErrorID);
		tdLogger->info("IsSwapOrder {}",pParkedOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pParkedOrder->OrderPriceType);
		tdLogger->info("Direction {}",pParkedOrder->Direction);
		tdLogger->info("TimeCondition {}",pParkedOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pParkedOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pParkedOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pParkedOrder->ForceCloseReason);
		tdLogger->info("UserType {}",pParkedOrder->UserType);
		tdLogger->info("Status {}",pParkedOrder->Status);
		tdLogger->info("LimitPrice {}",pParkedOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pParkedOrder->StopPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspParkedOrderInsert>");
};

void TdSpi::OnRspParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspParkedOrderAction>");
	if (pParkedOrderAction)
	{
		tdLogger->info("BrokerID {}",pParkedOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pParkedOrderAction->InvestorID);
		tdLogger->info("OrderRef {}",pParkedOrderAction->OrderRef);
		tdLogger->info("ExchangeID {}",pParkedOrderAction->ExchangeID);
		tdLogger->info("OrderSysID {}",pParkedOrderAction->OrderSysID);
		tdLogger->info("UserID {}",pParkedOrderAction->UserID);
		tdLogger->info("InstrumentID {}",pParkedOrderAction->InstrumentID);
		tdLogger->info("ParkedOrderActionID {}",pParkedOrderAction->ParkedOrderActionID);
		tdLogger->info("ErrorMsg {}",pParkedOrderAction->ErrorMsg);
		tdLogger->info("InvestUnitID {}",pParkedOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pParkedOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pParkedOrderAction->MacAddress);
		tdLogger->info("OrderActionRef {}",pParkedOrderAction->OrderActionRef);
		tdLogger->info("RequestID {}",pParkedOrderAction->RequestID);
		tdLogger->info("FrontID {}",pParkedOrderAction->FrontID);
		tdLogger->info("SessionID {}",pParkedOrderAction->SessionID);
		tdLogger->info("VolumeChange {}",pParkedOrderAction->VolumeChange);
		tdLogger->info("ErrorID {}",pParkedOrderAction->ErrorID);
		tdLogger->info("ActionFlag {}",pParkedOrderAction->ActionFlag);
		tdLogger->info("UserType {}",pParkedOrderAction->UserType);
		tdLogger->info("Status {}",pParkedOrderAction->Status);
		tdLogger->info("LimitPrice {}",pParkedOrderAction->LimitPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspParkedOrderAction>");
};
int TdSpi::getSymbolExchange(string &symbolName,string & symbolExc)
{
	try
	{
		/* code */
		for(auto itr : ptrTdData->InstrumentData){
			if(itr.InstrumentID == symbolName.c_str()){
				symbolExc = itr.ExchangeID;
				return 0;
			}
		}
		return -1;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return -2;
	}
	
    return 0;
}
// void TdSpi::cancelOrder(string &ordSysId)
//{
// 	//第一种方法，使用OrderSysID撤单（推荐使用）

// CThostFtdcInputOrderActionField a = { 0 };
// strcpy(a.BrokerID, td_acc->brkId.c_str());
// strcpy(a.InvestorID,  td_acc->inId.c_str());
// strcpy(a.UserID, td_acc->userId.c_str());
// strcpy(a.OrderSysID, ordSysId.c_str());  //对应要撤报单的OrderSysID
// string _exch;
// if(getSymbolExchange()){

// }
// //strcpy(a.ExchangeID, );
// strcpy(a.InstrumentID, td_acc->brkId.c_str());
// a.ActionFlag = THOST_FTDC_AF_Delete;
// td_Api->ReqOrderAction(&a, ++td_reqNum);


// //第二种方法，使用FrontID+SessionID+OrderRef撤单

// CThostFtdcInputOrderActionField a = { 0 };
// strcpy(a.BrokerID, "9999");
// strcpy(a.InvestorID, "1000001");
// strcpy(a.UserID, "1000001");
// a.FrontID = 1;   //对应要撤报单的FrontID
// a.SessionID = -788541;   //对应要撤报单的sessionid
// strcpy(a.OrderRef, "        3");   //对应要撤报单的OrderRef
// strcpy(a.ExchangeID, "SHFE");
// strcpy(a.InstrumentID, "rb1809");
// ActionFlag = THOST_FTDC_AF_Delete;
// m_pUserApi->ReqOrderAction(&a, nRequestID++);
//}
void TdSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspOrderAction>");
	if (pInputOrderAction)
	{
		tdLogger->info("BrokerID {}",pInputOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputOrderAction->InvestorID);
		tdLogger->info("OrderRef {}",pInputOrderAction->OrderRef);
		tdLogger->info("ExchangeID {}",pInputOrderAction->ExchangeID);
		tdLogger->info("OrderSysID {}",pInputOrderAction->OrderSysID);
		tdLogger->info("UserID {}",pInputOrderAction->UserID);
		tdLogger->info("InstrumentID {}",pInputOrderAction->InstrumentID);
		tdLogger->info("InvestUnitID {}",pInputOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pInputOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputOrderAction->MacAddress);
		tdLogger->info("OrderActionRef {}",pInputOrderAction->OrderActionRef);
		tdLogger->info("RequestID {}",pInputOrderAction->RequestID);
		tdLogger->info("FrontID {}",pInputOrderAction->FrontID);
		tdLogger->info("SessionID {}",pInputOrderAction->SessionID);
		tdLogger->info("VolumeChange {}",pInputOrderAction->VolumeChange);
		tdLogger->info("ActionFlag {}",pInputOrderAction->ActionFlag);
		tdLogger->info("LimitPrice {}",pInputOrderAction->LimitPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspOrderAction>");
};

// void TdSpi::OnRspQueryMaxOrderVolume(CThostFtdcQueryMaxOrderVolumeField *pQueryMaxOrderVolume, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
// {
// 	tdLogger->info("<OnRspQueryMaxOrderVolume>");
// 	if (pQueryMaxOrderVolume)
// 	{
// 		tdLogger->info("BrokerID {}",pQueryMaxOrderVolume->BrokerID);
// 		tdLogger->info("InvestorID {}",pQueryMaxOrderVolume->InvestorID);
// 		tdLogger->info("InstrumentID {}",pQueryMaxOrderVolume->InstrumentID);
// 		tdLogger->info("ExchangeID {}",pQueryMaxOrderVolume->ExchangeID);
// 		tdLogger->info("InvestUnitID {}",pQueryMaxOrderVolume->InvestUnitID);
// 		tdLogger->info("MaxVolume {}",pQueryMaxOrderVolume->MaxVolume);
// 		tdLogger->info("Direction {}",pQueryMaxOrderVolume->Direction);
// 		tdLogger->info("OffsetFlag {}",pQueryMaxOrderVolume->OffsetFlag);
// 		tdLogger->info("HedgeFlag {}",pQueryMaxOrderVolume->HedgeFlag);
// 	}
// 	if (pRspInfo)
// 	{
// 		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
// 		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
// 	}
// 	tdLogger->info("nRequestID {}",nRequestID);
// 	tdLogger->info("bIsLast {}",bIsLast);
// 	tdLogger->info("</OnRspQueryMaxOrderVolume>");
// };
void TdSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcQrySettlementInfoConfirmField a = { 0 };
	strcpy(a.BrokerID, td_acc->brkId.c_str());
	strcpy(a.InvestorID, td_acc->inId.c_str());
	//strcpy(a.TradingDay,“20180101”);
	int res = td_Api->ReqQrySettlementInfoConfirm(&a, ++td_reqNum);
	
   if(res == 0)
   {
		tdLogger->info("请求确认结算单 请求发送成功");
		tdLogger->flush();
	}else{
		tdLogger->info("请求确认结算单 请求发送失败");
		tdLogger->flush();
		sleep(3);
		exit(-1004);
	}
}
void TdSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspSettlementInfoConfirm>");
	if (pSettlementInfoConfirm)
	{
		tdLogger->info("BrokerID {}",pSettlementInfoConfirm->BrokerID);
		tdLogger->info("InvestorID {}",pSettlementInfoConfirm->InvestorID);
		tdLogger->info("ConfirmDate {}",pSettlementInfoConfirm->ConfirmDate);
		tdLogger->info("ConfirmTime {}",pSettlementInfoConfirm->ConfirmTime);
		tdLogger->info("AccountID {}",pSettlementInfoConfirm->AccountID);
		tdLogger->info("CurrencyID {}",pSettlementInfoConfirm->CurrencyID);
		tdLogger->info("SettlementID {}",pSettlementInfoConfirm->SettlementID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspSettlementInfoConfirm>");
	tdLogger->info("准备确认结算单信息  confirm");
	tdLogger->flush();
	if(bIsLast){
	sleep(1);
	//ReqSettlementInfoConfirm();
	ReqQryInvestorPosition();
	}

};

void TdSpi::OnRspRemoveParkedOrder(CThostFtdcRemoveParkedOrderField *pRemoveParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspRemoveParkedOrder>");
	if (pRemoveParkedOrder)
	{
		tdLogger->info("BrokerID {}",pRemoveParkedOrder->BrokerID);
		tdLogger->info("InvestorID {}",pRemoveParkedOrder->InvestorID);
		tdLogger->info("ParkedOrderID {}",pRemoveParkedOrder->ParkedOrderID);
		tdLogger->info("InvestUnitID {}",pRemoveParkedOrder->InvestUnitID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspRemoveParkedOrder>");
};

void TdSpi::OnRspRemoveParkedOrderAction(CThostFtdcRemoveParkedOrderActionField *pRemoveParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspRemoveParkedOrderAction>");
	if (pRemoveParkedOrderAction)
	{
		tdLogger->info("BrokerID {}",pRemoveParkedOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pRemoveParkedOrderAction->InvestorID);
		tdLogger->info("ParkedOrderActionID {}",pRemoveParkedOrderAction->ParkedOrderActionID);
		tdLogger->info("InvestUnitID {}",pRemoveParkedOrderAction->InvestUnitID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspRemoveParkedOrderAction>");
};

void TdSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspExecOrderInsert>");
	if (pInputExecOrder)
	{
		tdLogger->info("BrokerID {}",pInputExecOrder->BrokerID);
		tdLogger->info("InvestorID {}",pInputExecOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pInputExecOrder->InstrumentID);
		tdLogger->info("ExecOrderRef {}",pInputExecOrder->ExecOrderRef);
		tdLogger->info("UserID {}",pInputExecOrder->UserID);
		tdLogger->info("BusinessUnit {}",pInputExecOrder->BusinessUnit);
		tdLogger->info("ExchangeID {}",pInputExecOrder->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputExecOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pInputExecOrder->AccountID);
		tdLogger->info("CurrencyID {}",pInputExecOrder->CurrencyID);
		tdLogger->info("ClientID {}",pInputExecOrder->ClientID);
		tdLogger->info("IPAddress {}",pInputExecOrder->IPAddress);
		tdLogger->info("MacAddress {}",pInputExecOrder->MacAddress);
		tdLogger->info("Volume {}",pInputExecOrder->Volume);
		tdLogger->info("RequestID {}",pInputExecOrder->RequestID);
		tdLogger->info("OffsetFlag {}",pInputExecOrder->OffsetFlag);
		tdLogger->info("HedgeFlag {}",pInputExecOrder->HedgeFlag);
		tdLogger->info("ActionType {}",pInputExecOrder->ActionType);
		tdLogger->info("PosiDirection {}",pInputExecOrder->PosiDirection);
		tdLogger->info("ReservePositionFlag {}",pInputExecOrder->ReservePositionFlag);
		tdLogger->info("CloseFlag {}",pInputExecOrder->CloseFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspExecOrderInsert>");
};

void TdSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInputExecOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspExecOrderAction>");
	if (pInputExecOrderAction)
	{
		tdLogger->info("BrokerID {}",pInputExecOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputExecOrderAction->InvestorID);
		tdLogger->info("ExecOrderRef {}",pInputExecOrderAction->ExecOrderRef);
		tdLogger->info("ExchangeID {}",pInputExecOrderAction->ExchangeID);
		tdLogger->info("ExecOrderSysID {}",pInputExecOrderAction->ExecOrderSysID);
		tdLogger->info("UserID {}",pInputExecOrderAction->UserID);
		tdLogger->info("InstrumentID {}",pInputExecOrderAction->InstrumentID);
		tdLogger->info("InvestUnitID {}",pInputExecOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pInputExecOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputExecOrderAction->MacAddress);
		tdLogger->info("ExecOrderActionRef {}",pInputExecOrderAction->ExecOrderActionRef);
		tdLogger->info("RequestID {}",pInputExecOrderAction->RequestID);
		tdLogger->info("FrontID {}",pInputExecOrderAction->FrontID);
		tdLogger->info("SessionID {}",pInputExecOrderAction->SessionID);
		tdLogger->info("ActionFlag {}",pInputExecOrderAction->ActionFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspExecOrderAction>");
};

void TdSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspForQuoteInsert>");
	if (pInputForQuote)
	{
		tdLogger->info("BrokerID {}",pInputForQuote->BrokerID);
		tdLogger->info("InvestorID {}",pInputForQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pInputForQuote->InstrumentID);
		tdLogger->info("ForQuoteRef {}",pInputForQuote->ForQuoteRef);
		tdLogger->info("UserID {}",pInputForQuote->UserID);
		tdLogger->info("ExchangeID {}",pInputForQuote->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputForQuote->InvestUnitID);
		tdLogger->info("IPAddress {}",pInputForQuote->IPAddress);
		tdLogger->info("MacAddress {}",pInputForQuote->MacAddress);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspForQuoteInsert>");
};

void TdSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQuoteInsert>");
	if (pInputQuote)
	{
		tdLogger->info("BrokerID {}",pInputQuote->BrokerID);
		tdLogger->info("InvestorID {}",pInputQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pInputQuote->InstrumentID);
		tdLogger->info("QuoteRef {}",pInputQuote->QuoteRef);
		tdLogger->info("UserID {}",pInputQuote->UserID);
		tdLogger->info("BusinessUnit {}",pInputQuote->BusinessUnit);
		tdLogger->info("AskOrderRef {}",pInputQuote->AskOrderRef);
		tdLogger->info("BidOrderRef {}",pInputQuote->BidOrderRef);
		tdLogger->info("ForQuoteSysID {}",pInputQuote->ForQuoteSysID);
		tdLogger->info("ExchangeID {}",pInputQuote->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputQuote->InvestUnitID);
		tdLogger->info("ClientID {}",pInputQuote->ClientID);
		tdLogger->info("IPAddress {}",pInputQuote->IPAddress);
		tdLogger->info("MacAddress {}",pInputQuote->MacAddress);
		tdLogger->info("AskVolume {}",pInputQuote->AskVolume);
		tdLogger->info("BidVolume {}",pInputQuote->BidVolume);
		tdLogger->info("RequestID {}",pInputQuote->RequestID);
		tdLogger->info("AskOffsetFlag {}",pInputQuote->AskOffsetFlag);
		tdLogger->info("BidOffsetFlag {}",pInputQuote->BidOffsetFlag);
		tdLogger->info("AskHedgeFlag {}",pInputQuote->AskHedgeFlag);
		tdLogger->info("BidHedgeFlag {}",pInputQuote->BidHedgeFlag);
		tdLogger->info("AskPrice {}",pInputQuote->AskPrice);
		tdLogger->info("BidPrice {}",pInputQuote->BidPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQuoteInsert>");
};

void TdSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInputQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQuoteAction>");
	if (pInputQuoteAction)
	{
		tdLogger->info("BrokerID {}",pInputQuoteAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputQuoteAction->InvestorID);
		tdLogger->info("QuoteRef {}",pInputQuoteAction->QuoteRef);
		tdLogger->info("ExchangeID {}",pInputQuoteAction->ExchangeID);
		tdLogger->info("QuoteSysID {}",pInputQuoteAction->QuoteSysID);
		tdLogger->info("UserID {}",pInputQuoteAction->UserID);
		tdLogger->info("InstrumentID {}",pInputQuoteAction->InstrumentID);
		tdLogger->info("InvestUnitID {}",pInputQuoteAction->InvestUnitID);
		tdLogger->info("ClientID {}",pInputQuoteAction->ClientID);
		tdLogger->info("IPAddress {}",pInputQuoteAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputQuoteAction->MacAddress);
		tdLogger->info("QuoteActionRef {}",pInputQuoteAction->QuoteActionRef);
		tdLogger->info("RequestID {}",pInputQuoteAction->RequestID);
		tdLogger->info("FrontID {}",pInputQuoteAction->FrontID);
		tdLogger->info("SessionID {}",pInputQuoteAction->SessionID);
		tdLogger->info("ActionFlag {}",pInputQuoteAction->ActionFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQuoteAction>");
};

void TdSpi::OnRspBatchOrderAction(CThostFtdcInputBatchOrderActionField *pInputBatchOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspBatchOrderAction>");
	if (pInputBatchOrderAction)
	{
		tdLogger->info("BrokerID {}",pInputBatchOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputBatchOrderAction->InvestorID);
		tdLogger->info("ExchangeID {}",pInputBatchOrderAction->ExchangeID);
		tdLogger->info("UserID {}",pInputBatchOrderAction->UserID);
		tdLogger->info("InvestUnitID {}",pInputBatchOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pInputBatchOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputBatchOrderAction->MacAddress);
		tdLogger->info("OrderActionRef {}",pInputBatchOrderAction->OrderActionRef);
		tdLogger->info("RequestID {}",pInputBatchOrderAction->RequestID);
		tdLogger->info("FrontID {}",pInputBatchOrderAction->FrontID);
		tdLogger->info("SessionID {}",pInputBatchOrderAction->SessionID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspBatchOrderAction>");
};

void TdSpi::OnRspOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspOptionSelfCloseInsert>");
	if (pInputOptionSelfClose)
	{
		tdLogger->info("BrokerID {}",pInputOptionSelfClose->BrokerID);
		tdLogger->info("InvestorID {}",pInputOptionSelfClose->InvestorID);
		tdLogger->info("InstrumentID {}",pInputOptionSelfClose->InstrumentID);
		tdLogger->info("OptionSelfCloseRef {}",pInputOptionSelfClose->OptionSelfCloseRef);
		tdLogger->info("UserID {}",pInputOptionSelfClose->UserID);
		tdLogger->info("BusinessUnit {}",pInputOptionSelfClose->BusinessUnit);
		tdLogger->info("ExchangeID {}",pInputOptionSelfClose->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputOptionSelfClose->InvestUnitID);
		tdLogger->info("AccountID {}",pInputOptionSelfClose->AccountID);
		tdLogger->info("CurrencyID {}",pInputOptionSelfClose->CurrencyID);
		tdLogger->info("ClientID {}",pInputOptionSelfClose->ClientID);
		tdLogger->info("IPAddress {}",pInputOptionSelfClose->IPAddress);
		tdLogger->info("MacAddress {}",pInputOptionSelfClose->MacAddress);
		tdLogger->info("Volume {}",pInputOptionSelfClose->Volume);
		tdLogger->info("RequestID {}",pInputOptionSelfClose->RequestID);
		tdLogger->info("HedgeFlag {}",pInputOptionSelfClose->HedgeFlag);
		tdLogger->info("OptSelfCloseFlag {}",pInputOptionSelfClose->OptSelfCloseFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspOptionSelfCloseInsert>");
};

void TdSpi::OnRspOptionSelfCloseAction(CThostFtdcInputOptionSelfCloseActionField *pInputOptionSelfCloseAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspOptionSelfCloseAction>");
	if (pInputOptionSelfCloseAction)
	{
		tdLogger->info("BrokerID {}",pInputOptionSelfCloseAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputOptionSelfCloseAction->InvestorID);
		tdLogger->info("OptionSelfCloseRef {}",pInputOptionSelfCloseAction->OptionSelfCloseRef);
		tdLogger->info("ExchangeID {}",pInputOptionSelfCloseAction->ExchangeID);
		tdLogger->info("OptionSelfCloseSysID {}",pInputOptionSelfCloseAction->OptionSelfCloseSysID);
		tdLogger->info("UserID {}",pInputOptionSelfCloseAction->UserID);
		tdLogger->info("InstrumentID {}",pInputOptionSelfCloseAction->InstrumentID);
		tdLogger->info("InvestUnitID {}",pInputOptionSelfCloseAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pInputOptionSelfCloseAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputOptionSelfCloseAction->MacAddress);
		tdLogger->info("OptionSelfCloseActionRef {}",pInputOptionSelfCloseAction->OptionSelfCloseActionRef);
		tdLogger->info("RequestID {}",pInputOptionSelfCloseAction->RequestID);
		tdLogger->info("FrontID {}",pInputOptionSelfCloseAction->FrontID);
		tdLogger->info("SessionID {}",pInputOptionSelfCloseAction->SessionID);
		tdLogger->info("ActionFlag {}",pInputOptionSelfCloseAction->ActionFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspOptionSelfCloseAction>");
};

void TdSpi::OnRspCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspCombActionInsert>");
	if (pInputCombAction)
	{
		tdLogger->info("BrokerID {}",pInputCombAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputCombAction->InvestorID);
		tdLogger->info("InstrumentID {}",pInputCombAction->InstrumentID);
		tdLogger->info("CombActionRef {}",pInputCombAction->CombActionRef);
		tdLogger->info("UserID {}",pInputCombAction->UserID);
		tdLogger->info("ExchangeID {}",pInputCombAction->ExchangeID);
		tdLogger->info("IPAddress {}",pInputCombAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputCombAction->MacAddress);
		tdLogger->info("InvestUnitID {}",pInputCombAction->InvestUnitID);
		tdLogger->info("Volume {}",pInputCombAction->Volume);
		tdLogger->info("Direction {}",pInputCombAction->Direction);
		tdLogger->info("CombDirection {}",pInputCombAction->CombDirection);
		tdLogger->info("HedgeFlag {}",pInputCombAction->HedgeFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspCombActionInsert>");
};

void TdSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryOrder>");
	if (pOrder)
	{
		tdLogger->info("BrokerID {}",pOrder->BrokerID);
		tdLogger->info("InvestorID {}",pOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pOrder->OrderRef);
		tdLogger->info("UserID {}",pOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pOrder->BusinessUnit);
		tdLogger->info("OrderLocalID {}",pOrder->OrderLocalID);
		tdLogger->info("ExchangeID {}",pOrder->ExchangeID);
		tdLogger->info("ParticipantID {}",pOrder->ParticipantID);
		tdLogger->info("ClientID {}",pOrder->ClientID);
		tdLogger->info("ExchangeInstID {}",pOrder->ExchangeInstID);
		tdLogger->info("TraderID {}",pOrder->TraderID);
		tdLogger->info("TradingDay {}",pOrder->TradingDay);
		tdLogger->info("OrderSysID {}",pOrder->OrderSysID);
		tdLogger->info("InsertDate {}",pOrder->InsertDate);
		tdLogger->info("InsertTime {}",pOrder->InsertTime);
		tdLogger->info("ActiveTime {}",pOrder->ActiveTime);
		tdLogger->info("SuspendTime {}",pOrder->SuspendTime);
		tdLogger->info("UpdateTime {}",pOrder->UpdateTime);
		tdLogger->info("CancelTime {}",pOrder->CancelTime);
		tdLogger->info("ActiveTraderID {}",pOrder->ActiveTraderID);
		tdLogger->info("ClearingPartID {}",pOrder->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pOrder->UserProductInfo);
		tdLogger->info("StatusMsg {}",pOrder->StatusMsg);
		tdLogger->info("ActiveUserID {}",pOrder->ActiveUserID);
		tdLogger->info("RelativeOrderSysID {}",pOrder->RelativeOrderSysID);
		tdLogger->info("BranchID {}",pOrder->BranchID);
		tdLogger->info("InvestUnitID {}",pOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pOrder->AccountID);
		tdLogger->info("CurrencyID {}",pOrder->CurrencyID);
		tdLogger->info("IPAddress {}",pOrder->IPAddress);
		tdLogger->info("MacAddress {}",pOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pOrder->RequestID);
		tdLogger->info("InstallID {}",pOrder->InstallID);
		tdLogger->info("NotifySequence {}",pOrder->NotifySequence);
		tdLogger->info("SettlementID {}",pOrder->SettlementID);
		tdLogger->info("VolumeTraded {}",pOrder->VolumeTraded);
		tdLogger->info("VolumeTotal {}",pOrder->VolumeTotal);
		tdLogger->info("SequenceNo {}",pOrder->SequenceNo);
		tdLogger->info("FrontID {}",pOrder->FrontID);
		tdLogger->info("SessionID {}",pOrder->SessionID);
		tdLogger->info("UserForceClose {}",pOrder->UserForceClose);
		tdLogger->info("BrokerOrderSeq {}",pOrder->BrokerOrderSeq);
		tdLogger->info("ZCETotalTradedVolume {}",pOrder->ZCETotalTradedVolume);
		tdLogger->info("IsSwapOrder {}",pOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pOrder->OrderPriceType);
		tdLogger->info("Direction {}",pOrder->Direction);
		tdLogger->info("TimeCondition {}",pOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pOrder->ForceCloseReason);
		tdLogger->info("OrderSubmitStatus {}",pOrder->OrderSubmitStatus);
		tdLogger->info("OrderSource {}",pOrder->OrderSource);
		tdLogger->info("OrderStatus {}",pOrder->OrderStatus);
		tdLogger->info("OrderType {}",pOrder->OrderType);
		tdLogger->info("LimitPrice {}",pOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pOrder->StopPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryOrder>");
};

void TdSpi::OnRspQryTrade(CThostFtdcTradeField *pTrade, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryTrade>");
	if (pTrade)
	{
		tdLogger->info("BrokerID {}",pTrade->BrokerID);
		tdLogger->info("InvestorID {}",pTrade->InvestorID);
		tdLogger->info("InstrumentID {}",pTrade->InstrumentID);
		tdLogger->info("OrderRef {}",pTrade->OrderRef);
		tdLogger->info("UserID {}",pTrade->UserID);
		tdLogger->info("ExchangeID {}",pTrade->ExchangeID);
		tdLogger->info("TradeID {}",pTrade->TradeID);
		tdLogger->info("OrderSysID {}",pTrade->OrderSysID);
		tdLogger->info("ParticipantID {}",pTrade->ParticipantID);
		tdLogger->info("ClientID {}",pTrade->ClientID);
		tdLogger->info("ExchangeInstID {}",pTrade->ExchangeInstID);
		tdLogger->info("TradeDate {}",pTrade->TradeDate);
		tdLogger->info("TradeTime {}",pTrade->TradeTime);
		tdLogger->info("TraderID {}",pTrade->TraderID);
		tdLogger->info("OrderLocalID {}",pTrade->OrderLocalID);
		tdLogger->info("ClearingPartID {}",pTrade->ClearingPartID);
		tdLogger->info("BusinessUnit {}",pTrade->BusinessUnit);
		tdLogger->info("TradingDay {}",pTrade->TradingDay);
		tdLogger->info("InvestUnitID {}",pTrade->InvestUnitID);
		tdLogger->info("Volume {}",pTrade->Volume);
		tdLogger->info("SequenceNo {}",pTrade->SequenceNo);
		tdLogger->info("SettlementID {}",pTrade->SettlementID);
		tdLogger->info("BrokerOrderSeq {}",pTrade->BrokerOrderSeq);
		tdLogger->info("Direction {}",pTrade->Direction);
		tdLogger->info("TradingRole {}",pTrade->TradingRole);
		tdLogger->info("OffsetFlag {}",pTrade->OffsetFlag);
		tdLogger->info("HedgeFlag {}",pTrade->HedgeFlag);
		tdLogger->info("TradeType {}",pTrade->TradeType);
		tdLogger->info("PriceSource {}",pTrade->PriceSource);
		tdLogger->info("TradeSource {}",pTrade->TradeSource);
		tdLogger->info("Price {}",pTrade->Price);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryTrade>");
};
void TdSpi::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField a = { 0 };
strcpy(a.BrokerID, td_acc->brkId.c_str());
strcpy(a.InvestorID,  td_acc->inId.c_str());
//strcpy(a.InstrumentID, "rb1809");//不填写合约则返回所有持仓
int res = td_Api->ReqQryInvestorPosition(&a, ++td_reqNum);

	
   if(res == 0)
   {
		tdLogger->info("请求查询 持仓汇总 请求发送成功{}",td_reqNum);
		tdLogger->flush();
	}else{
		tdLogger->info("请求查询 持仓汇总 请求发送失败");
		tdLogger->flush();
		sleep(3);
		exit(-1005);
	}
}

void TdSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspQryInvestorPosition>");
	if (pInvestorPosition)
	{
		tdLogger->info("InstrumentID {}",pInvestorPosition->InstrumentID);
		tdLogger->info("BrokerID {}",pInvestorPosition->BrokerID);
		tdLogger->info("InvestorID {}",pInvestorPosition->InvestorID);
		tdLogger->info("TradingDay {}",pInvestorPosition->TradingDay);
		tdLogger->info("ExchangeID {}",pInvestorPosition->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInvestorPosition->InvestUnitID);
		tdLogger->info("YdPosition {}",pInvestorPosition->YdPosition);
		tdLogger->info("Position {}",pInvestorPosition->Position);
		tdLogger->info("LongFrozen {}",pInvestorPosition->LongFrozen);
		tdLogger->info("ShortFrozen {}",pInvestorPosition->ShortFrozen);
		tdLogger->info("OpenVolume {}",pInvestorPosition->OpenVolume);
		tdLogger->info("CloseVolume {}",pInvestorPosition->CloseVolume);
		tdLogger->info("SettlementID {}",pInvestorPosition->SettlementID);
		tdLogger->info("CombPosition {}",pInvestorPosition->CombPosition);
		tdLogger->info("CombLongFrozen {}",pInvestorPosition->CombLongFrozen);
		tdLogger->info("CombShortFrozen {}",pInvestorPosition->CombShortFrozen);
		tdLogger->info("TodayPosition {}",pInvestorPosition->TodayPosition);
		tdLogger->info("StrikeFrozen {}",pInvestorPosition->StrikeFrozen);
		tdLogger->info("AbandonFrozen {}",pInvestorPosition->AbandonFrozen);
		tdLogger->info("YdStrikeFrozen {}",pInvestorPosition->YdStrikeFrozen);
		tdLogger->info("PosiDirection {}",pInvestorPosition->PosiDirection);
		tdLogger->info("HedgeFlag {}",pInvestorPosition->HedgeFlag);
		tdLogger->info("PositionDate {}",pInvestorPosition->PositionDate);
		tdLogger->info("LongFrozenAmount {}",pInvestorPosition->LongFrozenAmount);
		tdLogger->info("ShortFrozenAmount {}",pInvestorPosition->ShortFrozenAmount);
		tdLogger->info("OpenAmount {}",pInvestorPosition->OpenAmount);
		tdLogger->info("CloseAmount {}",pInvestorPosition->CloseAmount);
		tdLogger->info("PositionCost {}",pInvestorPosition->PositionCost);
		tdLogger->info("PreMargin {}",pInvestorPosition->PreMargin);
		tdLogger->info("UseMargin {}",pInvestorPosition->UseMargin);
		tdLogger->info("FrozenMargin {}",pInvestorPosition->FrozenMargin);
		tdLogger->info("FrozenCash {}",pInvestorPosition->FrozenCash);
		tdLogger->info("FrozenCommission {}",pInvestorPosition->FrozenCommission);
		tdLogger->info("CashIn {}",pInvestorPosition->CashIn);
		tdLogger->info("Commission {}",pInvestorPosition->Commission);
		tdLogger->info("CloseProfit {}",pInvestorPosition->CloseProfit);
		tdLogger->info("PositionProfit {}",pInvestorPosition->PositionProfit);
		tdLogger->info("PreSettlementPrice {}",pInvestorPosition->PreSettlementPrice);
		tdLogger->info("SettlementPrice {}",pInvestorPosition->SettlementPrice);
		tdLogger->info("OpenCost {}",pInvestorPosition->OpenCost);
		tdLogger->info("ExchangeMargin {}",pInvestorPosition->ExchangeMargin);
		tdLogger->info("CloseProfitByDate {}",pInvestorPosition->CloseProfitByDate);
		tdLogger->info("CloseProfitByTrade {}",pInvestorPosition->CloseProfitByTrade);
		tdLogger->info("MarginRateByMoney {}",pInvestorPosition->MarginRateByMoney);
		tdLogger->info("MarginRateByVolume {}",pInvestorPosition->MarginRateByVolume);
		tdLogger->info("StrikeFrozenAmount {}",pInvestorPosition->StrikeFrozenAmount);
		tdLogger->info("PositionCostOffset {}",pInvestorPosition->PositionCostOffset);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInvestorPosition>");
	tdLogger->flush();
	
	if(bIsLast){
		sleep(1);
    ReqQryInvestorPositionDetail();
	}
	
};

void TdSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryTradingAccount>");
	if (pTradingAccount)
	{
		tdLogger->info("BrokerID {}",pTradingAccount->BrokerID);
		tdLogger->info("AccountID {}",pTradingAccount->AccountID);
		tdLogger->info("TradingDay {}",pTradingAccount->TradingDay);
		tdLogger->info("CurrencyID {}",pTradingAccount->CurrencyID);
		tdLogger->info("SettlementID {}",pTradingAccount->SettlementID);
		tdLogger->info("BizType {}",pTradingAccount->BizType);
		tdLogger->info("PreMortgage {}",pTradingAccount->PreMortgage);
		tdLogger->info("PreCredit {}",pTradingAccount->PreCredit);
		tdLogger->info("PreDeposit {}",pTradingAccount->PreDeposit);
		tdLogger->info("PreBalance {}",pTradingAccount->PreBalance);
		tdLogger->info("PreMargin {}",pTradingAccount->PreMargin);
		tdLogger->info("InterestBase {}",pTradingAccount->InterestBase);
		tdLogger->info("Interest {}",pTradingAccount->Interest);
		tdLogger->info("Deposit {}",pTradingAccount->Deposit);
		tdLogger->info("Withdraw {}",pTradingAccount->Withdraw);
		tdLogger->info("FrozenMargin {}",pTradingAccount->FrozenMargin);
		tdLogger->info("FrozenCash {}",pTradingAccount->FrozenCash);
		tdLogger->info("FrozenCommission {}",pTradingAccount->FrozenCommission);
		tdLogger->info("CurrMargin {}",pTradingAccount->CurrMargin);
		tdLogger->info("CashIn {}",pTradingAccount->CashIn);
		tdLogger->info("Commission {}",pTradingAccount->Commission);
		tdLogger->info("CloseProfit {}",pTradingAccount->CloseProfit);
		tdLogger->info("PositionProfit {}",pTradingAccount->PositionProfit);
		tdLogger->info("Balance {}",pTradingAccount->Balance);
		tdLogger->info("Available {}",pTradingAccount->Available);
		tdLogger->info("WithdrawQuota {}",pTradingAccount->WithdrawQuota);
		tdLogger->info("Reserve {}",pTradingAccount->Reserve);
		tdLogger->info("Credit {}",pTradingAccount->Credit);
		tdLogger->info("Mortgage {}",pTradingAccount->Mortgage);
		tdLogger->info("ExchangeMargin {}",pTradingAccount->ExchangeMargin);
		tdLogger->info("DeliveryMargin {}",pTradingAccount->DeliveryMargin);
		tdLogger->info("ExchangeDeliveryMargin {}",pTradingAccount->ExchangeDeliveryMargin);
		tdLogger->info("ReserveBalance {}",pTradingAccount->ReserveBalance);
		tdLogger->info("PreFundMortgageIn {}",pTradingAccount->PreFundMortgageIn);
		tdLogger->info("PreFundMortgageOut {}",pTradingAccount->PreFundMortgageOut);
		tdLogger->info("FundMortgageIn {}",pTradingAccount->FundMortgageIn);
		tdLogger->info("FundMortgageOut {}",pTradingAccount->FundMortgageOut);
		tdLogger->info("FundMortgageAvailable {}",pTradingAccount->FundMortgageAvailable);
		tdLogger->info("MortgageableFund {}",pTradingAccount->MortgageableFund);
		tdLogger->info("SpecProductMargin {}",pTradingAccount->SpecProductMargin);
		tdLogger->info("SpecProductFrozenMargin {}",pTradingAccount->SpecProductFrozenMargin);
		tdLogger->info("SpecProductCommission {}",pTradingAccount->SpecProductCommission);
		tdLogger->info("SpecProductFrozenCommission {}",pTradingAccount->SpecProductFrozenCommission);
		tdLogger->info("SpecProductPositionProfit {}",pTradingAccount->SpecProductPositionProfit);
		tdLogger->info("SpecProductCloseProfit {}",pTradingAccount->SpecProductCloseProfit);
		tdLogger->info("SpecProductPositionProfitByAlg {}",pTradingAccount->SpecProductPositionProfitByAlg);
		tdLogger->info("SpecProductExchangeMargin {}",pTradingAccount->SpecProductExchangeMargin);
		tdLogger->info("FrozenSwap {}",pTradingAccount->FrozenSwap);
		tdLogger->info("RemainSwap {}",pTradingAccount->RemainSwap);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryTradingAccount>");
};

void TdSpi::OnRspQryInvestor(CThostFtdcInvestorField *pInvestor, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryInvestor>");
	if (pInvestor)
	{
		tdLogger->info("InvestorID {}",pInvestor->InvestorID);
		tdLogger->info("BrokerID {}",pInvestor->BrokerID);
		tdLogger->info("InvestorGroupID {}",pInvestor->InvestorGroupID);
		tdLogger->info("InvestorName {}",pInvestor->InvestorName);
		tdLogger->info("IdentifiedCardNo {}",pInvestor->IdentifiedCardNo);
		tdLogger->info("Telephone {}",pInvestor->Telephone);
		tdLogger->info("Address {}",pInvestor->Address);
		tdLogger->info("OpenDate {}",pInvestor->OpenDate);
		tdLogger->info("Mobile {}",pInvestor->Mobile);
		tdLogger->info("CommModelID {}",pInvestor->CommModelID);
		tdLogger->info("MarginModelID {}",pInvestor->MarginModelID);
		tdLogger->info("IsActive {}",pInvestor->IsActive);
		tdLogger->info("IdentifiedCardType {}",pInvestor->IdentifiedCardType);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInvestor>");
};

void TdSpi::OnRspQryTradingCode(CThostFtdcTradingCodeField *pTradingCode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryTradingCode>");
	if (pTradingCode)
	{
		tdLogger->info("InvestorID {}",pTradingCode->InvestorID);
		tdLogger->info("BrokerID {}",pTradingCode->BrokerID);
		tdLogger->info("ExchangeID {}",pTradingCode->ExchangeID);
		tdLogger->info("ClientID {}",pTradingCode->ClientID);
		tdLogger->info("BranchID {}",pTradingCode->BranchID);
		tdLogger->info("InvestUnitID {}",pTradingCode->InvestUnitID);
		tdLogger->info("IsActive {}",pTradingCode->IsActive);
		tdLogger->info("ClientIDType {}",pTradingCode->ClientIDType);
		tdLogger->info("BizType {}",pTradingCode->BizType);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryTradingCode>");
};

void TdSpi::OnRspQryInstrumentMarginRate(CThostFtdcInstrumentMarginRateField *pInstrumentMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryInstrumentMarginRate>");
	if (pInstrumentMarginRate)
	{
		tdLogger->info("InstrumentID {}",pInstrumentMarginRate->InstrumentID);
		tdLogger->info("BrokerID {}",pInstrumentMarginRate->BrokerID);
		tdLogger->info("InvestorID {}",pInstrumentMarginRate->InvestorID);
		tdLogger->info("ExchangeID {}",pInstrumentMarginRate->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInstrumentMarginRate->InvestUnitID);
		tdLogger->info("IsRelative {}",pInstrumentMarginRate->IsRelative);
		tdLogger->info("InvestorRange {}",pInstrumentMarginRate->InvestorRange);
		tdLogger->info("HedgeFlag {}",pInstrumentMarginRate->HedgeFlag);
		tdLogger->info("LongMarginRatioByMoney {}",pInstrumentMarginRate->LongMarginRatioByMoney);
		tdLogger->info("LongMarginRatioByVolume {}",pInstrumentMarginRate->LongMarginRatioByVolume);
		tdLogger->info("ShortMarginRatioByMoney {}",pInstrumentMarginRate->ShortMarginRatioByMoney);
		tdLogger->info("ShortMarginRatioByVolume {}",pInstrumentMarginRate->ShortMarginRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInstrumentMarginRate>");
};

void TdSpi::OnRspQryInstrumentCommissionRate(CThostFtdcInstrumentCommissionRateField *pInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryInstrumentCommissionRate>");
	if (pInstrumentCommissionRate)
	{
		tdLogger->info("InstrumentID {}",pInstrumentCommissionRate->InstrumentID);
		tdLogger->info("BrokerID {}",pInstrumentCommissionRate->BrokerID);
		tdLogger->info("InvestorID {}",pInstrumentCommissionRate->InvestorID);
		tdLogger->info("ExchangeID {}",pInstrumentCommissionRate->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInstrumentCommissionRate->InvestUnitID);
		tdLogger->info("InvestorRange {}",pInstrumentCommissionRate->InvestorRange);
		tdLogger->info("BizType {}",pInstrumentCommissionRate->BizType);
		tdLogger->info("OpenRatioByMoney {}",pInstrumentCommissionRate->OpenRatioByMoney);
		tdLogger->info("OpenRatioByVolume {}",pInstrumentCommissionRate->OpenRatioByVolume);
		tdLogger->info("CloseRatioByMoney {}",pInstrumentCommissionRate->CloseRatioByMoney);
		tdLogger->info("CloseRatioByVolume {}",pInstrumentCommissionRate->CloseRatioByVolume);
		tdLogger->info("CloseTodayRatioByMoney {}",pInstrumentCommissionRate->CloseTodayRatioByMoney);
		tdLogger->info("CloseTodayRatioByVolume {}",pInstrumentCommissionRate->CloseTodayRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInstrumentCommissionRate>");
};

void TdSpi::OnRspQryExchange(CThostFtdcExchangeField *pExchange, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryExchange>");
	if (pExchange)
	{
		tdLogger->info("ExchangeID {}",pExchange->ExchangeID);
		tdLogger->info("ExchangeName {}",pExchange->ExchangeName);
		tdLogger->info("ExchangeProperty {}",pExchange->ExchangeProperty);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryExchange>");
};

void TdSpi::OnRspQryProduct(CThostFtdcProductField *pProduct, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryProduct>");
	if (pProduct)
	{
		tdLogger->info("ProductID {}",pProduct->ProductID);
		tdLogger->info("ProductName {}",pProduct->ProductName);
		tdLogger->info("ExchangeID {}",pProduct->ExchangeID);
		tdLogger->info("TradeCurrencyID {}",pProduct->TradeCurrencyID);
		tdLogger->info("ExchangeProductID {}",pProduct->ExchangeProductID);
		tdLogger->info("VolumeMultiple {}",pProduct->VolumeMultiple);
		tdLogger->info("MaxMarketOrderVolume {}",pProduct->MaxMarketOrderVolume);
		tdLogger->info("MinMarketOrderVolume {}",pProduct->MinMarketOrderVolume);
		tdLogger->info("MaxLimitOrderVolume {}",pProduct->MaxLimitOrderVolume);
		tdLogger->info("MinLimitOrderVolume {}",pProduct->MinLimitOrderVolume);
		tdLogger->info("ProductClass {}",pProduct->ProductClass);
		tdLogger->info("PositionType {}",pProduct->PositionType);
		tdLogger->info("PositionDateType {}",pProduct->PositionDateType);
		tdLogger->info("CloseDealType {}",pProduct->CloseDealType);
		tdLogger->info("MortgageFundUseRange {}",pProduct->MortgageFundUseRange);
		tdLogger->info("PriceTick {}",pProduct->PriceTick);
		tdLogger->info("UnderlyingMultiple {}",pProduct->UnderlyingMultiple);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryProduct>");
};

void TdSpi::ReqQryInstrument()
{
	CThostFtdcQryInstrumentField a = { 0 };
//strcpy(a.InstrumentID, "rb1809");
//strcpy(a.ExchangeID, "SHFE");
 //
   int res = td_Api->ReqQryInstrument(&a, ++td_reqNum);

	
   if(res == 0)
   {
		tdLogger->info("请求查询 市场全品种    请求发送成功{}",td_reqNum);
		tdLogger->flush();
	}else{
		tdLogger->info("请求查询 市场全品种    请求发送失败");
		tdLogger->flush();
		sleep(1);
		exit(-1007);
	}
}



void TdSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	
	tdLogger->info("<OnRspQryInstrument>");
	if (pInstrument)
	{
		ptrTdData->InstrumentData.push_back(*pInstrument);
		// tdLogger->info("InstrumentID {}",pInstrument->InstrumentID);
		// tdLogger->info("ExchangeID {}",pInstrument->ExchangeID);
		// tdLogger->info("InstrumentName {}",pInstrument->InstrumentName);
		// tdLogger->info("ExchangeInstID {}",pInstrument->ExchangeInstID);
		// tdLogger->info("ProductID {}",pInstrument->ProductID);
		// tdLogger->info("CreateDate {}",pInstrument->CreateDate);
		// tdLogger->info("OpenDate {}",pInstrument->OpenDate);
		// tdLogger->info("ExpireDate {}",pInstrument->ExpireDate);
		// tdLogger->info("StartDelivDate {}",pInstrument->StartDelivDate);
		// tdLogger->info("EndDelivDate {}",pInstrument->EndDelivDate);
		// tdLogger->info("UnderlyingInstrID {}",pInstrument->UnderlyingInstrID);
		// tdLogger->info("DeliveryYear {}",pInstrument->DeliveryYear);
		// tdLogger->info("DeliveryMonth {}",pInstrument->DeliveryMonth);
		// tdLogger->info("MaxMarketOrderVolume {}",pInstrument->MaxMarketOrderVolume);
		// tdLogger->info("MinMarketOrderVolume {}",pInstrument->MinMarketOrderVolume);
		// tdLogger->info("MaxLimitOrderVolume {}",pInstrument->MaxLimitOrderVolume);
		// tdLogger->info("MinLimitOrderVolume {}",pInstrument->MinLimitOrderVolume);
		// tdLogger->info("VolumeMultiple {}",pInstrument->VolumeMultiple);
		// tdLogger->info("IsTrading {}",pInstrument->IsTrading);
		// tdLogger->info("ProductClass {}",pInstrument->ProductClass);
		// tdLogger->info("InstLifePhase {}",pInstrument->InstLifePhase);
		// tdLogger->info("PositionType {}",pInstrument->PositionType);
		// tdLogger->info("PositionDateType {}",pInstrument->PositionDateType);
		// tdLogger->info("MaxMarginSideAlgorithm {}",pInstrument->MaxMarginSideAlgorithm);
		// tdLogger->info("OptionsType {}",pInstrument->OptionsType);
		// tdLogger->info("CombinationType {}",pInstrument->CombinationType);
		// tdLogger->info("PriceTick {}",pInstrument->PriceTick);
		// tdLogger->info("LongMarginRatio {}",pInstrument->LongMarginRatio);
		// tdLogger->info("ShortMarginRatio {}",pInstrument->ShortMarginRatio);
		// tdLogger->info("StrikePrice {}",pInstrument->StrikePrice);
		// tdLogger->info("UnderlyingMultiple {}",pInstrument->UnderlyingMultiple);
        // tdLogger->info("InstrumentID {},ExchangeID {},InstrumentName {},ExchangeInstID {},
		// ProductID {},CreateDate {},OpenDate {},ExpireDate {},StartDelivDate {},EndDelivDate {},
		// UnderlyingInstrID {},DeliveryYear {},DeliveryMonth {},MaxMarketOrderVolume {},MinMarketOrderVolume {},
		// MaxLimitOrderVolume {},MinLimitOrderVolume {},VolumeMultiple {},IsTrading {},ProductClass {},InstLifePhase {},
		// PositionType {},PositionDateType {},MaxMarginSideAlgorithm {},OptionsType {},CombinationType {},PriceTick {},LongMarginRatio {},
		// ShortMarginRatio {},StrikePrice {},UnderlyingMultiple {},",pInstrument->InstrumentID,pInstrument->ExchangeID,
		// pInstrument->InstrumentName,pInstrument->ExchangeInstID,pInstrument->ProductID,pInstrument->CreateDate,pInstrument->OpenDate,
		// pInstrument->ExpireDate,pInstrument->StartDelivDate,pInstrument->EndDelivDate,pInstrument->UnderlyingInstrID,pInstrument->DeliveryYear,
		// pInstrument->DeliveryMonth,pInstrument->MaxMarketOrderVolume,pInstrument->MinMarketOrderVolume,pInstrument->MaxLimitOrderVolume,
		// pInstrument->MinLimitOrderVolume,pInstrument->VolumeMultiple,pInstrument->IsTrading,pInstrument->ProductClass,pInstrument->InstLifePhase,
		// pInstrument->PositionType,pInstrument->PositionDateType,pInstrument->MaxMarginSideAlgorithm,pInstrument->OptionsType,pInstrument->CombinationType,
		// pInstrument->PriceTick,pInstrument->LongMarginRatio,pInstrument->ShortMarginRatio,pInstrument->StrikePrice,pInstrument->UnderlyingMultiple);


	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInstrument>");
	if(bIsLast){
		sleep(1);
		cout << "保存数据 等待下一步操作。。。" << endl;
		ofstream out;
		out.open("./somefile/Instrument.csv");
		
		if(out.is_open()){
			static int InstrumentEnum(0);
			if(InstrumentEnum == 0){
				out << "InstrumentID"<<","<< "ExchangeID"<<","<< "InstrumentName"<<","<< "ExchangeInstID"<<","<< "ProductID"<<","<< "ProductClass"<<","
				<< "DeliveryYear"<<","<< "DeliveryMonth"<<","<< "MaxMarketOrderVolume"<<","<< "MinMarketOrderVolume"<<","<< "MaxLimitOrderVolume"<<","
				<< "MinLimitOrderVolume"<<","<< "VolumeMultiple"<<","<< "PriceTick"<<","<< "CreateDate"<<","<< "OpenDate"<<","<< "ExpireDate"<<","
				<< "StartDelivDate"<<","<< "EndDelivDate"<<","<< "InstLifePhase"<<","<< "IsTrading"<<","<< "PositionType"<<","<< "PositionDateType"<<","
				<< "LongMarginRatio"<<","<< "ShortMarginRatio"<<","<< "MaxMarginSideAlgorithm"<<","<< "UnderlyingInstrID"<<","<< "StrikePrice"<<","
				<< "OptionsType "<<","<< "UnderlyingMultiple"<<","<< "CombinationType"<<endl;
			}

			auto itr = ptrTdData->InstrumentData.begin();
			for(;itr != ptrTdData->InstrumentData.end();++itr)
			{
				++InstrumentEnum;
				out <<itr->InstrumentID<<","<<itr->ExchangeID<<","<<itr->InstrumentName<<","<<itr->ExchangeInstID<<","<<itr->ProductID<<","<<itr->ProductClass<<","
				<<itr->DeliveryYear<<","<<itr->DeliveryMonth<<","<<itr->MaxMarketOrderVolume<<","<<itr->MinMarketOrderVolume<<","<<itr->MaxLimitOrderVolume<<","
				<<itr->MinLimitOrderVolume<<","<<itr->VolumeMultiple<<","<<itr->PriceTick<<","<<itr->CreateDate<<","<<itr->OpenDate<<","<<itr->ExpireDate<<","
				<<itr->StartDelivDate<<","<<itr->EndDelivDate<<","<<itr->InstLifePhase<<","<<itr->IsTrading<<","<<itr->PositionType<<","<<itr->PositionDateType<<","
				<<itr->LongMarginRatio<<","<<itr->ShortMarginRatio<<","<<itr->MaxMarginSideAlgorithm<<","<<itr->UnderlyingInstrID<<","<<itr->StrikePrice<<","
				<<itr->OptionsType <<","<<itr->UnderlyingMultiple<<","<<itr->CombinationType<<endl;

		// pInstrument->InstrumentName,pInstrument->ExchangeInstID,pInstrument->ProductID,pInstrument->CreateDate,pInstrument->OpenDate,
		// pInstrument->ExpireDate,pInstrument->StartDelivDate,pInstrument->EndDelivDate,pInstrument->UnderlyingInstrID,pInstrument->DeliveryYear,
		// pInstrument->DeliveryMonth,pInstrument->MaxMarketOrderVolume,pInstrument->MinMarketOrderVolume,pInstrument->MaxLimitOrderVolume,
		// pInstrument->MinLimitOrderVolume,pInstrument->VolumeMultiple,pInstrument->IsTrading,pInstrument->ProductClass,pInstrument->InstLifePhase,
		// pInstrument->PositionType,pInstrument->PositionDateType,pInstrument->MaxMarginSideAlgorithm,pInstrument->OptionsType,pInstrument->CombinationType,
		// pInstrument->PriceTick,pInstrument->LongMarginRatio,pInstrument->ShortMarginRatio,pInstrument->StrikePrice,pInstrument->UnderlyingMultiple)

			}
		out.close();

		}
		cout << "交易线程 等待下一步操作。。。" << endl;
		setSedordAllowTrue();
	}
};

void TdSpi::OnRspQryDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryDepthMarketData>");
	if (pDepthMarketData)
	{
		tdLogger->info("TradingDay {}",pDepthMarketData->TradingDay);
		tdLogger->info("InstrumentID {}",pDepthMarketData->InstrumentID);
		tdLogger->info("ExchangeID {}",pDepthMarketData->ExchangeID);
		tdLogger->info("ExchangeInstID {}",pDepthMarketData->ExchangeInstID);
		tdLogger->info("UpdateTime {}",pDepthMarketData->UpdateTime);
		tdLogger->info("ActionDay {}",pDepthMarketData->ActionDay);
		tdLogger->info("Volume {}",pDepthMarketData->Volume);
		tdLogger->info("UpdateMillisec {}",pDepthMarketData->UpdateMillisec);
		tdLogger->info("BidVolume1 {}",pDepthMarketData->BidVolume1);
		tdLogger->info("AskVolume1 {}",pDepthMarketData->AskVolume1);
		tdLogger->info("BidVolume2 {}",pDepthMarketData->BidVolume2);
		tdLogger->info("AskVolume2 {}",pDepthMarketData->AskVolume2);
		tdLogger->info("BidVolume3 {}",pDepthMarketData->BidVolume3);
		tdLogger->info("AskVolume3 {}",pDepthMarketData->AskVolume3);
		tdLogger->info("BidVolume4 {}",pDepthMarketData->BidVolume4);
		tdLogger->info("AskVolume4 {}",pDepthMarketData->AskVolume4);
		tdLogger->info("BidVolume5 {}",pDepthMarketData->BidVolume5);
		tdLogger->info("AskVolume5 {}",pDepthMarketData->AskVolume5);
		tdLogger->info("LastPrice {}",pDepthMarketData->LastPrice);
		tdLogger->info("PreSettlementPrice {}",pDepthMarketData->PreSettlementPrice);
		tdLogger->info("PreClosePrice {}",pDepthMarketData->PreClosePrice);
		tdLogger->info("PreOpenInterest {}",pDepthMarketData->PreOpenInterest);
		tdLogger->info("OpenPrice {}",pDepthMarketData->OpenPrice);
		tdLogger->info("HighestPrice {}",pDepthMarketData->HighestPrice);
		tdLogger->info("LowestPrice {}",pDepthMarketData->LowestPrice);
		tdLogger->info("Turnover {}",pDepthMarketData->Turnover);
		tdLogger->info("OpenInterest {}",pDepthMarketData->OpenInterest);
		tdLogger->info("ClosePrice {}",pDepthMarketData->ClosePrice);
		tdLogger->info("SettlementPrice {}",pDepthMarketData->SettlementPrice);
		tdLogger->info("UpperLimitPrice {}",pDepthMarketData->UpperLimitPrice);
		tdLogger->info("LowerLimitPrice {}",pDepthMarketData->LowerLimitPrice);
		tdLogger->info("PreDelta {}",pDepthMarketData->PreDelta);
		tdLogger->info("CurrDelta {}",pDepthMarketData->CurrDelta);
		tdLogger->info("BidPrice1 {}",pDepthMarketData->BidPrice1);
		tdLogger->info("AskPrice1 {}",pDepthMarketData->AskPrice1);
		tdLogger->info("BidPrice2 {}",pDepthMarketData->BidPrice2);
		tdLogger->info("AskPrice2 {}",pDepthMarketData->AskPrice2);
		tdLogger->info("BidPrice3 {}",pDepthMarketData->BidPrice3);
		tdLogger->info("AskPrice3 {}",pDepthMarketData->AskPrice3);
		tdLogger->info("BidPrice4 {}",pDepthMarketData->BidPrice4);
		tdLogger->info("AskPrice4 {}",pDepthMarketData->AskPrice4);
		tdLogger->info("BidPrice5 {}",pDepthMarketData->BidPrice5);
		tdLogger->info("AskPrice5 {}",pDepthMarketData->AskPrice5);
		tdLogger->info("AveragePrice {}",pDepthMarketData->AveragePrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryDepthMarketData>");
};

void TdSpi::ReqQrySettlementInfo()
{
	CThostFtdcQrySettlementInfoField a = { 0 };
	strcpy(a.BrokerID, td_acc->brkId.c_str());
	strcpy(a.InvestorID, td_acc->inId.c_str());
	//strcpy(a.TradingDay,“20180101”);
	int res = td_Api->ReqQrySettlementInfo(&a, ++td_reqNum);
	
   if(res == 0)
   {
		tdLogger->info("请求查询结算单 请求发送成功");
		tdLogger->flush();
	}else{
		tdLogger->info("请求查询结算单 请求发送失败");
		tdLogger->flush();
		sleep(3);
		exit(-1003);
	}
}

void TdSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspQrySettlementInfo>");
	if (pSettlementInfo)
	{
		tdLogger->info("TradingDay {}",pSettlementInfo->TradingDay);
		tdLogger->info("BrokerID {}",pSettlementInfo->BrokerID);
		tdLogger->info("InvestorID {}",pSettlementInfo->InvestorID);
		tdLogger->info("Content {}",pSettlementInfo->Content);
		tdLogger->info("AccountID {}",pSettlementInfo->AccountID);
		tdLogger->info("CurrencyID {}",pSettlementInfo->CurrencyID);
		tdLogger->info("SettlementID {}",pSettlementInfo->SettlementID);
		tdLogger->info("SequenceNo {}",pSettlementInfo->SequenceNo);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQrySettlementInfo>");
	tdLogger->flush();
	if(bIsLast){
	sleep(1);
	ReqSettlementInfoConfirm();
	}
};

void TdSpi::OnRspQryTransferBank(CThostFtdcTransferBankField *pTransferBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryTransferBank>");
	if (pTransferBank)
	{
		tdLogger->info("BankID {}",pTransferBank->BankID);
		tdLogger->info("BankBrchID {}",pTransferBank->BankBrchID);
		tdLogger->info("BankName {}",pTransferBank->BankName);
		tdLogger->info("IsActive {}",pTransferBank->IsActive);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryTransferBank>");
};

void TdSpi::ReqQryInvestorPositionDetail()
{   
	CThostFtdcQryInvestorPositionDetailField  a = { 0 };
	strcpy(a.BrokerID, td_acc->brkId.c_str());
	strcpy(a.InvestorID,  td_acc->inId.c_str());
//strcpy(a.InstrumentID, "rb1809");//不填写合约则返回所有持仓
	int res = td_Api->ReqQryInvestorPositionDetail(&a, ++td_reqNum);

	
   if(res == 0)
   {
		tdLogger->info("请求查询 持仓明细 请求发送成功{}",td_reqNum);
		tdLogger->flush();
	}else{
		tdLogger->info("请求查询 持仓明细 请求发送失败");
		tdLogger->flush();
		sleep(3);
		exit(-1006);
	}
}

void TdSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspQryInvestorPositionDetail>");
	if (pInvestorPositionDetail)
	{
		tdLogger->info("InstrumentID {}",pInvestorPositionDetail->InstrumentID);
		tdLogger->info("BrokerID {}",pInvestorPositionDetail->BrokerID);
		tdLogger->info("InvestorID {}",pInvestorPositionDetail->InvestorID);
		tdLogger->info("OpenDate {}",pInvestorPositionDetail->OpenDate);
		tdLogger->info("TradeID {}",pInvestorPositionDetail->TradeID);
		tdLogger->info("TradingDay {}",pInvestorPositionDetail->TradingDay);
		tdLogger->info("CombInstrumentID {}",pInvestorPositionDetail->CombInstrumentID);
		tdLogger->info("ExchangeID {}",pInvestorPositionDetail->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInvestorPositionDetail->InvestUnitID);
		tdLogger->info("Volume {}",pInvestorPositionDetail->Volume);
		tdLogger->info("SettlementID {}",pInvestorPositionDetail->SettlementID);
		tdLogger->info("CloseVolume {}",pInvestorPositionDetail->CloseVolume);
		tdLogger->info("TimeFirstVolume {}",pInvestorPositionDetail->TimeFirstVolume);
		tdLogger->info("HedgeFlag {}",pInvestorPositionDetail->HedgeFlag);
		tdLogger->info("Direction {}",pInvestorPositionDetail->Direction);
		tdLogger->info("TradeType {}",pInvestorPositionDetail->TradeType);
		tdLogger->info("OpenPrice {}",pInvestorPositionDetail->OpenPrice);
		tdLogger->info("CloseProfitByDate {}",pInvestorPositionDetail->CloseProfitByDate);
		tdLogger->info("CloseProfitByTrade {}",pInvestorPositionDetail->CloseProfitByTrade);
		tdLogger->info("PositionProfitByDate {}",pInvestorPositionDetail->PositionProfitByDate);
		tdLogger->info("PositionProfitByTrade {}",pInvestorPositionDetail->PositionProfitByTrade);
		tdLogger->info("Margin {}",pInvestorPositionDetail->Margin);
		tdLogger->info("ExchMargin {}",pInvestorPositionDetail->ExchMargin);
		tdLogger->info("MarginRateByMoney {}",pInvestorPositionDetail->MarginRateByMoney);
		tdLogger->info("MarginRateByVolume {}",pInvestorPositionDetail->MarginRateByVolume);
		tdLogger->info("LastSettlementPrice {}",pInvestorPositionDetail->LastSettlementPrice);
		tdLogger->info("SettlementPrice {}",pInvestorPositionDetail->SettlementPrice);
		tdLogger->info("CloseAmount {}",pInvestorPositionDetail->CloseAmount);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInvestorPositionDetail>");
	tdLogger->flush();
	if(bIsLast){
		sleep(1);
		ReqQryInstrument(); //查询合约
	}
};

void TdSpi::OnRspQryNotice(CThostFtdcNoticeField *pNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryNotice>");
	if (pNotice)
	{
		tdLogger->info("BrokerID {}",pNotice->BrokerID);
		tdLogger->info("Content {}",pNotice->Content);
		tdLogger->info("SequenceLabel {}",pNotice->SequenceLabel);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryNotice>");
};


void TdSpi::ReqQrySettlementInfoConfirm()
{
}



void TdSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspQrySettlementInfoConfirm>");
	if (pSettlementInfoConfirm)
	{
		tdLogger->info("BrokerID {}",pSettlementInfoConfirm->BrokerID);
		tdLogger->info("InvestorID {}",pSettlementInfoConfirm->InvestorID);
		tdLogger->info("ConfirmDate {}",pSettlementInfoConfirm->ConfirmDate);
		tdLogger->info("ConfirmTime {}",pSettlementInfoConfirm->ConfirmTime);
		tdLogger->info("AccountID {}",pSettlementInfoConfirm->AccountID);
		tdLogger->info("CurrencyID {}",pSettlementInfoConfirm->CurrencyID);
		tdLogger->info("SettlementID {}",pSettlementInfoConfirm->SettlementID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQrySettlementInfoConfirm>");

};
void TdSpi::ReqQryInvestorPositionCombineDetail()
{
}

void TdSpi::OnRspQryInvestorPositionCombineDetail(CThostFtdcInvestorPositionCombineDetailField *pInvestorPositionCombineDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	tdLogger->info("<OnRspQryInvestorPositionCombineDetail>");
	if (pInvestorPositionCombineDetail)
	{
		tdLogger->info("TradingDay {}",pInvestorPositionCombineDetail->TradingDay);
		tdLogger->info("OpenDate {}",pInvestorPositionCombineDetail->OpenDate);
		tdLogger->info("ExchangeID {}",pInvestorPositionCombineDetail->ExchangeID);
		tdLogger->info("BrokerID {}",pInvestorPositionCombineDetail->BrokerID);
		tdLogger->info("InvestorID {}",pInvestorPositionCombineDetail->InvestorID);
		tdLogger->info("ComTradeID {}",pInvestorPositionCombineDetail->ComTradeID);
		tdLogger->info("TradeID {}",pInvestorPositionCombineDetail->TradeID);
		tdLogger->info("InstrumentID {}",pInvestorPositionCombineDetail->InstrumentID);
		tdLogger->info("CombInstrumentID {}",pInvestorPositionCombineDetail->CombInstrumentID);
		tdLogger->info("InvestUnitID {}",pInvestorPositionCombineDetail->InvestUnitID);
		tdLogger->info("SettlementID {}",pInvestorPositionCombineDetail->SettlementID);
		tdLogger->info("TotalAmt {}",pInvestorPositionCombineDetail->TotalAmt);
		tdLogger->info("LegID {}",pInvestorPositionCombineDetail->LegID);
		tdLogger->info("LegMultiple {}",pInvestorPositionCombineDetail->LegMultiple);
		tdLogger->info("TradeGroupID {}",pInvestorPositionCombineDetail->TradeGroupID);
		tdLogger->info("HedgeFlag {}",pInvestorPositionCombineDetail->HedgeFlag);
		tdLogger->info("Direction {}",pInvestorPositionCombineDetail->Direction);
		tdLogger->info("Margin {}",pInvestorPositionCombineDetail->Margin);
		tdLogger->info("ExchMargin {}",pInvestorPositionCombineDetail->ExchMargin);
		tdLogger->info("MarginRateByMoney {}",pInvestorPositionCombineDetail->MarginRateByMoney);
		tdLogger->info("MarginRateByVolume {}",pInvestorPositionCombineDetail->MarginRateByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInvestorPositionCombineDetail>");
};

void TdSpi::OnRspQryCFMMCTradingAccountKey(CThostFtdcCFMMCTradingAccountKeyField *pCFMMCTradingAccountKey, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryCFMMCTradingAccountKey>");
	if (pCFMMCTradingAccountKey)
	{
		tdLogger->info("BrokerID {}",pCFMMCTradingAccountKey->BrokerID);
		tdLogger->info("ParticipantID {}",pCFMMCTradingAccountKey->ParticipantID);
		tdLogger->info("AccountID {}",pCFMMCTradingAccountKey->AccountID);
		tdLogger->info("CurrentKey {}",pCFMMCTradingAccountKey->CurrentKey);
		tdLogger->info("KeyID {}",pCFMMCTradingAccountKey->KeyID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryCFMMCTradingAccountKey>");
};

void TdSpi::OnRspQryEWarrantOffset(CThostFtdcEWarrantOffsetField *pEWarrantOffset, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryEWarrantOffset>");
	if (pEWarrantOffset)
	{
		tdLogger->info("TradingDay {}",pEWarrantOffset->TradingDay);
		tdLogger->info("BrokerID {}",pEWarrantOffset->BrokerID);
		tdLogger->info("InvestorID {}",pEWarrantOffset->InvestorID);
		tdLogger->info("ExchangeID {}",pEWarrantOffset->ExchangeID);
		tdLogger->info("InstrumentID {}",pEWarrantOffset->InstrumentID);
		tdLogger->info("InvestUnitID {}",pEWarrantOffset->InvestUnitID);
		tdLogger->info("Volume {}",pEWarrantOffset->Volume);
		tdLogger->info("Direction {}",pEWarrantOffset->Direction);
		tdLogger->info("HedgeFlag {}",pEWarrantOffset->HedgeFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryEWarrantOffset>");
};

void TdSpi::OnRspQryInvestorProductGroupMargin(CThostFtdcInvestorProductGroupMarginField *pInvestorProductGroupMargin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryInvestorProductGroupMargin>");
	if (pInvestorProductGroupMargin)
	{
		tdLogger->info("ProductGroupID {}",pInvestorProductGroupMargin->ProductGroupID);
		tdLogger->info("BrokerID {}",pInvestorProductGroupMargin->BrokerID);
		tdLogger->info("InvestorID {}",pInvestorProductGroupMargin->InvestorID);
		tdLogger->info("TradingDay {}",pInvestorProductGroupMargin->TradingDay);
		tdLogger->info("ExchangeID {}",pInvestorProductGroupMargin->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInvestorProductGroupMargin->InvestUnitID);
		tdLogger->info("SettlementID {}",pInvestorProductGroupMargin->SettlementID);
		tdLogger->info("HedgeFlag {}",pInvestorProductGroupMargin->HedgeFlag);
		tdLogger->info("FrozenMargin {}",pInvestorProductGroupMargin->FrozenMargin);
		tdLogger->info("LongFrozenMargin {}",pInvestorProductGroupMargin->LongFrozenMargin);
		tdLogger->info("ShortFrozenMargin {}",pInvestorProductGroupMargin->ShortFrozenMargin);
		tdLogger->info("UseMargin {}",pInvestorProductGroupMargin->UseMargin);
		tdLogger->info("LongUseMargin {}",pInvestorProductGroupMargin->LongUseMargin);
		tdLogger->info("ShortUseMargin {}",pInvestorProductGroupMargin->ShortUseMargin);
		tdLogger->info("ExchMargin {}",pInvestorProductGroupMargin->ExchMargin);
		tdLogger->info("LongExchMargin {}",pInvestorProductGroupMargin->LongExchMargin);
		tdLogger->info("ShortExchMargin {}",pInvestorProductGroupMargin->ShortExchMargin);
		tdLogger->info("CloseProfit {}",pInvestorProductGroupMargin->CloseProfit);
		tdLogger->info("FrozenCommission {}",pInvestorProductGroupMargin->FrozenCommission);
		tdLogger->info("Commission {}",pInvestorProductGroupMargin->Commission);
		tdLogger->info("FrozenCash {}",pInvestorProductGroupMargin->FrozenCash);
		tdLogger->info("CashIn {}",pInvestorProductGroupMargin->CashIn);
		tdLogger->info("PositionProfit {}",pInvestorProductGroupMargin->PositionProfit);
		tdLogger->info("OffsetAmount {}",pInvestorProductGroupMargin->OffsetAmount);
		tdLogger->info("LongOffsetAmount {}",pInvestorProductGroupMargin->LongOffsetAmount);
		tdLogger->info("ShortOffsetAmount {}",pInvestorProductGroupMargin->ShortOffsetAmount);
		tdLogger->info("ExchOffsetAmount {}",pInvestorProductGroupMargin->ExchOffsetAmount);
		tdLogger->info("LongExchOffsetAmount {}",pInvestorProductGroupMargin->LongExchOffsetAmount);
		tdLogger->info("ShortExchOffsetAmount {}",pInvestorProductGroupMargin->ShortExchOffsetAmount);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInvestorProductGroupMargin>");
};

void TdSpi::OnRspQryExchangeMarginRate(CThostFtdcExchangeMarginRateField *pExchangeMarginRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryExchangeMarginRate>");
	if (pExchangeMarginRate)
	{
		tdLogger->info("BrokerID {}",pExchangeMarginRate->BrokerID);
		tdLogger->info("InstrumentID {}",pExchangeMarginRate->InstrumentID);
		tdLogger->info("ExchangeID {}",pExchangeMarginRate->ExchangeID);
		tdLogger->info("HedgeFlag {}",pExchangeMarginRate->HedgeFlag);
		tdLogger->info("LongMarginRatioByMoney {}",pExchangeMarginRate->LongMarginRatioByMoney);
		tdLogger->info("LongMarginRatioByVolume {}",pExchangeMarginRate->LongMarginRatioByVolume);
		tdLogger->info("ShortMarginRatioByMoney {}",pExchangeMarginRate->ShortMarginRatioByMoney);
		tdLogger->info("ShortMarginRatioByVolume {}",pExchangeMarginRate->ShortMarginRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryExchangeMarginRate>");
};

void TdSpi::OnRspQryExchangeMarginRateAdjust(CThostFtdcExchangeMarginRateAdjustField *pExchangeMarginRateAdjust, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryExchangeMarginRateAdjust>");
	if (pExchangeMarginRateAdjust)
	{
		tdLogger->info("BrokerID {}",pExchangeMarginRateAdjust->BrokerID);
		tdLogger->info("InstrumentID {}",pExchangeMarginRateAdjust->InstrumentID);
		tdLogger->info("HedgeFlag {}",pExchangeMarginRateAdjust->HedgeFlag);
		tdLogger->info("LongMarginRatioByMoney {}",pExchangeMarginRateAdjust->LongMarginRatioByMoney);
		tdLogger->info("LongMarginRatioByVolume {}",pExchangeMarginRateAdjust->LongMarginRatioByVolume);
		tdLogger->info("ShortMarginRatioByMoney {}",pExchangeMarginRateAdjust->ShortMarginRatioByMoney);
		tdLogger->info("ShortMarginRatioByVolume {}",pExchangeMarginRateAdjust->ShortMarginRatioByVolume);
		tdLogger->info("ExchLongMarginRatioByMoney {}",pExchangeMarginRateAdjust->ExchLongMarginRatioByMoney);
		tdLogger->info("ExchLongMarginRatioByVolume {}",pExchangeMarginRateAdjust->ExchLongMarginRatioByVolume);
		tdLogger->info("ExchShortMarginRatioByMoney {}",pExchangeMarginRateAdjust->ExchShortMarginRatioByMoney);
		tdLogger->info("ExchShortMarginRatioByVolume {}",pExchangeMarginRateAdjust->ExchShortMarginRatioByVolume);
		tdLogger->info("NoLongMarginRatioByMoney {}",pExchangeMarginRateAdjust->NoLongMarginRatioByMoney);
		tdLogger->info("NoLongMarginRatioByVolume {}",pExchangeMarginRateAdjust->NoLongMarginRatioByVolume);
		tdLogger->info("NoShortMarginRatioByMoney {}",pExchangeMarginRateAdjust->NoShortMarginRatioByMoney);
		tdLogger->info("NoShortMarginRatioByVolume {}",pExchangeMarginRateAdjust->NoShortMarginRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryExchangeMarginRateAdjust>");
};

void TdSpi::OnRspQryExchangeRate(CThostFtdcExchangeRateField *pExchangeRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryExchangeRate>");
	if (pExchangeRate)
	{
		tdLogger->info("BrokerID {}",pExchangeRate->BrokerID);
		tdLogger->info("FromCurrencyID {}",pExchangeRate->FromCurrencyID);
		tdLogger->info("ToCurrencyID {}",pExchangeRate->ToCurrencyID);
		tdLogger->info("FromCurrencyUnit {}",pExchangeRate->FromCurrencyUnit);
		tdLogger->info("ExchangeRate {}",pExchangeRate->ExchangeRate);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryExchangeRate>");
};

void TdSpi::OnRspQrySecAgentACIDMap(CThostFtdcSecAgentACIDMapField *pSecAgentACIDMap, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQrySecAgentACIDMap>");
	if (pSecAgentACIDMap)
	{
		tdLogger->info("BrokerID {}",pSecAgentACIDMap->BrokerID);
		tdLogger->info("UserID {}",pSecAgentACIDMap->UserID);
		tdLogger->info("AccountID {}",pSecAgentACIDMap->AccountID);
		tdLogger->info("CurrencyID {}",pSecAgentACIDMap->CurrencyID);
		tdLogger->info("BrokerSecAgentID {}",pSecAgentACIDMap->BrokerSecAgentID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQrySecAgentACIDMap>");
};

void TdSpi::OnRspQryProductExchRate(CThostFtdcProductExchRateField *pProductExchRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryProductExchRate>");
	if (pProductExchRate)
	{
		tdLogger->info("ProductID {}",pProductExchRate->ProductID);
		tdLogger->info("QuoteCurrencyID {}",pProductExchRate->QuoteCurrencyID);
		tdLogger->info("ExchangeID {}",pProductExchRate->ExchangeID);
		tdLogger->info("ExchangeRate {}",pProductExchRate->ExchangeRate);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryProductExchRate>");
};

void TdSpi::OnRspQryProductGroup(CThostFtdcProductGroupField *pProductGroup, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryProductGroup>");
	if (pProductGroup)
	{
		tdLogger->info("ProductID {}",pProductGroup->ProductID);
		tdLogger->info("ExchangeID {}",pProductGroup->ExchangeID);
		tdLogger->info("ProductGroupID {}",pProductGroup->ProductGroupID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryProductGroup>");
};

void TdSpi::OnRspQryMMInstrumentCommissionRate(CThostFtdcMMInstrumentCommissionRateField *pMMInstrumentCommissionRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryMMInstrumentCommissionRate>");
	if (pMMInstrumentCommissionRate)
	{
		tdLogger->info("InstrumentID {}",pMMInstrumentCommissionRate->InstrumentID);
		tdLogger->info("BrokerID {}",pMMInstrumentCommissionRate->BrokerID);
		tdLogger->info("InvestorID {}",pMMInstrumentCommissionRate->InvestorID);
		tdLogger->info("InvestorRange {}",pMMInstrumentCommissionRate->InvestorRange);
		tdLogger->info("OpenRatioByMoney {}",pMMInstrumentCommissionRate->OpenRatioByMoney);
		tdLogger->info("OpenRatioByVolume {}",pMMInstrumentCommissionRate->OpenRatioByVolume);
		tdLogger->info("CloseRatioByMoney {}",pMMInstrumentCommissionRate->CloseRatioByMoney);
		tdLogger->info("CloseRatioByVolume {}",pMMInstrumentCommissionRate->CloseRatioByVolume);
		tdLogger->info("CloseTodayRatioByMoney {}",pMMInstrumentCommissionRate->CloseTodayRatioByMoney);
		tdLogger->info("CloseTodayRatioByVolume {}",pMMInstrumentCommissionRate->CloseTodayRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryMMInstrumentCommissionRate>");
};

void TdSpi::OnRspQryMMOptionInstrCommRate(CThostFtdcMMOptionInstrCommRateField *pMMOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryMMOptionInstrCommRate>");
	if (pMMOptionInstrCommRate)
	{
		tdLogger->info("InstrumentID {}",pMMOptionInstrCommRate->InstrumentID);
		tdLogger->info("BrokerID {}",pMMOptionInstrCommRate->BrokerID);
		tdLogger->info("InvestorID {}",pMMOptionInstrCommRate->InvestorID);
		tdLogger->info("InvestorRange {}",pMMOptionInstrCommRate->InvestorRange);
		tdLogger->info("OpenRatioByMoney {}",pMMOptionInstrCommRate->OpenRatioByMoney);
		tdLogger->info("OpenRatioByVolume {}",pMMOptionInstrCommRate->OpenRatioByVolume);
		tdLogger->info("CloseRatioByMoney {}",pMMOptionInstrCommRate->CloseRatioByMoney);
		tdLogger->info("CloseRatioByVolume {}",pMMOptionInstrCommRate->CloseRatioByVolume);
		tdLogger->info("CloseTodayRatioByMoney {}",pMMOptionInstrCommRate->CloseTodayRatioByMoney);
		tdLogger->info("CloseTodayRatioByVolume {}",pMMOptionInstrCommRate->CloseTodayRatioByVolume);
		tdLogger->info("StrikeRatioByMoney {}",pMMOptionInstrCommRate->StrikeRatioByMoney);
		tdLogger->info("StrikeRatioByVolume {}",pMMOptionInstrCommRate->StrikeRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryMMOptionInstrCommRate>");
};

void TdSpi::OnRspQryInstrumentOrderCommRate(CThostFtdcInstrumentOrderCommRateField *pInstrumentOrderCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryInstrumentOrderCommRate>");
	if (pInstrumentOrderCommRate)
	{
		tdLogger->info("InstrumentID {}",pInstrumentOrderCommRate->InstrumentID);
		tdLogger->info("BrokerID {}",pInstrumentOrderCommRate->BrokerID);
		tdLogger->info("InvestorID {}",pInstrumentOrderCommRate->InvestorID);
		tdLogger->info("ExchangeID {}",pInstrumentOrderCommRate->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInstrumentOrderCommRate->InvestUnitID);
		tdLogger->info("InvestorRange {}",pInstrumentOrderCommRate->InvestorRange);
		tdLogger->info("HedgeFlag {}",pInstrumentOrderCommRate->HedgeFlag);
		tdLogger->info("OrderCommByVolume {}",pInstrumentOrderCommRate->OrderCommByVolume);
		tdLogger->info("OrderActionCommByVolume {}",pInstrumentOrderCommRate->OrderActionCommByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInstrumentOrderCommRate>");
};

void TdSpi::OnRspQrySecAgentTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQrySecAgentTradingAccount>");
	if (pTradingAccount)
	{
		tdLogger->info("BrokerID {}",pTradingAccount->BrokerID);
		tdLogger->info("AccountID {}",pTradingAccount->AccountID);
		tdLogger->info("TradingDay {}",pTradingAccount->TradingDay);
		tdLogger->info("CurrencyID {}",pTradingAccount->CurrencyID);
		tdLogger->info("SettlementID {}",pTradingAccount->SettlementID);
		tdLogger->info("BizType {}",pTradingAccount->BizType);
		tdLogger->info("PreMortgage {}",pTradingAccount->PreMortgage);
		tdLogger->info("PreCredit {}",pTradingAccount->PreCredit);
		tdLogger->info("PreDeposit {}",pTradingAccount->PreDeposit);
		tdLogger->info("PreBalance {}",pTradingAccount->PreBalance);
		tdLogger->info("PreMargin {}",pTradingAccount->PreMargin);
		tdLogger->info("InterestBase {}",pTradingAccount->InterestBase);
		tdLogger->info("Interest {}",pTradingAccount->Interest);
		tdLogger->info("Deposit {}",pTradingAccount->Deposit);
		tdLogger->info("Withdraw {}",pTradingAccount->Withdraw);
		tdLogger->info("FrozenMargin {}",pTradingAccount->FrozenMargin);
		tdLogger->info("FrozenCash {}",pTradingAccount->FrozenCash);
		tdLogger->info("FrozenCommission {}",pTradingAccount->FrozenCommission);
		tdLogger->info("CurrMargin {}",pTradingAccount->CurrMargin);
		tdLogger->info("CashIn {}",pTradingAccount->CashIn);
		tdLogger->info("Commission {}",pTradingAccount->Commission);
		tdLogger->info("CloseProfit {}",pTradingAccount->CloseProfit);
		tdLogger->info("PositionProfit {}",pTradingAccount->PositionProfit);
		tdLogger->info("Balance {}",pTradingAccount->Balance);
		tdLogger->info("Available {}",pTradingAccount->Available);
		tdLogger->info("WithdrawQuota {}",pTradingAccount->WithdrawQuota);
		tdLogger->info("Reserve {}",pTradingAccount->Reserve);
		tdLogger->info("Credit {}",pTradingAccount->Credit);
		tdLogger->info("Mortgage {}",pTradingAccount->Mortgage);
		tdLogger->info("ExchangeMargin {}",pTradingAccount->ExchangeMargin);
		tdLogger->info("DeliveryMargin {}",pTradingAccount->DeliveryMargin);
		tdLogger->info("ExchangeDeliveryMargin {}",pTradingAccount->ExchangeDeliveryMargin);
		tdLogger->info("ReserveBalance {}",pTradingAccount->ReserveBalance);
		tdLogger->info("PreFundMortgageIn {}",pTradingAccount->PreFundMortgageIn);
		tdLogger->info("PreFundMortgageOut {}",pTradingAccount->PreFundMortgageOut);
		tdLogger->info("FundMortgageIn {}",pTradingAccount->FundMortgageIn);
		tdLogger->info("FundMortgageOut {}",pTradingAccount->FundMortgageOut);
		tdLogger->info("FundMortgageAvailable {}",pTradingAccount->FundMortgageAvailable);
		tdLogger->info("MortgageableFund {}",pTradingAccount->MortgageableFund);
		tdLogger->info("SpecProductMargin {}",pTradingAccount->SpecProductMargin);
		tdLogger->info("SpecProductFrozenMargin {}",pTradingAccount->SpecProductFrozenMargin);
		tdLogger->info("SpecProductCommission {}",pTradingAccount->SpecProductCommission);
		tdLogger->info("SpecProductFrozenCommission {}",pTradingAccount->SpecProductFrozenCommission);
		tdLogger->info("SpecProductPositionProfit {}",pTradingAccount->SpecProductPositionProfit);
		tdLogger->info("SpecProductCloseProfit {}",pTradingAccount->SpecProductCloseProfit);
		tdLogger->info("SpecProductPositionProfitByAlg {}",pTradingAccount->SpecProductPositionProfitByAlg);
		tdLogger->info("SpecProductExchangeMargin {}",pTradingAccount->SpecProductExchangeMargin);
		tdLogger->info("FrozenSwap {}",pTradingAccount->FrozenSwap);
		tdLogger->info("RemainSwap {}",pTradingAccount->RemainSwap);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQrySecAgentTradingAccount>");
};

void TdSpi::OnRspQrySecAgentCheckMode(CThostFtdcSecAgentCheckModeField *pSecAgentCheckMode, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQrySecAgentCheckMode>");
	if (pSecAgentCheckMode)
	{
		tdLogger->info("InvestorID {}",pSecAgentCheckMode->InvestorID);
		tdLogger->info("BrokerID {}",pSecAgentCheckMode->BrokerID);
		tdLogger->info("CurrencyID {}",pSecAgentCheckMode->CurrencyID);
		tdLogger->info("BrokerSecAgentID {}",pSecAgentCheckMode->BrokerSecAgentID);
		tdLogger->info("CheckSelfAccount {}",pSecAgentCheckMode->CheckSelfAccount);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQrySecAgentCheckMode>");
};

void TdSpi::OnRspQrySecAgentTradeInfo(CThostFtdcSecAgentTradeInfoField *pSecAgentTradeInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQrySecAgentTradeInfo>");
	if (pSecAgentTradeInfo)
	{
		tdLogger->info("BrokerID {}",pSecAgentTradeInfo->BrokerID);
		tdLogger->info("BrokerSecAgentID {}",pSecAgentTradeInfo->BrokerSecAgentID);
		tdLogger->info("InvestorID {}",pSecAgentTradeInfo->InvestorID);
		tdLogger->info("LongCustomerName {}",pSecAgentTradeInfo->LongCustomerName);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQrySecAgentTradeInfo>");
};

void TdSpi::OnRspQryOptionInstrTradeCost(CThostFtdcOptionInstrTradeCostField *pOptionInstrTradeCost, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryOptionInstrTradeCost>");
	if (pOptionInstrTradeCost)
	{
		tdLogger->info("BrokerID {}",pOptionInstrTradeCost->BrokerID);
		tdLogger->info("InvestorID {}",pOptionInstrTradeCost->InvestorID);
		tdLogger->info("InstrumentID {}",pOptionInstrTradeCost->InstrumentID);
		tdLogger->info("ExchangeID {}",pOptionInstrTradeCost->ExchangeID);
		tdLogger->info("InvestUnitID {}",pOptionInstrTradeCost->InvestUnitID);
		tdLogger->info("HedgeFlag {}",pOptionInstrTradeCost->HedgeFlag);
		tdLogger->info("FixedMargin {}",pOptionInstrTradeCost->FixedMargin);
		tdLogger->info("MiniMargin {}",pOptionInstrTradeCost->MiniMargin);
		tdLogger->info("Royalty {}",pOptionInstrTradeCost->Royalty);
		tdLogger->info("ExchFixedMargin {}",pOptionInstrTradeCost->ExchFixedMargin);
		tdLogger->info("ExchMiniMargin {}",pOptionInstrTradeCost->ExchMiniMargin);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryOptionInstrTradeCost>");
};

void TdSpi::OnRspQryOptionInstrCommRate(CThostFtdcOptionInstrCommRateField *pOptionInstrCommRate, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryOptionInstrCommRate>");
	if (pOptionInstrCommRate)
	{
		tdLogger->info("InstrumentID {}",pOptionInstrCommRate->InstrumentID);
		tdLogger->info("BrokerID {}",pOptionInstrCommRate->BrokerID);
		tdLogger->info("InvestorID {}",pOptionInstrCommRate->InvestorID);
		tdLogger->info("ExchangeID {}",pOptionInstrCommRate->ExchangeID);
		tdLogger->info("InvestUnitID {}",pOptionInstrCommRate->InvestUnitID);
		tdLogger->info("InvestorRange {}",pOptionInstrCommRate->InvestorRange);
		tdLogger->info("OpenRatioByMoney {}",pOptionInstrCommRate->OpenRatioByMoney);
		tdLogger->info("OpenRatioByVolume {}",pOptionInstrCommRate->OpenRatioByVolume);
		tdLogger->info("CloseRatioByMoney {}",pOptionInstrCommRate->CloseRatioByMoney);
		tdLogger->info("CloseRatioByVolume {}",pOptionInstrCommRate->CloseRatioByVolume);
		tdLogger->info("CloseTodayRatioByMoney {}",pOptionInstrCommRate->CloseTodayRatioByMoney);
		tdLogger->info("CloseTodayRatioByVolume {}",pOptionInstrCommRate->CloseTodayRatioByVolume);
		tdLogger->info("StrikeRatioByMoney {}",pOptionInstrCommRate->StrikeRatioByMoney);
		tdLogger->info("StrikeRatioByVolume {}",pOptionInstrCommRate->StrikeRatioByVolume);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryOptionInstrCommRate>");
};

void TdSpi::OnRspQryExecOrder(CThostFtdcExecOrderField *pExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryExecOrder>");
	if (pExecOrder)
	{
		tdLogger->info("BrokerID {}",pExecOrder->BrokerID);
		tdLogger->info("InvestorID {}",pExecOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pExecOrder->InstrumentID);
		tdLogger->info("ExecOrderRef {}",pExecOrder->ExecOrderRef);
		tdLogger->info("UserID {}",pExecOrder->UserID);
		tdLogger->info("BusinessUnit {}",pExecOrder->BusinessUnit);
		tdLogger->info("ExecOrderLocalID {}",pExecOrder->ExecOrderLocalID);
		tdLogger->info("ExchangeID {}",pExecOrder->ExchangeID);
		tdLogger->info("ParticipantID {}",pExecOrder->ParticipantID);
		tdLogger->info("ClientID {}",pExecOrder->ClientID);
		tdLogger->info("ExchangeInstID {}",pExecOrder->ExchangeInstID);
		tdLogger->info("TraderID {}",pExecOrder->TraderID);
		tdLogger->info("TradingDay {}",pExecOrder->TradingDay);
		tdLogger->info("ExecOrderSysID {}",pExecOrder->ExecOrderSysID);
		tdLogger->info("InsertDate {}",pExecOrder->InsertDate);
		tdLogger->info("InsertTime {}",pExecOrder->InsertTime);
		tdLogger->info("CancelTime {}",pExecOrder->CancelTime);
		tdLogger->info("ClearingPartID {}",pExecOrder->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pExecOrder->UserProductInfo);
		tdLogger->info("StatusMsg {}",pExecOrder->StatusMsg);
		tdLogger->info("ActiveUserID {}",pExecOrder->ActiveUserID);
		tdLogger->info("BranchID {}",pExecOrder->BranchID);
		tdLogger->info("InvestUnitID {}",pExecOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pExecOrder->AccountID);
		tdLogger->info("CurrencyID {}",pExecOrder->CurrencyID);
		tdLogger->info("IPAddress {}",pExecOrder->IPAddress);
		tdLogger->info("MacAddress {}",pExecOrder->MacAddress);
		tdLogger->info("Volume {}",pExecOrder->Volume);
		tdLogger->info("RequestID {}",pExecOrder->RequestID);
		tdLogger->info("InstallID {}",pExecOrder->InstallID);
		tdLogger->info("NotifySequence {}",pExecOrder->NotifySequence);
		tdLogger->info("SettlementID {}",pExecOrder->SettlementID);
		tdLogger->info("SequenceNo {}",pExecOrder->SequenceNo);
		tdLogger->info("FrontID {}",pExecOrder->FrontID);
		tdLogger->info("SessionID {}",pExecOrder->SessionID);
		tdLogger->info("BrokerExecOrderSeq {}",pExecOrder->BrokerExecOrderSeq);
		tdLogger->info("OffsetFlag {}",pExecOrder->OffsetFlag);
		tdLogger->info("HedgeFlag {}",pExecOrder->HedgeFlag);
		tdLogger->info("ActionType {}",pExecOrder->ActionType);
		tdLogger->info("PosiDirection {}",pExecOrder->PosiDirection);
		tdLogger->info("ReservePositionFlag {}",pExecOrder->ReservePositionFlag);
		tdLogger->info("CloseFlag {}",pExecOrder->CloseFlag);
		tdLogger->info("OrderSubmitStatus {}",pExecOrder->OrderSubmitStatus);
		tdLogger->info("ExecResult {}",pExecOrder->ExecResult);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryExecOrder>");
};

void TdSpi::OnRspQryForQuote(CThostFtdcForQuoteField *pForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryForQuote>");
	if (pForQuote)
	{
		tdLogger->info("BrokerID {}",pForQuote->BrokerID);
		tdLogger->info("InvestorID {}",pForQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pForQuote->InstrumentID);
		tdLogger->info("ForQuoteRef {}",pForQuote->ForQuoteRef);
		tdLogger->info("UserID {}",pForQuote->UserID);
		tdLogger->info("ForQuoteLocalID {}",pForQuote->ForQuoteLocalID);
		tdLogger->info("ExchangeID {}",pForQuote->ExchangeID);
		tdLogger->info("ParticipantID {}",pForQuote->ParticipantID);
		tdLogger->info("ClientID {}",pForQuote->ClientID);
		tdLogger->info("ExchangeInstID {}",pForQuote->ExchangeInstID);
		tdLogger->info("TraderID {}",pForQuote->TraderID);
		tdLogger->info("InsertDate {}",pForQuote->InsertDate);
		tdLogger->info("InsertTime {}",pForQuote->InsertTime);
		tdLogger->info("StatusMsg {}",pForQuote->StatusMsg);
		tdLogger->info("ActiveUserID {}",pForQuote->ActiveUserID);
		tdLogger->info("InvestUnitID {}",pForQuote->InvestUnitID);
		tdLogger->info("IPAddress {}",pForQuote->IPAddress);
		tdLogger->info("MacAddress {}",pForQuote->MacAddress);
		tdLogger->info("InstallID {}",pForQuote->InstallID);
		tdLogger->info("FrontID {}",pForQuote->FrontID);
		tdLogger->info("SessionID {}",pForQuote->SessionID);
		tdLogger->info("BrokerForQutoSeq {}",pForQuote->BrokerForQutoSeq);
		tdLogger->info("ForQuoteStatus {}",pForQuote->ForQuoteStatus);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryForQuote>");
};

void TdSpi::OnRspQryQuote(CThostFtdcQuoteField *pQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryQuote>");
	if (pQuote)
	{
		tdLogger->info("BrokerID {}",pQuote->BrokerID);
		tdLogger->info("InvestorID {}",pQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pQuote->InstrumentID);
		tdLogger->info("QuoteRef {}",pQuote->QuoteRef);
		tdLogger->info("UserID {}",pQuote->UserID);
		tdLogger->info("BusinessUnit {}",pQuote->BusinessUnit);
		tdLogger->info("QuoteLocalID {}",pQuote->QuoteLocalID);
		tdLogger->info("ExchangeID {}",pQuote->ExchangeID);
		tdLogger->info("ParticipantID {}",pQuote->ParticipantID);
		tdLogger->info("ClientID {}",pQuote->ClientID);
		tdLogger->info("ExchangeInstID {}",pQuote->ExchangeInstID);
		tdLogger->info("TraderID {}",pQuote->TraderID);
		tdLogger->info("TradingDay {}",pQuote->TradingDay);
		tdLogger->info("QuoteSysID {}",pQuote->QuoteSysID);
		tdLogger->info("InsertDate {}",pQuote->InsertDate);
		tdLogger->info("InsertTime {}",pQuote->InsertTime);
		tdLogger->info("CancelTime {}",pQuote->CancelTime);
		tdLogger->info("ClearingPartID {}",pQuote->ClearingPartID);
		tdLogger->info("AskOrderSysID {}",pQuote->AskOrderSysID);
		tdLogger->info("BidOrderSysID {}",pQuote->BidOrderSysID);
		tdLogger->info("UserProductInfo {}",pQuote->UserProductInfo);
		tdLogger->info("StatusMsg {}",pQuote->StatusMsg);
		tdLogger->info("ActiveUserID {}",pQuote->ActiveUserID);
		tdLogger->info("AskOrderRef {}",pQuote->AskOrderRef);
		tdLogger->info("BidOrderRef {}",pQuote->BidOrderRef);
		tdLogger->info("ForQuoteSysID {}",pQuote->ForQuoteSysID);
		tdLogger->info("BranchID {}",pQuote->BranchID);
		tdLogger->info("InvestUnitID {}",pQuote->InvestUnitID);
		tdLogger->info("AccountID {}",pQuote->AccountID);
		tdLogger->info("CurrencyID {}",pQuote->CurrencyID);
		tdLogger->info("IPAddress {}",pQuote->IPAddress);
		tdLogger->info("MacAddress {}",pQuote->MacAddress);
		tdLogger->info("AskVolume {}",pQuote->AskVolume);
		tdLogger->info("BidVolume {}",pQuote->BidVolume);
		tdLogger->info("RequestID {}",pQuote->RequestID);
		tdLogger->info("InstallID {}",pQuote->InstallID);
		tdLogger->info("NotifySequence {}",pQuote->NotifySequence);
		tdLogger->info("SettlementID {}",pQuote->SettlementID);
		tdLogger->info("SequenceNo {}",pQuote->SequenceNo);
		tdLogger->info("FrontID {}",pQuote->FrontID);
		tdLogger->info("SessionID {}",pQuote->SessionID);
		tdLogger->info("BrokerQuoteSeq {}",pQuote->BrokerQuoteSeq);
		tdLogger->info("AskOffsetFlag {}",pQuote->AskOffsetFlag);
		tdLogger->info("BidOffsetFlag {}",pQuote->BidOffsetFlag);
		tdLogger->info("AskHedgeFlag {}",pQuote->AskHedgeFlag);
		tdLogger->info("BidHedgeFlag {}",pQuote->BidHedgeFlag);
		tdLogger->info("OrderSubmitStatus {}",pQuote->OrderSubmitStatus);
		tdLogger->info("QuoteStatus {}",pQuote->QuoteStatus);
		tdLogger->info("AskPrice {}",pQuote->AskPrice);
		tdLogger->info("BidPrice {}",pQuote->BidPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryQuote>");
};

void TdSpi::OnRspQryOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryOptionSelfClose>");
	if (pOptionSelfClose)
	{
		tdLogger->info("BrokerID {}",pOptionSelfClose->BrokerID);
		tdLogger->info("InvestorID {}",pOptionSelfClose->InvestorID);
		tdLogger->info("InstrumentID {}",pOptionSelfClose->InstrumentID);
		tdLogger->info("OptionSelfCloseRef {}",pOptionSelfClose->OptionSelfCloseRef);
		tdLogger->info("UserID {}",pOptionSelfClose->UserID);
		tdLogger->info("BusinessUnit {}",pOptionSelfClose->BusinessUnit);
		tdLogger->info("OptionSelfCloseLocalID {}",pOptionSelfClose->OptionSelfCloseLocalID);
		tdLogger->info("ExchangeID {}",pOptionSelfClose->ExchangeID);
		tdLogger->info("ParticipantID {}",pOptionSelfClose->ParticipantID);
		tdLogger->info("ClientID {}",pOptionSelfClose->ClientID);
		tdLogger->info("ExchangeInstID {}",pOptionSelfClose->ExchangeInstID);
		tdLogger->info("TraderID {}",pOptionSelfClose->TraderID);
		tdLogger->info("TradingDay {}",pOptionSelfClose->TradingDay);
		tdLogger->info("OptionSelfCloseSysID {}",pOptionSelfClose->OptionSelfCloseSysID);
		tdLogger->info("InsertDate {}",pOptionSelfClose->InsertDate);
		tdLogger->info("InsertTime {}",pOptionSelfClose->InsertTime);
		tdLogger->info("CancelTime {}",pOptionSelfClose->CancelTime);
		tdLogger->info("ClearingPartID {}",pOptionSelfClose->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pOptionSelfClose->UserProductInfo);
		tdLogger->info("StatusMsg {}",pOptionSelfClose->StatusMsg);
		tdLogger->info("ActiveUserID {}",pOptionSelfClose->ActiveUserID);
		tdLogger->info("BranchID {}",pOptionSelfClose->BranchID);
		tdLogger->info("InvestUnitID {}",pOptionSelfClose->InvestUnitID);
		tdLogger->info("AccountID {}",pOptionSelfClose->AccountID);
		tdLogger->info("CurrencyID {}",pOptionSelfClose->CurrencyID);
		tdLogger->info("IPAddress {}",pOptionSelfClose->IPAddress);
		tdLogger->info("MacAddress {}",pOptionSelfClose->MacAddress);
		tdLogger->info("Volume {}",pOptionSelfClose->Volume);
		tdLogger->info("RequestID {}",pOptionSelfClose->RequestID);
		tdLogger->info("InstallID {}",pOptionSelfClose->InstallID);
		tdLogger->info("NotifySequence {}",pOptionSelfClose->NotifySequence);
		tdLogger->info("SettlementID {}",pOptionSelfClose->SettlementID);
		tdLogger->info("SequenceNo {}",pOptionSelfClose->SequenceNo);
		tdLogger->info("FrontID {}",pOptionSelfClose->FrontID);
		tdLogger->info("SessionID {}",pOptionSelfClose->SessionID);
		tdLogger->info("BrokerOptionSelfCloseSeq {}",pOptionSelfClose->BrokerOptionSelfCloseSeq);
		tdLogger->info("HedgeFlag {}",pOptionSelfClose->HedgeFlag);
		tdLogger->info("OptSelfCloseFlag {}",pOptionSelfClose->OptSelfCloseFlag);
		tdLogger->info("OrderSubmitStatus {}",pOptionSelfClose->OrderSubmitStatus);
		tdLogger->info("ExecResult {}",pOptionSelfClose->ExecResult);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryOptionSelfClose>");
};

void TdSpi::OnRspQryInvestUnit(CThostFtdcInvestUnitField *pInvestUnit, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryInvestUnit>");
	if (pInvestUnit)
	{
		tdLogger->info("BrokerID {}",pInvestUnit->BrokerID);
		tdLogger->info("InvestorID {}",pInvestUnit->InvestorID);
		tdLogger->info("InvestUnitID {}",pInvestUnit->InvestUnitID);
		tdLogger->info("InvestorUnitName {}",pInvestUnit->InvestorUnitName);
		tdLogger->info("InvestorGroupID {}",pInvestUnit->InvestorGroupID);
		tdLogger->info("CommModelID {}",pInvestUnit->CommModelID);
		tdLogger->info("MarginModelID {}",pInvestUnit->MarginModelID);
		tdLogger->info("AccountID {}",pInvestUnit->AccountID);
		tdLogger->info("CurrencyID {}",pInvestUnit->CurrencyID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryInvestUnit>");
};

void TdSpi::OnRspQryCombInstrumentGuard(CThostFtdcCombInstrumentGuardField *pCombInstrumentGuard, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryCombInstrumentGuard>");
	if (pCombInstrumentGuard)
	{
		tdLogger->info("BrokerID {}",pCombInstrumentGuard->BrokerID);
		tdLogger->info("InstrumentID {}",pCombInstrumentGuard->InstrumentID);
		tdLogger->info("ExchangeID {}",pCombInstrumentGuard->ExchangeID);
		tdLogger->info("GuarantRatio {}",pCombInstrumentGuard->GuarantRatio);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryCombInstrumentGuard>");
};

void TdSpi::OnRspQryCombAction(CThostFtdcCombActionField *pCombAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryCombAction>");
	if (pCombAction)
	{
		tdLogger->info("BrokerID {}",pCombAction->BrokerID);
		tdLogger->info("InvestorID {}",pCombAction->InvestorID);
		tdLogger->info("InstrumentID {}",pCombAction->InstrumentID);
		tdLogger->info("CombActionRef {}",pCombAction->CombActionRef);
		tdLogger->info("UserID {}",pCombAction->UserID);
		tdLogger->info("ActionLocalID {}",pCombAction->ActionLocalID);
		tdLogger->info("ExchangeID {}",pCombAction->ExchangeID);
		tdLogger->info("ParticipantID {}",pCombAction->ParticipantID);
		tdLogger->info("ClientID {}",pCombAction->ClientID);
		tdLogger->info("ExchangeInstID {}",pCombAction->ExchangeInstID);
		tdLogger->info("TraderID {}",pCombAction->TraderID);
		tdLogger->info("TradingDay {}",pCombAction->TradingDay);
		tdLogger->info("UserProductInfo {}",pCombAction->UserProductInfo);
		tdLogger->info("StatusMsg {}",pCombAction->StatusMsg);
		tdLogger->info("IPAddress {}",pCombAction->IPAddress);
		tdLogger->info("MacAddress {}",pCombAction->MacAddress);
		tdLogger->info("ComTradeID {}",pCombAction->ComTradeID);
		tdLogger->info("BranchID {}",pCombAction->BranchID);
		tdLogger->info("InvestUnitID {}",pCombAction->InvestUnitID);
		tdLogger->info("Volume {}",pCombAction->Volume);
		tdLogger->info("InstallID {}",pCombAction->InstallID);
		tdLogger->info("NotifySequence {}",pCombAction->NotifySequence);
		tdLogger->info("SettlementID {}",pCombAction->SettlementID);
		tdLogger->info("SequenceNo {}",pCombAction->SequenceNo);
		tdLogger->info("FrontID {}",pCombAction->FrontID);
		tdLogger->info("SessionID {}",pCombAction->SessionID);
		tdLogger->info("Direction {}",pCombAction->Direction);
		tdLogger->info("CombDirection {}",pCombAction->CombDirection);
		tdLogger->info("HedgeFlag {}",pCombAction->HedgeFlag);
		tdLogger->info("ActionStatus {}",pCombAction->ActionStatus);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryCombAction>");
};

void TdSpi::OnRspQryTransferSerial(CThostFtdcTransferSerialField *pTransferSerial, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryTransferSerial>");
	if (pTransferSerial)
	{
		tdLogger->info("TradeDate {}",pTransferSerial->TradeDate);
		tdLogger->info("TradingDay {}",pTransferSerial->TradingDay);
		tdLogger->info("TradeTime {}",pTransferSerial->TradeTime);
		tdLogger->info("TradeCode {}",pTransferSerial->TradeCode);
		tdLogger->info("BankID {}",pTransferSerial->BankID);
		tdLogger->info("BankBranchID {}",pTransferSerial->BankBranchID);
		tdLogger->info("BankAccount {}",pTransferSerial->BankAccount);
		tdLogger->info("BankSerial {}",pTransferSerial->BankSerial);
		tdLogger->info("BrokerID {}",pTransferSerial->BrokerID);
		tdLogger->info("BrokerBranchID {}",pTransferSerial->BrokerBranchID);
		tdLogger->info("AccountID {}",pTransferSerial->AccountID);
		tdLogger->info("InvestorID {}",pTransferSerial->InvestorID);
		tdLogger->info("IdentifiedCardNo {}",pTransferSerial->IdentifiedCardNo);
		tdLogger->info("CurrencyID {}",pTransferSerial->CurrencyID);
		tdLogger->info("OperatorCode {}",pTransferSerial->OperatorCode);
		tdLogger->info("BankNewAccount {}",pTransferSerial->BankNewAccount);
		tdLogger->info("ErrorMsg {}",pTransferSerial->ErrorMsg);
		tdLogger->info("PlateSerial {}",pTransferSerial->PlateSerial);
		tdLogger->info("SessionID {}",pTransferSerial->SessionID);
		tdLogger->info("FutureSerial {}",pTransferSerial->FutureSerial);
		tdLogger->info("ErrorID {}",pTransferSerial->ErrorID);
		tdLogger->info("BankAccType {}",pTransferSerial->BankAccType);
		tdLogger->info("FutureAccType {}",pTransferSerial->FutureAccType);
		tdLogger->info("IdCardType {}",pTransferSerial->IdCardType);
		tdLogger->info("AvailabilityFlag {}",pTransferSerial->AvailabilityFlag);
		tdLogger->info("TradeAmount {}",pTransferSerial->TradeAmount);
		tdLogger->info("CustFee {}",pTransferSerial->CustFee);
		tdLogger->info("BrokerFee {}",pTransferSerial->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryTransferSerial>");
};

void TdSpi::OnRspQryAccountregister(CThostFtdcAccountregisterField *pAccountregister, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryAccountregister>");
	if (pAccountregister)
	{
		tdLogger->info("TradeDay {}",pAccountregister->TradeDay);
		tdLogger->info("BankID {}",pAccountregister->BankID);
		tdLogger->info("BankBranchID {}",pAccountregister->BankBranchID);
		tdLogger->info("BankAccount {}",pAccountregister->BankAccount);
		tdLogger->info("BrokerID {}",pAccountregister->BrokerID);
		tdLogger->info("BrokerBranchID {}",pAccountregister->BrokerBranchID);
		tdLogger->info("AccountID {}",pAccountregister->AccountID);
		tdLogger->info("IdentifiedCardNo {}",pAccountregister->IdentifiedCardNo);
		tdLogger->info("CustomerName {}",pAccountregister->CustomerName);
		tdLogger->info("CurrencyID {}",pAccountregister->CurrencyID);
		tdLogger->info("RegDate {}",pAccountregister->RegDate);
		tdLogger->info("OutDate {}",pAccountregister->OutDate);
		tdLogger->info("LongCustomerName {}",pAccountregister->LongCustomerName);
		tdLogger->info("TID {}",pAccountregister->TID);
		tdLogger->info("IdCardType {}",pAccountregister->IdCardType);
		tdLogger->info("OpenOrDestroy {}",pAccountregister->OpenOrDestroy);
		tdLogger->info("CustType {}",pAccountregister->CustType);
		tdLogger->info("BankAccType {}",pAccountregister->BankAccType);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryAccountregister>");
};

void TdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspError>");
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspError>");
};
void TdSpi::OnRtnOrder(CThostFtdcOrderField *pOrder) 
{
	tdLogger->info("<OnRtnOrder>");
	if (pOrder)
	{
		tdLogger->info("BrokerID {}",pOrder->BrokerID);
		tdLogger->info("InvestorID {}",pOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pOrder->OrderRef);
		tdLogger->info("UserID {}",pOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pOrder->BusinessUnit);
		tdLogger->info("OrderLocalID {}",pOrder->OrderLocalID);
		tdLogger->info("ExchangeID {}",pOrder->ExchangeID);
		tdLogger->info("ParticipantID {}",pOrder->ParticipantID);
		tdLogger->info("ClientID {}",pOrder->ClientID);
		tdLogger->info("ExchangeInstID {}",pOrder->ExchangeInstID);
		tdLogger->info("TraderID {}",pOrder->TraderID);
		tdLogger->info("TradingDay {}",pOrder->TradingDay);
		tdLogger->info("OrderSysID {}",pOrder->OrderSysID);
		tdLogger->info("InsertDate {}",pOrder->InsertDate);
		tdLogger->info("InsertTime {}",pOrder->InsertTime);
		tdLogger->info("ActiveTime {}",pOrder->ActiveTime);
		tdLogger->info("SuspendTime {}",pOrder->SuspendTime);
		tdLogger->info("UpdateTime {}",pOrder->UpdateTime);
		tdLogger->info("CancelTime {}",pOrder->CancelTime);
		tdLogger->info("ActiveTraderID {}",pOrder->ActiveTraderID);
		tdLogger->info("ClearingPartID {}",pOrder->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pOrder->UserProductInfo);
		tdLogger->info("StatusMsg {}",pOrder->StatusMsg);
		tdLogger->info("ActiveUserID {}",pOrder->ActiveUserID);
		tdLogger->info("RelativeOrderSysID {}",pOrder->RelativeOrderSysID);
		tdLogger->info("BranchID {}",pOrder->BranchID);
		tdLogger->info("InvestUnitID {}",pOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pOrder->AccountID);
		tdLogger->info("CurrencyID {}",pOrder->CurrencyID);
		tdLogger->info("IPAddress {}",pOrder->IPAddress);
		tdLogger->info("MacAddress {}",pOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pOrder->RequestID);
		tdLogger->info("InstallID {}",pOrder->InstallID);
		tdLogger->info("NotifySequence {}",pOrder->NotifySequence);
		tdLogger->info("SettlementID {}",pOrder->SettlementID);
		tdLogger->info("VolumeTraded {}",pOrder->VolumeTraded);
		tdLogger->info("VolumeTotal {}",pOrder->VolumeTotal);
		tdLogger->info("SequenceNo {}",pOrder->SequenceNo);
		tdLogger->info("FrontID {}",pOrder->FrontID);
		tdLogger->info("SessionID {}",pOrder->SessionID);
		tdLogger->info("UserForceClose {}",pOrder->UserForceClose);
		tdLogger->info("BrokerOrderSeq {}",pOrder->BrokerOrderSeq);
		tdLogger->info("ZCETotalTradedVolume {}",pOrder->ZCETotalTradedVolume);
		tdLogger->info("IsSwapOrder {}",pOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pOrder->OrderPriceType);
		tdLogger->info("Direction {}",pOrder->Direction);
		tdLogger->info("TimeCondition {}",pOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pOrder->ForceCloseReason);
		tdLogger->info("OrderSubmitStatus {}",pOrder->OrderSubmitStatus);
		tdLogger->info("OrderSource {}",pOrder->OrderSource);
		tdLogger->info("OrderStatus {}",pOrder->OrderStatus);
		tdLogger->info("OrderType {}",pOrder->OrderType);
		tdLogger->info("LimitPrice {}",pOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pOrder->StopPrice);
	}
	tdLogger->info("</OnRtnOrder>");
};
void TdSpi::OnRtnTrade(CThostFtdcTradeField *pTrade) 
{
	tdLogger->info("<OnRtnTrade>");
	if (pTrade)
	{
		tdLogger->info("BrokerID {}",pTrade->BrokerID);
		tdLogger->info("InvestorID {}",pTrade->InvestorID);
		tdLogger->info("InstrumentID {}",pTrade->InstrumentID);
		tdLogger->info("OrderRef {}",pTrade->OrderRef);
		tdLogger->info("UserID {}",pTrade->UserID);
		tdLogger->info("ExchangeID {}",pTrade->ExchangeID);
		tdLogger->info("TradeID {}",pTrade->TradeID);
		tdLogger->info("OrderSysID {}",pTrade->OrderSysID);
		tdLogger->info("ParticipantID {}",pTrade->ParticipantID);
		tdLogger->info("ClientID {}",pTrade->ClientID);
		tdLogger->info("ExchangeInstID {}",pTrade->ExchangeInstID);
		tdLogger->info("TradeDate {}",pTrade->TradeDate);
		tdLogger->info("TradeTime {}",pTrade->TradeTime);
		tdLogger->info("TraderID {}",pTrade->TraderID);
		tdLogger->info("OrderLocalID {}",pTrade->OrderLocalID);
		tdLogger->info("ClearingPartID {}",pTrade->ClearingPartID);
		tdLogger->info("BusinessUnit {}",pTrade->BusinessUnit);
		tdLogger->info("TradingDay {}",pTrade->TradingDay);
		tdLogger->info("InvestUnitID {}",pTrade->InvestUnitID);
		tdLogger->info("Volume {}",pTrade->Volume);
		tdLogger->info("SequenceNo {}",pTrade->SequenceNo);
		tdLogger->info("SettlementID {}",pTrade->SettlementID);
		tdLogger->info("BrokerOrderSeq {}",pTrade->BrokerOrderSeq);
		tdLogger->info("Direction {}",pTrade->Direction);
		tdLogger->info("TradingRole {}",pTrade->TradingRole);
		tdLogger->info("OffsetFlag {}",pTrade->OffsetFlag);
		tdLogger->info("HedgeFlag {}",pTrade->HedgeFlag);
		tdLogger->info("TradeType {}",pTrade->TradeType);
		tdLogger->info("PriceSource {}",pTrade->PriceSource);
		tdLogger->info("TradeSource {}",pTrade->TradeSource);
		tdLogger->info("Price {}",pTrade->Price);
	}
	tdLogger->info("</OnRtnTrade>");
};
void TdSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnOrderInsert>");
	if (pInputOrder)
	{
		tdLogger->info("BrokerID {}",pInputOrder->BrokerID);
		tdLogger->info("InvestorID {}",pInputOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pInputOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pInputOrder->OrderRef);
		tdLogger->info("UserID {}",pInputOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pInputOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pInputOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pInputOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pInputOrder->BusinessUnit);
		tdLogger->info("ExchangeID {}",pInputOrder->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pInputOrder->AccountID);
		tdLogger->info("CurrencyID {}",pInputOrder->CurrencyID);
		tdLogger->info("ClientID {}",pInputOrder->ClientID);
		tdLogger->info("IPAddress {}",pInputOrder->IPAddress);
		tdLogger->info("MacAddress {}",pInputOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pInputOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pInputOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pInputOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pInputOrder->RequestID);
		tdLogger->info("UserForceClose {}",pInputOrder->UserForceClose);
		tdLogger->info("IsSwapOrder {}",pInputOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pInputOrder->OrderPriceType);
		tdLogger->info("Direction {}",pInputOrder->Direction);
		tdLogger->info("TimeCondition {}",pInputOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pInputOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pInputOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pInputOrder->ForceCloseReason);
		tdLogger->info("LimitPrice {}",pInputOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pInputOrder->StopPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnOrderInsert>");
};
void TdSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnOrderAction>");
	if (pOrderAction)
	{
		tdLogger->info("BrokerID {}",pOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pOrderAction->InvestorID);
		tdLogger->info("OrderRef {}",pOrderAction->OrderRef);
		tdLogger->info("ExchangeID {}",pOrderAction->ExchangeID);
		tdLogger->info("OrderSysID {}",pOrderAction->OrderSysID);
		tdLogger->info("ActionDate {}",pOrderAction->ActionDate);
		tdLogger->info("ActionTime {}",pOrderAction->ActionTime);
		tdLogger->info("TraderID {}",pOrderAction->TraderID);
		tdLogger->info("OrderLocalID {}",pOrderAction->OrderLocalID);
		tdLogger->info("ActionLocalID {}",pOrderAction->ActionLocalID);
		tdLogger->info("ParticipantID {}",pOrderAction->ParticipantID);
		tdLogger->info("ClientID {}",pOrderAction->ClientID);
		tdLogger->info("BusinessUnit {}",pOrderAction->BusinessUnit);
		tdLogger->info("UserID {}",pOrderAction->UserID);
		tdLogger->info("StatusMsg {}",pOrderAction->StatusMsg);
		tdLogger->info("InstrumentID {}",pOrderAction->InstrumentID);
		tdLogger->info("BranchID {}",pOrderAction->BranchID);
		tdLogger->info("InvestUnitID {}",pOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pOrderAction->MacAddress);
		tdLogger->info("OrderActionRef {}",pOrderAction->OrderActionRef);
		tdLogger->info("RequestID {}",pOrderAction->RequestID);
		tdLogger->info("FrontID {}",pOrderAction->FrontID);
		tdLogger->info("SessionID {}",pOrderAction->SessionID);
		tdLogger->info("VolumeChange {}",pOrderAction->VolumeChange);
		tdLogger->info("InstallID {}",pOrderAction->InstallID);
		tdLogger->info("ActionFlag {}",pOrderAction->ActionFlag);
		tdLogger->info("OrderActionStatus {}",pOrderAction->OrderActionStatus);
		tdLogger->info("LimitPrice {}",pOrderAction->LimitPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnOrderAction>");
};
void TdSpi::OnRtnInstrumentStatus(CThostFtdcInstrumentStatusField *pInstrumentStatus) 
{
	tdLogger->info("<OnRtnInstrumentStatus>");
	if (pInstrumentStatus)
	{
		tdLogger->info("ExchangeID {}",pInstrumentStatus->ExchangeID);
		tdLogger->info("ExchangeInstID {}",pInstrumentStatus->ExchangeInstID);
		tdLogger->info("SettlementGroupID {}",pInstrumentStatus->SettlementGroupID);
		tdLogger->info("InstrumentID {}",pInstrumentStatus->InstrumentID);
		tdLogger->info("EnterTime {}",pInstrumentStatus->EnterTime);
		tdLogger->info("TradingSegmentSN {}",pInstrumentStatus->TradingSegmentSN);
		tdLogger->info("InstrumentStatus {}",pInstrumentStatus->InstrumentStatus);
		tdLogger->info("EnterReason {}",pInstrumentStatus->EnterReason);
	}
	tdLogger->info("</OnRtnInstrumentStatus>");
};
void TdSpi::OnRtnBulletin(CThostFtdcBulletinField *pBulletin) 
{
	tdLogger->info("<OnRtnBulletin>");
	if (pBulletin)
	{
		tdLogger->info("ExchangeID {}",pBulletin->ExchangeID);
		tdLogger->info("TradingDay {}",pBulletin->TradingDay);
		tdLogger->info("NewsType {}",pBulletin->NewsType);
		tdLogger->info("SendTime {}",pBulletin->SendTime);
		tdLogger->info("Abstract {}",pBulletin->Abstract);
		tdLogger->info("ComeFrom {}",pBulletin->ComeFrom);
		tdLogger->info("Content {}",pBulletin->Content);
		tdLogger->info("URLLink {}",pBulletin->URLLink);
		tdLogger->info("MarketID {}",pBulletin->MarketID);
		tdLogger->info("BulletinID {}",pBulletin->BulletinID);
		tdLogger->info("SequenceNo {}",pBulletin->SequenceNo);
		tdLogger->info("NewsUrgency {}",pBulletin->NewsUrgency);
	}
	tdLogger->info("</OnRtnBulletin>");
};
void TdSpi::OnRtnTradingNotice(CThostFtdcTradingNoticeInfoField *pTradingNoticeInfo) 
{
	tdLogger->info("<OnRtnTradingNotice>");
	if (pTradingNoticeInfo)
	{
		tdLogger->info("BrokerID {}",pTradingNoticeInfo->BrokerID);
		tdLogger->info("InvestorID {}",pTradingNoticeInfo->InvestorID);
		tdLogger->info("SendTime {}",pTradingNoticeInfo->SendTime);
		tdLogger->info("FieldContent {}",pTradingNoticeInfo->FieldContent);
		tdLogger->info("InvestUnitID {}",pTradingNoticeInfo->InvestUnitID);
		tdLogger->info("SequenceNo {}",pTradingNoticeInfo->SequenceNo);
		tdLogger->info("SequenceSeries {}",pTradingNoticeInfo->SequenceSeries);
	}
	tdLogger->info("</OnRtnTradingNotice>");
};
void TdSpi::OnRtnErrorConditionalOrder(CThostFtdcErrorConditionalOrderField *pErrorConditionalOrder) 
{
	tdLogger->info("<OnRtnErrorConditionalOrder>");
	if (pErrorConditionalOrder)
	{
		tdLogger->info("BrokerID {}",pErrorConditionalOrder->BrokerID);
		tdLogger->info("InvestorID {}",pErrorConditionalOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pErrorConditionalOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pErrorConditionalOrder->OrderRef);
		tdLogger->info("UserID {}",pErrorConditionalOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pErrorConditionalOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pErrorConditionalOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pErrorConditionalOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pErrorConditionalOrder->BusinessUnit);
		tdLogger->info("OrderLocalID {}",pErrorConditionalOrder->OrderLocalID);
		tdLogger->info("ExchangeID {}",pErrorConditionalOrder->ExchangeID);
		tdLogger->info("ParticipantID {}",pErrorConditionalOrder->ParticipantID);
		tdLogger->info("ClientID {}",pErrorConditionalOrder->ClientID);
		tdLogger->info("ExchangeInstID {}",pErrorConditionalOrder->ExchangeInstID);
		tdLogger->info("TraderID {}",pErrorConditionalOrder->TraderID);
		tdLogger->info("TradingDay {}",pErrorConditionalOrder->TradingDay);
		tdLogger->info("OrderSysID {}",pErrorConditionalOrder->OrderSysID);
		tdLogger->info("InsertDate {}",pErrorConditionalOrder->InsertDate);
		tdLogger->info("InsertTime {}",pErrorConditionalOrder->InsertTime);
		tdLogger->info("ActiveTime {}",pErrorConditionalOrder->ActiveTime);
		tdLogger->info("SuspendTime {}",pErrorConditionalOrder->SuspendTime);
		tdLogger->info("UpdateTime {}",pErrorConditionalOrder->UpdateTime);
		tdLogger->info("CancelTime {}",pErrorConditionalOrder->CancelTime);
		tdLogger->info("ActiveTraderID {}",pErrorConditionalOrder->ActiveTraderID);
		tdLogger->info("ClearingPartID {}",pErrorConditionalOrder->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pErrorConditionalOrder->UserProductInfo);
		tdLogger->info("StatusMsg {}",pErrorConditionalOrder->StatusMsg);
		tdLogger->info("ActiveUserID {}",pErrorConditionalOrder->ActiveUserID);
		tdLogger->info("RelativeOrderSysID {}",pErrorConditionalOrder->RelativeOrderSysID);
		tdLogger->info("ErrorMsg {}",pErrorConditionalOrder->ErrorMsg);
		tdLogger->info("BranchID {}",pErrorConditionalOrder->BranchID);
		tdLogger->info("InvestUnitID {}",pErrorConditionalOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pErrorConditionalOrder->AccountID);
		tdLogger->info("CurrencyID {}",pErrorConditionalOrder->CurrencyID);
		tdLogger->info("IPAddress {}",pErrorConditionalOrder->IPAddress);
		tdLogger->info("MacAddress {}",pErrorConditionalOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pErrorConditionalOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pErrorConditionalOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pErrorConditionalOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pErrorConditionalOrder->RequestID);
		tdLogger->info("InstallID {}",pErrorConditionalOrder->InstallID);
		tdLogger->info("NotifySequence {}",pErrorConditionalOrder->NotifySequence);
		tdLogger->info("SettlementID {}",pErrorConditionalOrder->SettlementID);
		tdLogger->info("VolumeTraded {}",pErrorConditionalOrder->VolumeTraded);
		tdLogger->info("VolumeTotal {}",pErrorConditionalOrder->VolumeTotal);
		tdLogger->info("SequenceNo {}",pErrorConditionalOrder->SequenceNo);
		tdLogger->info("FrontID {}",pErrorConditionalOrder->FrontID);
		tdLogger->info("SessionID {}",pErrorConditionalOrder->SessionID);
		tdLogger->info("UserForceClose {}",pErrorConditionalOrder->UserForceClose);
		tdLogger->info("BrokerOrderSeq {}",pErrorConditionalOrder->BrokerOrderSeq);
		tdLogger->info("ZCETotalTradedVolume {}",pErrorConditionalOrder->ZCETotalTradedVolume);
		tdLogger->info("ErrorID {}",pErrorConditionalOrder->ErrorID);
		tdLogger->info("IsSwapOrder {}",pErrorConditionalOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pErrorConditionalOrder->OrderPriceType);
		tdLogger->info("Direction {}",pErrorConditionalOrder->Direction);
		tdLogger->info("TimeCondition {}",pErrorConditionalOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pErrorConditionalOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pErrorConditionalOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pErrorConditionalOrder->ForceCloseReason);
		tdLogger->info("OrderSubmitStatus {}",pErrorConditionalOrder->OrderSubmitStatus);
		tdLogger->info("OrderSource {}",pErrorConditionalOrder->OrderSource);
		tdLogger->info("OrderStatus {}",pErrorConditionalOrder->OrderStatus);
		tdLogger->info("OrderType {}",pErrorConditionalOrder->OrderType);
		tdLogger->info("LimitPrice {}",pErrorConditionalOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pErrorConditionalOrder->StopPrice);
	}
	tdLogger->info("</OnRtnErrorConditionalOrder>");
};
void TdSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder) 
{
	tdLogger->info("<OnRtnExecOrder>");
	if (pExecOrder)
	{
		tdLogger->info("BrokerID {}",pExecOrder->BrokerID);
		tdLogger->info("InvestorID {}",pExecOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pExecOrder->InstrumentID);
		tdLogger->info("ExecOrderRef {}",pExecOrder->ExecOrderRef);
		tdLogger->info("UserID {}",pExecOrder->UserID);
		tdLogger->info("BusinessUnit {}",pExecOrder->BusinessUnit);
		tdLogger->info("ExecOrderLocalID {}",pExecOrder->ExecOrderLocalID);
		tdLogger->info("ExchangeID {}",pExecOrder->ExchangeID);
		tdLogger->info("ParticipantID {}",pExecOrder->ParticipantID);
		tdLogger->info("ClientID {}",pExecOrder->ClientID);
		tdLogger->info("ExchangeInstID {}",pExecOrder->ExchangeInstID);
		tdLogger->info("TraderID {}",pExecOrder->TraderID);
		tdLogger->info("TradingDay {}",pExecOrder->TradingDay);
		tdLogger->info("ExecOrderSysID {}",pExecOrder->ExecOrderSysID);
		tdLogger->info("InsertDate {}",pExecOrder->InsertDate);
		tdLogger->info("InsertTime {}",pExecOrder->InsertTime);
		tdLogger->info("CancelTime {}",pExecOrder->CancelTime);
		tdLogger->info("ClearingPartID {}",pExecOrder->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pExecOrder->UserProductInfo);
		tdLogger->info("StatusMsg {}",pExecOrder->StatusMsg);
		tdLogger->info("ActiveUserID {}",pExecOrder->ActiveUserID);
		tdLogger->info("BranchID {}",pExecOrder->BranchID);
		tdLogger->info("InvestUnitID {}",pExecOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pExecOrder->AccountID);
		tdLogger->info("CurrencyID {}",pExecOrder->CurrencyID);
		tdLogger->info("IPAddress {}",pExecOrder->IPAddress);
		tdLogger->info("MacAddress {}",pExecOrder->MacAddress);
		tdLogger->info("Volume {}",pExecOrder->Volume);
		tdLogger->info("RequestID {}",pExecOrder->RequestID);
		tdLogger->info("InstallID {}",pExecOrder->InstallID);
		tdLogger->info("NotifySequence {}",pExecOrder->NotifySequence);
		tdLogger->info("SettlementID {}",pExecOrder->SettlementID);
		tdLogger->info("SequenceNo {}",pExecOrder->SequenceNo);
		tdLogger->info("FrontID {}",pExecOrder->FrontID);
		tdLogger->info("SessionID {}",pExecOrder->SessionID);
		tdLogger->info("BrokerExecOrderSeq {}",pExecOrder->BrokerExecOrderSeq);
		tdLogger->info("OffsetFlag {}",pExecOrder->OffsetFlag);
		tdLogger->info("HedgeFlag {}",pExecOrder->HedgeFlag);
		tdLogger->info("ActionType {}",pExecOrder->ActionType);
		tdLogger->info("PosiDirection {}",pExecOrder->PosiDirection);
		tdLogger->info("ReservePositionFlag {}",pExecOrder->ReservePositionFlag);
		tdLogger->info("CloseFlag {}",pExecOrder->CloseFlag);
		tdLogger->info("OrderSubmitStatus {}",pExecOrder->OrderSubmitStatus);
		tdLogger->info("ExecResult {}",pExecOrder->ExecResult);
	}
	tdLogger->info("</OnRtnExecOrder>");
};
void TdSpi::OnErrRtnExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnExecOrderInsert>");
	if (pInputExecOrder)
	{
		tdLogger->info("BrokerID {}",pInputExecOrder->BrokerID);
		tdLogger->info("InvestorID {}",pInputExecOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pInputExecOrder->InstrumentID);
		tdLogger->info("ExecOrderRef {}",pInputExecOrder->ExecOrderRef);
		tdLogger->info("UserID {}",pInputExecOrder->UserID);
		tdLogger->info("BusinessUnit {}",pInputExecOrder->BusinessUnit);
		tdLogger->info("ExchangeID {}",pInputExecOrder->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputExecOrder->InvestUnitID);
		tdLogger->info("AccountID {}",pInputExecOrder->AccountID);
		tdLogger->info("CurrencyID {}",pInputExecOrder->CurrencyID);
		tdLogger->info("ClientID {}",pInputExecOrder->ClientID);
		tdLogger->info("IPAddress {}",pInputExecOrder->IPAddress);
		tdLogger->info("MacAddress {}",pInputExecOrder->MacAddress);
		tdLogger->info("Volume {}",pInputExecOrder->Volume);
		tdLogger->info("RequestID {}",pInputExecOrder->RequestID);
		tdLogger->info("OffsetFlag {}",pInputExecOrder->OffsetFlag);
		tdLogger->info("HedgeFlag {}",pInputExecOrder->HedgeFlag);
		tdLogger->info("ActionType {}",pInputExecOrder->ActionType);
		tdLogger->info("PosiDirection {}",pInputExecOrder->PosiDirection);
		tdLogger->info("ReservePositionFlag {}",pInputExecOrder->ReservePositionFlag);
		tdLogger->info("CloseFlag {}",pInputExecOrder->CloseFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnExecOrderInsert>");
};
void TdSpi::OnErrRtnExecOrderAction(CThostFtdcExecOrderActionField *pExecOrderAction, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnExecOrderAction>");
	if (pExecOrderAction)
	{
		tdLogger->info("BrokerID {}",pExecOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pExecOrderAction->InvestorID);
		tdLogger->info("ExecOrderRef {}",pExecOrderAction->ExecOrderRef);
		tdLogger->info("ExchangeID {}",pExecOrderAction->ExchangeID);
		tdLogger->info("ExecOrderSysID {}",pExecOrderAction->ExecOrderSysID);
		tdLogger->info("ActionDate {}",pExecOrderAction->ActionDate);
		tdLogger->info("ActionTime {}",pExecOrderAction->ActionTime);
		tdLogger->info("TraderID {}",pExecOrderAction->TraderID);
		tdLogger->info("ExecOrderLocalID {}",pExecOrderAction->ExecOrderLocalID);
		tdLogger->info("ActionLocalID {}",pExecOrderAction->ActionLocalID);
		tdLogger->info("ParticipantID {}",pExecOrderAction->ParticipantID);
		tdLogger->info("ClientID {}",pExecOrderAction->ClientID);
		tdLogger->info("BusinessUnit {}",pExecOrderAction->BusinessUnit);
		tdLogger->info("UserID {}",pExecOrderAction->UserID);
		tdLogger->info("StatusMsg {}",pExecOrderAction->StatusMsg);
		tdLogger->info("InstrumentID {}",pExecOrderAction->InstrumentID);
		tdLogger->info("BranchID {}",pExecOrderAction->BranchID);
		tdLogger->info("InvestUnitID {}",pExecOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pExecOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pExecOrderAction->MacAddress);
		tdLogger->info("ExecOrderActionRef {}",pExecOrderAction->ExecOrderActionRef);
		tdLogger->info("RequestID {}",pExecOrderAction->RequestID);
		tdLogger->info("FrontID {}",pExecOrderAction->FrontID);
		tdLogger->info("SessionID {}",pExecOrderAction->SessionID);
		tdLogger->info("InstallID {}",pExecOrderAction->InstallID);
		tdLogger->info("ActionFlag {}",pExecOrderAction->ActionFlag);
		tdLogger->info("OrderActionStatus {}",pExecOrderAction->OrderActionStatus);
		tdLogger->info("ActionType {}",pExecOrderAction->ActionType);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnExecOrderAction>");
};
void TdSpi::OnErrRtnForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnForQuoteInsert>");
	if (pInputForQuote)
	{
		tdLogger->info("BrokerID {}",pInputForQuote->BrokerID);
		tdLogger->info("InvestorID {}",pInputForQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pInputForQuote->InstrumentID);
		tdLogger->info("ForQuoteRef {}",pInputForQuote->ForQuoteRef);
		tdLogger->info("UserID {}",pInputForQuote->UserID);
		tdLogger->info("ExchangeID {}",pInputForQuote->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputForQuote->InvestUnitID);
		tdLogger->info("IPAddress {}",pInputForQuote->IPAddress);
		tdLogger->info("MacAddress {}",pInputForQuote->MacAddress);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnForQuoteInsert>");
};
void TdSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote) 
{
	tdLogger->info("<OnRtnQuote>");
	if (pQuote)
	{
		tdLogger->info("BrokerID {}",pQuote->BrokerID);
		tdLogger->info("InvestorID {}",pQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pQuote->InstrumentID);
		tdLogger->info("QuoteRef {}",pQuote->QuoteRef);
		tdLogger->info("UserID {}",pQuote->UserID);
		tdLogger->info("BusinessUnit {}",pQuote->BusinessUnit);
		tdLogger->info("QuoteLocalID {}",pQuote->QuoteLocalID);
		tdLogger->info("ExchangeID {}",pQuote->ExchangeID);
		tdLogger->info("ParticipantID {}",pQuote->ParticipantID);
		tdLogger->info("ClientID {}",pQuote->ClientID);
		tdLogger->info("ExchangeInstID {}",pQuote->ExchangeInstID);
		tdLogger->info("TraderID {}",pQuote->TraderID);
		tdLogger->info("TradingDay {}",pQuote->TradingDay);
		tdLogger->info("QuoteSysID {}",pQuote->QuoteSysID);
		tdLogger->info("InsertDate {}",pQuote->InsertDate);
		tdLogger->info("InsertTime {}",pQuote->InsertTime);
		tdLogger->info("CancelTime {}",pQuote->CancelTime);
		tdLogger->info("ClearingPartID {}",pQuote->ClearingPartID);
		tdLogger->info("AskOrderSysID {}",pQuote->AskOrderSysID);
		tdLogger->info("BidOrderSysID {}",pQuote->BidOrderSysID);
		tdLogger->info("UserProductInfo {}",pQuote->UserProductInfo);
		tdLogger->info("StatusMsg {}",pQuote->StatusMsg);
		tdLogger->info("ActiveUserID {}",pQuote->ActiveUserID);
		tdLogger->info("AskOrderRef {}",pQuote->AskOrderRef);
		tdLogger->info("BidOrderRef {}",pQuote->BidOrderRef);
		tdLogger->info("ForQuoteSysID {}",pQuote->ForQuoteSysID);
		tdLogger->info("BranchID {}",pQuote->BranchID);
		tdLogger->info("InvestUnitID {}",pQuote->InvestUnitID);
		tdLogger->info("AccountID {}",pQuote->AccountID);
		tdLogger->info("CurrencyID {}",pQuote->CurrencyID);
		tdLogger->info("IPAddress {}",pQuote->IPAddress);
		tdLogger->info("MacAddress {}",pQuote->MacAddress);
		tdLogger->info("AskVolume {}",pQuote->AskVolume);
		tdLogger->info("BidVolume {}",pQuote->BidVolume);
		tdLogger->info("RequestID {}",pQuote->RequestID);
		tdLogger->info("InstallID {}",pQuote->InstallID);
		tdLogger->info("NotifySequence {}",pQuote->NotifySequence);
		tdLogger->info("SettlementID {}",pQuote->SettlementID);
		tdLogger->info("SequenceNo {}",pQuote->SequenceNo);
		tdLogger->info("FrontID {}",pQuote->FrontID);
		tdLogger->info("SessionID {}",pQuote->SessionID);
		tdLogger->info("BrokerQuoteSeq {}",pQuote->BrokerQuoteSeq);
		tdLogger->info("AskOffsetFlag {}",pQuote->AskOffsetFlag);
		tdLogger->info("BidOffsetFlag {}",pQuote->BidOffsetFlag);
		tdLogger->info("AskHedgeFlag {}",pQuote->AskHedgeFlag);
		tdLogger->info("BidHedgeFlag {}",pQuote->BidHedgeFlag);
		tdLogger->info("OrderSubmitStatus {}",pQuote->OrderSubmitStatus);
		tdLogger->info("QuoteStatus {}",pQuote->QuoteStatus);
		tdLogger->info("AskPrice {}",pQuote->AskPrice);
		tdLogger->info("BidPrice {}",pQuote->BidPrice);
	}
	tdLogger->info("</OnRtnQuote>");
};
void TdSpi::OnErrRtnQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnQuoteInsert>");
	if (pInputQuote)
	{
		tdLogger->info("BrokerID {}",pInputQuote->BrokerID);
		tdLogger->info("InvestorID {}",pInputQuote->InvestorID);
		tdLogger->info("InstrumentID {}",pInputQuote->InstrumentID);
		tdLogger->info("QuoteRef {}",pInputQuote->QuoteRef);
		tdLogger->info("UserID {}",pInputQuote->UserID);
		tdLogger->info("BusinessUnit {}",pInputQuote->BusinessUnit);
		tdLogger->info("AskOrderRef {}",pInputQuote->AskOrderRef);
		tdLogger->info("BidOrderRef {}",pInputQuote->BidOrderRef);
		tdLogger->info("ForQuoteSysID {}",pInputQuote->ForQuoteSysID);
		tdLogger->info("ExchangeID {}",pInputQuote->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputQuote->InvestUnitID);
		tdLogger->info("ClientID {}",pInputQuote->ClientID);
		tdLogger->info("IPAddress {}",pInputQuote->IPAddress);
		tdLogger->info("MacAddress {}",pInputQuote->MacAddress);
		tdLogger->info("AskVolume {}",pInputQuote->AskVolume);
		tdLogger->info("BidVolume {}",pInputQuote->BidVolume);
		tdLogger->info("RequestID {}",pInputQuote->RequestID);
		tdLogger->info("AskOffsetFlag {}",pInputQuote->AskOffsetFlag);
		tdLogger->info("BidOffsetFlag {}",pInputQuote->BidOffsetFlag);
		tdLogger->info("AskHedgeFlag {}",pInputQuote->AskHedgeFlag);
		tdLogger->info("BidHedgeFlag {}",pInputQuote->BidHedgeFlag);
		tdLogger->info("AskPrice {}",pInputQuote->AskPrice);
		tdLogger->info("BidPrice {}",pInputQuote->BidPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnQuoteInsert>");
};
void TdSpi::OnErrRtnQuoteAction(CThostFtdcQuoteActionField *pQuoteAction, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnQuoteAction>");
	if (pQuoteAction)
	{
		tdLogger->info("BrokerID {}",pQuoteAction->BrokerID);
		tdLogger->info("InvestorID {}",pQuoteAction->InvestorID);
		tdLogger->info("QuoteRef {}",pQuoteAction->QuoteRef);
		tdLogger->info("ExchangeID {}",pQuoteAction->ExchangeID);
		tdLogger->info("QuoteSysID {}",pQuoteAction->QuoteSysID);
		tdLogger->info("ActionDate {}",pQuoteAction->ActionDate);
		tdLogger->info("ActionTime {}",pQuoteAction->ActionTime);
		tdLogger->info("TraderID {}",pQuoteAction->TraderID);
		tdLogger->info("QuoteLocalID {}",pQuoteAction->QuoteLocalID);
		tdLogger->info("ActionLocalID {}",pQuoteAction->ActionLocalID);
		tdLogger->info("ParticipantID {}",pQuoteAction->ParticipantID);
		tdLogger->info("ClientID {}",pQuoteAction->ClientID);
		tdLogger->info("BusinessUnit {}",pQuoteAction->BusinessUnit);
		tdLogger->info("UserID {}",pQuoteAction->UserID);
		tdLogger->info("StatusMsg {}",pQuoteAction->StatusMsg);
		tdLogger->info("InstrumentID {}",pQuoteAction->InstrumentID);
		tdLogger->info("BranchID {}",pQuoteAction->BranchID);
		tdLogger->info("InvestUnitID {}",pQuoteAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pQuoteAction->IPAddress);
		tdLogger->info("MacAddress {}",pQuoteAction->MacAddress);
		tdLogger->info("QuoteActionRef {}",pQuoteAction->QuoteActionRef);
		tdLogger->info("RequestID {}",pQuoteAction->RequestID);
		tdLogger->info("FrontID {}",pQuoteAction->FrontID);
		tdLogger->info("SessionID {}",pQuoteAction->SessionID);
		tdLogger->info("InstallID {}",pQuoteAction->InstallID);
		tdLogger->info("ActionFlag {}",pQuoteAction->ActionFlag);
		tdLogger->info("OrderActionStatus {}",pQuoteAction->OrderActionStatus);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnQuoteAction>");
};
void TdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) 
{
	tdLogger->info("<OnRtnForQuoteRsp>");
	if (pForQuoteRsp)
	{
		tdLogger->info("TradingDay {}",pForQuoteRsp->TradingDay);
		tdLogger->info("InstrumentID {}",pForQuoteRsp->InstrumentID);
		tdLogger->info("ForQuoteSysID {}",pForQuoteRsp->ForQuoteSysID);
		tdLogger->info("ForQuoteTime {}",pForQuoteRsp->ForQuoteTime);
		tdLogger->info("ActionDay {}",pForQuoteRsp->ActionDay);
		tdLogger->info("ExchangeID {}",pForQuoteRsp->ExchangeID);
	}
	tdLogger->info("</OnRtnForQuoteRsp>");
};
void TdSpi::OnRtnCFMMCTradingAccountToken(CThostFtdcCFMMCTradingAccountTokenField *pCFMMCTradingAccountToken) 
{
	tdLogger->info("<OnRtnCFMMCTradingAccountToken>");
	if (pCFMMCTradingAccountToken)
	{
		tdLogger->info("BrokerID {}",pCFMMCTradingAccountToken->BrokerID);
		tdLogger->info("ParticipantID {}",pCFMMCTradingAccountToken->ParticipantID);
		tdLogger->info("AccountID {}",pCFMMCTradingAccountToken->AccountID);
		tdLogger->info("Token {}",pCFMMCTradingAccountToken->Token);
		tdLogger->info("KeyID {}",pCFMMCTradingAccountToken->KeyID);
	}
	tdLogger->info("</OnRtnCFMMCTradingAccountToken>");
};
void TdSpi::OnErrRtnBatchOrderAction(CThostFtdcBatchOrderActionField *pBatchOrderAction, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnBatchOrderAction>");
	if (pBatchOrderAction)
	{
		tdLogger->info("BrokerID {}",pBatchOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pBatchOrderAction->InvestorID);
		tdLogger->info("ExchangeID {}",pBatchOrderAction->ExchangeID);
		tdLogger->info("ActionDate {}",pBatchOrderAction->ActionDate);
		tdLogger->info("ActionTime {}",pBatchOrderAction->ActionTime);
		tdLogger->info("TraderID {}",pBatchOrderAction->TraderID);
		tdLogger->info("ActionLocalID {}",pBatchOrderAction->ActionLocalID);
		tdLogger->info("ParticipantID {}",pBatchOrderAction->ParticipantID);
		tdLogger->info("ClientID {}",pBatchOrderAction->ClientID);
		tdLogger->info("BusinessUnit {}",pBatchOrderAction->BusinessUnit);
		tdLogger->info("UserID {}",pBatchOrderAction->UserID);
		tdLogger->info("StatusMsg {}",pBatchOrderAction->StatusMsg);
		tdLogger->info("InvestUnitID {}",pBatchOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pBatchOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pBatchOrderAction->MacAddress);
		tdLogger->info("OrderActionRef {}",pBatchOrderAction->OrderActionRef);
		tdLogger->info("RequestID {}",pBatchOrderAction->RequestID);
		tdLogger->info("FrontID {}",pBatchOrderAction->FrontID);
		tdLogger->info("SessionID {}",pBatchOrderAction->SessionID);
		tdLogger->info("InstallID {}",pBatchOrderAction->InstallID);
		tdLogger->info("OrderActionStatus {}",pBatchOrderAction->OrderActionStatus);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnBatchOrderAction>");
};
void TdSpi::OnRtnOptionSelfClose(CThostFtdcOptionSelfCloseField *pOptionSelfClose) 
{
	tdLogger->info("<OnRtnOptionSelfClose>");
	if (pOptionSelfClose)
	{
		tdLogger->info("BrokerID {}",pOptionSelfClose->BrokerID);
		tdLogger->info("InvestorID {}",pOptionSelfClose->InvestorID);
		tdLogger->info("InstrumentID {}",pOptionSelfClose->InstrumentID);
		tdLogger->info("OptionSelfCloseRef {}",pOptionSelfClose->OptionSelfCloseRef);
		tdLogger->info("UserID {}",pOptionSelfClose->UserID);
		tdLogger->info("BusinessUnit {}",pOptionSelfClose->BusinessUnit);
		tdLogger->info("OptionSelfCloseLocalID {}",pOptionSelfClose->OptionSelfCloseLocalID);
		tdLogger->info("ExchangeID {}",pOptionSelfClose->ExchangeID);
		tdLogger->info("ParticipantID {}",pOptionSelfClose->ParticipantID);
		tdLogger->info("ClientID {}",pOptionSelfClose->ClientID);
		tdLogger->info("ExchangeInstID {}",pOptionSelfClose->ExchangeInstID);
		tdLogger->info("TraderID {}",pOptionSelfClose->TraderID);
		tdLogger->info("TradingDay {}",pOptionSelfClose->TradingDay);
		tdLogger->info("OptionSelfCloseSysID {}",pOptionSelfClose->OptionSelfCloseSysID);
		tdLogger->info("InsertDate {}",pOptionSelfClose->InsertDate);
		tdLogger->info("InsertTime {}",pOptionSelfClose->InsertTime);
		tdLogger->info("CancelTime {}",pOptionSelfClose->CancelTime);
		tdLogger->info("ClearingPartID {}",pOptionSelfClose->ClearingPartID);
		tdLogger->info("UserProductInfo {}",pOptionSelfClose->UserProductInfo);
		tdLogger->info("StatusMsg {}",pOptionSelfClose->StatusMsg);
		tdLogger->info("ActiveUserID {}",pOptionSelfClose->ActiveUserID);
		tdLogger->info("BranchID {}",pOptionSelfClose->BranchID);
		tdLogger->info("InvestUnitID {}",pOptionSelfClose->InvestUnitID);
		tdLogger->info("AccountID {}",pOptionSelfClose->AccountID);
		tdLogger->info("CurrencyID {}",pOptionSelfClose->CurrencyID);
		tdLogger->info("IPAddress {}",pOptionSelfClose->IPAddress);
		tdLogger->info("MacAddress {}",pOptionSelfClose->MacAddress);
		tdLogger->info("Volume {}",pOptionSelfClose->Volume);
		tdLogger->info("RequestID {}",pOptionSelfClose->RequestID);
		tdLogger->info("InstallID {}",pOptionSelfClose->InstallID);
		tdLogger->info("NotifySequence {}",pOptionSelfClose->NotifySequence);
		tdLogger->info("SettlementID {}",pOptionSelfClose->SettlementID);
		tdLogger->info("SequenceNo {}",pOptionSelfClose->SequenceNo);
		tdLogger->info("FrontID {}",pOptionSelfClose->FrontID);
		tdLogger->info("SessionID {}",pOptionSelfClose->SessionID);
		tdLogger->info("BrokerOptionSelfCloseSeq {}",pOptionSelfClose->BrokerOptionSelfCloseSeq);
		tdLogger->info("HedgeFlag {}",pOptionSelfClose->HedgeFlag);
		tdLogger->info("OptSelfCloseFlag {}",pOptionSelfClose->OptSelfCloseFlag);
		tdLogger->info("OrderSubmitStatus {}",pOptionSelfClose->OrderSubmitStatus);
		tdLogger->info("ExecResult {}",pOptionSelfClose->ExecResult);
	}
	tdLogger->info("</OnRtnOptionSelfClose>");
};
void TdSpi::OnErrRtnOptionSelfCloseInsert(CThostFtdcInputOptionSelfCloseField *pInputOptionSelfClose, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnOptionSelfCloseInsert>");
	if (pInputOptionSelfClose)
	{
		tdLogger->info("BrokerID {}",pInputOptionSelfClose->BrokerID);
		tdLogger->info("InvestorID {}",pInputOptionSelfClose->InvestorID);
		tdLogger->info("InstrumentID {}",pInputOptionSelfClose->InstrumentID);
		tdLogger->info("OptionSelfCloseRef {}",pInputOptionSelfClose->OptionSelfCloseRef);
		tdLogger->info("UserID {}",pInputOptionSelfClose->UserID);
		tdLogger->info("BusinessUnit {}",pInputOptionSelfClose->BusinessUnit);
		tdLogger->info("ExchangeID {}",pInputOptionSelfClose->ExchangeID);
		tdLogger->info("InvestUnitID {}",pInputOptionSelfClose->InvestUnitID);
		tdLogger->info("AccountID {}",pInputOptionSelfClose->AccountID);
		tdLogger->info("CurrencyID {}",pInputOptionSelfClose->CurrencyID);
		tdLogger->info("ClientID {}",pInputOptionSelfClose->ClientID);
		tdLogger->info("IPAddress {}",pInputOptionSelfClose->IPAddress);
		tdLogger->info("MacAddress {}",pInputOptionSelfClose->MacAddress);
		tdLogger->info("Volume {}",pInputOptionSelfClose->Volume);
		tdLogger->info("RequestID {}",pInputOptionSelfClose->RequestID);
		tdLogger->info("HedgeFlag {}",pInputOptionSelfClose->HedgeFlag);
		tdLogger->info("OptSelfCloseFlag {}",pInputOptionSelfClose->OptSelfCloseFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnOptionSelfCloseInsert>");
};
void TdSpi::OnErrRtnOptionSelfCloseAction(CThostFtdcOptionSelfCloseActionField *pOptionSelfCloseAction, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnOptionSelfCloseAction>");
	if (pOptionSelfCloseAction)
	{
		tdLogger->info("BrokerID {}",pOptionSelfCloseAction->BrokerID);
		tdLogger->info("InvestorID {}",pOptionSelfCloseAction->InvestorID);
		tdLogger->info("OptionSelfCloseRef {}",pOptionSelfCloseAction->OptionSelfCloseRef);
		tdLogger->info("ExchangeID {}",pOptionSelfCloseAction->ExchangeID);
		tdLogger->info("OptionSelfCloseSysID {}",pOptionSelfCloseAction->OptionSelfCloseSysID);
		tdLogger->info("ActionDate {}",pOptionSelfCloseAction->ActionDate);
		tdLogger->info("ActionTime {}",pOptionSelfCloseAction->ActionTime);
		tdLogger->info("TraderID {}",pOptionSelfCloseAction->TraderID);
		tdLogger->info("OptionSelfCloseLocalID {}",pOptionSelfCloseAction->OptionSelfCloseLocalID);
		tdLogger->info("ActionLocalID {}",pOptionSelfCloseAction->ActionLocalID);
		tdLogger->info("ParticipantID {}",pOptionSelfCloseAction->ParticipantID);
		tdLogger->info("ClientID {}",pOptionSelfCloseAction->ClientID);
		tdLogger->info("BusinessUnit {}",pOptionSelfCloseAction->BusinessUnit);
		tdLogger->info("UserID {}",pOptionSelfCloseAction->UserID);
		tdLogger->info("StatusMsg {}",pOptionSelfCloseAction->StatusMsg);
		tdLogger->info("InstrumentID {}",pOptionSelfCloseAction->InstrumentID);
		tdLogger->info("BranchID {}",pOptionSelfCloseAction->BranchID);
		tdLogger->info("InvestUnitID {}",pOptionSelfCloseAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pOptionSelfCloseAction->IPAddress);
		tdLogger->info("MacAddress {}",pOptionSelfCloseAction->MacAddress);
		tdLogger->info("OptionSelfCloseActionRef {}",pOptionSelfCloseAction->OptionSelfCloseActionRef);
		tdLogger->info("RequestID {}",pOptionSelfCloseAction->RequestID);
		tdLogger->info("FrontID {}",pOptionSelfCloseAction->FrontID);
		tdLogger->info("SessionID {}",pOptionSelfCloseAction->SessionID);
		tdLogger->info("InstallID {}",pOptionSelfCloseAction->InstallID);
		tdLogger->info("ActionFlag {}",pOptionSelfCloseAction->ActionFlag);
		tdLogger->info("OrderActionStatus {}",pOptionSelfCloseAction->OrderActionStatus);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnOptionSelfCloseAction>");
};
void TdSpi::OnRtnCombAction(CThostFtdcCombActionField *pCombAction) 
{
	tdLogger->info("<OnRtnCombAction>");
	if (pCombAction)
	{
		tdLogger->info("BrokerID {}",pCombAction->BrokerID);
		tdLogger->info("InvestorID {}",pCombAction->InvestorID);
		tdLogger->info("InstrumentID {}",pCombAction->InstrumentID);
		tdLogger->info("CombActionRef {}",pCombAction->CombActionRef);
		tdLogger->info("UserID {}",pCombAction->UserID);
		tdLogger->info("ActionLocalID {}",pCombAction->ActionLocalID);
		tdLogger->info("ExchangeID {}",pCombAction->ExchangeID);
		tdLogger->info("ParticipantID {}",pCombAction->ParticipantID);
		tdLogger->info("ClientID {}",pCombAction->ClientID);
		tdLogger->info("ExchangeInstID {}",pCombAction->ExchangeInstID);
		tdLogger->info("TraderID {}",pCombAction->TraderID);
		tdLogger->info("TradingDay {}",pCombAction->TradingDay);
		tdLogger->info("UserProductInfo {}",pCombAction->UserProductInfo);
		tdLogger->info("StatusMsg {}",pCombAction->StatusMsg);
		tdLogger->info("IPAddress {}",pCombAction->IPAddress);
		tdLogger->info("MacAddress {}",pCombAction->MacAddress);
		tdLogger->info("ComTradeID {}",pCombAction->ComTradeID);
		tdLogger->info("BranchID {}",pCombAction->BranchID);
		tdLogger->info("InvestUnitID {}",pCombAction->InvestUnitID);
		tdLogger->info("Volume {}",pCombAction->Volume);
		tdLogger->info("InstallID {}",pCombAction->InstallID);
		tdLogger->info("NotifySequence {}",pCombAction->NotifySequence);
		tdLogger->info("SettlementID {}",pCombAction->SettlementID);
		tdLogger->info("SequenceNo {}",pCombAction->SequenceNo);
		tdLogger->info("FrontID {}",pCombAction->FrontID);
		tdLogger->info("SessionID {}",pCombAction->SessionID);
		tdLogger->info("Direction {}",pCombAction->Direction);
		tdLogger->info("CombDirection {}",pCombAction->CombDirection);
		tdLogger->info("HedgeFlag {}",pCombAction->HedgeFlag);
		tdLogger->info("ActionStatus {}",pCombAction->ActionStatus);
	}
	tdLogger->info("</OnRtnCombAction>");
};
void TdSpi::OnErrRtnCombActionInsert(CThostFtdcInputCombActionField *pInputCombAction, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnCombActionInsert>");
	if (pInputCombAction)
	{
		tdLogger->info("BrokerID {}",pInputCombAction->BrokerID);
		tdLogger->info("InvestorID {}",pInputCombAction->InvestorID);
		tdLogger->info("InstrumentID {}",pInputCombAction->InstrumentID);
		tdLogger->info("CombActionRef {}",pInputCombAction->CombActionRef);
		tdLogger->info("UserID {}",pInputCombAction->UserID);
		tdLogger->info("ExchangeID {}",pInputCombAction->ExchangeID);
		tdLogger->info("IPAddress {}",pInputCombAction->IPAddress);
		tdLogger->info("MacAddress {}",pInputCombAction->MacAddress);
		tdLogger->info("InvestUnitID {}",pInputCombAction->InvestUnitID);
		tdLogger->info("Volume {}",pInputCombAction->Volume);
		tdLogger->info("Direction {}",pInputCombAction->Direction);
		tdLogger->info("CombDirection {}",pInputCombAction->CombDirection);
		tdLogger->info("HedgeFlag {}",pInputCombAction->HedgeFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnCombActionInsert>");
};
void TdSpi::OnRspQryContractBank(CThostFtdcContractBankField *pContractBank, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryContractBank>");
	if (pContractBank)
	{
		tdLogger->info("BrokerID {}",pContractBank->BrokerID);
		tdLogger->info("BankID {}",pContractBank->BankID);
		tdLogger->info("BankBrchID {}",pContractBank->BankBrchID);
		tdLogger->info("BankName {}",pContractBank->BankName);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryContractBank>");
};

void TdSpi::OnRspQryParkedOrder(CThostFtdcParkedOrderField *pParkedOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryParkedOrder>");
	if (pParkedOrder)
	{
		tdLogger->info("BrokerID {}",pParkedOrder->BrokerID);
		tdLogger->info("InvestorID {}",pParkedOrder->InvestorID);
		tdLogger->info("InstrumentID {}",pParkedOrder->InstrumentID);
		tdLogger->info("OrderRef {}",pParkedOrder->OrderRef);
		tdLogger->info("UserID {}",pParkedOrder->UserID);
		tdLogger->info("CombOffsetFlag {}",pParkedOrder->CombOffsetFlag);
		tdLogger->info("CombHedgeFlag {}",pParkedOrder->CombHedgeFlag);
		tdLogger->info("GTDDate {}",pParkedOrder->GTDDate);
		tdLogger->info("BusinessUnit {}",pParkedOrder->BusinessUnit);
		tdLogger->info("ExchangeID {}",pParkedOrder->ExchangeID);
		tdLogger->info("ParkedOrderID {}",pParkedOrder->ParkedOrderID);
		tdLogger->info("ErrorMsg {}",pParkedOrder->ErrorMsg);
		tdLogger->info("AccountID {}",pParkedOrder->AccountID);
		tdLogger->info("CurrencyID {}",pParkedOrder->CurrencyID);
		tdLogger->info("ClientID {}",pParkedOrder->ClientID);
		tdLogger->info("InvestUnitID {}",pParkedOrder->InvestUnitID);
		tdLogger->info("IPAddress {}",pParkedOrder->IPAddress);
		tdLogger->info("MacAddress {}",pParkedOrder->MacAddress);
		tdLogger->info("VolumeTotalOriginal {}",pParkedOrder->VolumeTotalOriginal);
		tdLogger->info("MinVolume {}",pParkedOrder->MinVolume);
		tdLogger->info("IsAutoSuspend {}",pParkedOrder->IsAutoSuspend);
		tdLogger->info("RequestID {}",pParkedOrder->RequestID);
		tdLogger->info("UserForceClose {}",pParkedOrder->UserForceClose);
		tdLogger->info("ErrorID {}",pParkedOrder->ErrorID);
		tdLogger->info("IsSwapOrder {}",pParkedOrder->IsSwapOrder);
		tdLogger->info("OrderPriceType {}",pParkedOrder->OrderPriceType);
		tdLogger->info("Direction {}",pParkedOrder->Direction);
		tdLogger->info("TimeCondition {}",pParkedOrder->TimeCondition);
		tdLogger->info("VolumeCondition {}",pParkedOrder->VolumeCondition);
		tdLogger->info("ContingentCondition {}",pParkedOrder->ContingentCondition);
		tdLogger->info("ForceCloseReason {}",pParkedOrder->ForceCloseReason);
		tdLogger->info("UserType {}",pParkedOrder->UserType);
		tdLogger->info("Status {}",pParkedOrder->Status);
		tdLogger->info("LimitPrice {}",pParkedOrder->LimitPrice);
		tdLogger->info("StopPrice {}",pParkedOrder->StopPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryParkedOrder>");
};

void TdSpi::OnRspQryParkedOrderAction(CThostFtdcParkedOrderActionField *pParkedOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryParkedOrderAction>");
	if (pParkedOrderAction)
	{
		tdLogger->info("BrokerID {}",pParkedOrderAction->BrokerID);
		tdLogger->info("InvestorID {}",pParkedOrderAction->InvestorID);
		tdLogger->info("OrderRef {}",pParkedOrderAction->OrderRef);
		tdLogger->info("ExchangeID {}",pParkedOrderAction->ExchangeID);
		tdLogger->info("OrderSysID {}",pParkedOrderAction->OrderSysID);
		tdLogger->info("UserID {}",pParkedOrderAction->UserID);
		tdLogger->info("InstrumentID {}",pParkedOrderAction->InstrumentID);
		tdLogger->info("ParkedOrderActionID {}",pParkedOrderAction->ParkedOrderActionID);
		tdLogger->info("ErrorMsg {}",pParkedOrderAction->ErrorMsg);
		tdLogger->info("InvestUnitID {}",pParkedOrderAction->InvestUnitID);
		tdLogger->info("IPAddress {}",pParkedOrderAction->IPAddress);
		tdLogger->info("MacAddress {}",pParkedOrderAction->MacAddress);
		tdLogger->info("OrderActionRef {}",pParkedOrderAction->OrderActionRef);
		tdLogger->info("RequestID {}",pParkedOrderAction->RequestID);
		tdLogger->info("FrontID {}",pParkedOrderAction->FrontID);
		tdLogger->info("SessionID {}",pParkedOrderAction->SessionID);
		tdLogger->info("VolumeChange {}",pParkedOrderAction->VolumeChange);
		tdLogger->info("ErrorID {}",pParkedOrderAction->ErrorID);
		tdLogger->info("ActionFlag {}",pParkedOrderAction->ActionFlag);
		tdLogger->info("UserType {}",pParkedOrderAction->UserType);
		tdLogger->info("Status {}",pParkedOrderAction->Status);
		tdLogger->info("LimitPrice {}",pParkedOrderAction->LimitPrice);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryParkedOrderAction>");
};

void TdSpi::OnRspQryTradingNotice(CThostFtdcTradingNoticeField *pTradingNotice, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryTradingNotice>");
	if (pTradingNotice)
	{
		tdLogger->info("BrokerID {}",pTradingNotice->BrokerID);
		tdLogger->info("InvestorID {}",pTradingNotice->InvestorID);
		tdLogger->info("UserID {}",pTradingNotice->UserID);
		tdLogger->info("SendTime {}",pTradingNotice->SendTime);
		tdLogger->info("FieldContent {}",pTradingNotice->FieldContent);
		tdLogger->info("InvestUnitID {}",pTradingNotice->InvestUnitID);
		tdLogger->info("SequenceNo {}",pTradingNotice->SequenceNo);
		tdLogger->info("InvestorRange {}",pTradingNotice->InvestorRange);
		tdLogger->info("SequenceSeries {}",pTradingNotice->SequenceSeries);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryTradingNotice>");
};

void TdSpi::OnRspQryBrokerTradingParams(CThostFtdcBrokerTradingParamsField *pBrokerTradingParams, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryBrokerTradingParams>");
	if (pBrokerTradingParams)
	{
		tdLogger->info("BrokerID {}",pBrokerTradingParams->BrokerID);
		tdLogger->info("InvestorID {}",pBrokerTradingParams->InvestorID);
		tdLogger->info("CurrencyID {}",pBrokerTradingParams->CurrencyID);
		tdLogger->info("AccountID {}",pBrokerTradingParams->AccountID);
		tdLogger->info("MarginPriceType {}",pBrokerTradingParams->MarginPriceType);
		tdLogger->info("Algorithm {}",pBrokerTradingParams->Algorithm);
		tdLogger->info("AvailIncludeCloseProfit {}",pBrokerTradingParams->AvailIncludeCloseProfit);
		tdLogger->info("OptionRoyaltyPriceType {}",pBrokerTradingParams->OptionRoyaltyPriceType);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryBrokerTradingParams>");
};

void TdSpi::OnRspQryBrokerTradingAlgos(CThostFtdcBrokerTradingAlgosField *pBrokerTradingAlgos, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQryBrokerTradingAlgos>");
	if (pBrokerTradingAlgos)
	{
		tdLogger->info("BrokerID {}",pBrokerTradingAlgos->BrokerID);
		tdLogger->info("ExchangeID {}",pBrokerTradingAlgos->ExchangeID);
		tdLogger->info("InstrumentID {}",pBrokerTradingAlgos->InstrumentID);
		tdLogger->info("HandlePositionAlgoID {}",pBrokerTradingAlgos->HandlePositionAlgoID);
		tdLogger->info("FindMarginRateAlgoID {}",pBrokerTradingAlgos->FindMarginRateAlgoID);
		tdLogger->info("HandleTradingAccountAlgoID {}",pBrokerTradingAlgos->HandleTradingAccountAlgoID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQryBrokerTradingAlgos>");
};

void TdSpi::OnRspQueryCFMMCTradingAccountToken(CThostFtdcQueryCFMMCTradingAccountTokenField *pQueryCFMMCTradingAccountToken, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQueryCFMMCTradingAccountToken>");
	if (pQueryCFMMCTradingAccountToken)
	{
		tdLogger->info("BrokerID {}",pQueryCFMMCTradingAccountToken->BrokerID);
		tdLogger->info("InvestorID {}",pQueryCFMMCTradingAccountToken->InvestorID);
		tdLogger->info("InvestUnitID {}",pQueryCFMMCTradingAccountToken->InvestUnitID);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQueryCFMMCTradingAccountToken>");
};

void TdSpi::OnRtnFromBankToFutureByBank(CThostFtdcRspTransferField *pRspTransfer) 
{
	tdLogger->info("<OnRtnFromBankToFutureByBank>");
	if (pRspTransfer)
	{
		tdLogger->info("TradeCode {}",pRspTransfer->TradeCode);
		tdLogger->info("BankID {}",pRspTransfer->BankID);
		tdLogger->info("BankBranchID {}",pRspTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pRspTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pRspTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pRspTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pRspTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pRspTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pRspTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pRspTransfer->AccountID);
		tdLogger->info("Password {}",pRspTransfer->Password);
		tdLogger->info("UserID {}",pRspTransfer->UserID);
		tdLogger->info("CurrencyID {}",pRspTransfer->CurrencyID);
		tdLogger->info("Message {}",pRspTransfer->Message);
		tdLogger->info("Digest {}",pRspTransfer->Digest);
		tdLogger->info("DeviceID {}",pRspTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspTransfer->OperNo);
		tdLogger->info("ErrorMsg {}",pRspTransfer->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pRspTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pRspTransfer->SessionID);
		tdLogger->info("InstallID {}",pRspTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pRspTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pRspTransfer->RequestID);
		tdLogger->info("TID {}",pRspTransfer->TID);
		tdLogger->info("ErrorID {}",pRspTransfer->ErrorID);
		tdLogger->info("LastFragment {}",pRspTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pRspTransfer->IdCardType);
		tdLogger->info("CustType {}",pRspTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pRspTransfer->BrokerFee);
	}
	tdLogger->info("</OnRtnFromBankToFutureByBank>");
};
void TdSpi::OnRtnFromFutureToBankByBank(CThostFtdcRspTransferField *pRspTransfer) 
{
	tdLogger->info("<OnRtnFromFutureToBankByBank>");
	if (pRspTransfer)
	{
		tdLogger->info("TradeCode {}",pRspTransfer->TradeCode);
		tdLogger->info("BankID {}",pRspTransfer->BankID);
		tdLogger->info("BankBranchID {}",pRspTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pRspTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pRspTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pRspTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pRspTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pRspTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pRspTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pRspTransfer->AccountID);
		tdLogger->info("Password {}",pRspTransfer->Password);
		tdLogger->info("UserID {}",pRspTransfer->UserID);
		tdLogger->info("CurrencyID {}",pRspTransfer->CurrencyID);
		tdLogger->info("Message {}",pRspTransfer->Message);
		tdLogger->info("Digest {}",pRspTransfer->Digest);
		tdLogger->info("DeviceID {}",pRspTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspTransfer->OperNo);
		tdLogger->info("ErrorMsg {}",pRspTransfer->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pRspTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pRspTransfer->SessionID);
		tdLogger->info("InstallID {}",pRspTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pRspTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pRspTransfer->RequestID);
		tdLogger->info("TID {}",pRspTransfer->TID);
		tdLogger->info("ErrorID {}",pRspTransfer->ErrorID);
		tdLogger->info("LastFragment {}",pRspTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pRspTransfer->IdCardType);
		tdLogger->info("CustType {}",pRspTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pRspTransfer->BrokerFee);
	}
	tdLogger->info("</OnRtnFromFutureToBankByBank>");
};
void TdSpi::OnRtnRepealFromBankToFutureByBank(CThostFtdcRspRepealField *pRspRepeal) 
{
	tdLogger->info("<OnRtnRepealFromBankToFutureByBank>");
	if (pRspRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pRspRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pRspRepeal->TradeCode);
		tdLogger->info("BankID {}",pRspRepeal->BankID);
		tdLogger->info("BankBranchID {}",pRspRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pRspRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pRspRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pRspRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pRspRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pRspRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pRspRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pRspRepeal->AccountID);
		tdLogger->info("Password {}",pRspRepeal->Password);
		tdLogger->info("UserID {}",pRspRepeal->UserID);
		tdLogger->info("CurrencyID {}",pRspRepeal->CurrencyID);
		tdLogger->info("Message {}",pRspRepeal->Message);
		tdLogger->info("Digest {}",pRspRepeal->Digest);
		tdLogger->info("DeviceID {}",pRspRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspRepeal->OperNo);
		tdLogger->info("ErrorMsg {}",pRspRepeal->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pRspRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pRspRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pRspRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pRspRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pRspRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pRspRepeal->SessionID);
		tdLogger->info("InstallID {}",pRspRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pRspRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pRspRepeal->RequestID);
		tdLogger->info("TID {}",pRspRepeal->TID);
		tdLogger->info("ErrorID {}",pRspRepeal->ErrorID);
		tdLogger->info("BankRepealFlag {}",pRspRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pRspRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pRspRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pRspRepeal->IdCardType);
		tdLogger->info("CustType {}",pRspRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pRspRepeal->BrokerFee);
	}
	tdLogger->info("</OnRtnRepealFromBankToFutureByBank>");
};
void TdSpi::OnRtnRepealFromFutureToBankByBank(CThostFtdcRspRepealField *pRspRepeal) 
{
	tdLogger->info("<OnRtnRepealFromFutureToBankByBank>");
	if (pRspRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pRspRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pRspRepeal->TradeCode);
		tdLogger->info("BankID {}",pRspRepeal->BankID);
		tdLogger->info("BankBranchID {}",pRspRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pRspRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pRspRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pRspRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pRspRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pRspRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pRspRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pRspRepeal->AccountID);
		tdLogger->info("Password {}",pRspRepeal->Password);
		tdLogger->info("UserID {}",pRspRepeal->UserID);
		tdLogger->info("CurrencyID {}",pRspRepeal->CurrencyID);
		tdLogger->info("Message {}",pRspRepeal->Message);
		tdLogger->info("Digest {}",pRspRepeal->Digest);
		tdLogger->info("DeviceID {}",pRspRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspRepeal->OperNo);
		tdLogger->info("ErrorMsg {}",pRspRepeal->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pRspRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pRspRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pRspRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pRspRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pRspRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pRspRepeal->SessionID);
		tdLogger->info("InstallID {}",pRspRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pRspRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pRspRepeal->RequestID);
		tdLogger->info("TID {}",pRspRepeal->TID);
		tdLogger->info("ErrorID {}",pRspRepeal->ErrorID);
		tdLogger->info("BankRepealFlag {}",pRspRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pRspRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pRspRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pRspRepeal->IdCardType);
		tdLogger->info("CustType {}",pRspRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pRspRepeal->BrokerFee);
	}
	tdLogger->info("</OnRtnRepealFromFutureToBankByBank>");
};
void TdSpi::OnRtnFromBankToFutureByFuture(CThostFtdcRspTransferField *pRspTransfer) 
{
	tdLogger->info("<OnRtnFromBankToFutureByFuture>");
	if (pRspTransfer)
	{
		tdLogger->info("TradeCode {}",pRspTransfer->TradeCode);
		tdLogger->info("BankID {}",pRspTransfer->BankID);
		tdLogger->info("BankBranchID {}",pRspTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pRspTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pRspTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pRspTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pRspTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pRspTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pRspTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pRspTransfer->AccountID);
		tdLogger->info("Password {}",pRspTransfer->Password);
		tdLogger->info("UserID {}",pRspTransfer->UserID);
		tdLogger->info("CurrencyID {}",pRspTransfer->CurrencyID);
		tdLogger->info("Message {}",pRspTransfer->Message);
		tdLogger->info("Digest {}",pRspTransfer->Digest);
		tdLogger->info("DeviceID {}",pRspTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspTransfer->OperNo);
		tdLogger->info("ErrorMsg {}",pRspTransfer->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pRspTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pRspTransfer->SessionID);
		tdLogger->info("InstallID {}",pRspTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pRspTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pRspTransfer->RequestID);
		tdLogger->info("TID {}",pRspTransfer->TID);
		tdLogger->info("ErrorID {}",pRspTransfer->ErrorID);
		tdLogger->info("LastFragment {}",pRspTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pRspTransfer->IdCardType);
		tdLogger->info("CustType {}",pRspTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pRspTransfer->BrokerFee);
	}
	tdLogger->info("</OnRtnFromBankToFutureByFuture>");
};
void TdSpi::OnRtnFromFutureToBankByFuture(CThostFtdcRspTransferField *pRspTransfer) 
{
	tdLogger->info("<OnRtnFromFutureToBankByFuture>");
	if (pRspTransfer)
	{
		tdLogger->info("TradeCode {}",pRspTransfer->TradeCode);
		tdLogger->info("BankID {}",pRspTransfer->BankID);
		tdLogger->info("BankBranchID {}",pRspTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pRspTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pRspTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pRspTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pRspTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pRspTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pRspTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pRspTransfer->AccountID);
		tdLogger->info("Password {}",pRspTransfer->Password);
		tdLogger->info("UserID {}",pRspTransfer->UserID);
		tdLogger->info("CurrencyID {}",pRspTransfer->CurrencyID);
		tdLogger->info("Message {}",pRspTransfer->Message);
		tdLogger->info("Digest {}",pRspTransfer->Digest);
		tdLogger->info("DeviceID {}",pRspTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspTransfer->OperNo);
		tdLogger->info("ErrorMsg {}",pRspTransfer->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pRspTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pRspTransfer->SessionID);
		tdLogger->info("InstallID {}",pRspTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pRspTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pRspTransfer->RequestID);
		tdLogger->info("TID {}",pRspTransfer->TID);
		tdLogger->info("ErrorID {}",pRspTransfer->ErrorID);
		tdLogger->info("LastFragment {}",pRspTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pRspTransfer->IdCardType);
		tdLogger->info("CustType {}",pRspTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pRspTransfer->BrokerFee);
	}
	tdLogger->info("</OnRtnFromFutureToBankByFuture>");
};
void TdSpi::OnRtnRepealFromBankToFutureByFutureManual(CThostFtdcRspRepealField *pRspRepeal) 
{
	tdLogger->info("<OnRtnRepealFromBankToFutureByFutureManual>");
	if (pRspRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pRspRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pRspRepeal->TradeCode);
		tdLogger->info("BankID {}",pRspRepeal->BankID);
		tdLogger->info("BankBranchID {}",pRspRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pRspRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pRspRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pRspRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pRspRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pRspRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pRspRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pRspRepeal->AccountID);
		tdLogger->info("Password {}",pRspRepeal->Password);
		tdLogger->info("UserID {}",pRspRepeal->UserID);
		tdLogger->info("CurrencyID {}",pRspRepeal->CurrencyID);
		tdLogger->info("Message {}",pRspRepeal->Message);
		tdLogger->info("Digest {}",pRspRepeal->Digest);
		tdLogger->info("DeviceID {}",pRspRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspRepeal->OperNo);
		tdLogger->info("ErrorMsg {}",pRspRepeal->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pRspRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pRspRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pRspRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pRspRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pRspRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pRspRepeal->SessionID);
		tdLogger->info("InstallID {}",pRspRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pRspRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pRspRepeal->RequestID);
		tdLogger->info("TID {}",pRspRepeal->TID);
		tdLogger->info("ErrorID {}",pRspRepeal->ErrorID);
		tdLogger->info("BankRepealFlag {}",pRspRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pRspRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pRspRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pRspRepeal->IdCardType);
		tdLogger->info("CustType {}",pRspRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pRspRepeal->BrokerFee);
	}
	tdLogger->info("</OnRtnRepealFromBankToFutureByFutureManual>");
};
void TdSpi::OnRtnRepealFromFutureToBankByFutureManual(CThostFtdcRspRepealField *pRspRepeal) 
{
	tdLogger->info("<OnRtnRepealFromFutureToBankByFutureManual>");
	if (pRspRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pRspRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pRspRepeal->TradeCode);
		tdLogger->info("BankID {}",pRspRepeal->BankID);
		tdLogger->info("BankBranchID {}",pRspRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pRspRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pRspRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pRspRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pRspRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pRspRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pRspRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pRspRepeal->AccountID);
		tdLogger->info("Password {}",pRspRepeal->Password);
		tdLogger->info("UserID {}",pRspRepeal->UserID);
		tdLogger->info("CurrencyID {}",pRspRepeal->CurrencyID);
		tdLogger->info("Message {}",pRspRepeal->Message);
		tdLogger->info("Digest {}",pRspRepeal->Digest);
		tdLogger->info("DeviceID {}",pRspRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspRepeal->OperNo);
		tdLogger->info("ErrorMsg {}",pRspRepeal->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pRspRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pRspRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pRspRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pRspRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pRspRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pRspRepeal->SessionID);
		tdLogger->info("InstallID {}",pRspRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pRspRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pRspRepeal->RequestID);
		tdLogger->info("TID {}",pRspRepeal->TID);
		tdLogger->info("ErrorID {}",pRspRepeal->ErrorID);
		tdLogger->info("BankRepealFlag {}",pRspRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pRspRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pRspRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pRspRepeal->IdCardType);
		tdLogger->info("CustType {}",pRspRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pRspRepeal->BrokerFee);
	}
	tdLogger->info("</OnRtnRepealFromFutureToBankByFutureManual>");
};
void TdSpi::OnRtnQueryBankBalanceByFuture(CThostFtdcNotifyQueryAccountField *pNotifyQueryAccount) 
{
	tdLogger->info("<OnRtnQueryBankBalanceByFuture>");
	if (pNotifyQueryAccount)
	{
		tdLogger->info("TradeCode {}",pNotifyQueryAccount->TradeCode);
		tdLogger->info("BankID {}",pNotifyQueryAccount->BankID);
		tdLogger->info("BankBranchID {}",pNotifyQueryAccount->BankBranchID);
		tdLogger->info("BrokerID {}",pNotifyQueryAccount->BrokerID);
		tdLogger->info("BrokerBranchID {}",pNotifyQueryAccount->BrokerBranchID);
		tdLogger->info("TradeDate {}",pNotifyQueryAccount->TradeDate);
		tdLogger->info("TradeTime {}",pNotifyQueryAccount->TradeTime);
		tdLogger->info("BankSerial {}",pNotifyQueryAccount->BankSerial);
		tdLogger->info("TradingDay {}",pNotifyQueryAccount->TradingDay);
		tdLogger->info("CustomerName {}",pNotifyQueryAccount->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pNotifyQueryAccount->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pNotifyQueryAccount->BankAccount);
		tdLogger->info("BankPassWord {}",pNotifyQueryAccount->BankPassWord);
		tdLogger->info("AccountID {}",pNotifyQueryAccount->AccountID);
		tdLogger->info("Password {}",pNotifyQueryAccount->Password);
		tdLogger->info("UserID {}",pNotifyQueryAccount->UserID);
		tdLogger->info("CurrencyID {}",pNotifyQueryAccount->CurrencyID);
		tdLogger->info("Digest {}",pNotifyQueryAccount->Digest);
		tdLogger->info("DeviceID {}",pNotifyQueryAccount->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pNotifyQueryAccount->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pNotifyQueryAccount->BankSecuAcc);
		tdLogger->info("OperNo {}",pNotifyQueryAccount->OperNo);
		tdLogger->info("ErrorMsg {}",pNotifyQueryAccount->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pNotifyQueryAccount->LongCustomerName);
		tdLogger->info("PlateSerial {}",pNotifyQueryAccount->PlateSerial);
		tdLogger->info("SessionID {}",pNotifyQueryAccount->SessionID);
		tdLogger->info("FutureSerial {}",pNotifyQueryAccount->FutureSerial);
		tdLogger->info("InstallID {}",pNotifyQueryAccount->InstallID);
		tdLogger->info("RequestID {}",pNotifyQueryAccount->RequestID);
		tdLogger->info("TID {}",pNotifyQueryAccount->TID);
		tdLogger->info("ErrorID {}",pNotifyQueryAccount->ErrorID);
		tdLogger->info("LastFragment {}",pNotifyQueryAccount->LastFragment);
		tdLogger->info("IdCardType {}",pNotifyQueryAccount->IdCardType);
		tdLogger->info("CustType {}",pNotifyQueryAccount->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pNotifyQueryAccount->VerifyCertNoFlag);
		tdLogger->info("BankAccType {}",pNotifyQueryAccount->BankAccType);
		tdLogger->info("BankSecuAccType {}",pNotifyQueryAccount->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pNotifyQueryAccount->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pNotifyQueryAccount->SecuPwdFlag);
		tdLogger->info("BankUseAmount {}",pNotifyQueryAccount->BankUseAmount);
		tdLogger->info("BankFetchAmount {}",pNotifyQueryAccount->BankFetchAmount);
	}
	tdLogger->info("</OnRtnQueryBankBalanceByFuture>");
};
void TdSpi::OnErrRtnBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnBankToFutureByFuture>");
	if (pReqTransfer)
	{
		tdLogger->info("TradeCode {}",pReqTransfer->TradeCode);
		tdLogger->info("BankID {}",pReqTransfer->BankID);
		tdLogger->info("BankBranchID {}",pReqTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pReqTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pReqTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pReqTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pReqTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pReqTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pReqTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pReqTransfer->AccountID);
		tdLogger->info("Password {}",pReqTransfer->Password);
		tdLogger->info("UserID {}",pReqTransfer->UserID);
		tdLogger->info("CurrencyID {}",pReqTransfer->CurrencyID);
		tdLogger->info("Message {}",pReqTransfer->Message);
		tdLogger->info("Digest {}",pReqTransfer->Digest);
		tdLogger->info("DeviceID {}",pReqTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqTransfer->OperNo);
		tdLogger->info("LongCustomerName {}",pReqTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pReqTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pReqTransfer->SessionID);
		tdLogger->info("InstallID {}",pReqTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pReqTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pReqTransfer->RequestID);
		tdLogger->info("TID {}",pReqTransfer->TID);
		tdLogger->info("LastFragment {}",pReqTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pReqTransfer->IdCardType);
		tdLogger->info("CustType {}",pReqTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pReqTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pReqTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pReqTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pReqTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pReqTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pReqTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pReqTransfer->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnBankToFutureByFuture>");
};
void TdSpi::OnErrRtnFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnFutureToBankByFuture>");
	if (pReqTransfer)
	{
		tdLogger->info("TradeCode {}",pReqTransfer->TradeCode);
		tdLogger->info("BankID {}",pReqTransfer->BankID);
		tdLogger->info("BankBranchID {}",pReqTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pReqTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pReqTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pReqTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pReqTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pReqTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pReqTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pReqTransfer->AccountID);
		tdLogger->info("Password {}",pReqTransfer->Password);
		tdLogger->info("UserID {}",pReqTransfer->UserID);
		tdLogger->info("CurrencyID {}",pReqTransfer->CurrencyID);
		tdLogger->info("Message {}",pReqTransfer->Message);
		tdLogger->info("Digest {}",pReqTransfer->Digest);
		tdLogger->info("DeviceID {}",pReqTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqTransfer->OperNo);
		tdLogger->info("LongCustomerName {}",pReqTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pReqTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pReqTransfer->SessionID);
		tdLogger->info("InstallID {}",pReqTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pReqTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pReqTransfer->RequestID);
		tdLogger->info("TID {}",pReqTransfer->TID);
		tdLogger->info("LastFragment {}",pReqTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pReqTransfer->IdCardType);
		tdLogger->info("CustType {}",pReqTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pReqTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pReqTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pReqTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pReqTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pReqTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pReqTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pReqTransfer->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnFutureToBankByFuture>");
};
void TdSpi::OnErrRtnRepealBankToFutureByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnRepealBankToFutureByFutureManual>");
	if (pReqRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pReqRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pReqRepeal->TradeCode);
		tdLogger->info("BankID {}",pReqRepeal->BankID);
		tdLogger->info("BankBranchID {}",pReqRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pReqRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pReqRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pReqRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pReqRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pReqRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pReqRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pReqRepeal->AccountID);
		tdLogger->info("Password {}",pReqRepeal->Password);
		tdLogger->info("UserID {}",pReqRepeal->UserID);
		tdLogger->info("CurrencyID {}",pReqRepeal->CurrencyID);
		tdLogger->info("Message {}",pReqRepeal->Message);
		tdLogger->info("Digest {}",pReqRepeal->Digest);
		tdLogger->info("DeviceID {}",pReqRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqRepeal->OperNo);
		tdLogger->info("LongCustomerName {}",pReqRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pReqRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pReqRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pReqRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pReqRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pReqRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pReqRepeal->SessionID);
		tdLogger->info("InstallID {}",pReqRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pReqRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pReqRepeal->RequestID);
		tdLogger->info("TID {}",pReqRepeal->TID);
		tdLogger->info("BankRepealFlag {}",pReqRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pReqRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pReqRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pReqRepeal->IdCardType);
		tdLogger->info("CustType {}",pReqRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pReqRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pReqRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pReqRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pReqRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pReqRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pReqRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pReqRepeal->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnRepealBankToFutureByFutureManual>");
};
void TdSpi::OnErrRtnRepealFutureToBankByFutureManual(CThostFtdcReqRepealField *pReqRepeal, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnRepealFutureToBankByFutureManual>");
	if (pReqRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pReqRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pReqRepeal->TradeCode);
		tdLogger->info("BankID {}",pReqRepeal->BankID);
		tdLogger->info("BankBranchID {}",pReqRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pReqRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pReqRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pReqRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pReqRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pReqRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pReqRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pReqRepeal->AccountID);
		tdLogger->info("Password {}",pReqRepeal->Password);
		tdLogger->info("UserID {}",pReqRepeal->UserID);
		tdLogger->info("CurrencyID {}",pReqRepeal->CurrencyID);
		tdLogger->info("Message {}",pReqRepeal->Message);
		tdLogger->info("Digest {}",pReqRepeal->Digest);
		tdLogger->info("DeviceID {}",pReqRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqRepeal->OperNo);
		tdLogger->info("LongCustomerName {}",pReqRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pReqRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pReqRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pReqRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pReqRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pReqRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pReqRepeal->SessionID);
		tdLogger->info("InstallID {}",pReqRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pReqRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pReqRepeal->RequestID);
		tdLogger->info("TID {}",pReqRepeal->TID);
		tdLogger->info("BankRepealFlag {}",pReqRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pReqRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pReqRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pReqRepeal->IdCardType);
		tdLogger->info("CustType {}",pReqRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pReqRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pReqRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pReqRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pReqRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pReqRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pReqRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pReqRepeal->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnRepealFutureToBankByFutureManual>");
};
void TdSpi::OnErrRtnQueryBankBalanceByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo) 
{
	tdLogger->info("<OnErrRtnQueryBankBalanceByFuture>");
	if (pReqQueryAccount)
	{
		tdLogger->info("TradeCode {}",pReqQueryAccount->TradeCode);
		tdLogger->info("BankID {}",pReqQueryAccount->BankID);
		tdLogger->info("BankBranchID {}",pReqQueryAccount->BankBranchID);
		tdLogger->info("BrokerID {}",pReqQueryAccount->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqQueryAccount->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqQueryAccount->TradeDate);
		tdLogger->info("TradeTime {}",pReqQueryAccount->TradeTime);
		tdLogger->info("BankSerial {}",pReqQueryAccount->BankSerial);
		tdLogger->info("TradingDay {}",pReqQueryAccount->TradingDay);
		tdLogger->info("CustomerName {}",pReqQueryAccount->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqQueryAccount->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqQueryAccount->BankAccount);
		tdLogger->info("BankPassWord {}",pReqQueryAccount->BankPassWord);
		tdLogger->info("AccountID {}",pReqQueryAccount->AccountID);
		tdLogger->info("Password {}",pReqQueryAccount->Password);
		tdLogger->info("UserID {}",pReqQueryAccount->UserID);
		tdLogger->info("CurrencyID {}",pReqQueryAccount->CurrencyID);
		tdLogger->info("Digest {}",pReqQueryAccount->Digest);
		tdLogger->info("DeviceID {}",pReqQueryAccount->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqQueryAccount->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqQueryAccount->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqQueryAccount->OperNo);
		tdLogger->info("LongCustomerName {}",pReqQueryAccount->LongCustomerName);
		tdLogger->info("PlateSerial {}",pReqQueryAccount->PlateSerial);
		tdLogger->info("SessionID {}",pReqQueryAccount->SessionID);
		tdLogger->info("FutureSerial {}",pReqQueryAccount->FutureSerial);
		tdLogger->info("InstallID {}",pReqQueryAccount->InstallID);
		tdLogger->info("RequestID {}",pReqQueryAccount->RequestID);
		tdLogger->info("TID {}",pReqQueryAccount->TID);
		tdLogger->info("LastFragment {}",pReqQueryAccount->LastFragment);
		tdLogger->info("IdCardType {}",pReqQueryAccount->IdCardType);
		tdLogger->info("CustType {}",pReqQueryAccount->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqQueryAccount->VerifyCertNoFlag);
		tdLogger->info("BankAccType {}",pReqQueryAccount->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqQueryAccount->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqQueryAccount->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqQueryAccount->SecuPwdFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("</OnErrRtnQueryBankBalanceByFuture>");
};
void TdSpi::OnRtnRepealFromBankToFutureByFuture(CThostFtdcRspRepealField *pRspRepeal) 
{
	tdLogger->info("<OnRtnRepealFromBankToFutureByFuture>");
	if (pRspRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pRspRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pRspRepeal->TradeCode);
		tdLogger->info("BankID {}",pRspRepeal->BankID);
		tdLogger->info("BankBranchID {}",pRspRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pRspRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pRspRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pRspRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pRspRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pRspRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pRspRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pRspRepeal->AccountID);
		tdLogger->info("Password {}",pRspRepeal->Password);
		tdLogger->info("UserID {}",pRspRepeal->UserID);
		tdLogger->info("CurrencyID {}",pRspRepeal->CurrencyID);
		tdLogger->info("Message {}",pRspRepeal->Message);
		tdLogger->info("Digest {}",pRspRepeal->Digest);
		tdLogger->info("DeviceID {}",pRspRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspRepeal->OperNo);
		tdLogger->info("ErrorMsg {}",pRspRepeal->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pRspRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pRspRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pRspRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pRspRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pRspRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pRspRepeal->SessionID);
		tdLogger->info("InstallID {}",pRspRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pRspRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pRspRepeal->RequestID);
		tdLogger->info("TID {}",pRspRepeal->TID);
		tdLogger->info("ErrorID {}",pRspRepeal->ErrorID);
		tdLogger->info("BankRepealFlag {}",pRspRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pRspRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pRspRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pRspRepeal->IdCardType);
		tdLogger->info("CustType {}",pRspRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pRspRepeal->BrokerFee);
	}
	tdLogger->info("</OnRtnRepealFromBankToFutureByFuture>");
};
void TdSpi::OnRtnRepealFromFutureToBankByFuture(CThostFtdcRspRepealField *pRspRepeal) 
{
	tdLogger->info("<OnRtnRepealFromFutureToBankByFuture>");
	if (pRspRepeal)
	{
		tdLogger->info("BankRepealSerial {}",pRspRepeal->BankRepealSerial);
		tdLogger->info("TradeCode {}",pRspRepeal->TradeCode);
		tdLogger->info("BankID {}",pRspRepeal->BankID);
		tdLogger->info("BankBranchID {}",pRspRepeal->BankBranchID);
		tdLogger->info("BrokerID {}",pRspRepeal->BrokerID);
		tdLogger->info("BrokerBranchID {}",pRspRepeal->BrokerBranchID);
		tdLogger->info("TradeDate {}",pRspRepeal->TradeDate);
		tdLogger->info("TradeTime {}",pRspRepeal->TradeTime);
		tdLogger->info("BankSerial {}",pRspRepeal->BankSerial);
		tdLogger->info("TradingDay {}",pRspRepeal->TradingDay);
		tdLogger->info("CustomerName {}",pRspRepeal->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pRspRepeal->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pRspRepeal->BankAccount);
		tdLogger->info("BankPassWord {}",pRspRepeal->BankPassWord);
		tdLogger->info("AccountID {}",pRspRepeal->AccountID);
		tdLogger->info("Password {}",pRspRepeal->Password);
		tdLogger->info("UserID {}",pRspRepeal->UserID);
		tdLogger->info("CurrencyID {}",pRspRepeal->CurrencyID);
		tdLogger->info("Message {}",pRspRepeal->Message);
		tdLogger->info("Digest {}",pRspRepeal->Digest);
		tdLogger->info("DeviceID {}",pRspRepeal->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pRspRepeal->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pRspRepeal->BankSecuAcc);
		tdLogger->info("OperNo {}",pRspRepeal->OperNo);
		tdLogger->info("ErrorMsg {}",pRspRepeal->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pRspRepeal->LongCustomerName);
		tdLogger->info("RepealTimeInterval {}",pRspRepeal->RepealTimeInterval);
		tdLogger->info("RepealedTimes {}",pRspRepeal->RepealedTimes);
		tdLogger->info("PlateRepealSerial {}",pRspRepeal->PlateRepealSerial);
		tdLogger->info("FutureRepealSerial {}",pRspRepeal->FutureRepealSerial);
		tdLogger->info("PlateSerial {}",pRspRepeal->PlateSerial);
		tdLogger->info("SessionID {}",pRspRepeal->SessionID);
		tdLogger->info("InstallID {}",pRspRepeal->InstallID);
		tdLogger->info("FutureSerial {}",pRspRepeal->FutureSerial);
		tdLogger->info("RequestID {}",pRspRepeal->RequestID);
		tdLogger->info("TID {}",pRspRepeal->TID);
		tdLogger->info("ErrorID {}",pRspRepeal->ErrorID);
		tdLogger->info("BankRepealFlag {}",pRspRepeal->BankRepealFlag);
		tdLogger->info("BrokerRepealFlag {}",pRspRepeal->BrokerRepealFlag);
		tdLogger->info("LastFragment {}",pRspRepeal->LastFragment);
		tdLogger->info("IdCardType {}",pRspRepeal->IdCardType);
		tdLogger->info("CustType {}",pRspRepeal->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pRspRepeal->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pRspRepeal->FeePayFlag);
		tdLogger->info("BankAccType {}",pRspRepeal->BankAccType);
		tdLogger->info("BankSecuAccType {}",pRspRepeal->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pRspRepeal->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pRspRepeal->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pRspRepeal->TransferStatus);
		tdLogger->info("TradeAmount {}",pRspRepeal->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pRspRepeal->FutureFetchAmount);
		tdLogger->info("CustFee {}",pRspRepeal->CustFee);
		tdLogger->info("BrokerFee {}",pRspRepeal->BrokerFee);
	}
	tdLogger->info("</OnRtnRepealFromFutureToBankByFuture>");
};
void TdSpi::OnRspFromBankToFutureByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspFromBankToFutureByFuture>");
	if (pReqTransfer)
	{
		tdLogger->info("TradeCode {}",pReqTransfer->TradeCode);
		tdLogger->info("BankID {}",pReqTransfer->BankID);
		tdLogger->info("BankBranchID {}",pReqTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pReqTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pReqTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pReqTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pReqTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pReqTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pReqTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pReqTransfer->AccountID);
		tdLogger->info("Password {}",pReqTransfer->Password);
		tdLogger->info("UserID {}",pReqTransfer->UserID);
		tdLogger->info("CurrencyID {}",pReqTransfer->CurrencyID);
		tdLogger->info("Message {}",pReqTransfer->Message);
		tdLogger->info("Digest {}",pReqTransfer->Digest);
		tdLogger->info("DeviceID {}",pReqTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqTransfer->OperNo);
		tdLogger->info("LongCustomerName {}",pReqTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pReqTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pReqTransfer->SessionID);
		tdLogger->info("InstallID {}",pReqTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pReqTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pReqTransfer->RequestID);
		tdLogger->info("TID {}",pReqTransfer->TID);
		tdLogger->info("LastFragment {}",pReqTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pReqTransfer->IdCardType);
		tdLogger->info("CustType {}",pReqTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pReqTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pReqTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pReqTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pReqTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pReqTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pReqTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pReqTransfer->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspFromBankToFutureByFuture>");
};

void TdSpi::OnRspFromFutureToBankByFuture(CThostFtdcReqTransferField *pReqTransfer, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspFromFutureToBankByFuture>");
	if (pReqTransfer)
	{
		tdLogger->info("TradeCode {}",pReqTransfer->TradeCode);
		tdLogger->info("BankID {}",pReqTransfer->BankID);
		tdLogger->info("BankBranchID {}",pReqTransfer->BankBranchID);
		tdLogger->info("BrokerID {}",pReqTransfer->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqTransfer->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqTransfer->TradeDate);
		tdLogger->info("TradeTime {}",pReqTransfer->TradeTime);
		tdLogger->info("BankSerial {}",pReqTransfer->BankSerial);
		tdLogger->info("TradingDay {}",pReqTransfer->TradingDay);
		tdLogger->info("CustomerName {}",pReqTransfer->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqTransfer->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqTransfer->BankAccount);
		tdLogger->info("BankPassWord {}",pReqTransfer->BankPassWord);
		tdLogger->info("AccountID {}",pReqTransfer->AccountID);
		tdLogger->info("Password {}",pReqTransfer->Password);
		tdLogger->info("UserID {}",pReqTransfer->UserID);
		tdLogger->info("CurrencyID {}",pReqTransfer->CurrencyID);
		tdLogger->info("Message {}",pReqTransfer->Message);
		tdLogger->info("Digest {}",pReqTransfer->Digest);
		tdLogger->info("DeviceID {}",pReqTransfer->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqTransfer->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqTransfer->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqTransfer->OperNo);
		tdLogger->info("LongCustomerName {}",pReqTransfer->LongCustomerName);
		tdLogger->info("PlateSerial {}",pReqTransfer->PlateSerial);
		tdLogger->info("SessionID {}",pReqTransfer->SessionID);
		tdLogger->info("InstallID {}",pReqTransfer->InstallID);
		tdLogger->info("FutureSerial {}",pReqTransfer->FutureSerial);
		tdLogger->info("RequestID {}",pReqTransfer->RequestID);
		tdLogger->info("TID {}",pReqTransfer->TID);
		tdLogger->info("LastFragment {}",pReqTransfer->LastFragment);
		tdLogger->info("IdCardType {}",pReqTransfer->IdCardType);
		tdLogger->info("CustType {}",pReqTransfer->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqTransfer->VerifyCertNoFlag);
		tdLogger->info("FeePayFlag {}",pReqTransfer->FeePayFlag);
		tdLogger->info("BankAccType {}",pReqTransfer->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqTransfer->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqTransfer->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqTransfer->SecuPwdFlag);
		tdLogger->info("TransferStatus {}",pReqTransfer->TransferStatus);
		tdLogger->info("TradeAmount {}",pReqTransfer->TradeAmount);
		tdLogger->info("FutureFetchAmount {}",pReqTransfer->FutureFetchAmount);
		tdLogger->info("CustFee {}",pReqTransfer->CustFee);
		tdLogger->info("BrokerFee {}",pReqTransfer->BrokerFee);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspFromFutureToBankByFuture>");
};

void TdSpi::OnRspQueryBankAccountMoneyByFuture(CThostFtdcReqQueryAccountField *pReqQueryAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{
	tdLogger->info("<OnRspQueryBankAccountMoneyByFuture>");
	if (pReqQueryAccount)
	{
		tdLogger->info("TradeCode {}",pReqQueryAccount->TradeCode);
		tdLogger->info("BankID {}",pReqQueryAccount->BankID);
		tdLogger->info("BankBranchID {}",pReqQueryAccount->BankBranchID);
		tdLogger->info("BrokerID {}",pReqQueryAccount->BrokerID);
		tdLogger->info("BrokerBranchID {}",pReqQueryAccount->BrokerBranchID);
		tdLogger->info("TradeDate {}",pReqQueryAccount->TradeDate);
		tdLogger->info("TradeTime {}",pReqQueryAccount->TradeTime);
		tdLogger->info("BankSerial {}",pReqQueryAccount->BankSerial);
		tdLogger->info("TradingDay {}",pReqQueryAccount->TradingDay);
		tdLogger->info("CustomerName {}",pReqQueryAccount->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pReqQueryAccount->IdentifiedCardNo);
		tdLogger->info("BankAccount {}",pReqQueryAccount->BankAccount);
		tdLogger->info("BankPassWord {}",pReqQueryAccount->BankPassWord);
		tdLogger->info("AccountID {}",pReqQueryAccount->AccountID);
		tdLogger->info("Password {}",pReqQueryAccount->Password);
		tdLogger->info("UserID {}",pReqQueryAccount->UserID);
		tdLogger->info("CurrencyID {}",pReqQueryAccount->CurrencyID);
		tdLogger->info("Digest {}",pReqQueryAccount->Digest);
		tdLogger->info("DeviceID {}",pReqQueryAccount->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pReqQueryAccount->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pReqQueryAccount->BankSecuAcc);
		tdLogger->info("OperNo {}",pReqQueryAccount->OperNo);
		tdLogger->info("LongCustomerName {}",pReqQueryAccount->LongCustomerName);
		tdLogger->info("PlateSerial {}",pReqQueryAccount->PlateSerial);
		tdLogger->info("SessionID {}",pReqQueryAccount->SessionID);
		tdLogger->info("FutureSerial {}",pReqQueryAccount->FutureSerial);
		tdLogger->info("InstallID {}",pReqQueryAccount->InstallID);
		tdLogger->info("RequestID {}",pReqQueryAccount->RequestID);
		tdLogger->info("TID {}",pReqQueryAccount->TID);
		tdLogger->info("LastFragment {}",pReqQueryAccount->LastFragment);
		tdLogger->info("IdCardType {}",pReqQueryAccount->IdCardType);
		tdLogger->info("CustType {}",pReqQueryAccount->CustType);
		tdLogger->info("VerifyCertNoFlag {}",pReqQueryAccount->VerifyCertNoFlag);
		tdLogger->info("BankAccType {}",pReqQueryAccount->BankAccType);
		tdLogger->info("BankSecuAccType {}",pReqQueryAccount->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pReqQueryAccount->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pReqQueryAccount->SecuPwdFlag);
	}
	if (pRspInfo)
	{
		tdLogger->info("ErrorMsg {}",pRspInfo->ErrorMsg);
		tdLogger->info("ErrorID {}",pRspInfo->ErrorID);
	}
	tdLogger->info("nRequestID {}",nRequestID);
	tdLogger->info("bIsLast {}",bIsLast);
	tdLogger->info("</OnRspQueryBankAccountMoneyByFuture>");
};

void TdSpi::OnRtnOpenAccountByBank(CThostFtdcOpenAccountField *pOpenAccount) 
{
	tdLogger->info("<OnRtnOpenAccountByBank>");
	if (pOpenAccount)
	{
		tdLogger->info("TradeCode {}",pOpenAccount->TradeCode);
		tdLogger->info("BankID {}",pOpenAccount->BankID);
		tdLogger->info("BankBranchID {}",pOpenAccount->BankBranchID);
		tdLogger->info("BrokerID {}",pOpenAccount->BrokerID);
		tdLogger->info("BrokerBranchID {}",pOpenAccount->BrokerBranchID);
		tdLogger->info("TradeDate {}",pOpenAccount->TradeDate);
		tdLogger->info("TradeTime {}",pOpenAccount->TradeTime);
		tdLogger->info("BankSerial {}",pOpenAccount->BankSerial);
		tdLogger->info("TradingDay {}",pOpenAccount->TradingDay);
		tdLogger->info("CustomerName {}",pOpenAccount->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pOpenAccount->IdentifiedCardNo);
		tdLogger->info("CountryCode {}",pOpenAccount->CountryCode);
		tdLogger->info("Address {}",pOpenAccount->Address);
		tdLogger->info("ZipCode {}",pOpenAccount->ZipCode);
		tdLogger->info("Telephone {}",pOpenAccount->Telephone);
		tdLogger->info("MobilePhone {}",pOpenAccount->MobilePhone);
		tdLogger->info("Fax {}",pOpenAccount->Fax);
		tdLogger->info("EMail {}",pOpenAccount->EMail);
		tdLogger->info("BankAccount {}",pOpenAccount->BankAccount);
		tdLogger->info("BankPassWord {}",pOpenAccount->BankPassWord);
		tdLogger->info("AccountID {}",pOpenAccount->AccountID);
		tdLogger->info("Password {}",pOpenAccount->Password);
		tdLogger->info("CurrencyID {}",pOpenAccount->CurrencyID);
		tdLogger->info("Digest {}",pOpenAccount->Digest);
		tdLogger->info("DeviceID {}",pOpenAccount->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pOpenAccount->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pOpenAccount->BankSecuAcc);
		tdLogger->info("OperNo {}",pOpenAccount->OperNo);
		tdLogger->info("UserID {}",pOpenAccount->UserID);
		tdLogger->info("ErrorMsg {}",pOpenAccount->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pOpenAccount->LongCustomerName);
		tdLogger->info("PlateSerial {}",pOpenAccount->PlateSerial);
		tdLogger->info("SessionID {}",pOpenAccount->SessionID);
		tdLogger->info("InstallID {}",pOpenAccount->InstallID);
		tdLogger->info("TID {}",pOpenAccount->TID);
		tdLogger->info("ErrorID {}",pOpenAccount->ErrorID);
		tdLogger->info("LastFragment {}",pOpenAccount->LastFragment);
		tdLogger->info("IdCardType {}",pOpenAccount->IdCardType);
		tdLogger->info("Gender {}",pOpenAccount->Gender);
		tdLogger->info("CustType {}",pOpenAccount->CustType);
		tdLogger->info("MoneyAccountStatus {}",pOpenAccount->MoneyAccountStatus);
		tdLogger->info("VerifyCertNoFlag {}",pOpenAccount->VerifyCertNoFlag);
		tdLogger->info("CashExchangeCode {}",pOpenAccount->CashExchangeCode);
		tdLogger->info("BankAccType {}",pOpenAccount->BankAccType);
		tdLogger->info("BankSecuAccType {}",pOpenAccount->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pOpenAccount->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pOpenAccount->SecuPwdFlag);
	}
	tdLogger->info("</OnRtnOpenAccountByBank>");
};
void TdSpi::OnRtnCancelAccountByBank(CThostFtdcCancelAccountField *pCancelAccount) 
{
	tdLogger->info("<OnRtnCancelAccountByBank>");
	if (pCancelAccount)
	{
		tdLogger->info("TradeCode {}",pCancelAccount->TradeCode);
		tdLogger->info("BankID {}",pCancelAccount->BankID);
		tdLogger->info("BankBranchID {}",pCancelAccount->BankBranchID);
		tdLogger->info("BrokerID {}",pCancelAccount->BrokerID);
		tdLogger->info("BrokerBranchID {}",pCancelAccount->BrokerBranchID);
		tdLogger->info("TradeDate {}",pCancelAccount->TradeDate);
		tdLogger->info("TradeTime {}",pCancelAccount->TradeTime);
		tdLogger->info("BankSerial {}",pCancelAccount->BankSerial);
		tdLogger->info("TradingDay {}",pCancelAccount->TradingDay);
		tdLogger->info("CustomerName {}",pCancelAccount->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pCancelAccount->IdentifiedCardNo);
		tdLogger->info("CountryCode {}",pCancelAccount->CountryCode);
		tdLogger->info("Address {}",pCancelAccount->Address);
		tdLogger->info("ZipCode {}",pCancelAccount->ZipCode);
		tdLogger->info("Telephone {}",pCancelAccount->Telephone);
		tdLogger->info("MobilePhone {}",pCancelAccount->MobilePhone);
		tdLogger->info("Fax {}",pCancelAccount->Fax);
		tdLogger->info("EMail {}",pCancelAccount->EMail);
		tdLogger->info("BankAccount {}",pCancelAccount->BankAccount);
		tdLogger->info("BankPassWord {}",pCancelAccount->BankPassWord);
		tdLogger->info("AccountID {}",pCancelAccount->AccountID);
		tdLogger->info("Password {}",pCancelAccount->Password);
		tdLogger->info("CurrencyID {}",pCancelAccount->CurrencyID);
		tdLogger->info("Digest {}",pCancelAccount->Digest);
		tdLogger->info("DeviceID {}",pCancelAccount->DeviceID);
		tdLogger->info("BrokerIDByBank {}",pCancelAccount->BrokerIDByBank);
		tdLogger->info("BankSecuAcc {}",pCancelAccount->BankSecuAcc);
		tdLogger->info("OperNo {}",pCancelAccount->OperNo);
		tdLogger->info("UserID {}",pCancelAccount->UserID);
		tdLogger->info("ErrorMsg {}",pCancelAccount->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pCancelAccount->LongCustomerName);
		tdLogger->info("PlateSerial {}",pCancelAccount->PlateSerial);
		tdLogger->info("SessionID {}",pCancelAccount->SessionID);
		tdLogger->info("InstallID {}",pCancelAccount->InstallID);
		tdLogger->info("TID {}",pCancelAccount->TID);
		tdLogger->info("ErrorID {}",pCancelAccount->ErrorID);
		tdLogger->info("LastFragment {}",pCancelAccount->LastFragment);
		tdLogger->info("IdCardType {}",pCancelAccount->IdCardType);
		tdLogger->info("Gender {}",pCancelAccount->Gender);
		tdLogger->info("CustType {}",pCancelAccount->CustType);
		tdLogger->info("MoneyAccountStatus {}",pCancelAccount->MoneyAccountStatus);
		tdLogger->info("VerifyCertNoFlag {}",pCancelAccount->VerifyCertNoFlag);
		tdLogger->info("CashExchangeCode {}",pCancelAccount->CashExchangeCode);
		tdLogger->info("BankAccType {}",pCancelAccount->BankAccType);
		tdLogger->info("BankSecuAccType {}",pCancelAccount->BankSecuAccType);
		tdLogger->info("BankPwdFlag {}",pCancelAccount->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pCancelAccount->SecuPwdFlag);
	}
	tdLogger->info("</OnRtnCancelAccountByBank>");
};
void TdSpi::OnRtnChangeAccountByBank(CThostFtdcChangeAccountField *pChangeAccount) 
{
	tdLogger->info("<OnRtnChangeAccountByBank>");
	if (pChangeAccount)
	{
		tdLogger->info("TradeCode {}",pChangeAccount->TradeCode);
		tdLogger->info("BankID {}",pChangeAccount->BankID);
		tdLogger->info("BankBranchID {}",pChangeAccount->BankBranchID);
		tdLogger->info("BrokerID {}",pChangeAccount->BrokerID);
		tdLogger->info("BrokerBranchID {}",pChangeAccount->BrokerBranchID);
		tdLogger->info("TradeDate {}",pChangeAccount->TradeDate);
		tdLogger->info("TradeTime {}",pChangeAccount->TradeTime);
		tdLogger->info("BankSerial {}",pChangeAccount->BankSerial);
		tdLogger->info("TradingDay {}",pChangeAccount->TradingDay);
		tdLogger->info("CustomerName {}",pChangeAccount->CustomerName);
		tdLogger->info("IdentifiedCardNo {}",pChangeAccount->IdentifiedCardNo);
		tdLogger->info("CountryCode {}",pChangeAccount->CountryCode);
		tdLogger->info("Address {}",pChangeAccount->Address);
		tdLogger->info("ZipCode {}",pChangeAccount->ZipCode);
		tdLogger->info("Telephone {}",pChangeAccount->Telephone);
		tdLogger->info("MobilePhone {}",pChangeAccount->MobilePhone);
		tdLogger->info("Fax {}",pChangeAccount->Fax);
		tdLogger->info("EMail {}",pChangeAccount->EMail);
		tdLogger->info("BankAccount {}",pChangeAccount->BankAccount);
		tdLogger->info("BankPassWord {}",pChangeAccount->BankPassWord);
		tdLogger->info("NewBankAccount {}",pChangeAccount->NewBankAccount);
		tdLogger->info("NewBankPassWord {}",pChangeAccount->NewBankPassWord);
		tdLogger->info("AccountID {}",pChangeAccount->AccountID);
		tdLogger->info("Password {}",pChangeAccount->Password);
		tdLogger->info("CurrencyID {}",pChangeAccount->CurrencyID);
		tdLogger->info("BrokerIDByBank {}",pChangeAccount->BrokerIDByBank);
		tdLogger->info("Digest {}",pChangeAccount->Digest);
		tdLogger->info("ErrorMsg {}",pChangeAccount->ErrorMsg);
		tdLogger->info("LongCustomerName {}",pChangeAccount->LongCustomerName);
		tdLogger->info("PlateSerial {}",pChangeAccount->PlateSerial);
		tdLogger->info("SessionID {}",pChangeAccount->SessionID);
		tdLogger->info("InstallID {}",pChangeAccount->InstallID);
		tdLogger->info("TID {}",pChangeAccount->TID);
		tdLogger->info("ErrorID {}",pChangeAccount->ErrorID);
		tdLogger->info("LastFragment {}",pChangeAccount->LastFragment);
		tdLogger->info("IdCardType {}",pChangeAccount->IdCardType);
		tdLogger->info("Gender {}",pChangeAccount->Gender);
		tdLogger->info("CustType {}",pChangeAccount->CustType);
		tdLogger->info("MoneyAccountStatus {}",pChangeAccount->MoneyAccountStatus);
		tdLogger->info("BankAccType {}",pChangeAccount->BankAccType);
		tdLogger->info("VerifyCertNoFlag {}",pChangeAccount->VerifyCertNoFlag);
		tdLogger->info("BankPwdFlag {}",pChangeAccount->BankPwdFlag);
		tdLogger->info("SecuPwdFlag {}",pChangeAccount->SecuPwdFlag);
	}
	tdLogger->info("</OnRtnChangeAccountByBank>");
};

