#include <gtk/gtk.h>
#include "CrashSender.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<time.h>
#include<dirent.h>
#include<sys/time.h>

using namespace std;

class CrashSender;

class CrashSenderGUI {
	private:
		GtkWidget *frame_window;
		GtkWidget *label;
		GtkWidget *content_area;
		GtkWidget *button;

	public:
		CrashSenderGUI(int argc, char *argv[]) : frame_window(NULL), label(NULL) {
			gtk_init(&argc, &argv);
			frame_window = gtk_dialog_new_with_buttons ("MiniSIP Report",
					NULL,
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_STOCK_YES,
					GTK_RESPONSE_YES,
					GTK_STOCK_NO,
					GTK_RESPONSE_NO,
					NULL);

			content_area = gtk_dialog_get_content_area (GTK_DIALOG (frame_window));
			label = gtk_label_new ("Do you want to send the Crash Report?");
			gtk_container_add (GTK_CONTAINER (content_area), label);
   			gtk_widget_show_all (frame_window);
			
			gint result = gtk_dialog_run (GTK_DIALOG (frame_window));	
			CrashSender crashSender;

			switch (result){
				case GTK_RESPONSE_YES:
	                                if(crashSender.send()){
               	                        	printf("[MiniSIP-CrashSender] crash report sent\n");
                               		}
					break;
				case GTK_RESPONSE_NO:
					printf("No");
					break;
			}
			gtk_widget_destroy (frame_window);
		}
	};
	
 
	int main (int argc, char *argv[]) {
		struct dirent **files;
		bool filesExist = false;
		int count, i;
		std::string crashDirectoryPath =  string(getenv("HOME"))+"/.minisip/crash_reports";

                count = scandir(crashDirectoryPath.c_str(), &files, 0, alphasort);

                if (count >= 0) {
                	if (count == 0) {
                        	cerr << "No crash reports to be sent" << endl;
                        } else {
                                for (i = 1; i < count + 1; ++i) {
                        	        //Reads the files with .report extension
                                        std::string fileName = string(files[i - 1]->d_name);
                                        std::string extension = ".report";
                                        size_t pos = fileName.find(".");

                                        if (strcmp(fileName.substr(pos).c_str(),
                	                                          extension.c_str()) == 0) {
 						filesExist = true;
						break; 
					}
				}
			}
		}
		
		if(filesExist){
			CrashSenderGUI crash_sender_gui(argc, argv);
                        return 0;
		}
	}
