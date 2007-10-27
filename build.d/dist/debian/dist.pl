###
# dist.pl - extends support for build.pl for Debian/dpkg
#
# Debian packages directly required by the script:
#
# * svn-buildpackage
# * devscripts
# * dpkg-dev
# * libparse-debianchangelog-perl
#


sub autodetect
{
    my ($module) = @_;

    eval "require $module";
    if ($@) {
	warn $@ if $verbose;
	return 0;
    } else {
	return 1;
    }
}


# Auto detect Parse::DebianChangelog
BEGIN {
    $debian_changelog_loaded = &autodetect( Parse::DebianChangelog );
}

our $debian_tarballsdir = "$topdir/build/tarballs";
our $default_buildareadir = "$topdir/build/build-area";
our $debian_dir = "$confdir/dist/debian/src";
our $svn_url_base = 'minisip+svn://svn.minisip.org/minisip';
my $debian_tags_url = "${svn_url_base}/tags/build.d/dist/src/debian";
my $upstream_tags_url = "${svn_url_base}/tags";

#
# Debian object
#
sub debian_new {
	my ($class) = @_;
	my $self = {
	};
#	bless $self, $class;

	&debian_init($self);
	return $self;
}


#
# Initializer
#
sub debian_init {
	my ($self) = @_;
#	my $release = $ARGV[0] || $debian_release_default;
	my $release = $debian_release_default;
	$self->{_params} = $debian_releases{$release};

	print "+Dist: $release\n" if $verbose;

	my $chglog = Parse::DebianChangelog->init();
	$chglog->parse( { infile => "${debian_dir}/$pkg/debian/changelog" } );
	$self->{_chglog} = $chglog;

	$self->{_source_only} = 0;
#	$self->{_source_only} = 1;
}

sub debian_set_callback {
	my ($name, $callback) = @_;

	$debian_callbacks{$name} = $callback;
}

sub debian_release {
	my ($self) = @_;
	return $self->{_params};
}

