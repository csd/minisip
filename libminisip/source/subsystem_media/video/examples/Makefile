#view: ../SdlDisplay.o ../VideoDisplay.o ../V4LGrabber.o ../Dc1394Grabber.o ../Grabber.o ../ImageMixer.o view.cxx
#	g++ -L/usr/X11R6/lib -o view view.cxx ../SdlDisplay.o ../VideoDisplay.o ../V4LGrabber.o  ../Dc1394Grabber.o ../Grabber.o ../ImageMixer.o -lSDL -lX11 -lssl -lmutil -ldc1394_control -lraw1394

view: ../XvDisplay.o ../VideoDisplay.o ../V4LGrabber.o ../Dc1394Grabber.o ../Grabber.o ../ImageMixer.o view.cxx
	g++ -L/usr/X11R6/lib -o view view.cxx ../XvDisplay.o ../VideoDisplay.o ../V4LGrabber.o  ../Dc1394Grabber.o ../Grabber.o ../ImageMixer.o -lXv -lX11 -lssl -lmutil -ldc1394_control -lraw1394 -lpthread


yuv_file: ../V4LGrabber.o ../Dc1394Grabber.o ../Grabber.o yuv_file.cxx
	g++ -o yuv_file yuv_file.cxx ../V4LGrabber.o  ../Dc1394Grabber.o ../Grabber.o -lssl -lmutil -ldc1394_control -lraw1394 -lpthread
