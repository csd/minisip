noinst_LIBRARIES = libsoundcard.a

if HAVE_LIBASOUND
libsoundcard_a_SOURCES = SilenceSensor.cxx SoundIO.cxx AlsaCard.cxx SoundDevice.cxx AlsaSoundDevice.cxx OssSoundDevice.cxx FileSoundDevice.cxx FileSoundSource.cxx
else !HAVE_LIBASOUND 
if DIRECT_SOUND
libsoundcard_a_SOURCES = SilenceSensor.cxx SoundIO.cxx SoundDevice.cxx DirectSoundDevice.cxx FileSoundDevice.cxx FileSoundSource.cxx
else !DIRECT_SOUND
libsoundcard_a_SOURCES = SilenceSensor.cxx SoundIO.cxx SoundDevice.cxx OssSoundDevice.cxx FileSoundDevice.cxx FileSoundSource.cxx
endif !DIRECT_SOUND
endif !HAVE_LIBASOUND

noinst_HEADERS  = SoundIO.h \
		  SoundDevice.h \
		  SilenceSensor.h \
		  OssSoundDevice.h \
		  AlsaSoundDevice.h \
		  DirectSoundDevice.h \
		  FileSoundDevice.h \
                  SoundIOPLCInterface.h \
                  SoundRecorderCallback.h \
                  FileSoundSource.h \
		  AlsaCard.h

duplex: duplex.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DDEBUG_OUTPUT -o duplex duplex.cxx libsoundcard.a $(LDFLAGS) $(LIBS) -lmutil

tone: tone.cxx libsoundcard.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -DDEBUG_OUTPUT -o tone tone.cxx libsoundcard.a $(LDFLAGS) $(LIBS) $(INCLUDES) -lmutil
