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


#ifndef _TEXTUI_H
#define _TEXTUI_H

#ifdef _MSC_VER
#ifdef LIBMUTIL_EXPORTS
#define LIBMUTIL_API __declspec(dllexport)
#else
#define LIBMUTIL_API __declspec(dllimport)
#endif
#else
#define LIBMUTIL_API
#endif


#include<libmutil/CommandString.h>
#include<libmutil/minilist.h>

/**
 *
 *
 *
 * 
*/

class LIBMUTIL_API TextUICompletionCallback{
public:
	virtual minilist<string> textuiCompletionSuggestion(string match)=0;
	
};

struct completion_cb_item{
	std::string match;
	TextUICompletionCallback *callback;
	
};

class LIBMUTIL_API TextUI{
public:
	static const int plain;
	static const int bold;
	static const int red;
	static const int blue;
	static const int green;


	TextUI();
	virtual ~TextUI(){};

	virtual void guiExecute(string cmd)=0;
	
	virtual void displayMessage(string msg, int style=-1);

	void guimain();

	void setPrompt(string p);

	void addCommand(string c);
	void addCompletionCallback(string match, TextUICompletionCallback *cb);

protected:
	///The maximum number of hints that will be output to the screen.
	//The default is 2000.
	int maxHints;
	virtual void keyPressed(int ){}; /// key pressed is the argument.
private:
	void outputSuggestions(minilist<string> &l);
	string displaySuggestions(string);

	string input;

	///Formating codes that will not take up any screen columns when printed
	int promptFormat;

	string promptText; ///The text that is displayed before the "$".

	///List of known commands
	minilist<string> commands;

	///List of commands that can be further completed by asking a callback
	minilist<struct completion_cb_item> completionCallbacks;
};

#endif
