#!/usr/bin/perl
#  build.pl - a build system management tool
#  Developed originally for the minisip project by Zachary T Welch.
#  Copyright (C) 2005-2006, Superlucidity Services, LLC
#  Released under the GNU GPL v2 license.

use strict;
use warnings;

#######
# script configuration option definitions

use Cwd;
our $topdir = getcwd();	# path to common source directory (svn trunk)
our $builddir = undef;	# path to common build directory
our $installdir = undef; # path to common installation directory

my $bindir = undef;	#   path to installed executables
my $pkgconfigdir = undef; # path to installed .pc files
my $aclocaldir = undef; # path to installed .m4 files

our $srcdir = undef;	# path to package source directory
our $objdir = undef;	# path to package build directory

our $debug = 0;		# enable debugging build
our $static = 0;	# enable static build (--disable-shared)

my $batch = 0;		# perform actions on dependencies as well as targets
our $toggle_batch = 0;  # reverse meaning of batch flag (default to batch mode)

my $njobs = 0;		# number of parallel jobs during makes (0 = autodetect)
my $ccache = 1;		# enable ccache support
my $force = 0;		# continue despite errors
my $show_env = 0;	# output environment variables this script changes

my $pretend = 0;	# don't actually do anything
my $verbose = 0;	# enable verbose script output
my $quiet = 0;		# enable quiet script output
my $help = 0;
my $man = 0;

my $list_actions = 0;	# show the actions permitted by this script
my $list_targets = 0;	# show the targets known by this script

our $default_ui = 'gtkgui';	# Which ui does the user want to use?

our $hostdist = 'autodetect';	# This host's distribution (e.g. gentoo).
our $buildspec = 'autodetect';  # This host's compiler specification.
our $hostspec = 'autodetect';	# The target host's compiler specification.
				# e.g. x86-pc-linux-gnu, arm-unknown-linux-gnu

# reset umask; fixes problems from mixing ccache with merge action
umask(0007) or die "unable to set umask to 0007: $!";

#######
# process option arguments

sub usage { 
	print @_, "\n" if @_;
	die <<USAGE;
usage: $0 [<options>] <action>[+<action>...] [<targets>]
Build Options:
    -d|--debug		Enabled debugging in resulting builds
    -s|--static		Enabled static build (uses --disable-shared)

Advanced Build Options:
    -b|--build=...	Build libraries and programs on this platform.
    -t|--host=...	Build libraries and programs that run on this platform.
    -S|--batch		Perform actions on dependencies as well as targets.
    -j|--jobs=n		Set number of parallel jobs (default: $njobs)
    -c|--ccache		Enable ccache support (is on by default; use --noccache)
    -f|--force		Continue building despite any errors along the way
    -E|--show-env	Show environment variables when we update them

Directory Options:
    -T|--topdir=...	Select location of svn repository. 
			(currently: $topdir)
    -B|--builddir=...	Select location for build directories.
			(currently: $topdir/build)
    -I|--installdir=...	Select common install directory (sets --prefix).
	                (currently: $topdir/install)

Distribution Merge Options:
    -D|--distro=...	Sets package manager features (default: $hostdist)

Run Options:
    -U|--ui=...		Sets user interface to use (default: $default_ui)

General Options:
    -p|--pretend	Do not actually perform actions
    -v|--verbose	Verbose output mode
    -q|--quiet		Quiet output mode
    -?|--help		Show built-in help
    --list-actions	Show permitted command actions
    --list-targets	Show permitted targets
USAGE
#	--man		Show built-in man page
}

use Getopt::Long qw( :config gnu_getopt );
my $result = GetOptions(
		"topdir|T=s" => \$topdir,

		"builddir|B=s" => \$builddir,
		"installdir|I=s" => \$installdir,
		"distro|D=s" => \$hostdist,
		"ui|U=s" => \$default_ui,

		"debug|d!" => \$debug,
		"static|s!" => \$static,
		"batch|S!" => \$batch,

		"build|b=s" => \$buildspec,
		"host|t=s" => \$hostspec,

		"jobs|j=i" => \$njobs,
		"ccache|c!" => \$ccache,
		"force|f!" => \$force,
		"show-env|E!" => \$show_env,

		"pretend|p!" => \$pretend,
		"verbose|v!" => \$verbose,
		"quiet|q!" => \$quiet,
		'help|?' => \$help, 
		'list-actions' => \$list_actions, 
		'list-targets' => \$list_targets, 
#		man => \$man,
	);
