#include<iostream>
#include<string>
#include<xlnt/xlnt.hpp>
#include<algorithm>
#include<tinyxml2.h>
#include<set>
#include<fstream>
#include"ele.H"
using namespace std;
using namespace libconfig;
typedef tinyxml2::XMLElement xmlElement;
typedef tinyxml2::XMLDocument xmlDocument;
typedef tinyxml2::XMLText xmlText;
typedef tinyxml2::XMLDeclaration xmlDeclaration;
typedef xlnt::cell_reference cell_ref;
std::string& trim(std::string &s)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}  

int string_replace(string &s1, const string &s2, const string &s3)
{
	string::size_type pos = 0;
	string::size_type a = s2.size();
	string::size_type b = s3.size();
	while ((pos = s1.find(s2,pos)) != string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
	return 0;
}
bool excelToXml(Config &cfg)
{
	string ExcelFile,XmlFile,rootElem,pieceElem;
	int startRow,endRow;
	if(!(cfg.lookupValue("XmlFile",XmlFile)) ||
		!(cfg.lookupValue("ExcelFile",ExcelFile)))
	{
		cerr<<"Error:No 'ExcelFile' or 'XmlFile' setting in configuration or in wrong format"<<endl;
		exit;
	}
	if(!(cfg.lookupValue("rootElem",rootElem)) ||
		!(cfg.lookupValue("pieceElem",pieceElem)))
	{
		cerr<<"Warning:No 'rootElem' or 'pieceElem' setting in configuration ,default ArrayCas and Cas"<<endl;
		rootElem="ArrayCas";
		pieceElem="Cas";
	}
	if(!(cfg.lookupValue("startRow",startRow)) ||
		!(cfg.lookupValue("endRow",endRow)))
	{
		cerr<<"Warning:No 'startRow' or 'endRow' setting in configuration ,default all"<<endl;
		startRow=1;
		endRow=INT_MAX;
   	}
	ofstream errorLog("errorLog.dat");
	xlnt::workbook wb;
	wb.load(ExcelFile);
	auto ws=wb.active_sheet();
	xmlDocument nXml;
	xmlDeclaration* dec = nXml.NewDeclaration();	
	nXml.InsertFirstChild(dec);
	int totalRowNum=ws.highest_row();
	totalRowNum=min(endRow,totalRowNum);
	xmlElement* root=nXml.NewElement(rootElem.c_str());
	nXml.InsertAfterChild(dec,root);
	xmlElement* Cas=NULL;
	xmlElement* elePointer=NULL;
	const Setting& cfgRoot = cfg.getRoot();
	const Setting &Elems = cfgRoot["Elems"];
	string xmlNull;
	cfgRoot.lookupValue("null",xmlNull);
	int elemsLength=Elems.getLength();
	string tempI;
	string textCon;
	string name;
	vector<classElem> vecElem;	
	for(int j=0;j<elemsLength;j++)
	{
		Elems[j].lookupValue("name",name);
		Elems[j].lookupValue("column",tempI);
		classElem tempcE(tempI,name);
		if(Elems[j].exists("enumIdentier"))
		{
			string recEnum;
			string identier;
			Elems[j].lookupValue("enum",recEnum);
			Elems[j].lookupValue("enumIdentier",identier);
			tempcE.setEnum(identier,recEnum);
		}
		if(Elems[j].exists("replaces"))
		{
			int k=0;
			string value;
			int repLength=Elems[j]["replaces"].getLength();
			for(int k=1;k<repLength+1;k++)
			{
				Elems[j]["replaces"].lookupValue("replace"+to_string(k),value);
				tempcE.setMapRep(value);
			}
		}
		vecElem.push_back(tempcE);
	}
	Setting *delCon=NULL;
	map<char,string> equalBreak;
	if(cfg.exists("deleteCondition"))
	{
		delCon= &cfgRoot["deleteCondition"];
		for(int i=0;i<(*delCon).getLength();i++)
		{
			string delType;
			string comment;
			(*delCon)[i].lookupValue("type",delType);
			if(delType=="equal")
			{
				(*delCon)[i].lookupValue("comment",comment);
				auto flag1=0;
				auto flag2=comment.find(',');
				string pieceOne;
				for(;flag2!=comment.npos;)
				{
					pieceOne=comment.substr(flag1,flag2-flag1);
					equalBreak[pieceOne[0]]=pieceOne.substr(2);
					flag1=flag2+1;
					flag2=comment.find(flag1,',');
				}
				pieceOne=comment.substr(flag1);
				equalBreak[pieceOne[0]]=pieceOne.substr(2);
			}
		}
	}
	int breakInt=0;
	for(int i=max(startRow,1);i<=totalRowNum;i++)
	{
		Cas=nXml.NewElement(pieceElem.c_str());
		root->InsertEndChild(Cas);
		breakInt=0;
		//元素读取
		for(auto pieceE:vecElem)
		{
	//		Elems[j].lookupValue("name",name);
			elePointer=nXml.NewElement(pieceE.getName().c_str());
			string columnNum=pieceE.getColumn();
			tempI=columnNum+to_string(i);
			{}
			textCon=ws[tempI.c_str()].to_string();
			trim(textCon);
			string_replace(textCon,"\n"," ");
			if(textCon=="") textCon=xmlNull;
			//处理替换
			if(pieceE.getReplaceSwitcher()&&
				pieceE.getRep().find(textCon)
				!=pieceE.getRep().end())
			{
				textCon=pieceE.getRep()[textCon];
			}
			//判断一下舍弃
			if(equalBreak.find(columnNum[0])!=equalBreak.end()&&equalBreak[columnNum[0]]==textCon)
			{
				breakInt++;
			}
			//处理枚举类型
			if(pieceE.getEnumSwitcher())
			{
				if(pieceE.getEnum().find(textCon)==pieceE.getEnum().end())//惨痛的教训,这里返回的是临时变量，必须返回私有变量的引用,否则比较的是两个不同的set
				{
					nXml.DeleteNode(elePointer);
					elePointer=NULL;
					root->DeleteChild(Cas);
					Cas=NULL;
					errorLog<<"error1: "<<i<<','<<textCon<<endl;
					break;
				}
			}
			//处理舍弃条件
			if(breakInt==equalBreak.size()&&breakInt!=0)
			{
				nXml.DeleteNode(elePointer);
				elePointer=NULL;
				root->DeleteChild(Cas);
				Cas=NULL;
				errorLog<<"error2: "<<i-1<<','<<textCon<<endl;
				break;
			}
			elePointer->SetText(textCon.c_str());
			Cas->InsertEndChild(elePointer);
		}
	}
	nXml.SaveFile(XmlFile.c_str());
	errorLog.close();
	return true;
}
