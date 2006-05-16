###
# dist.pl - extends support for build.pl for Gentoo/emerge

sub gentoo_distdir {
	my $distdir = `portageq envvar DISTDIR` or die "can't get DISTDIR";
	chomp $distdir;
	print "+portageq envvar DISTDIR=$distdir\n" if $verbose;
	return $distdir;
}

sub gentoo_package {
	warn "warning: The Gentoo plug-in does not support binary packaging.\n"
		unless $quiet;
	# XXX: Gentoo supports binary packages, but this plug-in doesn't;
	# as such, this could be problematic and should be optionally enabled.
	#return gentoo_merge('-b') if $gentoo_binary_packages;
	# XXX: for now, we're okay if we have distfiles to merge
	return distfiles();
}

sub gentoo_pkgfiles {
	# For now, just return tarballs required for merge
	return distfiles(); 
}

sub gentoo_merge {
	my @distfiles = distfiles();
	unless (@distfiles) {
		warn <<NEED_TARBALLS;
warning: No tarballs found to 'merge'; use 'dist' to create them.
NEED_TARBALLS
		return;
	}

	my $distdir = gentoo_distdir();
	for my $p ( @distfiles ) {
		my $tgt = File::Spec->catdir($distdir, basename($p));
		if ( -e $tgt ) {
			print "+removing old $tgt\n" unless $quiet;
			unlink($tgt) or die "can't unlink $tgt: $!"; 
		}
		print "+copying $p -> $tgt\n" unless $quiet;
		unless (link($p, $tgt) or copy($p, $tgt)) {
			die "copy failed: $p -> $tgt: $!";
		}
	}
	act('gentoo: merge', qw( sudo emerge ), $pkg, '--digest', @_);
}

sub gentoo_purge {
	act('gentoo: purge', qw( sudo emerge ), '-C', $pkg);
}

set_dist_callbacks(
		'pkgfiles' => \&gentoo_pkgfiles,
		'package' => \&gentoo_package,
		merge => \&gentoo_merge,
		purge => \&gentoo_purge, 
	);

