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
#include<list>
#include<vector>

class LIBMUTIL_API TextUICompletionCallback{
	public:
		virtual ~TextUICompletionCallback() {}
		virtual minilist<std::string> textuiCompletionSuggestion(std::string match)=0;
	
};

struct completion_cb_item{
	std::string match;
	TextUICompletionCallback *callback;
	
};


/**
 * A set of questions that the user will be asked to answer.
 * When the user has answered all questions the object will
 * be passed back to the GUI super class (guiExecute(...)).
 */
class QuestionDialog : public MObject{
public:
	QuestionDialog():nAnswered(0){}
	std::string questionId;
	std::string questionId2;
	std::vector<std::string> questions;
	std::vector<std::string> answers;
	int nAnswered;
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

	static const int KEY_LEFT;
	static const int KEY_RIGHT;
	static const int KEY_UP;
	static const int KEY_DOWN;


	TextUI();
	virtual ~TextUI();


	/**
	 * Ask the user a set of questions. If the user is 
	 * answering questions already, then this dialog
	 * will be queued.
	 */
	void showQuestionDialog(const MRef<QuestionDialog*> &d);
	
	/**
	 * Method to be implemented by a sub-class.
	 * @param cmd	Command entered by the user (single line)
	 */
	virtual void guiExecute(std::string cmd)=0;
	
	/**
	 * Method called when a dialog has been answered by the user.
	 * The parameter contains a list with the answers to the 
	 * questions.
	 *
	 * The dialog was initiated by calling "showQuestionDialog".
	 *
	 * If multiple dialogs are shown at the same time they are
	 * served (i.e. the user is asked to input answers) in the 
	 * same order as were passed to showQuestionDialog.
	 */
	virtual void guiExecute(const MRef<QuestionDialog*> &questionAnswer)=0;
	
	/**
	 * @param msg	String to be displayed on the screen
	 * @param style	Color/bold/plain
	 */
	virtual void displayMessage(std::string msg, int style=-1);

	/**
	 * Blocks and runs the gui
	 */
	virtual void guimain();

	/**
	 * Set the prompt of the UI
	 */
	void setPrompt(std::string p);

	/**
	 * Add a string to the list of auto-complete commands.
	 * @param c 	String that the TextUI will use to auto-complete
	 */
	void addCommand(std::string c);

	/**
	 * A fixed set of strings to auto-complete with is not
	 * enough in all situations. Therefore the TextUI supports
	 * callbacks to retreive strings depending on what the
	 * user has already written.
	 */
	void addCompletionCallback(std::string match, TextUICompletionCallback *cb);

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

	// Dialog/question management functions
	void askQuestion();
	void answerQuestion(std::string answer);

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
	
	void outputSuggestions(minilist<std::string> &l);
	std::string displaySuggestions(std::string);
	

	//dialogs not yet completely answered. First in queue
	//asked first.
	std::list<MRef<QuestionDialog*> > questionDialogs;
	bool isAskingDialogQuestion;
	
	//the normal prompt is interrupted, and its state is 
	//stored in the following two attributes
	std::string savedPromptText;
	std::string savedInput;


	std::string input;

	///Formating codes that will not take up any screen columns when printed
	int promptFormat;

	std::string promptText; ///The text that is displayed before the "$".

	///List of known commands
	minilist<std::string> commands;

	///List of commands that can be further completed by asking a callback
	minilist<struct completion_cb_item> completionCallbacks;
};

#endif