usage() if !$result || $help || $man;

use Pod::Usage;
pod2usage(2) unless $result;
pod2usage(1) if $help;
pod2usage(-exitstatus => 0, -verbose => 2) if $man;

# cross-compiling support

sub cross_compiling { return $buildspec ne $hostspec }
sub autodetect_buildspec { return 'x86-pc-linux-gnu'; }
sub autodetect_hostspec { return autodetect_buildspec(); }

for ($buildspec) {
/^autodetect$/ and do { $buildspec = autodetect_buildspec(); };
}
for ($hostspec) {
/^autodetect$/ and do { $hostspec = autodetect_hostspec(); };
}
die "$hostspec-gcc not found." 
	if cross_compiling() && ! -x "/usr/bin/$hostspec-gcc";

# set-up paths
my $top_builddir = $builddir || "$topdir/build";
$builddir .= "$top_builddir/$hostspec";
my $top_installdir = $installdir || "$topdir/install";
$installdir .= "$top_installdir/$hostspec";
$bindir = "$installdir/usr/bin";

# set-up common options
$show_env = 1 if $pretend && $verbose;
$verbose = 1 if $pretend;
$verbose = 0 if $quiet;
$quiet = 0 if $pretend;

# guess number of jobs
# XXX: will not work for all platforms; needs revisiting, but okay for now
$njobs = `grep processor /proc/cpuinfo | wc -l` unless $njobs;

# extra arguments to pass to make
my @make_args = ();
push @make_args, '-k' if $force;
push @make_args, "-j$njobs" if $njobs > 1;

#######
# load package, dependency, and configure parameter definitions (see build.conf)

our $default_action;
our $default_target;

our @packages; 		# absolute package build order
our %dependencies; 	# package dependency lists
our %configure_params;	# package configure parameter tables

my @dist_actions = ( qw( packages package pkgclean merge purge ) );
our @actions = ( qw( bootstrap configure compile ),
		qw( install uninstall ),
		qw( clean dclean mclean ),
		qw( dist distcheck tarballs tarclean ),
		@dist_actions, 
		qw( check run allclean repoclean ),
	);

# load primary definitions
do "$topdir/build.conf" or die "error: unable to load build.conf:\n$@";

sub set_configure_param {
	my ( $package, %params ) = @_;
	if (ref $package) {
		set_configure_param($_, %params) for @$package;
		return;
	}
	die "no package called '$package'" unless grep /^$package$/, @packages;
	for my $key ( keys %params ) {
		die "no parameter called '$key' in $package" 
			unless exists($configure_params{$package}->{$key});
		$configure_params{$package}->{$key} = $params{$key};
	}
	return 1;
}
sub set_global_configure_param {
	my ( %params ) = @_;
	for my $key ( keys %params ) {
		my $value = $params{$key};
		# reassign global variables for build system related params
		for ($key) {
		/^debug$/ and do { $debug = $value; last; };
		/^shared$/ and do { $static = !$value; last; };
		}
		set_configure_param($_, $key, $value) for @packages;
	}
	return 1;
}

# allow overrides using the above pair of accessors
my $localconf = "$topdir/build.local";
if (-f "$localconf") {
	do "$localconf" or die "error: unable to load build.local:\n$@";
}

list_actions() if $list_actions;
list_targets() if $list_targets;

#######
# process action and target arguments

my $action = shift @ARGV || $default_action;
unless ($action) {
	print "error: No action specified! You must provide at least ",
		"one valid action.\n       You may combine actions with ",
		"'+', such as 'bootstrap+configure'.\n";
	list_actions();
	usage();
}
my @action = split(/\+/, $action); 
for ( @action ) {
	die "'$_' is not a valid action.\n" unless grep /^$_$/, @actions;
}

sub add_targets { 
	my @targets = map { ( add_targets(@{$dependencies{$_}}), $_) } @_;
	my %targets = map { $_ => 1 } @targets;
	return grep { delete $targets{$_} } @targets;
};
my @targets = @ARGV;
push @targets, $default_target if !@targets && $default_target;
for ( @targets ) {
	die "'$_' is not a valid target.\n" unless exists $configure_params{$_};
}
$batch = !$batch if $toggle_batch;
@targets = add_targets(@targets) if $batch;
usage("error: no target specified!\nPossible packages are:\n",
	map { "\t$_\n" } @packages, "\n") unless @targets;

