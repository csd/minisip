/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/



#include<config.h>

#include<stdio.h>

#include"libmutil/TextUI.h"



#ifdef HAVE_TERMIOS_H
#include<termios.h>
#endif

#if defined WIN32 || defined _MSC_VER
#include<conio.h>
#include<windows.h>
#endif

#ifdef _MSC_VER
#include <sys/types.h>  
#include <sys/stat.h>
#include <fcntl.h>
#include<io.h>
#else
#include<unistd.h>
#endif


#include<libmutil/merror.h>
#include<libmutil/stringutils.h>
#include<libmutil/massert.h>

#include<iostream>

using namespace std;

const int TextUI::plain=0;
const int TextUI::bold=1;
const int TextUI::red=2;
const int TextUI::blue=3;
const int TextUI::green=4;

const int TextUI::KEY_UP=0x5b41;
const int TextUI::KEY_DOWN=0x5b42;
const int TextUI::KEY_RIGHT=0x5b43;
const int TextUI::KEY_LEFT=0x5b44;

#if defined(_MSC_VER) || defined(WIN32)
const char *termCodes[]= { "", "", "", "", "" };
#else
const char *termCodes[]= { "\033[m", "\033[2m\033[1m", "\033[31m", "\033[34m", "\033[42m" };
#endif

/**
 * Purpose: Tries to make STDIN non-blocking.
 * 
 * This method stores the current state of the terminal to an internal
 * structure that is used to restore the state when restoreStdinBlocking()
 * is called.
 *
 * This method is not implementedMicrosoft Windows.
 * 
 * @return 	0 	If successful
 * 		!=0	If the terminal/stdin could not be set to
 * 			nonblocking.
 */
int TextUI::makeStdinNonblocking(){
#ifdef HAVE_TERMIOS_H
	massert(terminalSavedState);
	tcgetattr(STDIN_FILENO, (struct termios*)terminalSavedState);
	
	struct termios termattr;
	int ret=tcgetattr(STDIN_FILENO, &termattr );
	if (ret < 0) {
		delete (struct termios*)terminalSavedState;
		terminalSavedState=NULL;
		merror("tcgetattr:");
		return -1;
	}
	termattr.c_cc[VMIN]=1;
	termattr.c_cc[VTIME]=0;
	termattr.c_lflag &= ~(ICANON | ECHO | ECHONL);

	ret = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
	if (ret < 0) {
		merror("tcsetattr");
		return -1;
	}
	return 0;
#else
#if defined(WIN32) || defined(_MSC_VER)
	HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
	if (!console){
		merr << "Error: could not get input handle"<<endl;
		return -1;
	}
	DWORD oldMode;
	if (!GetConsoleMode(console, &oldMode)){
		merr << "Error: could not get console mode"<<endl;
		return -1;
	}
	DWORD mode = oldMode & ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT;
	if (!SetConsoleMode(console, mode)){
		cerr << "Error: could not set console mode"<<endl;
		return -1;
	}
	return 0;
#endif
#endif
}

/**
 * Restores the terminal state to the one previous to running 
 * makeStdinNonblocking.
 * Precondition: makeStdinNonblocking has been run.
 */
void TextUI::restoreStdinBlocking(){
#ifdef HAVE_TERMIOS_H
	if (terminalSavedState){
		int ret = tcsetattr (STDIN_FILENO, TCSANOW, (struct termios*)terminalSavedState);
		if (ret < 0) {
			merror("tcsetattr");
		}
	}
#else
#if defined(WIN32) || defined(_MSC_VER) 
	HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
	if (!console){
		cerr << "Error: could get console handle"<<endl;
		return;
	}
	DWORD oldMode;
	if (!GetConsoleMode(console, &oldMode)){
		cerr << "Error: could get console mode"<<endl;
		return;
	}
	DWORD mode = oldMode | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT;
	if (!SetConsoleMode(console, mode)){
		cerr << "Error: could not restore console mode"<<endl;
	}
#endif
#endif
}

