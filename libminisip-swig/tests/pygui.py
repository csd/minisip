#!/usr/bin/python
#
# Copyright (C) 2006  Mikael Magnusson
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

# Copyright (C) 2006
#
# Authors: Mikael Magnusson <mikma@users.sourceforge.net>

import minisip
import sys
from threading import Thread

class CommandThread(Thread):
    def __init__(self, gui):
        Thread.__init__(self)
        self.gui = gui;
        
    def run(self):
        self.gui.run()

class MyGui(minisip.GuiProxy):
    def __init__(self):
        minisip.GuiProxy.__init__(self)
        print "MyGui init"

    def __del__(self):
        print "MyGui del"

    def run(self):
        while 1:
#            print("Before waitCommand")
            cmd = self.waitCommand()
#            print("After waitCommand")
            if cmd.getOp() == 'stop':
                print("Return thread")
                return
            self.handleCommand(cmd)

    def setSipSoftPhoneConfiguration(self, sipphoneconfig):
        print "setSipSoftPhoneConfiguration"

    def setContactDb(self, contactdb):
        print "setContactDb"

    def configDialog(self, conf):
        print "configDialog"
        return false

    def sendCommand(self, destinationId, operation,
                    param1="", param2="", param3=""):
        cmd = minisip.CommandString(destinationId, operation,
                                    param1, param2, param3)
        self.getCallback().handleCommand("sip", cmd)

    def handle_register_sent(self, cmd):
        print "handle_register_sent"

    def handle_register_ok(self, cmd):
        print "handle_register_ok"

    def handle_incoming_available(self, cmd):
        print "handle_incoming_available"

        self.sendCommand(cmd.getDestinationId(), 'reject_invite')

    def handle_remote_ringing(self, Cmd):
        print "RINGING"

    def handle_invite_ok(self, Cmd):
        print "ANSWERED"
        sendersOn = minisip.CommandString(self.callId, minisip.MediaCommandString.set_session_sound_settings, "senders", "ON");
        self.getCallback().handleCommand("media", sendersOn)

    def handle_remote_reject(self, Cmd):
        print "REJECTED"

    def handle_remote_hang_up(self, Cmd):
        print "HANGUP"
        
    def handleCommand(self, cmd):
#        print "handleCommand"
        print("Command " + cmd.getString())
        try:
            getattr(self, 'handle_' + cmd.getOp())(cmd)
        except AttributeError:
            print "Unhandled command: " + cmd.getOp()

    def sendInvite(self, uri):
        print("sendInvite " + uri)
        invite = minisip.CommandString("", minisip.SipCommandString.invite, uri)
        resp = self.getCallback().handleCommandResp("sip", invite)
        print("Calling: " + resp.getString())
        self.callId = resp.get("destination_id")

def mainFunc():
    print "Starting MiniSIP PythonUI ... welcome!"

    gui = MyGui()
    guiref = minisip.GuiRef(gui)

    cb = CommandThread(gui)
    cb.start()

    sip = minisip.MinisipMain( guiref, 0, None );

    if( sip.startSip() > 0 ):
        #gui.sendInvite("xxx@yyy")

        #cmd_name = minisip.SipCommandString.proxy_register
        #command = minisip.CommandString("", cmd_name)
        #command.setKey("proxy_domain", "proxy.domain")
        #cb = gui.getCallback()
        
        print("Press ENTER to quit")
        sys.stdin.readline()
        gui.stop()
        cb.join()
        sip.exit()

    else:
        print "ERROR while starting SIP!";

mainFunc()
