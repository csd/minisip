###
# dist.pl - extends support for build.pl for Win32
# Requires Nullsoft Scriptable Install System
#

my $minisip_ui_version = $minisip_ui_version || '0.7.1';

sub win32_pkgfiles {
    return unless -r "$confdir/dist/$hostdist/$pkg.nsi";
    return ("$builddir/$pkg-installer-${minisip_ui_version}.exe");
}

sub win32_package {
    if ( ! -r "$confdir/dist/$hostdist/$pkg.nsi" ) {
	warn "warning: $pkg.nsi not found";
	return;
    }

    easy_chdir("$confdir/dist/$hostdist");

    act('win32: strip', "$hostspec-strip",
	bsd_glob("${destdir}${prefixdir}/bin/*.exe"),
	bsd_glob("${destdir}${prefixdir}/bin/*.dll"),
	bsd_glob("${destdir}${prefixdir}/lib/libminisip/plugins/*.dll")
	);

    act('win32: makensis', qw( makensis ),
	"-DMINISIPDIR=${destdir}${prefixdir}",
	"-DSSLDIR=${ssldir}",
	"-DVERSION=${minisip_ui_version}",
	"-DBUILDDIR=${builddir}",
	"-DOUTFILE=" . &win32_pkgfiles,
	'minisip.nsi'
	);

    return &win32_pkgfiles;
}

set_dist_callbacks(
		   'pkgfiles' => \&win32_pkgfiles,
		   'package' => \&win32_package,
		   );
