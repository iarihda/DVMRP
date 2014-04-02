#include<iostream>
#include<cstring>
#include<fstream>
#include<ctime>
#include<cerrno>
#include<sstream>
#include<fstream>
#include<cstdlib>
#include<unistd.h>

using namespace std;

int num;
int lan;
char type[10];
int start_time;
int duration;
int count;
int lptr;

void sleep(int s) 
{ 
    int sec = int(s*1000000); 
    usleep(sec); 
}

void initialize(char* line[])
{
	count = lptr = 0;
	num = line[1][0]-'0';
	lan = line[2][0]-'0';
	strcpy(type,line[3]);
	if(strcmp(type,"sender")==0)
	{
		start_time = atoi(line[4]);
		duration = atoi(line[5]);	
	}
	else
	{
		start_time = 0;
		duration = 0;
	}
}

void print()
{
	cout<<endl<<"Num="<<num<<"\tLan="<<lan<<"\tType="<<type<<"\tStartTime="<<start_time<<"\tDuration"<<duration;
}

string intToString(int n)
{
	if(n==0) return "0";
	string temp="";
	string rv="";
	while(n>0)
	{
		temp+=n%10+48;
		n/=10;
	}
	for(int i=0;i<temp.length();i++)
		rv+=temp[temp.length()-i-1];
	return rv;
}

void sendData(string str) 
{
	string hfname = "hout";
	hfname.append(intToString(num));
	hfname.append(".txt");
	ofstream myfile;
	myfile.open(hfname.c_str(), fstream::out | fstream::app);
	myfile<<str.c_str();
	myfile<<"\r\n";
	myfile.close();
	if(strcmp(type,"sender")==0) 
	{
		cout<<endl<<"In Host "<<num<<endl;
		sleep(duration);
		count=count+duration;
		if(count>=100) exit(0);
		sendData(str);
	}
}

string* split(string str)
{
	string* s=new string[50];
	int ix=0;
	for(int i=0;i<str.length();i++)
	{
		if(str[i]==' ')
		{
			ix++;
			i++;
		}
		s[ix]+=str[i];
	}
	return s;
}

void readData()
{
	cout<<endl<<"In Host "<<num<<endl;
	string lfname = "lan";
	lfname.append(intToString(lan));
	lfname.append(".txt");
	ifstream myfile(lfname.c_str());
	myfile.seekg(lptr,ios::beg);
	string* s=new string[50];
	string line;
	if(myfile.is_open())
	{
		while(getline(myfile,line))
		{
			s=split(line);
			if(s[0]=="data")
			{
				string hfname = "hin";
				hfname.append(intToString(num));
				hfname.append(".txt");
				ofstream myfile1;
				myfile1.open(hfname.c_str(), fstream::out | fstream::app);
				myfile1<<line;
				myfile1<<"\r\n";
				myfile1.close();
			}
			lptr=myfile.tellg();
		}
		myfile.close();
	}
	count++;
	if(count%10==0)
	{
		string str="receiver ";
		str.append(intToString(lan));
		sendData(str);
	}
	if(count>=100) exit(0);
	sleep(1);
	readData();
}

int main(int argc, char* argv[])
{
	initialize(argv);
	string str="data ";
	str.append(intToString(lan));
	str.append(" ");
	str.append(intToString(lan));
	if(strcmp(type,"sender")==0)
	{
		sleep(start_time);
		count=count+start_time;
		sendData(str);
	}
	else if(strcmp(type,"receiver")==0)
		readData();
	return 0;
}
