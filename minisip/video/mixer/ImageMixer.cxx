/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* Copyright (C) 2004 
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
*/

#include<config.h>
#include"ImageMixer.h"


ImageMixer::ImageMixer(){
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

void ImageMixer::releaseImage( MImage * image ){
	output->releaseImage( image );
}

bool ImageMixer::handlesChroma( uint32_t chroma ){
	return chroma == M_CHROMA_I420;
}


void ImageMixer::handle( MImage * image ){
	MImage * toMix;
	uint32_t i,j;
	uint8_t * yd, *ud, *vd, *ys, *us, *vs;
	uint32_t factor = 6;
	uint32_t widthOffset = 10;
	uint32_t heightOffset = height - height / factor - 10;

	mixedImagesLock.lock();
	
	if( !mixedImages.empty() ){
		toMix = (*mixedImages.begin());
	}
	else return;

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
	mixedImagesLock.unlock();

	output->handle( image );
	
}

void ImageMixer::addMixedImage( MImage * image ){
	mixedImagesLock.lock();
	mixedImages.push_back( image );
	mixedImagesLock.unlock();
}

void ImageMixer::removeMixedImage( MImage * image ){
	mixedImagesLock.lock();
	mixedImages.remove( image );
	mixedImagesLock.unlock();
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
