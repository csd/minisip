OTHER_FILES =

if HAVE_EVP_SHA256
OTHER_FILES += hmac256.h
endif

pkginclude_HEADERS = \
		aes.h \
		cert.h \
		DH.h \
		hmac.h \
		$(OTHER_FILES)

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
