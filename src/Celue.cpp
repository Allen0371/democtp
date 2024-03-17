#include "Celue.h"
#include <stdio.h>
#include <dirent.h>

#define DIR_CELUE "./strategy/"
Celue::Celue(shared_ptr<TickMarkets> & m_hqTick,TdSpi * w_tdapi):cl_Tick(m_hqTick),cl_tdapi( w_tdapi)
{

   cout << "策略线程启动" << endl;
   clStart();
}

Celue::~Celue()
{
}

void Celue::clStart()
{
   vector<string> libName;
   clGetFile(libName);
   if(!libName.empty()){
      for(auto itr = libName.begin();itr != libName.end();itr++){
        cout<< "  策略名称：" << *itr << endl;
		string _strName = *itr ;
		//Baselib.insert(make_pair(_strName,new strategyBase);
      }
   }else{
	cout << " >>策略文件夹为空 " << endl;
   }
   for(;;){
   if(cl_tdapi->getSedordAllow()){
	orderData ord = {0};
	ord.ord_symbol = "rb2409";
	//获取最新价格
	ord.ord_price = this->getSymbolNewPrice(ord.ord_symbol);
	//jg 不等0
	ord.ord_kpdk = OR_KD ;

	ord.ord_lots = 1 ;
	//下单

	cl_tdapi->xd(ord);

	cout << "下单成功" << endl;
	sleep(5);
   }else{
	cout << "交易线程 下单不被 允许" << endl;
	
   }
   cout << "停下3秒" << endl;
   sleep(3);
   
   }
}

double Celue::getSymbolNewPrice(string &symName,int canshu)
{
	try
	{
		/* code */
		double _tt{0},p5 ,p4, p3, p2,b2 ,b3,b4,b5;
		switch( canshu){
			case 5 :
			p5 = cl_Tick->at(symName).back().AskPrice5 ;
			if(p5 != 0.0 || p5 == My_Max_Double){
				_tt = p5;
				}else{
					_tt =  cl_Tick->at(symName).back().AskPrice1 ;
			}
			break;

			case 4 :
			p4 = cl_Tick->at(symName).back().AskPrice4 ;
			if(p4 != 0.0 || p4 == My_Max_Double){
				_tt = p4;
				}else{
				_tt = cl_Tick->at(symName).back().AskPrice1 ;
			}
			break;

			case 3 :
			p3 = cl_Tick->at(symName).back().AskPrice3 ;
			if(p3 != 0.0 || p3 == My_Max_Double){
				_tt = p3 ;
				}else{
					_tt = cl_Tick->at(symName).back().AskPrice1 ;
			}
			break;

			case 2 :
			p2 = cl_Tick->at(symName).back().AskPrice2 ;
			if(p2!= 0.0  || p2 == My_Max_Double){
				_tt = p2;
				}else{
				_tt = cl_Tick->at(symName).back().AskPrice1 ;
			}
			break;

			case 1 :
		
					_tt = cl_Tick->at(symName).back().AskPrice1 ;
			
			break;

			case -1 :
			_tt = cl_Tick->at(symName).back().BidPrice1 ;
			break;

			case -2 :
			b2 = cl_Tick->at(symName).back().BidPrice2 ;
			if(b2 != 0.0 || b2 == My_Max_Double){
				_tt = b2;
				}else{
					_tt = cl_Tick->at(symName).back().BidPrice1 ;
			}
			break;

			case -3 :
			b3 = cl_Tick->at(symName).back().BidPrice3 ;
			if(b3 != 0.0 || b3 == My_Max_Double){
				_tt = b3;
				}else{
					_tt = cl_Tick->at(symName).back().BidPrice1 ;
			}
			break;

			case -4 :
			b4 = cl_Tick->at(symName).back().BidPrice4 ;
			if(b4 != 0.0  || b4 == My_Max_Double){
				_tt = b4;
				}else{
					_tt = cl_Tick->at(symName).back().BidPrice1 ;
			}
			break;

			case -5 :
			b5 = cl_Tick->at(symName).back().BidPrice5 ;
			if(b5 != 0.0  || b5 == My_Max_Double){
				_tt = b5;
				}else{
					_tt = cl_Tick->at(symName).back().BidPrice1 ;
			}
			break;


			default :
			    _tt = cl_Tick->at(symName).back().LastPrice ;
				break;


		}
		return _tt;
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 0.0;
	}
	
    
}

int Celue::clGetFile(vector<string> &vecfile)
{
    DIR *dirp = opendir(DIR_CELUE);	//打开/home/pikaqiu/xxx 目
	if(dirp == NULL)
	{
		perror("opendir");
		return -1;
	}
// 	struct dirent {
//     ino_t          d_ino;       /* 索引节点号 Inode number */
//     off_t          d_off;       /* 在目录文件中的偏移 Not an offset; see below */
//     unsigned short d_reclen;    /* 文件名长 Length of this record */
//     unsigned char  d_type;      /* 文件类型 Type of file; not supported by all filesystem types */
//     char           d_name[256]; /* 文件名，最长255字符 Null-terminated filename */
// };
    smatch m;
    std::regex reg_str("(\\.so)$");


	struct dirent *dir;
	while((dir = readdir(dirp)) != NULL)
	{
		if(dir->d_type == 8)		//如果是普通文件则打印文件名字
		{
			string str = dir->d_name;
			if(regex_search(str,m,reg_str))
			vecfile.push_back(str);
		}
	}
	
	closedir(dirp);		//关闭目录
	return 0;
}


