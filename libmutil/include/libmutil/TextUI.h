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

#include <libmutil/libmutil_config.h>

#include<libmutil/CommandString.h>
#include<libmutil/minilist.h>
#include<libmutil/dbg.h>

class LIBMUTIL_API TextUICompletionCallback{
	public:
		virtual ~TextUICompletionCallback() {}
		virtual minilist<string> textuiCompletionSuggestion(string match)=0;
	
};

struct completion_cb_item{
	std::string match;
	TextUICompletionCallback *callback;
	
};


/**
 * General purpose text ui implementation with 
 * auto-complete functionality.
 *
 * Extend this class to generate a user interface. Implement 
 * the TextUICompletionsCallback to support "dynamic" auto-complete.
 * For normal auto-complete with a fixed set of commands, register
 * the commands with the addCommand method.
 *
 * You must implement the guiExecute method to interpret the user
 * input.
*/
class LIBMUTIL_API TextUI : public DbgHandler{
public:
	///Colors that can be used when displaying text
	static const int plain;
	static const int bold;
	static const int red;
	static const int blue;
	static const int green;


	TextUI();
	virtual ~TextUI();

	/**
	 * Method to be implemented by a sub-class.
	 * @param cmd	Command entered by the user (single line)
	 */
	virtual void guiExecute(string cmd)=0;
	
	/**
	 * @param msg	String to be displayed on the screen
	 * @param style	Color/bold/plain
	 */
	virtual void displayMessage(string msg, int style=-1);

	/**
	 * Blocks and runs the gui
	 */
	virtual void guimain();

	/**
	 * Set the prompt of the UI
	 */
	void setPrompt(string p);

	/**
	 * Add a string to the list of auto-complete commands.
	 * @param c 	String that the TextUI will use to auto-complete
	 */
	void addCommand(string c);

	/**
	 * A fixed set of strings to auto-complete with is not
	 * enough in all situations. Therefore the TextUI supports
	 * callbacks to retreive strings depending on what the
	 * user has already written.
	 */
	void addCompletionCallback(string match, TextUICompletionCallback *cb);

protected:
	///The maximum number of hints that will be output to the screen.
	//The default is 2000.
	int maxHints;

	/**
	 * If the sub-class wants to react on individual key presses,
	 * implement the keyPressed method.
	 */
	virtual void keyPressed(int ){}; /// key pressed is the argument.

	/**
	 * Whether or not the main loop should continue on next iteration
	 */
	bool running;

private:

	/**
	 * Store the current state of the console to "terminalSavedState"
	 * and make it non-blocking.
	 */
	int makeStdinNonblocking();

	/**
	 * Restores state saved by makeStdinNonblocking.
	 */
	void restoreStdinBlocking();
	void *terminalSavedState; //Linux: stores state of console before
				  //making it non-blocking.
	
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
