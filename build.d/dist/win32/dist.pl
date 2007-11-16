###
# dist.pl - extends support for build.pl for Win32
# Requires Nullsoft Scriptable Install System
#

sub win32_pkgversion {
    my @distfiles = distfiles();
    my $version = undef;
    
    for my $file ( @distfiles ){
	$file =~ /.*$pkg-(.*)\.tar.*$/;
	$version = $1;
	break;
    }

    return $version;
}

sub win32_pkgfiles {
    my $version = &win32_pkgversion;
    return unless $version;
    $version = $version . $win32_extra_version;
    return unless -r "$confdir/dist/$hostdist/$pkg.nsi";
    return ("$builddir/$pkg-installer-${version}.exe");
}

sub win32_package {
    my $version = &win32_pkgversion;

    if ( ! defined $version ){
	warn "warning: Can't detect $pkg version";
	return;
    }

    $version = $version . $win32_extra_version;

    if ( ! -r "$confdir/dist/$hostdist/$pkg.nsi" ) {
	warn "warning: $pkg.nsi not found";
	return;
    }

    easy_chdir("$confdir/dist/$hostdist");

    act('win32: strip', "$hostspec-strip",
	@win32_contents,
	) if $win32_do_strip;

    act('win32: makensis', qw( makensis ),
	"-DVERSION=$version",
	"-DINSTALLDIR=${destdir}${prefixdir}",
	"-DBUILDDIR=${builddir}",
	"-DOUTFILE=" . &win32_pkgfiles,
	@win32_params,
	"$pkg.nsi",
	);

    return &win32_pkgfiles;
}

set_dist_callbacks(
		   'pkgfiles' => \&win32_pkgfiles,
		   'package' => \&win32_package,
		   );
