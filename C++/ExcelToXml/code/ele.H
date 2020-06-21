#include<string>
#include<set>
#include<map>
using namespace std;
class classElem
{
private:
	string column;
	string name;
	bool enumSwitcher;
	set<string> receiver;
	bool replaceSwitcher;
	map<string,string> repReceiver;
public:
	classElem(string s_column="A",string s_name="elem")
	{column=s_column;name=s_name;receiver.clear();enumSwitcher=replaceSwitcher=false;}
	void setEnum(string identier,string sentence);
	void setMapRep(string sentence);
	set<string>& getEnum() {return receiver;}
	map<string,string>& getRep() {return repReceiver;}
	string& getColumn(){ return column;}
	string& getName(){ return name;}
	bool& getEnumSwitcher() {return enumSwitcher;}
	bool& getReplaceSwitcher() {return replaceSwitcher;}

};
void classElem::setEnum(string identier,string sentence)
{
	enumSwitcher=true;
	string oneWord;
	for(auto s:sentence)	
	{
		if(s!=identier[0]) oneWord.append(1,s);
		else
		{
			receiver.insert(oneWord);
			oneWord.clear();
		}
	}
	receiver.insert(oneWord);
}
void classElem::setMapRep(string sentence)
{
	replaceSwitcher=true;
	string oneWord;
	string twoWord;
	auto flag=sentence.find(',');
	oneWord=sentence.substr(0,flag);
	twoWord=sentence.substr(flag+1);
	repReceiver[oneWord]=twoWord;
}
