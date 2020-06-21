#include<iostream>
#include<string>
#include<fstream>
#include<libconfig.h++>
using namespace std;
using namespace libconfig;
#include"excelToXml.H"
#include"readConfig.H"

int main()
{
	cout<<"1:generate cas xml for FIDAB "<<endl;
	cout<<"2:import dat data into mysql for FIDAB cloud version"<<endl;
	cout<<"3:generate initialized SQL script for FIDAB cloud version"<<endl;
	cout<<"4:generate wqar para xml for FIDAB"<<endl;
	cout<<"Input function number then 'Enter':"<<endl;
	char choice;
	cin>>choice;
	switch(choice)
	{
		//生成CAS配置文件XML
		case '1':
		{
//			loadCas("dirPath");
//			generateInfo("dirPath"); break;
		}
		case '2':
		{
//			loadConfig("xlsxPath");
//			mainLogic("dirPath"); break;
		}
		case '3':
		{
//			loadConfig("xlsxPath");
//			generateSql("dirPath"); break;
		}
		case '4':
		{
			Config cfg;
			readConfig(cfg);
			excelToXml(cfg); break;
		}
		default:
		{
			cout<<"Wrong Option!"<<endl;break;
		}
	}
	cout<<"finish,Bye!"<<endl;
}
