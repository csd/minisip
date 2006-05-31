###
# dist.pl - extends support for build.pl for Debian/dpkg

our $debian_tarballsdir = "$topdir/build/tarballs";
our $debian_buildareadir = "$topdir/build/build-area";
our $debian_dir = "$topdir/debian";
our $svn_url_base = 'minisip+svn://svn.minisip.org/minisip';

#
# debian_changes
# return .changes file
#
sub debian_changes {
    my $arch = `dpkg-architecture -qDEB_BUILD_ARCH`;

    $arch =~ s/\s*//g;

    my $versionline = `dpkg-parsechangelog -l${debian_dir}/$pkg/debian/changelog | grep ^Version`;

    $versionline =~ /^Version: (.*)/;
    my $version = $1;

    my $changes = "$debian_buildareadir/${pkg}_${version}_${arch}.changes";
    $changes = "$debian_buildareadir/${pkg}_${version}_source.changes" unless -e $changes;

    return $changes;
}

#
# debian_package
# build source and binary package
#
sub debian_package {
	my @distfiles = distfiles();
	unless (@distfiles) {
		warn <<NEED_TARBALLS;
warning: No tarballs found to 'merge'; use 'dist' to create them.
NEED_TARBALLS
		return;
	}

	easy_mkdir($debian_tarballsdir);

	for my $p ( @distfiles ) {
	    my $base = basename($p);

	    if ( $base =~ /(.*)-(.*).tar.gz/ ) {
		my $pkg = $1;
		my $version = $2;
		my $orig = "${pkg}_${version}.orig.tar.gz";
		my $tgt = File::Spec->catdir($debian_tarballsdir, $orig);

		act('debian: ln', qw( ln -sf ), $p, $tgt);
	    } else {
		warn "warning: Can't parse tarball version";
		return;
	    }
	}

	easy_mkdir($debian_buildareadir);

	my $svn_override = "tagsUrl=${svn_url_base}/tags/debian/${pkg},upsTagUrl=${svn_url_base}/tags/${pkg},buildArea=$debian_buildareadir,origDir=$debian_tarballsdir";

	easy_chdir("$topdir/debian/$pkg");
	act('debian: svn-buildpackage', qw( svn-buildpackage -us -uc -rfakeroot --svn-dont-clean --svn-ignore-new ), "--svn-override=${svn_override}" );

	return debian_pkgfiles();
}


#
# debian_pkgfiles
# all files listed in the .changes file
#
sub debian_pkgfiles {
    my $changes = &debian_changes;

    open(CHANGES, "< $changes") or die "can't read $changes";

    my @files;
    my $found = 0;

    while(<CHANGES>) {
	my $line = $_;

	if ( $line =~ /^ / ) {
	    if ( $found ) {
		$line =~ s/^\s*//;
		$line =~ s/\n$//;
		my ($md4, $size, $section, $priority, $file) = split(/ /, $line);
		push( @files, "$debian_buildareadir/$file" );
	    }
	} else {
	    $found = $line =~ /^Files:/;
	}
    }
    close(CHANGES);

    return @files;
}

#
# debian_pkgcontents
# dump contents of all packages listed in .changes file
#
sub debian_pkgcontents {
    my $changes = &debian_changes;

    act('debian: debc', 'debc', $changes);
}

#
# debian_install
# install all packages listed in the .changes file
sub debian_merge {
    my $changes = &debian_changes;

    act('debian: debi', qw( debi ), '--debs-dir', $debian_buildareadir, $changes );
}

# TODO
# debian_purge
# purge all packages listed in the .changes file
sub debian_purge {
    die "debian: purge unsupported";
}

set_dist_callbacks(
		'pkgfiles' => \&debian_pkgfiles,
		'package' => \&debian_package,
		pkgcontents => \&debian_pkgcontents,
		merge => \&debian_install,
		purge => \&debian_purge, 
	);

