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
                                                                                         
/* Copyright (C) 2004
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/


#ifndef _FILESOUNDSOURCE_H
#define _FILESOUNDSOURCE_H

#include"SoundIO.h"




/**
 *
 * This can be used to play audio messages, ringing tones etc,
 *
 * Note: The current implementation does not support stereo.
 */
class FileSoundSource : public SoundSource{
    public:
        /**
         * 
         * @arg filename        Name of a file containing raw audio
         *                      samples (16 bit, mono).
         * @arg repeat          If true the audio buffer will be played
         *                      over and over again, if false only silence
         *                      will be returned by the sound source after
         *                      all samples have been played. The default
         *                      is false.
	 * @arg id              SoundSource id     
         */
        FileSoundSource(string filename, uint32_t id, bool repeat=false);


        /**
         * 
         * @arg raw_audio       Pointer to the mono audio data - note that 
         *                      the pointer will be deleted when the
         *                      FileSoundSource is deleted.
         * @arg samples         Number of samples - this should be half of
         *                      the number of bytes in the audio buffer.
         * @arg repeat          If true the audio buffer will be played
         *                      over and over again, if false only silence
         *                      will be returned by the sound source after
         *                      all samples have been played. The default
         *                      is false.
         */
        FileSoundSource(short *raw_audio, int samples, bool repeat=false);

        /**
         * Note that if a audio buffer was given to the source when it was
         * created this buffer will be deleted.
         */
        ~FileSoundSource();

        /**
         * Until the FileSoundSource has been enabled, only silence will be
         * returned.
         */
        void enable();

        /**
         * Disabling the sound source stops the playing of the sound and 
         * once it is enabled again the audio will be played from the
         * beginning.
         */
        void disable();

        /**
         * This method is illegal to use for this subclass (since it has
         * all audio data from when the object was created). 
         * If it is called it will be ignored and if debug output is
         * enabled then an error message is written to standard error.
         * 
         */
        virtual void pushSound(short *samples,
                                int32_t nSamples,
                                int32_t index,
                                bool isStereo=false);
        
        /**
         * This method is required by the SoundSource super class and is used by the
         * sound card to get audio data to be output.
         *
         * This method will return the audio data in the internal audio buffer
         * and when the end of the buffer is reached it will start playing
         * from the beginning if the <i>repeat</i> attribute is set.
         */
        virtual void getSound(short *dest,
                                int32_t nMono,
                                bool stereo,
                                bool dequeue=true);

    private:
        short *audio;           /// Pointer to raw audio data.
        uint32_t nSamples;         /// Number of samples in audio buffer.
        bool enabled;           /// true->playing, false->silence
        bool repeat;            /// Play the audio in a audio.
        int index;              /// Play out position
};


#endif
