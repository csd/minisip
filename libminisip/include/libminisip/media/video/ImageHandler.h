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

#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include<libminisip/libminisip_config.h>

#include<string>
#include<stdint.h>

#ifdef WORDS_BIGENDIAN
#   define FOURCC( a, b, c, d ) \
        ( ((uint32_t)d) | ( ((uint32_t)c) << 8 ) \
           | ( ((uint32_t)b) << 16 ) | ( ((uint32_t)a) << 24 ) )

#else
#   define FOURCC( a, b, c, d ) \
        ( ((uint32_t)a) | ( ((uint32_t)b) << 8 ) \
           | ( ((uint32_t)c) << 16 ) | ( ((uint32_t)d) << 24 ) )

#endif

#define M_CHROMA_I420           FOURCC( 'I', '4', '2', '0' )
#define M_CHROMA_RV16           FOURCC( 'R', 'V', '1', '6' )
#define M_CHROMA_RV24           FOURCC( 'R', 'V', '2', '4' )
#define M_CHROMA_RV32           FOURCC( 'R', 'V', '3', '2' )

/* Internal image type */
/* ffmpeg style */

typedef struct MData{
	uint8_t *data[4];
	int linesize[4];
} MData;

typedef struct MImage{
	uint8_t *data[4];
	int linesize[4];
	uint32_t ssrc;       
	uint32_t chroma;
	uint64_t mTime;      /* TimeStamp in msec */
	void * privateData;  /* Can be used by the ImageHandlers */
	uint32_t width;
	uint32_t height;
} MImage;

/* Interface used by image encoders and display */

class LIBMINISIP_API ImageHandler{
	public:
		virtual ~ImageHandler(){}

		virtual bool handlesChroma( uint32_t chroma )=0;
		virtual void init( uint32_t width, uint32_t height )=0;
		
		virtual void handle( MImage * )=0;
		
		virtual MImage * provideImage()=0;
		virtual MImage * provideImage( uint32_t /*ssrc*/ ){ return provideImage(); };
		virtual void releaseImage( MImage * )=0;
		virtual bool providesImage()=0;

		virtual uint32_t getRequiredWidth(){ return 0; };
		virtual uint32_t getRequiredHeight(){ return 0; };

		/*
		 * Tells the handler that it should try to change its size.
		 */
		virtual void resize(int w, int h)=0;
};

#if 0
/* Simple example */
class PpmWriter : public ImageHandler{
	public:
		PpmWriter( std::string fileName );
		virtual void handle( MImage& );
		virtual void init( uint32_t height, uint32_t width );
	private:
		std::ofstream file;
		uint32_t width;
		uint32_t height;
};
#endif

#endif
