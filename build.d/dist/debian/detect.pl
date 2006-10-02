# Debian autodetection support
set_dist_detect(sub { -e '/etc/debian_version' || -1 });
