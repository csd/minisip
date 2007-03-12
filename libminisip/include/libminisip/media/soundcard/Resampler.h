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

/* Copyright (C) 2004, 2005
 *
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *          Ignacio Sanchez Pardo <isp@kth.se>
*/

#ifndef RESAMPLER_H
#define RESAMPLER_H

#include<libminisip/libminisip_config.h>

#include<libmutil/MemObject.h>
#include<libmutil/MPlugin.h>
#include<libmutil/MSingleton.h>

class LIBMINISIP_API Resampler : public MObject{
	public: 
		virtual void resample( short * input, short * output )=0;

		virtual std::string getMemObjectType() const {return "Resampler";};


};

class LIBMINISIP_API ResamplerPlugin : public MPlugin{
	public:
		ResamplerPlugin(MRef<Library*> lib);
		virtual ~ResamplerPlugin();

		virtual MRef<Resampler*> createResampler(
			uint32_t inputFreq, uint32_t outputFreq,
			uint32_t duration, uint32_t nChannels ) const = 0;

		virtual std::string getPluginType() const{ return "Resampler"; }

};


class LIBMINISIP_API ResamplerRegistry : public MPluginRegistry, public MSingleton<ResamplerRegistry>{
	public:
		virtual std::string getPluginType(){ return "Resampler"; }

		MRef<Resampler *> create(
			uint32_t inputFreq, uint32_t outputFreq,
			uint32_t duration, uint32_t nChannels );

	protected:
		ResamplerRegistry();

	private:
		friend class MSingleton<ResamplerRegistry>;
};

#endif

