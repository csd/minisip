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

#include<config.h>
#include<libminisip/media/video/mixer/ImageMixer.h>
#include<libminisip/media/video/VideoMedia.h>
#include"../../MediaHandler.h"
#include<libminisip/media/codecs/Codec.h>
#include<libminisip/media/video/codec/VideoCodec.h>
#include<libminisip/media/video/codec/AVCoder.h>

#include<stdio.h>

#define MAX_SOURCE 256

using namespace std;

ImageMixer::ImageMixer():mainSource(0),newMainSource(0),
			 mainSourceImagesCounter(0){
}

void ImageMixer::init( uint32_t width, uint32_t height ){
	this->width = width;
	this->height = height;
}

bool ImageMixer::providesImage(){
	return output->providesImage();
}

MImage * ImageMixer::provideImage(){
	return output->provideImage();
}

MImage * ImageMixer::provideImage( uint32_t ssrc ){

	MImage * image;
	
	if( mainSourceImagesCounter == 0 && newMainSource != mainSource ){
		mainSource = newMainSource;
		fprintf( stderr, "Set main source to %i\n", mainSource );
	}
	
	if( ssrc == mainSource ){
		image = output->provideImage();
		image->ssrc = ssrc;
		mainSourceImagesCounter ++;
		return image;
	}

	cerr << "Before getSource" << endl;
	MRef<VideoMediaSource *> source = media->getSource( ssrc );
	cerr << "After getSource" << endl;

	if( source ){
		cerr << "Returning source->provideEmptyImage" << endl;
		return source->provideEmptyImage();
	}
	
	return NULL;
}

void ImageMixer::releaseImage( MImage * image ){
	if( output ){
		output->releaseImage( image );
	}
}

bool ImageMixer::handlesChroma( uint32_t chroma ){
	if( output ){
		return output->handlesChroma( chroma );
	}

	return false;
}


void ImageMixer::handle( MImage * image ){

	if( image->ssrc != mainSource ){
		/* It's not the main source, we just store it */
		MRef<VideoMediaSource *> source = media->getSource( image->ssrc );
		if( source ){
			source->addFilledImage( image );
		}
	}
	else{
		/* It's the main source, we will mix it with the other sources
		 * and display it */

		media->getImagesFromSources( images, nImagesToMix, mainSource );

		mix( image );

		media->releaseImagesToSources( images, nImagesToMix );
		
		output->handle( image );
		
		mainSourceImagesCounter --;
	}

	if( media->getSource( mainSource ).isNull() ){
		/* The main source is no longer available,
		 * switch to that one */

		fprintf( stderr, "Main source not available, switching to %i\n", image->ssrc );
		selectMainSource( image->ssrc );
	}
}


void ImageMixer::mix( MImage * image ){
	uint32_t i;


	for( i = 0; i < nImagesToMix; i++ ){

		MImage * toMix = images[i];

		if( !images[i] ){
			continue;
		}
		
		uint32_t i,j;
		uint8_t * yd, *ud, *vd, *ys, *us, *vs;
		uint32_t factor = 4;
		uint32_t widthOffset = 10;
		uint32_t heightOffset = height - height / factor - 10;

		

		yd = image->data[0] + heightOffset*image->linesize[0] + widthOffset;
		ud = image->data[1] + heightOffset*image->linesize[1]/2 + widthOffset/2;
		vd = image->data[2] + heightOffset*image->linesize[1]/2 + widthOffset/2;

		ys = toMix->data[0];
		us = toMix->data[1];
		vs = toMix->data[2];

		for( j = 0; (j < (height)/(2*factor)) && (heightOffset + 2*j<height); j++ ){
			for( i = 0; (i < image->linesize[0] / (2*factor)) && (widthOffset + 2*i < width ); i++ ){
				yd[2*i] = ys[2*i*factor];
				yd[2*i+1] = ys[2*i*factor+1];
				yd[2*i+image->linesize[0]] = ys[2*i*factor+image->linesize[0]];
				yd[2*i+image->linesize[0]+1] = ys[2*i*factor+image->linesize[0]+1];
				ud[i] = us[i*factor];
				vd[i] = vs[i*factor];
			}
			yd += image->linesize[0]*2;
			ud += image->linesize[1];
			vd += image->linesize[2];

			ys += toMix->linesize[0]*2*factor;
			us += toMix->linesize[1]*factor;
			vs += toMix->linesize[2]*factor;

		}

	}

	
}

void ImageMixer::setOutput( ImageHandler * output ){
	this->output = output;
}

uint32_t ImageMixer::getRequiredWidth(){
	return output->getRequiredWidth();
}

uint32_t ImageMixer::getRequiredHeight(){
	return output->getRequiredHeight();
}

void ImageMixer::setMedia( MRef<VideoMedia *> media ){
	this->media = media;
}

void ImageMixer::selectMainSource( uint32_t ssrc ){
	newMainSource = ssrc;
}
