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
int lans[10];
int num_lans;
int dest[10];
int cost[10];
int nexthop[10];
int nextrout[10];
string lan_routers[10][10];
int count;
int fdata;
int rflag;
int lptr[10];
int rptr[10];
string* child = new string[10];
string* child_added=new string[10];
string lr_added[10][10];
string receiver_lans;

void sleep(int s) 
{ 
    int sec = int(s*1000000); 
    usleep(sec); 
}

void initialize()
{
	num = 0;
	count = 0;
	for(int i=0;i<10;i++)
		lans[i] = lptr[i] = 0;
	num_lans = 0;
	fdata = -1;
	for(int i=0;i<10;i++)
	{
		dest[i]=i;
		cost[i]=10;
		nexthop[i]=10;
		nextrout[i]=10;
		rflag = 0;
	}
}

void printInput()
{
	cout<<"Router "<<num<<endl;
	for(int i=0;i<num_lans;i++)
		cout<<lans[i]<<"\t";
}

void create_RT() 
{
	for(int j=0;j<10;j++)
	{
		for(int k=0;k<num_lans;k++)
		{
			if(j==lans[k])
			{
				cost[j] = 0;
				nexthop[j] = dest[j];
				nextrout[j] = num;
			}
		}
	}
	//print_RT();
}

