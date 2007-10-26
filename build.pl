#!/usr/bin/perl
#  build.pl - a build system management tool
#  Developed originally for the minisip project by Zachary T Welch.
#  Copyright (C) 2005-2006, Superlucidity Services, LLC
#  Released under the GNU GPL v2 license.

use strict;
use warnings;

use Cwd;
use File::Spec;
use File::Glob ':glob';
use File::Copy;
use File::Path;
use File::Basename;
use Getopt::Long qw( :config gnu_getopt );
use Pod::Usage;
use Text::Wrap;

my $app_name = basename($0);

#######
# script configuration option definitions

our $topdir = getcwd();	# path to common source directory (svn trunk)
our $confdir = undef;	# path to common build.d directory
our $builddir = undef;	# path to common build directory
our $installdir = undef; # path to common installation directory
our $prefix = undef;	# installation prefix directory
my $prefixdir = undef;	# active prefix; set to one of previous four paths
our $destdir = "";	# destination installation directory

my $bindir = undef;	#   path to installed executables
my $libdir = undef;	#   path to installed libraries
my $includedir = undef;	#   path to installed header files
my $pkgconfigdir = undef; # path to installed .pc files
my $aclocaldir = undef; # path to installed .m4 files

our $srcdir = undef;	# path to package source directory
our $objdir = undef;	# path to package build directory

our $debug = 0;		# enable debugging build
our $static = 0;	# enable static build (--disable-shared)

my $batch = 0;		# perform actions on dependencies as well as targets
our $toggle_batch = 0;  # reverse meaning of batch flag (default to batch mode)

my $njobs = 0;		# number of parallel jobs during makes (0 = autodetect)
my $supercompute = 0;   #  run unlimited parallel jobs during makes (yikes!)
my $ccache = 1;		# enable ccache support
my $force = 0;		# continue despite errors
my $show_env = 0;	# output environment variables this script changes
my $localconf = undef;	# local config overrides

my $no_local = 0;	# do not extend environment to no-install development:
			#  ACLOCAL_FLAGS, PKG_CONFIG_PATH, and LD_LIBRARY_PATH

my $pretend = 0;	# don't actually do anything
our $verbose = 0;	# enable verbose script output
my $quiet = 0;		# enable quiet script output
my $help = 0;
my $man = 0;

my $list_actions = 0;	# show the actions permitted by this script
my $list_targets = 0;	# show the targets known by this script

my $_run_app = undef;	# Which application did the user specify with -A?
our $run_app = undef;	# Which application does the user want to run?
my @run_paths = qw( bin sbin );

our $hostdist = 'autodetect';	# This host's distribution (e.g. gentoo).
our $buildspec = 'autodetect';  # This host's compiler specification.
our $hostspec = 'autodetect';	# The target host's compiler specification.
				# e.g. x86-pc-linux-gnu, arm-unknown-linux-gnu

# reset umask; fixes problems from mixing ccache with merge action
umask(0007) or die "unable to set umask to 0007: $!";

#######
# option processing helpers - use this API in configuration files

sub add_run_paths { push @run_paths, @_ }

#######
# process option arguments