print "$action: @targets\n" unless $quiet;

#######
# common action funtions

use File::Spec;
use File::Glob ':glob';
use File::Copy;
use File::Path;
use File::Basename;

our $pkg;
my %actions;
my %act_deps;

sub easy_mkdir {
	my ( $path, $print ) = @_;
	eval { mkpath($path, $print, 0775) };
	die "unable to create $path:\n$@" if $@;
	return $path;
}
sub easy_chdir {
	my $path = shift;
	easy_mkdir($path) unless -d $path;
	print "+Changing to $path...\n" if $verbose;
	chdir($path) or die "unable to change to $path: $!";
}

sub create_working_paths {
	my $print = shift;
	easy_mkdir($builddir, $print);
	easy_mkdir($installdir, $print);
	easy_mkdir($aclocaldir);
	easy_mkdir($pkgconfigdir);
}

sub act {
	my $label = shift;
	print "Running $label in $pkg...", $pretend ? ' (dry run)' : '', "\n" 
		unless $quiet;
	print '+ ', join(' ', @_), "\n" if $verbose;
	return if $pretend;
	system(@_) == 0 or die "system @_ failed: $?";
}
sub callact {
	my $a = shift;
	die "no action '$a'" unless exists $actions{$a};

	$act_deps{$a}->() if exists $act_deps{$a};

	my $tgtdir = $a eq 'bootstrap' ? $srcdir : $objdir;
	if ($tgtdir ne $ENV{PWD}) {
		easy_chdir($tgtdir);
	}
	$actions{$a}->();
}

sub distfiles {
	return bsd_glob("$objdir/$pkg*.tar.gz");
}

sub _is_feature_param { return defined $_[0] && $_[0] =~ /^\d?$/; }
sub _feature_configure_params {
	my $spec = shift;
	my @keys = grep { _is_feature_param($spec->{$_}) } keys %$spec;
	my %spec = map { $_ => $spec->{$_} ? 'en' : 'dis' } @keys;
	return map { join('', '--', $spec{$_}, 'able-', $_) } @keys;
}
sub _is_package_param { return defined $_[0] && $_[0] !~ /^\d?$/; }
sub _package_configure_params {
	my $spec = shift;
	my @keys = grep { _is_package_param($spec->{$_}) } keys %$spec;
	my %spec = map { $_ => $spec->{$_} ? '' : 'out' } @keys;
	return map { join('', '--', 'with', $spec{$_}, '-', $_, 
			$spec->{$_} ? '=' : '', $spec->{$_} ) } @keys;
}
sub configure_params {
	my $spec = $configure_params{$pkg};
	my @spec = ( 
			"--srcdir=$srcdir", 
			"--prefix=$installdir/usr",
			_feature_configure_params($spec),
			_package_configure_params($spec),
		);
	unshift @spec, "--build=$buildspec", "--host=$hostspec"
		if cross_compiling();
	# XXX: the following changes require further configure.ac changes;
	#      however, they should not interfere until that time
	my $deps = $dependencies{$pkg};
	my %deps = map { $_ => /^lib(.*)$/ } @$deps;
	push @spec, map { "--with-" . $deps{$_} . "=$topdir/$_" } @$deps;
	return @spec;
}

sub list_files {
	my $label = shift;
	print $label, join(", ", map { basename($_) } @_), "\n";
	return @_;
}
sub remove_files {
	my ( $label, @files ) = @_;
	for my $p ( @files ) {
		print "+removing $pkg $label: $p\n";
		unlink($p) or die "can't unlink $p: $!"; 
	}
}

########
# XXX: add better distribution support (currently only supports Gentoo)
#  How to do that:  
#   1) factor existing implementation into build.d/dists/gentoo.pl module
#   2) define common set of entry points, change existing references
#   3) add new modules (the details are left as an exercise to reader)
# For bonus points, move configuration files into build.d when you're done.
#
# Finally, these suggestions are not meant to be implemented without review;
# this does not represent the best possible plan, just that plan is possible.

###
# Gentoo support

