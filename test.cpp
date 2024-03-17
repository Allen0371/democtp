#include <iostream>
#include <memory>
#include "mainThread.h"
using namespace std;

int main(){

cout << "Pragram Begin >>> "<< endl;
cout << "开始时间"<< getTimeStr() << endl;
std::make_unique<mainThread>();
cout << "Pragram OVER !!! "<< endl;
cout << "结束时间"<< getTimeStr() << endl;

}