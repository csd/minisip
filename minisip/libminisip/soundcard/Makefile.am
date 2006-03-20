noinst_LTLIBRARIES = libsoundcard.la

libsoundcard_la_LIBADD =

RESAMPLER_SRC = resampler/Resampler.cxx \
		resampler/SimpleResampler.cxx
if FLOAT_RESAMPLER
RESAMPLER_SRC += resampler/FloatResampler.cxx
libsoundcard_la_LIBADD += $(SAMPLERATE_LIBS)
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

ALSA_SRC =	AlsaCard.cxx \
		AlsaSoundDevice.cxx 

DSOUND_SRC = 	DirectSoundDevice.cxx 

PORTAUDIO_SRC = PortAudioDevice.h	\
		PortAudioDevice.cxx

libsoundcard_la_SOURCES = $(BASIC_SOUNDCARD_SRC)

if !OS_WIN
libsoundcard_la_SOURCES += OssSoundDevice.cxx 
endif !OS_WIN

if HAVE_LIBASOUND
libsoundcard_la_SOURCES += $(ALSA_SRC)
libsoundcard_la_LIBADD += $(ALSA_LIBS)
endif HAVE_LIBASOUND

if DIRECT_SOUND
libsoundcard_la_SOURCES += $(DSOUND_SRC)
libsoundcard_la_LIBADD += $(DSOUND_LIBS)
endif DIRECT_SOUND

if PORTAUDIO_SUPPORT
libsoundcard_la_SOURCES += $(PORTAUDIO_SRC)
libsoundcard_la_LIBADD += $(PORTAUDIO_LIBS)
endif PORTAUDIO_SUPPORT

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
		  ${RESAMPLER_HDR} \
		  ${AUDIOMIXER_HDR}


duplex: duplex.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o duplex duplex.cxx libsoundcard.a ../spaudio/libspaudio.a $(LDFLAGS) $(LIBS) -lmutil

tone: tone.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DDEBUG_OUTPUT -o tone tone.cxx libsoundcard.a $(LDFLAGS) $(LIBS) $(INCLUDES) -lmutil