sub gentoo_distdir {
	my $distdir = `portageq envvar DISTDIR` or die "can't get DISTDIR";
	chomp $distdir;
	print "+portageq envvar DISTDIR=$distdir\n" if $verbose;
	return $distdir;
}

sub gentoo_package {
	act('gentoo: package', 'echo',
		'gentoo: This plug-in does not support binary packaging.');
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
	die "no distfiles!" unless @distfiles;
	my $distdir = gentoo_distdir();
	for my $p ( @distfiles ) {
		my $tgt = File::Spec->catdir($distdir, basename($p));
		if ( -e $tgt ) {
			print "+removing old $tgt\n";
			unlink($tgt) or die "can't unlink $tgt: $!"; 
		}
		print "+copying $p -> $tgt\n";
		unless (link($p, $tgt) or copy($p, $tgt)) {
			die "copy failed: $p -> $tgt: $!";
		}
	}
	act('gentoo: merge', qw( sudo emerge ), $pkg, '--digest', @_);
}

sub gentoo_purge {
	act('gentoo: purge', qw( sudo emerge ), '-C', $pkg);
}

my %gentoo_callbacks = (
		'pkgfiles' => \&gentoo_pkgfiles,
		'package' => \&gentoo_package,
		merge => \&gentoo_merge,
		purge => \&gentoo_purge, 
	);

###
# select approriate functions
# XXX: this needs revisiting as other distros become supported

my %distfuncs;

sub autodetect_hostdist {
	$hostdist = 'gentoo';
}

for ($hostdist) {
/^autodetect$/ and do { autodetect_hostdist(); }; # fall through!
/^gentoo$/ and do { %distfuncs = %gentoo_callbacks; last };
}

$distfuncs{packages} = sub { 
		list_files("$hostdist: $pkg packages: ", dist_pkgfiles()) 
	} unless exists $distfuncs{packages};
$distfuncs{pkgclean} = sub { 
		remove_files("$hostdist package", dist_pkgfiles()) 
	} unless exists $distfuncs{pkgclean};

# provide debuggable defaults and automatic accessors
for my $f ( @dist_actions, qw( pkgfiles ) ) {
	$distfuncs{$f} = sub {
			die "+BUG: unable to $f packages under '$hostdist'\n"
		} unless exists $distfuncs{$f};
	no strict 'refs';
	my $callback = "dist_$f";
	*$callback = sub { return $distfuncs{$f}->(@_) };
}

######
#  Common action functions

%actions = (
	bootstrap => sub {
		my @m4_paths = map { "$topdir/$_/m4" } @packages;
		my @bootstrap_args = map { ( '-I', $_ ) } @m4_paths, $aclocaldir;
		act('bootstrap', './bootstrap', @bootstrap_args);
	},
	configure => sub { 
		act('configure', "$srcdir/configure", configure_params()); 
	},
	compile => sub { act('compile', 'make', @make_args); },
	check => sub { act('check', 'make', @make_args, 'check'); },
	install => sub { act('install', 'make', @make_args, 'install'); },
	uninstall => sub { act('uninstall', 'make', @make_args, 'uninstall'); },
	run => sub {
		 # XXX: This needs to be generalized, but it works for now.
		 #  For what it's worth, it is "equivalent" to the old .sh
		return unless $pkg eq 'minisip';
		$ENV{LD_LIBRARY_PATH} = "$installdir/usr/lib";
		act('run', "$bindir/$pkg" . "_$default_ui");
	},
	tarballs => sub { list_files("$pkg tarballs: ", distfiles()) },
	tarclean => sub { remove_files("tarballs", distfiles()); },
	dist => sub { act('distribution', 'make', @make_args, 'dist'); },
	distcheck => sub { act('distcheck', 'make', @make_args, 'distcheck'); },
	%distfuncs,
	clean => sub { act('cleanup', 'make', 'clean'); }, 
	dclean => sub { act('distribution cleanup', 'make', 'distclean'); },
	mclean => sub { act('developer cleanup', 'make', 'maintainer-clean'); },
	allclean => sub {
		if (-e "$objdir/Makefile") {
			callact('uninstall');
			callact('mclean');
		}
		#  remove but recreate objdir, in case of pending actions
		easy_chdir($builddir);
		rmtree($objdir, 1, 0);
		easy_chdir($objdir);
	},
	repoclean => sub { callact('allclean') },
);

