/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#include<config.h>

#include<stdio.h>

#include"libmutil/TextUI.h"

#ifdef HAVE_TERMIOS_H
#include<termios.h>
#endif
#ifdef WIN32
#include<conio.h>
#endif
#include<unistd.h>
#include<libmutil/trim.h>
//#include<libmnetutil/UDPSocket.h>

#include<iostream>

using namespace std;

const int TextUI::plain=0;
const int TextUI::bold=1;
const int TextUI::red=2;
const int TextUI::blue=3;
const int TextUI::green=4;

char *termCodes[]= { "\033[m", "\033[2m\033[1m", "\033[31m", "\033[34m", "\033[42m" };

static int nonblockin_stdin() 
{
#ifdef HAVE_TERMIOS_H
    struct termios termattr;
    int ret=tcgetattr(STDIN_FILENO, &termattr);
    if (ret < 0) {
        perror("tcgetattr:");
        return -1;
    }
    termattr.c_cc[VMIN]=1;
    termattr.c_cc[VTIME]=0;
    termattr.c_lflag &= ~(ICANON | ECHO | ECHONL);

    ret = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
    if (ret < 0) {
        perror("tcsetattr");
        return -1;
    }
#endif
//#ifdef WIN32
//#warning nonblockin_stdin unimplemented on Win32
//#endif
    return 0;
}

TextUI::TextUI() : maxHints(2000){
    if ( nonblockin_stdin()!=0){
        cerr << "ERROR: Could not make stdin non-blocking"<< endl;
        exit(1);
    }
}

void TextUI::addCommand(string cmd){
	commands.push_back(cmd);
}

void TextUI::setPrompt(string p){
	promptText=p;
}

void TextUI::addCompletionCallback(string m, TextUICompletionCallback *cb){
	struct completion_cb_item cbi;
	cbi.match = m;
	cbi.callback = cb;
	completionCallbacks.push_back(cbi);
}

void TextUI::displayMessage(string msg, int style){
    cout << (char)13;
    if (msg[msg.size()-1]==10)
        msg=msg.substr(0,msg.size()-1);

    if (style<0){
        cout << msg;
    }else{
	cout << termCodes[style]; 
        cout << msg << termCodes[plain];
    }

    
    if (msg.size()<=input.size()+promptText.size()+2){	// 2==strlen("$ ")
	    
//	    cout << "will loop for "<< input.size()+prompt.size()-msg.size()+1 << endl;
//	    cout << "input.size="<< input.size() << " prompt.size=" << prompt.size()<< endl;
//	    cout << "prompt is <"<< prompt << ">"<< endl;
	    for (unsigned i=0; i< input.size()+promptText.size()+2-msg.size(); i++){
		    cout << ' ';
	    }
    }
    
    cout << endl;
    cout << promptText<<"$ ";
    cout << termCodes[bold]<< input << termCodes[plain] << flush;
}

void TextUI::guimain(){
//	fd_set set;
//	UDPSocket ispaceSocket(false, 3300);
	cout << promptText << "$ "<< flush;
	while (1){
		char c;
#ifdef LINUX
/*		FD_ZERO(&set);
		FD_SET(STDIN_FILENO, &set);
		FD_SET(ispaceSocket.getFd(), &set);


		int avail;
		do{
			avail = select(ispaceSocket.getFd()+1,&set,NULL,NULL,NULL );
		} while( avail < 0 );
		displayMessage("Awake!");
		
                if( FD_ISSET( STDIN_FILENO, &set )){
                        displayMessage("data on STDIN");
*/
		read(STDIN_FILENO, &c, 1);
/*
                }else{
                        displayMessage("data on iSpace connection");
                        char buf[66000];
                        for (int i=0; i< 1600; i++)
                                buf[i]=0;
                        ispaceSocket.recv(buf, 66000);
                        input = string(buf);
			for (unsigned e=0; e< input.size(); e++)
				keyPressed(input[e]);
                        c='\n';
			keyPressed(c);
                }
*/
#endif
#ifdef WIN32
		c= _getch();
//		displayMessage(string("read: ")+c+"("+itoa((int)c)+")");
#endif
		keyPressed(c);


		string command;
		string autocomplete;
		unsigned i;

		switch(c){
		case 9: autocomplete=displaySuggestions(input);
			if (autocomplete.size()>0){
				input=autocomplete;
			}

			cout << (char)13<<promptText<<"$ "<< termCodes[bold]<< input <<termCodes[plain] << flush;
			break;
		case 10:
		case 13:
			command = trim(input);
			if (command.size()>0)
				cout <<endl;
			input="";

			if (command.size()==0){
				displayMessage("");
			}else{
				guiExecute(command);
			}
			
		case 8:
		case 127:
			input = input.substr(0, input.size()-1);
			cout << (char)13<<promptText<< "$ ";

			for (i=0; i< input.size()+1; i++)
				cout << ' ';
			cout << (char)13<<promptText<<"$ ";
			cout << termCodes[bold]<< input << termCodes[plain] << flush;
			break;

		default:
			//     @A-Z               a-z                  . / 0-9 :           space     .
			if ( (c>=64 && c<=90) || (c>=97 && c<=122) || (c>=46 && c<=58) || c==32  || c=='-' || c=='_' || c=='.' ){
				input+=c;
				cout << termCodes[bold]<< c << termCodes[plain] << flush;
			}
		}

	}
}

