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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#ifndef SOUNDIO_H
#define SOUNDIO_H

//#define HAVE_LIBASOUND


#include"SoundDevice.h"

#ifndef WIN32

#include"OssSoundDevice.h"
#ifdef HAVE_LIBASOUND
#include"AlsaSoundDevice.h"
#endif

#else
#include"DirectSoundDevice.h"
#endif

#include"SoundRecorderCallback.h"
#include"SoundIOPLCInterface.h"
#include<libmutil/Mutex.h>
#include<libmutil/CondVar.h>
#include<libmutil/MemObject.h>

#include<iostream>
#include<list>
#include"../spaudio/SpAudio.h"

#include"SoundSource.h"


#ifdef HAVE_LIBASOUND
#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#include <alsa/asoundlib.h>
#endif

using namespace std;

/**
 * Minimal API for recording and playing samples from/to a soundcard.
 * The SoundCard class is a interface to a sound card device
 * in the "/dev" directory (typically "/dev/dsp"). It uses a
 * small output buffer and can therefore be used in VoIP
 * applications.
 * If recording is wanted a function can be registred as callback
 * when data is ready.
 * WARNING: The classes have only been tested with 160 samples/buffer, 16bit
 * @author Erik Eliasson eliasson@it.kth.se
 * @version 0.01
*/

/*             v                               <--- Play out point
 *   ........................................  <--- Audio buffer for one ssrc
 *             xxxxxxxxxxxxxxxxxxxxx           <--- Audio data in buffer
 *   
 *                           v
 *   ........................................
 *   xxxx                    xxxxxxxxxxxxxxxx
 * 
*/


class RecorderReceiver{
	public:
		RecorderReceiver(SoundRecorderCallback *, bool stereo);
		bool getStereo();
		SoundRecorderCallback *getCallback();
	private:
		SoundRecorderCallback *callback;
		bool stereo;
};



class SoundIO : public MObject{

	public:
		/**
		 * Constructor used to create an instance that is a interface 
                 * to a soundcard.
		 * @param device Device to use - for example
                 * AlsaSoundDevice or OSSSoundDevice
		 * @param speed Frequency to use for both recording and playing.
		 * 		Defaults to 8kHz.
		 */
		SoundIO(MRef<SoundDevice *>device, 
                        int nChannels=2, 
                        int32_t speed=8000, 
                        int format=SOUND_S16LE);
		
		/**
		 * Initializes the soundcard.
		 */
		void openPlayback();
		void openRecord();

		void startRecord();

		/**
		 * Closes the soundcard
		 */
		void closePlayback();
		void closeRecord();

		void stopRecord();
		
		/// Waits until the soundcard's internal buffers are empty.
		void sync();
		
		/**
		 *  Outputs a tone to the speakers.
		 *  @param secs Number of seconds to play the sound. Defaults 
                 *          to one second.
		 */
		void play_testtone(int secs=1);
		

		static void fileIOTimeout(int);
		
		/**
		 * The registred receiver will receive a buffer
		 * with samples read from the soundcard when ever 
                 * it is available.
		 * @param func	Function that should receive the recorded 
                 *              sample.
		*/
		void register_recorder_receiver(SoundRecorderCallback *callback,
                                                int32_t nrsamples, 
                                                bool stereo);
		
		/**
		 * Starts the thread that "polls" the soundcard for data
		 * and executes the registred receiver function
		*/
		void start_recorder();

		void set_recorder_buffer_size(int32_t bs);
		/**
		 * Pushes a buffer of 160 bytes to the souncard output queue.
		 * @param buf Pointer to a buffer with sound samples.
		 */
		void registerSource(int sourceId, 
                                    SoundIOPLCInterface *plc=NULL);

		void registerSource(MRef<SoundSource *> source);
                
		void unRegisterSource(int sourceId);
                
		void pushSound(int sourceId, 
                                short *buf, 
                                int32_t nMonoSamples, 
                                int index, 
                                bool stereo=false);
		
		/**
		 * Starts thread that takes data from queue 
                 * and sends to soundcard.
		 */
		void start_sound_player();

		void read_from_card(short *buf, int32_t n_samples);
		 
		MRef<SoundSource *> getSoundSource(int32_t id);

		std::string getDevice(){return soundDev?soundDev->dev:"";};

		virtual std::string getMemObjectType(){return "SoundIO";};
	private:

		void initLookup();
		
		void send_to_card(short *buf, int32_t n_samples);

		void cycle_sound_buffers();
		
		//internal funcs
		void init_dsp();
		void init_fileio();
			
		static void *recorderLoop(void *);

		static void *playerLoop(void *);
		

		static SpAudio spAudio;


		CondVar sourceListCond;
		list<MRef<SoundSource *> > sources;
		list<RecorderReceiver *> recorder_callbacks;
		CondVar recorderCond;
		volatile int32_t recorder_buffer_size;
		
		
		//synchronization of pushing data to buffers vs sending 
                //from buffers to card
		
                //pthread_mutex_t queueLock;
                Mutex queueLock;
                
		
		//int openCount;
		//bool duplex;
		int32_t nChannels;
		int32_t samplingRate;
		int32_t format;

		bool recording;

		MRef<SoundDevice *> soundDev;

};

#endif