TextUI::TextUI() : maxHints(2000),isAskingDialogQuestion(false){

#ifdef HAVE_TERMIOS_H
	terminalSavedState = new struct termios;
#else
	terminalSavedState = NULL;
#endif
	
	if ( makeStdinNonblocking()!=0){
		cerr << "ERROR: Could not make stdin non-blocking"<< endl;
		//        exit(1);
	}
	running = true;
}

TextUI::~TextUI(){
	restoreStdinBlocking();
#ifdef HAVE_TERMIOS_H
	if (terminalSavedState){
		delete (struct termios*)terminalSavedState;
	}
#endif
}

void TextUI::addCommand(string cmd){
	commands.push_back(cmd);
}

void TextUI::setPrompt(string p){
	promptText=p;
}

void TextUI::answerQuestion(string answer){
	MRef<QuestionDialog *> d = questionDialogs.front();
	d->answers.push_back(answer);
	d->nAnswered++;
	if (d->nAnswered >= (int)d->questions.size()){
		//all questions now answered
		guiExecute(d);
		isAskingDialogQuestion=false;
		questionDialogs.pop_front();
		input = savedInput;
		promptText=savedPromptText;
		displayMessage("");
	}
	askQuestion();
}

void TextUI::askQuestion(){
	if (questionDialogs.size()<=0){
		return;
	}
	
	if (!isAskingDialogQuestion){
		isAskingDialogQuestion=true;
		savedPromptText = promptText;
		savedInput = input;
	}
	MRef<QuestionDialog*> dialog = questionDialogs.front();
	string q = dialog->questions[dialog->nAnswered];
	promptText = "Question: "+q;
	input= "";
	displayMessage(""); //clear screen and input
}

void TextUI::showQuestionDialog(const MRef<QuestionDialog*> &d){
	questionDialogs.push_back(d);
	if (!isAskingDialogQuestion){
		askQuestion();
	}

}

void TextUI::addCompletionCallback(string m, TextUICompletionCallback *cb){
	struct completion_cb_item cbi;
	cbi.match = m;
	cbi.callback = cb;
	completionCallbacks.push_back(cbi);
}

void TextUI::displayMessage(string msg, int style){
    cout << (char)13;
    if (msg.size()>0 && msg[msg.size()-1]==10)
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

int readChar(){
	int err;
	int ret;
	char tmpc;
#if defined WIN32 || defined _MSC_VER
//	tmpc = getchar();
	int n;
	if (!ReadConsole(GetStdHandle(STD_INPUT_HANDLE),&tmpc,1,(LPDWORD)&n, NULL)){
		cerr << "Console Error: error reading from stdin"<<endl;
	}	
	err=1;
#else
	err = read(STDIN_FILENO, &tmpc, 1);
#endif
	ret = tmpc;

	if (err!=1)
		return -1;

	//handle special keys prefixed with escape
	if (tmpc==27){
		char c1,c2;
#if defined WIN32 || defined _MSC_VER
		//c1 = getchar();
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE),&c1,1,(LPDWORD)&n, NULL);
		//c2 = getchar();
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE),&c2,1,(LPDWORD)&n, NULL);
		err=1;
#else
		read(STDIN_FILENO, &c1, 1);
		err = read(STDIN_FILENO, &c2, 1);
		if (err!=1)
			return -1;
#endif
		ret = c1<<8 | c2;
	}

	return ret;
}

void TextUI::guimain(){
	cout << promptText << "$ "<< flush;
	while (running){
		int c = readChar();
		if(c >= 0){
			keyPressed(c);


			string command;
			string autocomplete;
			unsigned i;

			switch(c){
				case 9: 
					autocomplete=displaySuggestions(input);
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

					if (isAskingDialogQuestion){
						answerQuestion(command);
						break;
					}

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
						input+=(char)c;
						cout << termCodes[bold]<< (char)c << termCodes[plain] << flush;
					}
			}
		}
//		else sleep(1);

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
		for (i=0; i<completionCallbacks.size(); i++){
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
	unsigned index = (unsigned)hint.size();
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


