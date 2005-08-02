noinst_LIBRARIES = libsoundcard.a

RESAMPLER_SRC = resampler/Resampler.cxx \
		resampler/SimpleResampler.cxx
if FLOAT_RESAMPLER
RESAMPLER_SRC += resampler/FloatResampler.cxx
endif

RESAMPLER_HDR = resampler/Resampler.h \
                resampler/FloatResampler.h \
		resampler/SimpleResampler.h

AUDIOMIXER_SRC = AudioMixer.cxx \
		AudioMixerSimple.cxx \
		AudioMixerSpatial.cxx

AUDIOMIXER_HDR = AudioMixer.h \
		AudioMixerSimple.h \
		AudioMixerSpatial.h


BASIC_SOUNDCARD_SRC = SilenceSensor.cxx \
		SoundIO.cxx \
		SoundSource.cxx \
		${RESAMPLER_SRC} \
		${AUDIOMIXER_SRC} \
		FileSoundDevice.cxx \
		FileSoundSource.cxx \
		SoundDevice.cxx 
		
if HAVE_LIBASOUND

libsoundcard_a_SOURCES = ${BASIC_SOUNDCARD_SRC} \
			OssSoundDevice.cxx \
			AlsaCard.cxx \
			AlsaSoundDevice.cxx 

else !HAVE_LIBASOUND 

if DIRECT_SOUND
libsoundcard_a_SOURCES = ${BASIC_SOUNDCARD_SRC} \
			DirectSoundDevice.cxx 
else !DIRECT_SOUND
libsoundcard_a_SOURCES = ${BASIC_SOUNDCARD_SRC} \
			OssSoundDevice.cxx 
endif !DIRECT_SOUND

endif !HAVE_LIBASOUND

noinst_HEADERS  = SoundIO.h \
		  SoundDevice.h \
		  SoundSource.h \
		  SilenceSensor.h \
		  OssSoundDevice.h \
		  AlsaSoundDevice.h \
		  DirectSoundDevice.h \
		  FileSoundDevice.h \
		  SoundIOPLCInterface.h \
		  SoundRecorderCallback.h \
		  FileSoundSource.h \
		  AlsaCard.h \
		  ${RESAMPLER_HDR}
		  ${AUDIOMIXER_SRC}


duplex: duplex.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o duplex duplex.cxx libsoundcard.a ../spaudio/libspaudio.a $(LDFLAGS) $(LIBS) -lmutil

tone: tone.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DDEBUG_OUTPUT -o tone tone.cxx libsoundcard.a $(LDFLAGS) $(LIBS) $(INCLUDES) -lmutil
