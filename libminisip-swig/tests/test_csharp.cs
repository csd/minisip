/*
 Copyright (C) 2006-2007  Mikael Magnusson

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Library General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* Copyright (C) 2006-2007
 *
 * Authors: Mikael Magnusson <mikma@users.sourceforge.net>
 */

using System;
using System.Reflection;
using System.Threading;
using System.Runtime.InteropServices;
using Minisip;

public class MyDbgHandler : DbgHandler {
  public delegate void displayMessageDelegate(string output, int style);
  private string prefix;

  private displayMessageDelegate displayMessageMethod;

  public MyDbgHandler(displayMessageDelegate method, string prefix) {
    displayMessageMethod = method;
    this.prefix = prefix;
  }

  protected override void displayMessage(string output, int style) {
    if ( output != "" )
      displayMessageMethod(prefix + style + " " + output, style);
  }
}

public class MyGui : GuiProxy {
//   private DbgBuf outBuf;
//   private streambuf oldOutBuf;

//   private DbgBuf errBuf;
//   private streambuf oldErrBuf;
  private string callId;

  //  private SipSoftPhoneConfigurationRef config;

  public MyGui() {
    Console.WriteLine("MyGui");

//     Dbg.getOut().setEnabled(true);
//     Dbg.getErr().setEnabled(true);
    //Dbg.getDbg().setEnabled(true);

//     Dbg.getOut().setExternalHandler(new MyDbgHandler(displayMessage, "mout"));
//     Dbg.getErr().setExternalHandler(new MyDbgHandler(displayMessage, "merr"));
//     Dbg.getDbg().setExternalHandler(new MyDbgHandler(displayMessage, "mdbg"));

//     outBuf = new DbgBuf(new MyDbgHandler(displayMessage, "out"));
//     oldOutBuf = MinisipModule.setOutputBuf(outBuf);
    
//     errBuf = new DbgBuf(new MyDbgHandler(displayMessage, "err"));
//     oldErrBuf = MinisipModule.setErrorBuf(errBuf);
  }

  public void stopStreams() {
    Console.WriteLine("MyGui stop");
//     MinisipModule.setOutputBuf(oldOutBuf);
//     MinisipModule.setErrorBuf(oldErrBuf);
  }

  public override void run() {
    for(;;){
      //Console.WriteLine("Before waitCommand");
      CommandString cmd = waitCommand();
      //Console.WriteLine("After waitCommand");
      if( cmd.getOp() == "stop" ){
	//Console.Writ	eLine("Return thread");
	return;
      }
      handleCommand( cmd );
    }
  }

  protected virtual void displayMessage(string output, int style) {
    //System.Windows.Forms.MessageBox.Show(output);
    Console.Write(output);
  }

  public override void setSipSoftPhoneConfiguration(SipSoftPhoneConfigurationRef config) {
    if( config.isNull() ) {
      //Console.WriteLine("setSipSoftPhoneConfiguration NULL!");
    } else {
      Console.WriteLine("setSipSoftPhoneConfiguration");
      if( !config.pstnIdentity.isNull() )
	dumpIdentity(config.pstnIdentity);
      dumpIdentities(config.identities);
    }
  }

  public override void handleCommand(CommandString command) {
    switch( command.getOp() ){
    case "incoming_available": {
      callId = command.get("destination_id");
      Console.WriteLine("Incoming call!: " + callId);
      CommandString cmd = new CommandString(callId, SipCommandString.accept_invite, "");
      sendCommand("sip", cmd);
      break;
    }
    case "remote_cancelled_invite":
      Console.WriteLine("Missed call!");
      break;
    case "invite_ok": {
      CommandString cmd = new CommandString(callId, MediaCommandString.set_session_sound_settings, "senders", "ON");
      sendCommand("media", cmd);

      Console.WriteLine("Activated media");
      break;
    }
    case "remote_hang_up":
      Console.WriteLine("Hangup");
      break;
    default:
      Console.WriteLine("My handleCommand: " + command.getString());
      break;
    }
  }

  public override bool configDialog(SipSoftPhoneConfigurationRef conf) {
    Console.WriteLine("configDialog");
    return true;
  }

  private void dumpIdentity(SipIdentityRef id){
    Console.WriteLine("id     " + id.identityIdentifier);
    Console.WriteLine("id:    " + id.getId());
    Console.WriteLine("uri:   " + id.getSipUri().getString());
//     Console.WriteLine("sec    " + id.securitySupport);
    Console.WriteLine("reg    " + id.registerToProxy);
    Console.WriteLine("reg?   " + id.isRegistered());

    dumpRoute(id.getRouteSet());
  }

  private void dumpIdentities(SipIdentityList list) {
    foreach (SipIdentityRef id in list) {
      dumpIdentity(id);
    }

//     System.Collections.IEnumerator ien = list.GetEnumerator();
//     ien.Reset();
//     while(ien.MoveNext()) {
//       SipIdentityRef id = (SipIdentityRef)ien.Current;
//       dumpIdentity(id);
//     }
  }

  private void dumpRoute(SipUriList routeSet){
    foreach (SipUri route in routeSet) {
      Console.WriteLine("route:      " + route.getString());
    }
  }

  public override void setContactDb(ContactDbRef contactDb) {
    Console.WriteLine("setContactDb");    
  }

  public void sendInvite(string uri){
    CommandString invite = new CommandString("", SipCommandString.invite, uri);
    CommandString resp = getCallback().handleCommandResp("sip", invite);
    Console.WriteLine("Calling: " + resp.getString());
    callId = resp.get("destination_id");
  }
}


public class runme {
  static SWIGTYPE_p_p_char getArgv(string[] args, out int length) {
    SWIGTYPE_p_p_char argv = MinisipModule.new_CStrings(args.Length+2);
    MinisipModule.CStrings_setitem(argv, 0, Assembly.GetExecutingAssembly().Location);
    for (int i = 0; i < args.Length; i++){
      Console.WriteLine("String: " + i + " = " + args[i]);
      MinisipModule.CStrings_setitem(argv, i + 1, args[i]);
    }

    MinisipModule.CStrings_setitem(argv, args.Length + 1, null);
    length = args.Length + 2;
    return argv;
  }

  static void Main(string[] args) {
    MinisipModule.setupDefaultSignalHandling();

    Console.WriteLine("MiniSIP C# GUI ... welcome!");

    MyGui gui = new MyGui();
    GuiRef guiref = new GuiRef(gui);
    GC.SuppressFinalize(gui);

    ThreadStart job = new ThreadStart( gui.run );
    Thread thread = new Thread( job );
    thread.Start();

    int length;
    SWIGTYPE_p_p_char argv = getArgv(args, out length);
    MinisipMain sip = new MinisipMain(guiref, length, argv);
    

    if( sip.startSip() <= 0 ){
      return;
    }

    Console.WriteLine("SIP started");

//     CommandString register;
//     register = new CommandString("", SipCommandString.proxy_register);
//     register.setKey("proxy_domain", "fowley.hem.za.org");
//     cb.handleCommand("sip", register);

//     gui.sendInvite("sip:xxx@yyy");
    
    //gui.run();

    Console.WriteLine("<<< Press ENTER to quit >>>");
    Console.ReadLine();

    gui.stopStreams();
    gui.stop();
    thread.Join();
    sip.exit();
    Console.WriteLine("Bye");
    return;
  }
}
