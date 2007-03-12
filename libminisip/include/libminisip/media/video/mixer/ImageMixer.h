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

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#ifndef IMAGE_MIXER_H
#define IMAGE_MIXER_H

#include<libminisip/libminisip_config.h>

#include<libminisip/media/video/ImageHandler.h>

#include<libmutil/MemObject.h>
#include<libmutil/Mutex.h>
#include<libminisip/media/video/VideoMedia.h>

class LIBMINISIP_API ImageMixer : public ImageHandler, public MObject{
	public:

		ImageMixer();

		virtual bool handlesChroma( uint32_t chroma );
		virtual void init( uint32_t width, uint32_t height );

		virtual void handle( MImage * );

		virtual MImage * provideImage();
		
		virtual MImage * provideImage( uint32_t ssrc );
		virtual void releaseImage( MImage * );
		virtual bool providesImage();

		virtual uint32_t getRequiredWidth();
		virtual uint32_t getRequiredHeight();

		virtual void selectMainSource( uint32_t ssrc );

		virtual void setOutput( ImageHandler * output );

		virtual void mix( MImage * image );

		void setMedia( MRef<VideoMedia *> media );

		virtual std::string getMemObjectType() const {return "ImageMixer";};
	private:
		ImageHandler * output;

		uint32_t width;
		uint32_t height;

		uint32_t mainSource;
		uint32_t newMainSource;

		uint32_t mainSourceImagesCounter;

		MRef<VideoMedia *> media;

		MImage * images[MAX_SOURCES];
		uint32_t nImagesToMix;

		
};

#endif