sub usage { 
	warn @_, "\n" if @_;
	die <<USAGE;
usage: $app_name [<options>] <action>[+<action>...] [<targets>]
Build Options:
    -d|--debug		Enabled debugging in resulting builds
    -s|--static		Enabled static build (uses --disable-shared)

Advanced Build Options:
    -b|--build=...	Build libraries and programs on this platform.
    -t|--host=...	Build libraries and programs that run on this platform.
    -S|--batch		Perform actions on dependencies as well as targets.
    -j|--jobs=n		Set number of parallel jobs (default: $njobs)
    -J|--supercompute	Run an unlimited number of parallel jobs (for testing)
    -c|--ccache		Enable ccache support (is on by default; use --noccache)
    -f|--force		Continue building despite any errors along the way
    -E|--show-env	Show environment variables when we update them
    -l|--localconf	Config overrides
			(currently: $topdir/build.d/build.local)

Directory Options:
    -T|--topdir=...	Select location of svn repository. 
			(currently: $topdir)
    -C|--confdir=...	Select location of configuration directory.
			(currently: $topdir/build.d)
    -B|--builddir=...	Select location for build directories.
			(currently: $topdir/build)
    -I|--installdir=...	Select testing install directory (sets --prefix).
	                (currently: $topdir/install)
    -P|--prefix=...	Select "live" install prefix (overrides installdir)
       --destdir=...	Select destination install directory

Distribution Merge Options:
    -D|--distro=...	Sets package manager features (default: $hostdist)

Run Options:
    -A|--app=...	Select application to run (default: check build.local)

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

my $result = GetOptions(
		"topdir|T=s" => \$topdir,

		"confdir|C=s" => \$confdir,
		"builddir|B=s" => \$builddir,
		"installdir|I=s" => \$installdir,
		"destdir=s" => \$destdir,
		"prefix|P=s" => \$prefix,
		"distro|D=s" => \$hostdist,
		"app|A=s" => \$_run_app,

		"debug|d!" => \$debug,
		"static|s!" => \$static,
		"batch|S!" => \$batch,

		"build|b=s" => \$buildspec,
		"host|t=s" => \$hostspec,

		"jobs|j=i" => \$njobs,
		"supercompute|J!" => \$supercompute,
		"ccache|c!" => \$ccache,
		"force|f!" => \$force,
		"show-env|E!" => \$show_env,
		"localconf|l=s" => \$localconf,
		"no-local!" => \$no_local,

		"pretend|p!" => \$pretend,
		"verbose|v!" => \$verbose,
		"quiet|q!" => \$quiet,
		'help|?' => \$help, 
		'list-actions' => \$list_actions, 
		'list-targets' => \$list_targets, 
#		man => \$man,
	);
usage() if !$result || $help || $man;

pod2usage(2) unless $result;
pod2usage(1) if $help;
pod2usage(-exitstatus => 0, -verbose => 2) if $man;

# set-up configuration file paths
$confdir = $confdir || "$topdir/build.d";
my $conffile = "$confdir/build.conf";
die "'$confdir' is not a valid configuration directory.'" unless -f $conffile;

# set-up common options
$show_env = 1 if $pretend && $verbose;
$verbose = 1 if $pretend;
$verbose = 0 if $quiet;
$quiet = 0 if $pretend;

# guess number of jobs
# XXX: will not work for all platforms; needs revisiting, but okay for now
$njobs = int(`grep ^processor /proc/cpuinfo | wc -l`) unless $njobs;
$njobs = 1 unless $njobs;
$njobs = 'unlimited' if $supercompute;

# extra arguments to pass to make
my @make_args = ();
push @make_args, '-k' if $force;
push @make_args, '-j' if $supercompute;
push @make_args, "-j$njobs" if int($njobs) > 1;

#######
# load package, dependency, and configure parameter definitions (see build.conf)

our $default_action;
our $default_target;

our @packages; 		# absolute package build order
our %dependencies; 	# package dependency lists
our %configure_params;	# package configure parameter tables

my @dist_actions = ( qw( package packages pkgcontents pkgclean merge purge ) );
our @actions = ( qw( bootstrap configure compile ),
		qw( install uninstall ),
		qw( clean dclean mclean ),
		qw( dist distcheck tarballs tarcontents tarclean ),
		@dist_actions, 
		qw( check run debug allclean hostclean repoclean ),
		qw( confdump confclean envdump ),
	);

sub load_file_if_exists {
	my $file = shift;
	return 0 unless -f $file;
	do $file or die "error: unable to load '" . 
				pretty_path($file) . ":\n$@";
	return 1;
}

# load primary definitions
load_file_if_exists($conffile);

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

# To assist the setup of new working copies, create *.local configuration 
# files from their *.local.example counterparts.
sub replace_missing_conf_file {
	my ( $dest, $type, $src, $name ) = @_;
	return if -f $dest;
	$src = "$dest.example" unless $src;
	$name = $name ? " for '$name'" : '';
	my $create_msg = "local $type configuration file$name:\n\t" . 
		pretty_path($dest) . "\n\tfrom ". pretty_path($src);
	print "+Creating new $create_msg\n" unless $quiet;
	unless (-f $src) {
		my $prep = $name ? 'this ' : '';
		print "+warning: '", pretty_path($src), 
				"' does not exist$name\n" .
			"+warning: no default settings for $prep$type.\n"
			unless $quiet;
		return;
	}
	copy($src, $dest) or 
		die "error: unable to create $create_msg\nerror: $!\n";
}

# create local configuration file
my $default_build_local = "$confdir/build.local";
$localconf = $localconf || $default_build_local;
replace_missing_conf_file($localconf, 'build', "$default_build_local.example");

# allow overrides using the 'set*_configure_param' accessors
load_file_if_exists($localconf);

die list_actions() if $list_actions;
die list_targets() if $list_targets;

#######
# cross-compiling support

my $arch_class;
sub cur_arch_class { $arch_class }
sub set_arch_callbacks { 
	my %funcs = @_;
	my @funcs = map { $arch_class . '_' . $_ => $funcs{$_} } keys %funcs;
	set_callbacks('arch', @funcs); 
}
sub set_arch_detect { set_callbacks('arch', detect => $_[0]) }

sub cross_compiling { return $buildspec ne $hostspec }
sub autodetect_platform { $_[0] eq 'autodetect' ? $_[1]->() : $_[0] }
$buildspec = autodetect_platform($buildspec, sub { 'x86-pc-linux-gnu' });
$hostspec = autodetect_platform($hostspec, sub { $buildspec });
 
my ( $buildarch, $hostarch );
$arch_class = 'build';
$buildarch = autodetect_probe('arch', 'autodetect', $buildspec);
if (cross_compiling()) {
	$arch_class = 'host';
	$hostarch = autodetect_probe('arch', 'autodetect', $hostspec);
} else {
	$hostarch = $buildarch;
}
#$arch_class = 'target';
#my $targetarch = autodetect_probe('arch', $targetspec);  # XXX: insanity!
 
# do an early check for cross-compiler, if one will be required
my ( $cross_compiler ) = grep { -x "$_/$hostspec-gcc" } split(':', $ENV{PATH});
die "$hostspec-gcc not found." if !$cross_compiler && cross_compiling();

# set-up paths
my $top_builddir = $builddir || "$topdir/build";
$builddir = "$top_builddir/$hostspec";
my $top_installdir = $installdir || "$topdir/install";
$installdir = "$top_installdir/$hostspec";

sub prefix_is_live { return $prefix }
sub prefix_path { prefix_is_live() || "$installdir/usr" }

$prefixdir = prefix_path();
$bindir = "$destdir$prefixdir/bin";

push @make_args, "DESTDIR=$destdir" if $destdir ne '';

#######
# process action and target arguments

our $pkg;
my %actions;
my %act_deps;

our $action = shift @ARGV || $default_action;
die "error: No action specified! You must provide at least " .
	"one valid action.\n       You may combine actions with " .
	"'+', such as 'bootstrap+configure'.\n\n" . list_actions() .
	"\nFor more information, see '$app_name --help'.\n" unless $action;
my @action = split(/\+/, $action);
for my $act ( @action ) {
	die "'$act' is not a valid action.\n" unless grep /^$act$/, @actions;
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
die "error: no target specified!\nYou must specify at least " .
	"one valid target.\n\n" . list_targets() .
	"\nFor more information, see '$app_name --help'.\n" unless @targets;

sub current_action { $action }
sub requested_actions { @action }
sub current_target { $pkg }
sub requested_targets { @targets }

sub assert_single_action {
	return if requested_actions() == 1;
	die <<SINGLE_ACTION;
The '$action' action must be called alone; it can not be combined.
SINGLE_ACTION
}
sub assert_single_target {
	return if requested_targets() == 1;
	die <<SINGLE_TARGET;
The '$action' action must be called with a single target.  If you are using
batch mode by default, you may need to add the '-S' option to use 'one-shot' 
mode.  If not, then leave out the '-S' option or select the target that 
contains the program to run.
SINGLE_TARGET
}


#######
# common action funtions

sub pretty_path { $_[0] =~ /^$topdir\/(.*)$/ ? "\${topdir}/$1" : $_[0] }

sub easy_mkdir {
	my ( $path, $print ) = @_;
	eval { mkpath($path, $print, 0775) };
	die "unable to create " . pretty_path($path) . ":\n$@" if $@;
	return $path;
}
sub easy_chdir {
	my $path = shift;
	easy_mkdir($path) unless -d $path;
	print "+Changing to " . pretty_path($path) . "...\n" if $verbose;
	chdir($path) or die "unable to chdir to " . pretty_path($path) . ": $!";
}

sub create_working_paths {
	my @paths = ( 
			$builddir, $installdir,
			$bindir, $libdir, $includedir,
			$aclocaldir, $pkgconfigdir,
		);
	easy_mkdir($_, !$quiet) for @paths;
}

sub act {
	my $label = shift;
	my $target_label = $pkg ? " in $pkg" : '';
	my $pretend_label = $pretend ? ' (dry run)' : ''; 
	print "Running '$label'$target_label...$pretend_label\n" unless $quiet;
	print '+ ', join(' ', @_), "\n" if $verbose;
	return if $pretend;
	system(@_) == 0 or die "system @_ failed: $?";
}
sub callact {
	my $a = shift;
	die "undefined action" unless $a;
	die "no action '$a'" unless exists $actions{$a};

	local $action = $a;

	$act_deps{$a}->() if exists $act_deps{$a};

	my $tgtdir = $a eq 'bootstrap' ? $srcdir : $objdir;
	if ($tgtdir ne $ENV{PWD}) {
		easy_chdir($tgtdir);
	}
	try_callback($pkg, "$a-pre");
	$actions{$a}->();
	try_callback($pkg, "$a-post");
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
			"--cache=$builddir/config.cache",
			"--srcdir=$srcdir", 
			"--prefix=$prefixdir",
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
	print $label, join(", ", map { pretty_path($_) } @_), "\n";
	return @_;
}
sub remove_files {
	my ( $label, @files ) = @_;
	for my $p ( @files ) {
		my $pp = pretty_path($p);
		print "+removing $pkg $label: $pp\n";
		unlink($p) or die "can't unlink $pp: $!"; 
	}
}

sub list_tarballs {
	my $label = shift;
	for my $tarball ( @_ ) {
		my $file = pretty_path($tarball);
		print "$label$file\n";
		local *PIPE;
		my $dash_v = $verbose ? '-v' : '';
		open PIPE, "tar $dash_v -ztf $tarball |" or 
			die "unable to open $file: $!";
		while (my $line = <PIPE>) { print "  $line" }
		close PIPE or die "unable to close $file: $!";
	}
	return 1;
}

#######
# autodetection helpers

sub autodetection {
	my ( $type, $hint ) = @_;
	local $_ = $hint;
	return try_callback($type, 'detect') || 0;
}

sub autodetect_probe_path {
	my ( $type, $setting, $hint ) = @_;
	return $setting unless $setting eq 'autodetect';
	# probes can factor multiple scores, allow configurations to be 
	#  selected by more than one criteria.
	my %scores = ( );
	for my $dir ( bsd_glob("$confdir/$type/*") ) {
		my $file = "$dir/detect.pl";
#		print "+Looking for $type called $dir...\n" if $verbose;
		next unless load_file_if_exists($file);
#		print "+Probing the $type called $dir...\n" if $verbose;
		my $s = $scores{basename($dir)} = autodetection($type, $hint);
#		print "+Scoring the $type called $dir: $s\n" if $verbose;
	}
	# pick the top scoring probe
	( $setting ) = sort { $scores{$b} <=> $scores{$a} } keys %scores;
	return $setting;
} 

sub autodetect_warning {
	my ( $type, $setting, $ext, $disable ) = @_;
	my $warning = $disable ?
		'will be disabled until one is created' :
		'may not work correctly without it';
	print <<MISSING unless $quiet;
+warning: The '$type.$ext' script does not exist for '$setting'.
+warning: '$type' actions $warning.
MISSING
}

sub autodetect_probe {
	my ( $type, $setting, $hint ) = @_;
	( $setting ) = autodetect_probe_path($type, $setting, $hint);
	my $xconfdir = "$confdir/$type/$setting";
	my $conf = "$xconfdir/$type.pl";
	if (load_file_if_exists($conf)) {
		print "+Using '$type' functions '$setting':\n" .
			"\t$conf\n" if $verbose;
		my $xlocalconf = "$xconfdir/$type.conf";
		load_file_if_exists($xlocalconf) or
			autodetect_warning($type, $setting, 'conf');

		$xlocalconf = "$xconfdir/$type.local";
		replace_missing_conf_file($xlocalconf, $type, undef, $hint);
		load_file_if_exists($xlocalconf) or
			autodetect_warning($type, $setting, 'local');
	} else {
		autodetect_warning($type, $setting, 'pl', 1);
	} 
	return $setting;
}

#########
# Generic callback support

my %callbacks;

sub get_extended_actions { 
	return map { %$_ } grep !/^(?:pre|post)$/, values %callbacks 
}
sub get_extended_deps { $callbacks{dep} ? %{$callbacks{dep}} : () }


sub set_pre_callbacks { set_callbacks('pre', @_) }
sub set_post_callbacks { set_callbacks('post', @_) }
sub set_build_callbacks { set_callbacks('build', @_) }
sub set_dependency_callbacks { set_callbacks('dep', @_) }
sub set_callbacks {
	my ( $type, %newfuncs ) = @_;
	$callbacks{$type} = {} unless exists $callbacks{$type};
	$callbacks{$type}->{$_} = $newfuncs{$_} for keys %newfuncs;
	return 1;
}
sub set_default_callback {
	my ( $type, $callback, $func ) = @_;
	return unless exists $callbacks{$type};
	return if exists $callbacks{$type}->{$callback};
	return set_callbacks($type, $callback, $func);
}
sub run_callback { 
#	print "+callback: ", $_[0], "->", $_[1], "\n"; 
	local $action = $_[1];
	return $callbacks{$_[0]}->{$_[1]}->(@_);
}
sub try_callback { 
	return unless exists $callbacks{$_[0] || '_unknown_'};
	return unless exists $callbacks{$_[0]}->{$_[1] || '_unknown_'};
	return run_callback(@_);
}
 
 
########
# Target/host distribution support

sub set_dist_callbacks { set_callbacks('dist', @_) }
sub set_dist_detect { set_callbacks('dist', detect => $_[0]) }

# probe the given hostdist; load its configuration files
$hostdist = autodetect_probe('dist', $hostdist);

# create standard dist accessor implementations
set_default_callback(qw( dist pkgcontents ), sub { 
		list_tarballs("$hostdist: $pkg: ", dist_pkgfiles()) 
	});
set_default_callback(qw( dist packages ), sub { 
		list_files("$hostdist: $pkg packages: ", dist_pkgfiles());
		dist_pkgcontents() if $verbose;
	});
set_default_callback(qw( dist pkgclean ), sub { 
		remove_files("$hostdist package", dist_pkgfiles()) 
	});

# provide debuggable defaults and automatic accessors
for my $f ( @dist_actions, qw( pkgfiles ) ) {
	set_default_callback('dist', $f, sub {
			die "+BUG: unable to run '$f' under '$hostdist'\n"
		});
	no strict 'refs';
	my $callback = "dist_$f";
	*$callback = sub { return run_callback('dist', $f, @_) };
}

#######
# action callbacks

sub cb_noop { }

sub cb_run_post { act('run', run_app_path()) }
sub cb_debug_post { act('run', debug_app_path()) }

sub _first_line { $_[0] }
sub cb_envdump_pre {
	my $gxx;
	my $cpp;
	my $ld;
	if (cross_compiling()) {
	    $gxx = "$hostspec-g++";
	    $cpp = "$hostspec-cpp";
	    $ld = "$hostspec-ld";
	} else {
	    $gxx = $ENV{CXX} || 'g++';
	    $cpp = $ENV{CPP} || 'cpp';
	    $ld = 'ld';
	}
	print "Working Copy ", `svn info | grep ^Rev`;
	print "pkg-config: ", _first_line(`pkg-config --version`);
	print "autoconf: ", _first_line(`autoconf --version`);
	print "automake: ", _first_line(`automake --version`);
	print " libtool: ", _first_line(`libtool --version`);
	print "    make: ", _first_line(`make --version`);
	print "     g++: ", _first_line(`$gxx --version`);
	print "     cpp: ", _first_line(`$cpp --version`);
	print "      ld: ", _first_line(`$ld --version`);
	print "\n";
	$show_env = 1; 
	show_env();
	exit(0);
}

sub cb_confdump {
	my $spec = $configure_params{$pkg};
	# XXX: quick and dirty, to see the composite results of loading
	# multiple configuration files that may define/redefine settings.
	print "Active configuration for $pkg:\n";
	my @keys = sort grep { defined $spec->{$_} } keys %$spec;
	print "\t", join(' = ', $_, $spec->{$_}), "\n" for @keys;
	print "\n";
}

sub cb_hostclean_pre { assert_single_action() }
sub cb_hostclean { 
	#  remove but recreate objdir, in case of pending actions
	easy_chdir($builddir);
	rmtree($objdir, !$quiet, 0);
	easy_chdir($objdir);
}
sub cb_hostclean_post { 
	easy_chdir($topdir);
	rmtree( [ $builddir, $installdir ], !$quiet, 0 );
	exit(0);
}

# repoclean always operates on all packages
sub cb_repoclean_pre { 
	assert_single_action();
	@targets = @packages 
}
sub cb_repoclean_post {
	easy_chdir($topdir);
	rmtree( [ $top_builddir, $top_installdir ], !$quiet, 0 );
	exit(0);
}

sub cb_confclean_pre { assert_single_action() }
sub cb_confclean_post {
	# XXX: this could be highly undesirable.  maybe ask to confirm?
	my @sublocals = bsd_glob("$confdir/*/*/*.local");
	for my $file ( @sublocals, $default_build_local ) {
		print "+removing ", pretty_path($file), "...\n" unless $quiet;
		unlink $file or die "unable to remove '" . 
					pretty_path($file) . "': $!";
	}
}

set_pre_callbacks(
		envdump => \&cb_envdump_pre,
		hostclean => \&cb_repoclean_pre,
		repoclean => \&cb_repoclean_pre,
		confclean => \&cb_repoclean_pre,
	);
set_build_callbacks(
		run => \&cb_noop,
		debug => \&cb_noop,
		confdump => \&cb_confdump,
		hostclean => \&cb_hostclean,
		confclean => \&cb_noop,
	);
set_post_callbacks(
		run => \&cb_run_post,
		debug => \&cb_debug_post,
		repoclean => \&cb_repoclean_post,
		hostclean => \&cb_hostclean_post,
		confclean => \&cb_confclean_post,
	);

######
#  Common action functions

sub run_app_path {
	die "error: No application was specified in build.local or with -A.\n"
		unless $run_app;
	my $rootpath = $destdir . $prefixdir;
	for my $path ( @run_paths ) {
		my $target = "$rootpath/$path/$run_app";
#		print "+checking for $target...\n" if $verbose;
		next unless -x $target;
#		print "+found $target...\n" unless $quiet;
		return $target;
	}
	return $rootpath . $run_app;
}

sub debug_app_path {
	return ( 'gdb', run_app_path() );
}

%actions = (
	get_extended_actions(),
	bootstrap => sub {
		act('bootstrap', './bootstrap');
		remove_files('config.cache', "$builddir/config.cache") if -f "$builddir/config.cache";
	},
	configure => sub { 
		act('configure', "$srcdir/configure", configure_params()); 
	},
	compile => sub { act('compile', 'make', @make_args); },
	check => sub { act('check', 'make', @make_args, 'check'); },
	install => sub { act('install', 'make', @make_args, 'install'); },
	uninstall => sub { act('uninstall', 'make', @make_args, 'uninstall'); },
	tarballs => sub { list_files("$pkg tarballs: ", distfiles()) },
	tarcontents => sub { list_tarballs("$pkg tarball: ", distfiles()) },
	tarclean => sub { remove_files("tarballs", distfiles()); },
	dist => sub { act('distribution', 'make', @make_args, 'dist'); },
	distcheck => sub { act('distcheck', 'make', @make_args, 'distcheck'); },
	clean => sub { act('cleanup', 'make', 'clean'); }, 
	dclean => sub { act('distribution cleanup', 'make', 'distclean'); },
	mclean => sub { act('developer cleanup', 'make', 'maintainer-clean'); },
	allclean => sub {
		return unless -e "$objdir/Makefile";
		callact('uninstall');
		callact('mclean');
	},
);

# common checks for preconditions
my $need_bootstrap = sub { callact('bootstrap') unless -e "$srcdir/configure" };
my $need_configure = sub { callact('configure') unless -e "$objdir/Makefile" };
my $need_compile = sub { callact('compile') }; # always rebuild, just in case
my $need_install = sub { 
	callact('install') unless -x run_app_path();
}; 
my $need_tarclean = sub { callact('tarclean'); $need_compile->() };
my $need_dist = sub { callact('dist') unless scalar(distfiles()) };
my $need_package = sub { callact('package') unless scalar(dist_pkgfiles()) };
my $need_allclean = sub { callact('allclean') };

%act_deps = (
	get_extended_deps(),
	configure => $need_bootstrap,
	compile => $need_configure,
	check => $need_compile,
	install => $need_compile,
	run => $need_install,
	debug => $need_install,
	dist => $need_tarclean,
	distcheck => $need_tarclean,
	tarcontents => $need_dist,
	'package' => $need_dist,
	'packages' => $need_package,
	pkgcontents => $need_package,
	merge => $need_package,
	clean => $need_configure,
	dclean => $need_configure,
	mclean => $need_configure,
	hostclean => $need_allclean,
	repoclean => $need_allclean,
);

sub list_actions {
	return "This script supports the following actions:\n" .
		wrap("\t", "\t", join(", ", @actions)) . "\n";
}

sub list_targets {
	return "This script knows about the following targets:\n" .
		wrap("\t", "\t", join(", ", @packages)) . "\n";
}

########################################################################
#  Main Program Start

# override application to run, if user specified one via CLI
$run_app = $_run_app if defined $_run_app;

print "$action: @targets\n" unless $quiet;

if ($verbose || $show_env) {
	print "+\n+Top directory:              $topdir\n";
	print "+Build directory:            ", pretty_path($builddir), "\n";
	my $pdestdir = $destdir && pretty_path($destdir);
	print "+Install \$DESTDIR directory: $pdestdir\n" if $destdir;
	my $pprefixdir = pretty_path($prefixdir);
	print "+Install --prefix directory: $pprefixdir\n";
	print "+Full path to local install: $pdestdir$pprefixdir\n" if $destdir;
}
print "+Make will try to run $njobs parallel build jobs\n" if $verbose;

# setup common environment
#  LDFLAGS: add local install library path
$ENV{LDFLAGS} ||= '';
$libdir = "$destdir$prefixdir/lib";
#$ENV{LDFLAGS} .= " -L$libdir";
#  CPPFLAGS: add local install include path (XXX: needed?)
$ENV{CPPFLAGS} ||= '';
$includedir = "$destdir$prefixdir/include";
#$ENV{CPPFLAGS} .= " -I$includedir ";
#  CXXFLAGS: enabled all warnings and (optionally) debugging
$ENV{CXXFLAGS} ||= '';
$ENV{CXXFLAGS} .= " -Wall";
$ENV{CXXFLAGS} .= " -ggdb" if $debug;

$aclocaldir = "$destdir$prefixdir/share/aclocal";
$pkgconfigdir = "$destdir$prefixdir/lib/pkgconfig";

sub setup_aclocal_env {
	my @m4_paths = $no_local ? () : map { "$topdir/$_/m4" } @packages;
	my @aclocal_flags = map { ( '-I', $_ ) } @m4_paths, $aclocaldir;
	unshift @aclocal_flags, $ENV{ACLOCAL_FLAGS} if $ENV{ACLOCAL_FLAGS};
	$ENV{ACLOCAL_FLAGS} = join(' ', '', @aclocal_flags);
}

# pkg-config search order (tries to find the "most recent copy"):
#   1) Project directories
#   2) Local install directory
#   3) System directories 
sub setup_pkgconfig_env {
	my @pkgconfigdirs;
	push @pkgconfigdirs, map { "$builddir/$_" } @packages unless $no_local;
	push @pkgconfigdirs, $pkgconfigdir;
	push @pkgconfigdirs, $ENV{PKG_CONFIG_PATH} if $ENV{PKG_CONFIG_PATH};
	$ENV{PKG_CONFIG_PATH} = join(':', @pkgconfigdirs);
}

sub setup_ld_library_env {
	my @ld_paths;
	push @ld_paths, map { "$builddir/$_/.libs" } @packages unless $no_local;
	push @ld_paths, $libdir;
	push @ld_paths, $ENV{LD_LIBRARY_PATH} if $ENV{LD_LIBRARY_PATH};
	$ENV{LD_LIBRARY_PATH} = join(':', @ld_paths);
}

sub setup_ccache_env {
	return unless $ccache;
	$ENV{PATH} = join(':', '/usr/lib/ccache/bin', $ENV{PATH});
	$ENV{CCACHE_DIR} = easy_mkdir(File::Spec->catdir($topdir, '.ccache'))
		unless $ENV{CCACHE_DIR} && -d $ENV{CCACHE_DIR};
}

sub pretty_env_paths {
	my $value = $_[0] || '';
	$value =~ s/$topdir/\${topdir}/g;
	return $value;
}
sub show_env {
	return unless $show_env;
	my @envvars = qw( CXX CPP CXXFLAGS CPPFLAGS LDFLAGS PATH );
	push @envvars, qw( PKG_CONFIG_PATH ACLOCAL_FLAGS LD_LIBRARY_PATH )
		unless $pkg;
	push @envvars, 'CCACHE_DIR' if $ccache;
	my @env = map { "\n\t$_=" . pretty_env_paths($ENV{$_}) } @envvars;
	my $target_label = $pkg || 'all targets';
	print "Build environment for $target_label: @env\n";
}

setup_aclocal_env();
setup_pkgconfig_env();
setup_ld_library_env();
setup_ccache_env();
show_env();

# special pre-target processing
try_callback('pre', $_) for @action;

create_working_paths();

for $pkg ( @targets ) {
	# XXX: be afraid! dynamic scoping... icky icky icky, but so darn handy
	local $pkg = $pkg;
	local $srcdir = File::Spec->catdir($topdir, $pkg);
	local $objdir = File::Spec->catdir($builddir, $pkg);
	easy_mkdir($objdir);

	print "+Source directory: ", pretty_path($srcdir), "\n",
		"+Object directory: ", pretty_path($objdir), "\n" if $verbose;

	try_callback($pkg, 'pre');
	foreach my $a (@action) {
		callact($a);
	}
	try_callback($pkg, 'post');
}

# special post-target processing
try_callback('post', $_) for @action;
