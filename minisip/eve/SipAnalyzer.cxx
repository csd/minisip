#include"SipAnalyzer.h"

#include<string>
#include<iostream>
#include<libmutil/trim.h>
#include"EveGui.h"
#include <sys/time.h>
#include <time.h>
#include<libmutil/itoa.h>


using namespace std;

string now_timestamp(){
	struct timeval t;
	gettimeofday(&t, NULL);
	struct tm *tmp = localtime(&t.tv_sec);
	return itoa(tmp->tm_hour)+":"+itoa(tmp->tm_min)+"."+itoa(tmp->tm_sec);
}

int my_tolower(int c){
	if (c=='\r')
		return '\n';
	else
		return tolower(c);
}

int searchForHeader(string header, char *buf, int n){
	int slen = header.length()+2;
	char *searchstr = new char[slen+1];
	searchstr[0]=searchstr[1]='\n';
	memcpy(&searchstr[2], header.c_str(), header.length());
	
	
	for (int i=0; i<n-slen; i++){
		for (int j=0; j<slen; j++){
			if (my_tolower(searchstr[j])!=my_tolower(buf[i+j]))
				break;

			if (j==slen-1){
				delete searchstr;
				return i;
			}
		}
	
	}
	delete searchstr;
	return -1;
}

int searchFor(string s, char *buf, int n){
	int slen = s.length();
	char *searchstr = new char[slen+1];
	memcpy(searchstr, s.c_str(), slen);
	
	for (int i=0; i<n-slen; i++){
		for (int j=0; j<slen; j++){
			if (searchstr[j]!=buf[i+j])
				break;

			if (j==slen-1){
				delete searchstr;
				return i;
			}
		}
	
	}
	delete searchstr;
	return -1;
}


SipAnalyzer::SipAnalyzer(EveGui *g):gui(g){

}

void SipAnalyzer:: handlePacket(uint32_t from_ip, uint16_t from_port,
		uint32_t to_ip, uint16_t to_port,
		void *data, int n, int protocol ){


//	cerr <<"SipAnalyzer: got packet"<< endl;

	char *buf = (char*)data;
	
	
	int from_pos, to_pos, callid_pos;
	if (protocol==Sniffer::PROTOCOL_UDP){
		if ((from_pos=searchForHeader("from:",buf, n))>=0 && 
				(to_pos=searchForHeader("to:",buf,n))>=0&& 
				(callid_pos=searchForHeader("call-id:",buf,n))>=0){

			from_pos+=5+2;
			to_pos+=3+2;
			
			int from_end=from_pos;
			while (buf[from_end]!='\n' && buf[from_end]!='\r' && from_end<n)
				from_end++;
			
			int to_end=to_pos;
			while (buf[to_end]!='\n' && to_end<n)
				to_end++;
			
			string from;
			int i;
			for (i=from_pos; i<from_end&&buf[i]!=';'; i++)
				from+=buf[i];

			string to;
			for (i=to_pos; i<to_end&&buf[i]!=';'; i++)
				to+=buf[i];


			cerr << "found sip packet!"<< endl;
			cerr << "from: <"<< from << "> to <"<<to<< ">"<< endl;

			string method;
			if (searchFor("INVITE ",buf, n)>=0){
				method = "INVITE";
			}else if (searchFor("CANCEL",buf, n)>=0){
				method = "CANCEL";
			}else if (searchFor("REGISTER",buf, n)>=0){
				method = "REGISTER";
			}else if (searchFor("SUBSCRIBE",buf, n)>=0){
				method = "SUBSCRIBE";
			}else if (searchFor("SIP/2.0 2", buf, n)>=0){
				method = "Remote user accepted";
			}else if (searchFor("SIP/2.0 4", buf, n)>=0){
				method = "Remote user rejected";
			}else if (searchFor("ACK ",buf, n)>=0){
				method = "ACK";
			}else if (searchFor("BYE",buf, n)>=0){
				method = "BYE";
			}else if (searchFor("MESSAGE ",buf, n)>=0){
				method = "MESSAGE";
			}

			cerr << "method is "<< method<< endl;

			if (method.length()>0 &&method !="ACK"){
				gui->addSignallingMessage(now_timestamp()+": "+method +" from "+trim(from)+" to "+trim(to));
			
			}else{
			
			}
		
		}
	
	}


}

