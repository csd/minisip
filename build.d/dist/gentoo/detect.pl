# Gentoo autodetection support (yes, it can be this easy!)
set_dist_detect(sub { -e '/etc/gentoo_release' });