# common checks for preconditions
my $need_bootstrap = sub { callact('bootstrap') unless -e "$srcdir/configure" };
my $need_configure = sub { callact('configure') unless -e "$objdir/Makefile" };
my $need_compile = sub { callact('compile') }; # always rebuild, just in case
my $need_install = sub { 
	callact('install') unless -x "$bindir/minisip_$default_ui"; 
}; 
my $need_tarclean = sub { callact('tarclean'); $need_compile->() };
my $need_dist = sub { callact('dist') unless scalar(distfiles()) };
my $need_package = sub { callact('package') unless scalar(dist_pkgfiles()) };

%act_deps = (
	configure => $need_bootstrap,
	compile => $need_configure,
	check => $need_compile,
	install => $need_compile,
	run => $need_install,
	dist => $need_tarclean,
	distcheck => $need_tarclean,
	'package' => $need_dist,
	'packages' => $need_package,
	merge => $need_package,
	clean => $need_configure,
	dclean => $need_configure,
	mclean => $need_configure,
);

use Text::Wrap;
sub list_actions {
	print "This script supports the following actions:\n";
	print wrap("\t", "\t", join(", ", @actions)), "\n";
	exit(0);
}

sub list_targets {
	print "This script knows about the following targets:\n";
	print wrap("\t", "\t", join(", ", @packages)), "\n";
	exit(0);
}

########################################################################
#  Main Program Start

if ($verbose) {
	print "+Top directory: $topdir\n";
	print "+Build directory: $builddir\n";
	print "+Install directory: $installdir\n";
}

# setup common environment
#  LDFLAGS: add local install library path
$ENV{LDFLAGS} ||= '';
$ENV{LDFLAGS} .= " -L$installdir/usr/lib";
#  CPPFLAGS: add local install include path (XXX: needed?)
$ENV{CPPFLAGS} ||= '';
$ENV{CPPFLAGS} .= " -I$installdir/usr/include";
#  CXXFLAGS: enabled all warnings and (optionally) debugging
$ENV{CXXFLAGS} ||= '';
$ENV{CXXFLAGS} .= "-Wall";
$ENV{CXXFLAGS} .= " -ggdb" if $debug;

$aclocaldir = "$installdir/usr/share/aclocal";

# pkg-config search order (tries to find the "most recent copy"):
#   1) Project directories
#   2) Local install directory
#   3) System directories 
$pkgconfigdir = "$installdir/usr/lib/pkgconfig";
my @pkgconfigdirs = ( ( map { "$builddir/$_" } @packages ), $pkgconfigdir );
push @pkgconfigdirs, $ENV{PKG_CONFIG_PATH} if $ENV{PKG_CONFIG_PATH};
$ENV{PKG_CONFIG_PATH} = join(':', @pkgconfigdirs);

if ($ccache) {
	$ENV{PATH} = join(':', '/usr/lib/ccache/bin', $ENV{PATH});
	$ENV{CCACHE_DIR} = easy_mkdir(File::Spec->catdir($topdir, '.ccache'))
		unless $ENV{CCACHE_DIR} && -d $ENV{CCACHE_DIR};
}

# special pre-target processing
for ( @action ) {
/^repoclean$/ and do {
	# repoclean always operates on all packages
	@targets = @packages;
	last
};
# no pre-target work to do
}

create_working_paths(1);

for $pkg ( @targets ) {
	# XXX: be afraid! dynamic scoping... icky icky icky, but so darn handy
	local $pkg = $pkg;
	local $srcdir = File::Spec->catdir($topdir, $pkg);
	local $objdir = File::Spec->catdir($builddir, $pkg);
	easy_mkdir($objdir);

	print "+Source directory: $srcdir\n",
		"+Object directory: $objdir\n" if $verbose;

	if ($show_env) {
		my @envvars = ( qw( 
				PWD PATH CCACHE_DIR PKG_CONFIG_PATH
				CPPFLAGS CXXFLAGS LDFLAGS 
			) );
		my @env = map { "\n\t$_=" . $ENV{$_} } @envvars;
		print "Build environment for $pkg: @env\n";
	}

	callact($_) for @action;
}

# special post-target processing
for ( @action ) {
/^repoclean$/ and do {
	easy_chdir($top_builddir);
	rmtree( [ $builddir, $installdir ], 1, 0 );
	exit(0);
};
# no post-target work to do
}
