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

int hosts[10];
int routers[10];
int lans[10];
int rptr[10];
int hptr[10];
int h,r,l;
int count;
string newline;

void sleep(int s) 
{ 
    int sec = int(s*1000000); 
    usleep(sec); 
}

void initialize()
{
	h=r=l=0;
	newline="\r\n";
	count=0;
	for(int i=0;i<10;i++)
		rptr[i]=hptr[i]=0;
}

void printInput() 
{
	for(int i=0;i<h;i++)
		cout<<"h[i]="<<hosts[i]<<"\t";
	cout<<endl;
	for(int i=0;i<r;i++)
		cout<<"r[i]="<<routers[i]<<"\t";
	cout<<endl;
	for(int i=0;i<l;i++)
		cout<<"l[i]"<<lans[i]<<"\t";
	cout<<endl;
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

void copyRouterMsg() 
{
	cout<<endl<<"In Controller"<<endl;
	for(int i=0;i<h;i++)
	{
		string hfname = "hout";
		hfname.append(intToString(hosts[i]));
		hfname.append(".txt");
		ifstream myfile(hfname.c_str());
		if(myfile.is_open())
		{
			myfile.seekg(hptr[i],ios::beg);
			string line;
			string* line_split=new string[50];
			while(getline(myfile,line))
			{
				line_split = split(line);
				string lfname = "lan";
				int t=atoi(line_split[1].c_str());
				lfname.append(intToString(t));
				lfname.append(".txt");
				ofstream myfile1;
				myfile1.open(lfname.c_str(), fstream::out | fstream::app);
				myfile1<<line;
				myfile1<<"\r\n";
				myfile1.close();
				hptr[i] = myfile.tellg();
			}
			myfile.close();
		}
	}

	for(int i=0;i<r;i++)
	{
		string rfname = "rout";
		rfname.append(intToString(routers[i]));
		rfname.append(".txt");
		ifstream myfile(rfname.c_str());
		if(myfile.is_open())
		{
			myfile.seekg(rptr[i],ios::beg);
			string line;
			string* line_split = new string[50];
			while(getline(myfile,line))
			{
				line_split = split(line);
				string lfname = "lan";
				lfname.append(line_split[1]);
				lfname.append(".txt");
				ofstream myfile1;
				myfile1.open(lfname.c_str(), fstream::out | fstream::app);
				myfile1<<line;
				myfile1<<"\r\n";
				myfile1.close();
				rptr[i] = myfile.tellg();
			}
			myfile.close();
		}
	}
	sleep(1);
	count++;
	if(count>=110) exit(0);
	copyRouterMsg();
}

int main(int argc, char* args[])
{
	int i=2;
	initialize();	
	while(args[i][0]!='r')
	{
		hosts[h++] = args[i][0]-'0';
		i++;
	}
	i++;
	while(args[i][0]!='l')
	{
		routers[r++] = args[i][0]-'0';
		i++;
	}
	i++;
	while(i<argc)
	{
		lans[l++] = args[i][0]-'0';
		i++;
	}
	//printInput();
	copyRouterMsg();
	return 0;
}
