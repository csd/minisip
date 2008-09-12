%define name		minisip
%define version		0.7.2
%define release		1

Summary: 		A secure SIP user agent
Name:			%{name}
Version:		%{version}
Release:		%{release}
Packager:		Erik Eliasson <ere@kth.se>
License:		GPL
URL:			http://www.minisip.org/
Group:			Applications/Communication
Source:			http://www.minisip.org/source/%{name}-%{version}.tar.gz
BuildRoot:		%_tmppath/%name-%version-%release-root
Requires:		libmikey0 >= 0.8.0
Requires:		libmutil0 >= 0.8.0
Requires:		libmnetutil0 >= 0.8.0
Requires:		libmsip0 >= 0.8.0
Requires:		libglademm2 >= 0.2 


%description
minisip is a SIP user agent. It implements additional security features,
which allow end-to-end authentication, and protection of the media stream.

%prep
%setup -q


%build
%configure
make

%install
%makeinstall

%clean
rm -rf %buildroot

%files
%defattr(-,root,root,-)
%doc AUTHORS README COPYING ChangeLog
/usr/bin/minisip
/usr/share/minisip/minisip.glade
/usr/share/minisip/tray_icon.png
/usr/share/minisip/secure.png
/usr/share/minisip/insecure.png

%changelog
* Thu Oct 11 2008 Erik Eliasson <ere@kth.se>
- new upstream release
* Fri Mar 17 2005 Johan Bilien <jobi@via.ecp.fr>
- new upstream release
* Fri Feb 17 2005 Johan Bilien <jobi@via.ecp.fr>
- new upstream release
* Mon Nov 22 2004 Johan Bilien <jobi@via.ecp.fr>
- new upstream release
* Thu May 6 2004 Johan Bilien <jobi@via.ecp.fr>
- initial release

