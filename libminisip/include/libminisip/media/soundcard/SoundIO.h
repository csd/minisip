/*
 Copyright (C) 2004-2006 the Minisip Team
 
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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#ifndef SOUNDIO_H
#define SOUNDIO_H

#include<libminisip/libminisip_config.h>


#include<libminisip/media/soundcard/SoundRecorderCallback.h>
#include<libminisip/media/soundcard/AudioMixer.h>
class SoundIOPLCInterface;

#include<libmutil/Mutex.h>
#include<libmutil/CondVar.h>

#include<list>
#include<string>

class SoundSource;

	//we need sounddevice.h for some defines
#include<libminisip/media/soundcard/SoundDevice.h>
//class SoundDevice;


/*             v                               <--- Play out point
 *   ........................................  <--- Audio buffer for one ssrc
 *             xxxxxxxxxxxxxxxxxxxxx           <--- Audio data in buffer
 *   
 *                           v
 *   ........................................
 *   xxxx                    xxxxxxxxxxxxxxxx
 * 
*/


class LIBMINISIP_API RecorderReceiver{
	public:
		RecorderReceiver(SoundRecorderCallback *, bool stereo);
		bool getStereo();
		SoundRecorderCallback *getCallback();
	private:
		SoundRecorderCallback *callback;
		bool stereo;
};


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
class LIBMINISIP_API SoundIO : public MObject{

	public:
		/**
		 * Constructor used to create an instance that is a interface 
                 * to a soundcard.
		 * @param device Device to use - for example
                 * AlsaSoundDevice or OSSSoundDevice
		 * @param mixerType string identifying the type of mixer we want
		 * 	(spatial or simple).
		 * @param speed Frequency to use for both recording and playing.
		 * 		Defaults to 8kHz.
		 * @param format format of the audio samples (signed/unsigned, 
		 * 		bytes per sample, endiannes). See SoundDevice.h
		 * 		for a definition of valid types.
		 */
		SoundIO(MRef<SoundDevice *> inputDevice, 
			MRef<SoundDevice *> outputDevice,
			 std::string mixerType,
                        int nChannels=2, 
                        int32_t speed=8000, 
                        int format=SOUND_S16LE);

		/**
		 * Destructor
		 */
		~SoundIO();
		
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
						
		void unregisterRecorderReceiver( SoundRecorderCallback *callback );
						
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
	//	void registerSource(int sourceId, 
        //                            SoundIOPLCInterface *plc=NULL);

		void registerSource(MRef<SoundSource *> source);

		void unregisterSource(int sourceId);

		/**
		 * Starts thread that takes data from queue 
                 * and sends to soundcard.
		 */
		void start_sound_player();

		void read_from_card(short *buf, int32_t n_samples);
		 
		MRef<SoundSource *> getSoundSource(int32_t id);

		virtual std::string getMemObjectType() const {return "SoundIO";};
		
		/**
		Access the mixer.
		FIXME: this function needs to be made thread-safe is we intend
		to be able to change the mixer on the fly ...
		*/
		MRef< AudioMixer *> getMixer();
		
		/**
		Given a string, create the mixer type requested.
		If not understood, the Spatial Audio mixer is created.
		*/
		bool setMixer(  std::string type );

	private:

		void send_to_card(short *buf, int32_t n_samples);

		void cycle_sound_buffers();
		
		//internal funcs
		void init_dsp();
		void init_fileio();
			
		static void *recorderLoop(void *);

		static void *playerLoop(void *);
		
		/**
		An audio mixer object.
		Use the config file to specify your desired mixer type
		(see AudioMixerXXX in soundcard/ folder).
		The playerLoop() function runs in a thread, which whenever
		there is available audio from sources, calls the mixer to 
		perform a mix with the audio from the sources. 
		You can use minisip's standard mixers, or you can implement
		your own one.
		*/
		MRef< AudioMixer *> mixer;

		CondVar sourceListCond;

		std::list<MRef<SoundSource *> > sources;
		std::list<RecorderReceiver *> recorder_callbacks;

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

		MRef<SoundDevice *> soundDevIn;
		MRef<SoundDevice *> soundDevOut;
		//AEC aec;

};

#endif
