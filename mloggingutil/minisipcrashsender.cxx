#include "CrashSender.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


using namespace std;

int main(){
	FILE *stream;
	char line[256];
	
	putenv("MAIN_DIALOG=<window title=\"MiniSIP Crash\" window_position=\"1\"><vbox>"
			"<text><label>MiniSIP crashed unexpectedly.</label></text><text><label>Do you wish to send the crash"
			"report?</label></text><hbox><button ok></button><button cancel></button></hbox>"
			"</vbox></window>");
	
	stream = popen("gtkdialog --program=MAIN_DIALOG", "r");
	fgets(line, 255, stream);
	
	if(strstr(line, "OK") != NULL){
		CrashSender crashSender;
		if(crashSender.send()){
			printf("[MiniSIP-CrashSender] crash report sent\n");
		}else{
			printf("[MiniSIP-CrashSender] crash report sending failed\n");
		}
		pclose(stream);
		return 0;
	}
		
	if(strstr(line, "CANCEL") != NULL){
		pclose(stream);		
		return 0;
	}
	
		
	
}
