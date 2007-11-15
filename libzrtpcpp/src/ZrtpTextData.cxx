/*
  Copyright (C) 2006 Werner Dittmann

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Boston, MA 02111.
*/

/*
 * Authors: Werner Dittmann <Werner.Dittmann@t-online.de>
 */
#include <stdint.h>
/**
 *
 */
const char *clientId = "minisip        "; // must be 15 chars
const char *zrtpVersion = "0.02";	// must be 4 chars

/**
 *
 */
const char *HelloMsg =    "Hello   ";
const char *HelloAckMsg = "HelloACK";
const char *CommitMsg =   "Commit  ";
const char *DHPart1Msg =  "DHPart1 ";
const char *DHPart2Msg =  "DHPart2 ";
const char *Confirm1Msg = "Confirm1";
const char *Confirm2Msg = "Confirm2";
const char *Conf2AckMsg = "Conf2ACK";
const char *ErrorMsg =    "Error   ";
const char *GoClearMsg =  "GoClear ";
const char *ClearAckMsg = "ClearACK";

/**
 *
 */
const char *responder = "Responder";
const char *initiator = "Initiator";
const char *iniMasterKey = "Initiator SRTP master key";
const char *iniMasterSalt = "Initiator SRTP master salt";
const char *respMasterKey = "Responder SRTP master key";
const char *respMasterSalt = "Responder SRTP master salt";

const char *hmacKey = "HMAC key";
const char *retainedSec = "retained secret";
const char *knownPlain = "known plaintext";

const char *sasString = "Short Authentication String";

/**
 * The arrays are sorted: the most secure / best algorithm is first in the
 * array
 */
const char *supportedHashes[] = { "SHA256  ",
				  "        ",
				  "        ",
				  "        ",
				  "        " };

const char *supportedCipher[] = { "AES256  ",
				  "AES128  ",
				  "        ",
				  "        ",
				  "        " };

const char *supportedPubKey[] = { "DH4096  ",
				  "DH3072  ",
				  "        ",
				  "        ",
				  "        " };

const char *supportedSASType[] = { "libase32",
				   "        ",
				   "        ",
				   "        ",
				   "        " };

const char *supportedAuthLen[] = { "80      ",
	                           "32      ",
        	                   "        ",
                	           "        ",
                        	   "        " };

