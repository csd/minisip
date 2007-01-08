/*
  Copyright (C) 2006 Zachary T Welch

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
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Zachary T Welch <zach-minisip@splitstring.com>
 */

#ifndef MLIBMCRYPTO_INIT_H
#define MLIBMCRYPTO_INIT_H

#include <libmcrypto/config.h>
#include <libmutil/MemObject.h>
#include <vector>

// libmcryptoInit must be called before other APIs
LIBMCRYPTO_API void libmcryptoInit();
LIBMCRYPTO_API void libmcryptoUninit();

class LIBMCRYPTO_API CryptoThreadGuard : public virtual MObject 
{
public:
	CryptoThreadGuard();
	virtual ~CryptoThreadGuard();

protected:
	virtual int numLocks() = 0;
	void setLock(int i, bool isSet);

private:
	void changeGuards(bool onDuty);
	static std::vector<Mutex*> guards;
};

#endif // MLIBMCRYPTO_INIT_H
