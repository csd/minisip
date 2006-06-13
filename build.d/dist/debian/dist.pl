###
# dist.pl - extends support for build.pl for Debian/dpkg
#
# Debian packages directly required by the script:
#
# * svn-buildpackage
# * devscripts
# * dpkg-dev
#

our $debian_tarballsdir = "$topdir/build/tarballs";
our $default_buildareadir = "$topdir/build/build-area";
our $debian_dir = "$confdir/dist/debian/src";
our $svn_url_base = 'minisip+svn://svn.minisip.org/minisip';
my $debian_tags_url = "${svn_url_base}/tags/build.d/dist/src/debian";
my $upstream_tags_url = "${svn_url_base}/tags";


sub debian_set_callback {
	my ($name, $callback) = @_;

	$debian_callbacks{$name} = $callback;
}

sub debian_release {
	my $release = $debian_release_default;
	my $params = $debian_releases{$release};

	return $params;
}

sub debian_buildarea {
	my $params = &debian_release;

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my $debian_buildareadir = $params->{'buildareadir'} || $default_buildareadir;
	return $debian_buildareadir;
}

#
# debian_changelog_version
#
sub debian_changelog_version {
	my $versionline = `dpkg-parsechangelog -l${debian_dir}/$pkg/debian/changelog | grep ^Version`;

	$versionline =~ /^Version: (.*)/;
	my $version = $1;

	return $version;
}

#
# debian_version_full
# Return debian version of package based on 
#
sub debian_version_full {
	my ($params, $tar_version) = @_;
	my $version = &debian_changelog_version;
	my $suite = $params->{'suite'};

	my $version_base = substr($version, 0, length($tar_version));

	if( $version_base ne $tar_version ) {
		# New upstream version
		$version = "${tar_version}-1";
	}
	
	if( $suite ne 'unstable' ){
		my $codename = $params->{'codename'};

		if ( $version !~ /$codename/ ) {
			$version = "${version}${codename}1";
		}
	}

	return $version;
}

#
# debian_changes
# return .changes file
#
sub debian_changes {
	my ($params) = @_;
	my $debian_buildareadir = &debian_buildarea;

    my $arch = `dpkg-architecture -qDEB_BUILD_ARCH`;

    $arch =~ s/\s*//g;

    my $version = &debian_version_full($params, &debian_changelog_version);

    my $changes = "$debian_buildareadir/${pkg}_${version}_${arch}.changes";

	return $changes if -e $changes;

	$changes = "$debian_buildareadir/${pkg}_${version}_source.changes";

	return $changes if -e $changes;

	print "Changes not found: $changes";

	return undef;
}

#
# debian_package
# build source and binary package
#
sub debian_package {
	my $params = &debian_release;

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my $builder = $debian_callbacks{'builder'};
	my $buildareadir = $params->{'buildareadir'} || $default_buildaredir;

	my @distfiles = distfiles();
	unless (@distfiles) {
		warn <<NEED_TARBALLS;
warning: No tarballs found to 'merge'; use 'dist' to create them.
NEED_TARBALLS
		return;
	}

	easy_mkdir($debian_tarballsdir);

	my $tar_version;
	my $tar_mtime = undef;

	# Search for newest tarball
	for my $p ( @distfiles ) {
	    my $base = basename($p);

	    if ( $base =~ /(.*)-(.*).tar.gz/ ) {
		my $pkg = $1;
		my $version = $2;
		my $orig = "${pkg}_${version}.orig.tar.gz";
		my $tgt = File::Spec->catdir($debian_tarballsdir, $orig);
		my $newer;

		my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,
		    $atime,$mtime,$ctime,$blksize,$blocks) = stat($p);

		if ($tar_mtime) {
			$newer = $mtime > $tar_mtime;
		} else {
			$newer = true;
		}

		if ($newer) {
			$tar_mtime = $mtime;
			$tar_version = $version;
				
			act('debian: ln', qw( ln -sf ), $p, $tgt);
		}
	    } else {
		warn "warning: Can't parse tarball version";
		return;
	    }
	}

	easy_mkdir($buildareadir);

	my $svn_override = "tagsUrl=$debian_tags_url/${pkg},upsTagUrl=$upstream_tags_url/${pkg},buildArea=$buildareadir,origDir=$debian_tarballsdir";
	my @buildpackage_args = qw( --svn-dont-clean --svn-ignore-new );

	push(@buildpackage_args, "--svn-override=${svn_override}");

	my $builder_str;
	if ($builder) {
		$builder_str = &$builder($params);
	}

	if ($builder_str) {
		push(@buildpackage_args, "--svn-builder=${builder_str}");
	} else {
		push(@buildpackage_args, qw( -us -uc -rfakeroot ));
	}

	easy_chdir("$debian_dir/$pkg");

	my $suite = $params->{'suite'};
	my $description = $params->{'description'};
	my $version = &debian_changelog_version;
	my $version_full = &debian_version_full($params, $tar_version);
	
	if( $version ne $version_full ){
		my $codename = $params->{'codename'};

		act('debian: backup changelog',
		    qw( cp -a debian/changelog debian/changelog.build_bak ) );

		act('debian: update changelog', 'dch',
		    '--newversion', $version_full,
		    '--distribution', $suite,
		    "Package for $description");
	}

	if( act('debian: svn-buildpackage', 'svn-buildpackage', @buildpackage_args) ){
		my $changes = &debian_changes($params);
		my $package_post = $debian_callbacks{'package-post'};

		&$package_post( $params, $changes ) if $package_post;
	}

	if( $version ne $version_full ){
		act('debian: restore changelog',
		    qw( mv -f debian/changelog.build_bak debian/changelog ) );
	}

	return debian_pkgfiles();
}


#
# debian_pkgfiles
# all files listed in the .changes file
#
sub debian_pkgfiles {
	my $params = &debian_release;

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

    my $changes = &debian_changes($params);
    my $debian_buildareadir = &debian_buildarea;

    die "No changes file found" unless $changes;

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
	my $params = &debian_release;

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my $changes = &debian_changes($params);

    act('debian: debc', 'debc', $changes);
}

#
# debian_install
# install all packages listed in the .changes file
sub debian_install {
	my $params = &debian_release;

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my $changes = &debian_changes($params);
    my $debian_buildareadir = &debian_buildarea;

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

