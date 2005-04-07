noinst_LIBRARIES = libsoundcard.a

RESAMPLER_SRC = resampler/Resampler.cxx \
		resampler/SimpleResampler.cxx
if FLOAT_RESAMPLER
RESAMPLER_SRC += resampler/FloatResampler.cxx
endif

RESAMPLER_HDR = resampler/Resampler.h \
                resampler/FloatResampler.h \
		resampler/SimpleResampler.h

if HAVE_LIBASOUND
libsoundcard_a_SOURCES = SilenceSensor.cxx SoundIO.cxx SoundSource.cxx ${RESAMPLER_SRC} AlsaCard.cxx SoundDevice.cxx AlsaSoundDevice.cxx OssSoundDevice.cxx FileSoundDevice.cxx FileSoundSource.cxx
else !HAVE_LIBASOUND 
if DIRECT_SOUND
libsoundcard_a_SOURCES = SilenceSensor.cxx SoundIO.cxx SoundSource.cxx ${RESAMPLER_SRC} SoundDevice.cxx DirectSoundDevice.cxx FileSoundDevice.cxx FileSoundSource.cxx
else !DIRECT_SOUND
libsoundcard_a_SOURCES = SilenceSensor.cxx SoundIO.cxx SoundSource.cxx ${RESAMPLER_SRC} SoundDevice.cxx OssSoundDevice.cxx FileSoundDevice.cxx FileSoundSource.cxx
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

duplex: duplex.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o duplex duplex.cxx libsoundcard.a ../spaudio/libspaudio.a $(LDFLAGS) $(LIBS) -lmutil

tone: tone.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DDEBUG_OUTPUT -o tone tone.cxx libsoundcard.a $(LDFLAGS) $(LIBS) $(INCLUDES) -lmutil