sub debian_buildarea {
	my ($self) = @_;
	my $params = $self->{_params};

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
# Return version at offset in debian changelog
#
sub debian_changelog_version {
	my ($self, $offset) = @_;
	my $chglog = $self->{_chglog};

	my $dpkg = $chglog->dpkg( { count => 1, offset => $offset } );
	my $version = $dpkg->{'Version'};

	return $version;
}

#
# debian_changelog_released
#
# Check if top version in debian changelog is released.
#
sub debian_changelog_released {
	my ($self) = @_;

	my $dpkg = $self->{_chglog}->dpkg( { count => 1, offset => 0 } );

	return 0 if ($dpkg->{Version} =~ /^.*~/);
	return 0 if ($dpkg->{Distribution} =~ /UNRELEASED/);
	return 0 if ($dpkg->{Changes} =~ /NOT RELEASED/);
	return 1;
}

#
# debian_tar_version
#
sub debian_tar_version {
	my $tar_version = undef;

	my @distfiles = bsd_glob("${debian_tarballsdir}/${pkg}*.tar.gz");
	unless (@distfiles) {
		warn <<NEED_TARBALLS;
warning: No tarballs found to 'package'; use 'dist' to create them.
NEED_TARBALLS
		return undef;
	}

	# Search for newest tarball
	for my $p ( @distfiles ) {
		my $base = basename($p);

		if ( $base =~ /(.*)_(.*).orig.tar.gz/ ) {
			my $tar_pkg = $1;
			my $version = $2;

			my $orig = "${tar_pkg}_${version}.orig.tar.gz";
			my $newer;

			if ( !defined $tar_version ){
				$tar_version = $version;
				next;
			}

			if ( system("dpkg --compare-versions '$version' '>' '$tar_version'") == 0 ){
				$tar_version = $version;
			}
		} else {
			warn "warning: Can't parse tarball version: $base";
			next;
		}
	}

	return $tar_version;
}

#
# debian_version_full
# Return debian version of new package
#
sub debian_version_full {
	my ($self, $tar_version) = @_;
	my $params = $self->{_params};
	my $suite = $params->{'suite'};
	my $codename = $params->{'codename'};
	my $released = &debian_changelog_released($self);
	my $version = &debian_changelog_version($self, $released ? 0 : 1);

	$version =~ /^(.*)-.*/;
	my $version_base = $1;

	if( $version_base ne $tar_version ) {
		# New upstream version
		$version = "${tar_version}-0";
		$released = 0;
	}

	if( !$released ){
		my $svn_version = `LANG=C svnversion -c ${debian_dir}/${pkg} | cut -d: -f2`;
		$svn_version =~ s/[\r\n]//;

		$version = "${version}${codename}${svn_version}";
	}
	else {
		if( $suite ne 'unstable' ){
			$version = "${version}.${codename}.1";
		}
	}

	return $version;
}

#
# debian_changes
# return arch or source .changes files for current package, version and dist
#
sub debian_changes {
	my ($self) = @_;
	my $params = $self->{_params};
	my $debian_buildareadir = &debian_buildarea($self);

	my $arch = `dpkg-architecture -qDEB_BUILD_ARCH`;

	$arch =~ s/\s*//g;

	my $tar_version = &debian_tar_version;
	my $version = &debian_version_full($self, $tar_version);

	my @all_changes;

	my $arch_changes = "$debian_buildareadir/${pkg}_${version}_${arch}.changes";
	push(@all_changes, $arch_changes) if -e $arch_changes;

	my $src_changes = "$debian_buildareadir/${pkg}_${version}_source.changes";
	push(@all_changes, $src_changes) if -e $src_changes;

	return @all_changes;
}

#
# debian_package
# build source and binary package
#
sub debian_cb_package {
	my $deb = &debian_new;
	return &debian_package($deb);
}

sub debian_package {
	my ($self) = @_;
	my $params = $self->{_params};

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my $builder = $debian_callbacks{'builder'};
	my $buildareadir = $params->{'buildareadir'} || $default_buildaredir;
	my $tar_version = &debian_tar_version;

	easy_mkdir($buildareadir);

	my $svn_override = "tagsUrl=$debian_tags_url/${pkg},upsTagUrl=$upstream_tags_url/${pkg},buildArea=$buildareadir,origDir=$debian_tarballsdir";
	my @buildpackage_args = qw( --svn-dont-clean --svn-ignore-new );

	push(@buildpackage_args, "--svn-override=${svn_override}");

	my $builder_str;
	my $use_builder = 0;
	if ($builder) {
		$builder_str = &$builder($params);
	}

	if (!$self->{_source_only} && defined $builder_str) {
		$use_builder = 1;
	}

	if ($use_builder) {
		push(@buildpackage_args, "--svn-builder=${builder_str}");
	} else {
		push(@buildpackage_args, qw( -us -uc -rfakeroot ));

		if( $self->{_source_only} ){
			unshift(@buildpackage_args, '-S');
		}
	}

	easy_chdir("$debian_dir/$pkg");

	my $suite = $params->{'suite'};
	my $description = $params->{'description'};
	my $released = &debian_changelog_released($self);
	my $version = &debian_changelog_version($self, 0 );
	my $version_full = &debian_version_full($self, $tar_version);
	
	if( $version ne $version_full ){
		my $codename = $params->{'codename'};

		act('debian: backup changelog',
		    qw( cp -a debian/changelog debian/changelog.build_bak ) );

		act('debian: update changelog', 'dch',
		    '--newversion', $version_full,
		    '--distribution', $suite,
		    "Package for $description");
	}

	if( !$released ){
#		push(@buildpackage_args, '-sa');
	}

	my $result = act('debian: svn-buildpackage', 'svn-buildpackage', @buildpackage_args);

	if( $version ne $version_full ){
		act('debian: restore changelog',
		    qw( mv -f debian/changelog.build_bak debian/changelog ) );
	}

	if( $result ){
		my @changes = &debian_changes($self);
		my $package_post = $debian_callbacks{'package-post'};

		&$package_post( $params, \@changes ) if $package_post;
	}

	return &debian_pkgfiles($self);
}


sub debian_cb_pkgfiles {
	my $deb = &debian_new;
	return &debian_pkgfiles($deb);
}

#
# debian_pkgfiles
# all files listed in the .changes file
#
sub debian_pkgfiles {
	my ($self) = @_;
	my $params = $self->{_params};

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my @files;
	my @changes = &debian_changes($self);
	my $debian_buildareadir = &debian_buildarea($self);

	die "No changes file found" unless @changes;

	push( @files, @changes );

	foreach my $file (@changes) {
		push( @files, &debian_parse_changes($self, $file) );
	}
	
	return @files;
}

sub debian_parse_changes {
	my ($self, $changes) = @_;
	my $debian_buildareadir = &debian_buildarea($self);

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
sub debian_cb_pkgcontents {
	my $deb = &debian_new;
	return &debian_pkgcontents($deb);
}

sub debian_pkgcontents {
	my ($self) = @_;
	my $params = $self->{_params};

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my @all_changes = &debian_changes($self);

	for my $changes (@all_changes) {
		act('debian: debc', 'debc', $changes);
	}
}

#
# debian_install
# install all packages listed in the .changes file
sub debian_cb_install {
	my $deb = &debian_new;
	return &debian_install($deb);
}

sub debian_install {
	my ($self) = @_;
	my $params = $self->{_params};

	unless ($params) {
		warn "warning: Debian release not found\n";
		return;
	}

	my $changes = &debian_changes($self);
	my $debian_buildareadir = &debian_buildarea($self);

	act('debian: debi', qw( debi ), '--debs-dir', $debian_buildareadir, $changes );
}

# TODO
# debian_purge
# purge all packages listed in the .changes file
sub debian_cb_purge {
	my $deb = &debian_new;
#	die "debian: purge unsupported";
}

#
# debian_cb_dist_post
#
# Create links to tarballs in build/tarballs
#
sub debian_cb_dist_post {
	my @distfiles = distfiles();

	easy_mkdir($debian_tarballsdir);

	for my $tar (@distfiles) {
		my $base = basename($tar);
		
		if ( $base =~ /(.*)-(.*).tar.gz/ ) {
			my $tar_pkg = $1;
			my $version = $2;
			
			my $orig = "${pkg}_${version}.orig.tar.gz";
			my $tgt = File::Spec->catdir($debian_tarballsdir, $orig);

			act('debian: ln', qw( ln -sf ), $tar, $tgt);
		}
	}
}

#
# debian_cb_tarclean_pre
#
# Remove links to tarballs in build/tarballs
#
sub debian_cb_tarclean_pre {
	my @distfiles = distfiles();
	
	for my $tar (@distfiles) {
		my $base = basename($tar);
		
		if ( $base =~ /(.*)-(.*).tar.gz/ ) {
			my $tar_pkg = $1;
			my $version = $2;
			
			my $orig = "${pkg}_${version}.orig.tar.gz";
			my $tgt = File::Spec->catdir($debian_tarballsdir, $orig);

			remove_files('tarball link', $tgt) if -f $tgt;
		}
	}
}

if ($debian_changelog_loaded) {
set_dist_callbacks(
		'pkgfiles' => \&debian_cb_pkgfiles,
		'package' => \&debian_cb_package,
		pkgcontents => \&debian_cb_pkgcontents,
		merge => \&debian_cb_install,
		purge => \&debian_cb_purge, 
		   );
} else {
    set_dist_callbacks();
}
