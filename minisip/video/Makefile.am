
noinst_LIBRARIES = libvideo.a

AM_CPPFLAGS = @CPPFLAGS@ @AVCODEC_CPPFLAGS@

display_src = display/VideoDisplay.cxx
if XV_VIDEO_DISPLAY
  display_src += display/XvDisplay.cxx
endif

if SDL_VIDEO_DISPLAY
  display_src += display/SdlDisplay.cxx
endif

display_hdr = display/SdlDisplay.h display/XvDisplay.h display/VideoDisplay.h


grabber_src = grabber/Grabber.cxx

if V4L_VIDEO_GRABBER
  grabber_src += grabber/V4LGrabber.cxx
endif

if DC1394_VIDEO_GRABBER
  grabber_src += grabber/Dc1394Grabber.cxx
endif

grabber_hdr = grabber/V4LGrabber.h grabber/Dc1394Grabber.h grabber/Grabber.h

mixer_src = mixer/ImageMixer.cxx

mixer_hdr = mixer/ImageMixer.h

codec_src = codec/AVCoder.cxx codec/AVDecoder.cxx codec/VideoCodec.cxx

codec_hdr = codec/AVCoder.h codec/AVDecoder.h codec/VideoCodec.h codec/VideoEncoderCallback.h

libvideo_a_SOURCES = $(display_src) \
                     $(grabber_src) \
                     $(codec_src) \
		     $(mixer_src) \
		     VideoMedia.cxx

noinst_HEADERS =  ImageHandler.h \
                  VideoMedia.h \
                  $(display_hdr) $(grabber_hdr) $(codec_hdr) $(mixer_hdr)



