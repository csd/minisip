%define name		minisip
%define version		0.2
%define release		1

Summary: 		A secure SIP user agent
Name:			%{name}
Version:		%{version}
Release:		%{release}
Packager:		Johan Bilien <jobi@via.ecp.fr>
License:		GPL
URL:			http://www.minisip.org/
Group:			Applications/Communication
Source:			http://www.minisip.org/source/%{name}-%{version}.tar.gz
BuildRoot:		%_tmppath/%name-%version-%release-root
Requires:		libmikey1 >= 0.2
Requires:		libmutil1 >= 0.1
Requires:		libmnetutil1 >= 0.1
Requires:		libmsip1 >= 0.1


%description
minisip is a SIP user agent. It implements additional security features,
which allow end-to-end authentication, and protection of the media stream.

%prep
%setup -q


%build
%configure --enable-color-terminal --enable-debug
make

%install
%makeinstall

%clean
rm -rf %buildroot

%files
%defattr(-,root,root,-)
%doc AUTHORS README COPYING ChangeLog
/usr/bin/minisip

%changelog
* Thu May 6 2004 Johan Bilien <jobi@via.ecp.fr>
- initial release

