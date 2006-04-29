#!/bin/bash -i
# buildenv.sh - establish proper environment for manual minisip development

if test "${IN_BUILD_ENV}"; then
	echo "You already appear to be running under buildenv.sh."
	exit 1
fi
export IN_BUILD_ENV=yes

pkgs="libmutil libmcrypto libmnetutil libmikey libmsip libminisip minisip"

for pkg in ${pkgs}; do
	ACLOCAL_FLAGS="-I ${PWD}/${pkg}/m4 ${ACLOCAL_FLAGS}"
	PKG_CONFIG_PATH="${PWD}/${pkg}:${PKG_CONFIG_PATH}"
	LD_LIBRARY_PATH="${PWD}/${pkg}/.libs:${LD_LIBRARY_PATH}"
done

# trim trailing ':', if required
PKG_CONFIG_PATH="${PKG_CONFIG_PATH%:}"
LD_LIBRARY_PATH="${LD_LIBRARY_PATH%:}"

export ACLOCAL_FLAGS PKG_CONFIG_PATH LD_LIBRARY_PATH PATH

echo "The following minisip pkg-config variables have been set for you:"
set -x
# setup minisip pkg-config variables for in-tree builds
export MUTIL_LIBS="-L$PWD/libmutil/.libs -lmutil"
export MUTIL_CFLAGS="-I$PWD/libmutil/include"
export MCRYPTO_LIBS="-L$PWD/libmcrypto/.libs -lmcrypto -lcrypto -lssl"
export MCRYPTO_CFLAGS="-I$PWD/libmcrypto/include"
export MNETUTIL_LIBS="-L$PWD/libmnetutil/.libs -lmnetutil"
export MNETUTIL_CFLAGS="-I$PWD/libmnetutil/include"
export MIKEY_LIBS="-L$PWD/libmikey/.libs -lmikey"
export MIKEY_CFLAGS="-I$PWD/libmikey/include"
export MSIP_LIBS="-L$PWD/libmsip/.libs -lmsip"
export MSIP_CFLAGS="-I$PWD/libmsip/include"
export LIBMINISIP_LIBS="-L$PWD/libminisip/.libs -lminisip"
export LIBMINISIP_CFLAGS="-I$PWD/libminisip/include"

set +x

echo "In addition, these other environment variables have been updated:"
echo "  ACLOCAL_FLAGS=${ACLOCAL_FLAGS}"
echo "  PKG_CONFIG_PATH=${PKG_CONFIG_PATH}"
echo "  LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
echo "Type 'exit' to leave this new shell."
${SHELL}