void TextUI::outputSuggestions(minilist<string> &strings){
	string out;
	for (int i=0; i<maxHints && i< strings.size(); i++){
		if (i!=0)
			out+=" | ";
		out+= strings[i];
	}
	displayMessage(out);
}

string TextUI::displaySuggestions(string hint){
	minilist<string> cbSuggest;
	int i;
	for (i=0; i< completionCallbacks.size();i++){
		minilist<string> tmp;
		if (completionCallbacks[i].match == trim(hint).substr(0,completionCallbacks[i].match.size())){
			tmp = completionCallbacks[i].callback->textuiCompletionSuggestion(hint);
			for (int j=0; j<tmp.size(); j++){
				if (hint=="" || 
						(tmp[j].size() >= hint.size() && 
						 tmp[j].substr(0, hint.size()) == hint )){
					cbSuggest.push_back(tmp[j]);
				}
			}
		}
			
	}
	int ncmds = commands.size();
	if (ncmds+cbSuggest.size()<=0)
		return "";
	
	minilist<string> res;
	
	int nfound=0;
	for (i=0; i<ncmds; i++){ //Find possible matches
		if (hint=="" || 
				(commands[i].size()>=hint.size() && 
				 commands[i].substr(0,hint.size())==hint )){
			nfound++;
			res.push_back(commands[i]);
		}
	}
	
	if (cbSuggest.size()==0){
		for (int i=0; i<completionCallbacks.size(); i++){
			if (hint=="" || 
					(completionCallbacks[i].match.size()>=hint.size() && 
					 completionCallbacks[i].match.substr(0,hint.size())==hint )){
				nfound++;
				res.push_back(completionCallbacks[i].match);
			}
		}
	}
		
	
	if (nfound+cbSuggest.size()==0){
		return "";
	}

	if (nfound==1 && cbSuggest.size()==0){
		return res[0]+" ";
	}

	if (nfound==0 && cbSuggest.size()==1){
		return cbSuggest[0]+" ";
	}


	for (i=0; i<res.size(); i++){
		cbSuggest.push_back(res[i]);
	}
	

	//Now all suggested strings are in cbSuggest (the ones from the
	//callback plus the ones matching from "command")
	
	bool cont = true;
	unsigned index = hint.size();
	while (cont){
		char c=0;
		for (i=0; i< cbSuggest.size(); i++){
			if (cbSuggest[i].size()<=index){
				cont = false;
			}else{
				if (!c){
					c = cbSuggest[i][index];
				}else{
					if (cbSuggest[i].size()<=index || cbSuggest[i][index]!=c)
						cont=false;
				}
			}
		}
		if (cont)
			index++;
	}
	
	if (cbSuggest.size()>0 && index<=hint.size()){
		outputSuggestions(cbSuggest);
	}
	
	if (index>hint.size()){
		string ac = cbSuggest[0].substr(0,index);
		
		if (cbSuggest.size()==1 && ac[ac.size()-1] !=' ')
			ac+=" ";
		return ac;
	}else{
		return "";
	}
}