void print_RT() 
{
	cout<<endl;
	cout<<"Router "<<num;
	cout<<endl;
	cout<<"Dest\tCost\tNextLan\tNextRout\tChild"<<endl;
	for(int i=0;i<10;i++)
		cout<<dest[i]<<"\t"<<cost[i]<<"\t"<<nexthop[i]<<"\t"<<nextrout[i]<<"\t"<<child[i]<<"\t"<<receiver_lans<<endl;
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

int toInteger(string s)
{
	int i=atoi(s.c_str());
	if(s=="10") return 10;
	return i;
}

void send_DV()
{
	string fname = "rout";
	fname.append(intToString(num));
	fname.append(".txt");
	ofstream myfile;
	myfile.open(fname.c_str(), fstream::out | fstream::app);
	string msg;
	for(int i=0;i<num_lans;i++)
	{
		msg = "DV ";
		msg.append(intToString(lans[i]));
		msg.append(" ");
		msg.append(intToString(num));
		msg.append(" ");
		for(int j=0;j<10;j++)
		{
			msg.append(intToString(cost[j]));
			msg.append(" ");
			msg.append(intToString(nextrout[j]));
			msg.append(" ");
		}			
		myfile<<msg;
		myfile<<"\r\n";
	}
	myfile.close();
}

int check_duplicate(string lan2, int i) 
{
	for(int i1=0;i1<lan2.length();i1++)
	{
		int t=lan2[i1]-'0';
		if(t==i) return 1;
	}
	return 0;
}

void readLan() 
{
	for(int i=0;i<num_lans;i++)
	{
		string lfname = "lan";
		lfname.append(intToString(lans[i]));
		lfname.append(".txt");
		ifstream myfile(lfname.c_str());
		if(myfile.is_open())
		{
			myfile.seekg(lptr[i],ios::beg);
			string line;
			string* line_split = new string[10];
			while(getline(myfile,line))
			{	
				line_split=split(line);
				if(line_split[0]=="DV")
				{
					for(int k=0,j=3; k<10; k++,j=j+2)
					{
						if(toInteger(line_split[2])!=num)
						{
							if(toInteger(line_split[j+1])==num)
							{
								if(child[k].c_str()==NULL) 
								{
									child_added[k] = line_split[1];
									child[k] = line_split[1];
								}
								else if(check_duplicate(child_added[k],toInteger(line_split[1]))==0) 
								{
									child_added[k] = child_added[k]+line_split[1];
									child[k] = child[k]+line_split[1];
								}
								if(lan_routers[k][toInteger(line_split[1])].c_str()==NULL) 
								{
									if(lr_added[k][toInteger(line_split[1])].c_str()==NULL)
										lr_added[k][toInteger(line_split[1])]=line_split[2];
									else lr_added[k][toInteger(line_split[1])]=lr_added[k][toInteger(line_split[1])] + line_split[2];
									lan_routers[k][toInteger(line_split[1])]=line_split[2];
								}
								else if(check_duplicate(lr_added[k][toInteger(line_split[1])], toInteger(line_split[2]))==0) 
								{
									lr_added[k][toInteger(line_split[1])] = lr_added[k][toInteger(line_split[1])] + line_split[2];
									lan_routers[k][toInteger(line_split[1])] = lan_routers[k][toInteger(line_split[1])] + line_split[2];
								}	
							}	
						}
						if(cost[k]>toInteger(line_split[j])+1)
						{
							cost[k]=toInteger(line_split[j])+1;
							nexthop[k]=toInteger(line_split[1]);
							nextrout[k]=toInteger(line_split[2]);
						}			
						else if(cost[k]==toInteger(line_split[j])+1 && nextrout[k]>toInteger(line_split[2]))
						{
							nexthop[k]=toInteger(line_split[1]);
							nextrout[k]=toInteger(line_split[2]);
						}
					}
				}
				if((line_split[0]=="data") && lans[i] == nexthop[toInteger(line_split[2])])
				{
					fdata=toInteger(line_split[2]);
					for(int jx=0;jx<num_lans;jx++)
					{
						if(lans[jx]!=lans[i])
						{
							if(check_duplicate(child[toInteger(line_split[2])],lans[jx])==1 || check_duplicate(receiver_lans,lans[jx])==1)
							{
								string lanf = "rout";
								lanf.append(intToString(num));
								lanf.append(".txt");
								ofstream myfile;
								myfile.open(lanf.c_str(), fstream::out | fstream::app);
								string str = line_split[0];
								str.append(" ");
								str.append(intToString(lans[jx]));
								str.append(" ");
								str.append(line_split[2]);
								myfile<<str;
								myfile<<"\r\n";
								myfile.close();						
							}
						}
					}
				}
				if(line_split[0]=="receiver")
				{
					int k = toInteger(line_split[1]);
					if(receiver_lans.c_str()==NULL) receiver_lans = line_split[1];
					else if(check_duplicate(receiver_lans,toInteger(line_split[1]))==0) receiver_lans = receiver_lans+line_split[1];
					rflag = 1;
				}
				if(line_split[0]=="NMR")
				{
					if(line_split[1].c_str()!=NULL && nexthop[toInteger(line_split[3])]!=lans[i] && nexthop[toInteger(line_split[3])]!=toInteger(line_split[3]))
					{
						int lanid = toInteger(line_split[1]);
						int hostid = toInteger(line_split[3]);
						if(lan_routers[hostid][lanid].c_str()!=NULL)
						{
							string temp = "\0";
							for(int p=0;p<lan_routers[hostid][lanid].length();p++)
							{
								if(lan_routers[hostid][lanid][p]!=line_split[2][0]) 
								{
										if(temp=="\0") temp[0] = lan_routers[hostid][lanid][p];
										else temp = temp + lan_routers[hostid][lanid][p];
								}
							}
							lan_routers[hostid][lanid]=temp;
							if(lan_routers[hostid][lanid]=="\0") 
							{
								string ttemp="\0";
								for(int p=0;p<child[toInteger(line_split[3])].length();p++)
								{
									if(child[toInteger(line_split[3])][p]!=line_split[1][0]) 
									{
											if(ttemp=="\0") ttemp[0] = child[toInteger(line_split[3])][p];
											else ttemp = ttemp + child[toInteger(line_split[3])][p];
									}
								}		
								child[toInteger(line_split[3])]=ttemp;
							}

							if(child[toInteger(line_split[3])]=="\0" && receiver_lans=="\0") 
							{
								string routf = "rout";
								routf.append(intToString(num));
								routf.append(".txt");								
								ofstream myfile;
								myfile.open(routf.c_str(), fstream::out | fstream::app);
								string str = "NMR ";
								str.append(intToString(nexthop[toInteger(line_split[3])]));
								str.append(" ");
								str.append(intToString(num));
								str.append(" ");
								str.append(line_split[3]);
								myfile<<str;
								myfile<<"\r\n";
								myfile.close();			
							}													
						}
					}
				}
				lptr[i] = myfile.tellg();
			}
			myfile.close();
		}
	}
}

void checkReceiver() 
{
	for(int i=0;i<num_lans;i++)
	{
		if(rflag==0 && receiver_lans.c_str()!=NULL)
		{
			if(check_duplicate(receiver_lans,lans[i])==1) 
				receiver_lans = "\0";
		}
		rflag=0;
	}
}

void sendNMR()
{
	if(fdata!=-1 && child[fdata]=="\0" && receiver_lans=="\0")
	{
		string routf = "rout";
		routf.append(intToString(num));
		routf.append(".txt");
		ofstream myfile;
		myfile.open(routf.c_str(), fstream::out | fstream::app);
		string str = "NMR ";
		str.append(intToString(nexthop[fdata]));
		str.append(" ");
		str.append(intToString(num));
		str.append(" ");
		str.append(intToString(fdata));
		myfile<<str;
		myfile<<"\r\n";
		myfile.close();						
	}
}

void readWrite() 
{
	cout<<endl<<"In Router "<<num<<endl;
	readLan();
	if(count%20==0) checkReceiver();
	if(count%5==0) send_DV();
	if(count%10==0) sendNMR();
	if(count>=100) exit(0);
	count++;
	sleep(1);
	readWrite();
}

int main(int argc, char* args[])
{
	initialize();
	num = args[1][0]-'0';
	for(int i=2;i<argc;i++)
	{
		lans[i-2] = args[i][0]-'0';
		num_lans++;
	}
	//printInput();
	create_RT();
	readWrite();
	return 0;
}

