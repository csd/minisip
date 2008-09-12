%define name		libminisip
%define version		0.8.0
%define release		1

%define major		0
%define lib_name	%{name}%{major}

Summary: 		Application layer library to easily secure VoIP support to applications
Name:			%{name}
Version:		%{version}
Release:		%{release}
Packager:		Erik Eliasson <ere@kth.se>
License:		GPL
URL:			http://www.minisip.org/
Group:			System/Libraries
Source:			http://www.minisip.org/source/%{name}-%{version}.tar.gz
BuildRoot:		%_tmppath/%name-%version-%release-root

%description
Application layer library to easily secure VoIP support to applications

%package -n %{lib_name}
Summary: 		Application layer library to easily create GUI based apps
Group:			System/Libraries
Provides:		%{name}
Requires:       	libmutil0 >= 0.8.0, libmnetutil0 >= 0.8.0, libmikeyl0 >= 0.8.0, libmsipl0 >= 0.8.0, 


%description -n %{lib_name}
Application layer library to easily secure VoIP support to applications


%package -n %{lib_name}-devel
Summary: 		Development files for the libminisip library.
Group:          	Development/C
Provides:       	%name-devel
Requires:       	%{lib_name} = %{version}


%description -n %{lib_name}-devel
Application layer library to easily secure VoIP support to applications

This package includes the development files (headers and static library)

%prep
%setup -q


%build
%configure
make

%install
%makeinstall

%clean
rm -rf %buildroot

%post -n %{lib_name} -p /sbin/ldconfig

%postun -n %{lib_name} -p /sbin/ldconfig
										

%files -n %{lib_name}
%defattr(-,root,root,-)
%doc AUTHORS README COPYING ChangeLog
%{_libdir}/*.so.*

%files -n %{lib_name}-devel
%defattr(-,root,root)
%doc COPYING
%{_libdir}/*a
%{_libdir}/*.so
%{_includedir}/*


%changelog
* Thu Oct 11 2008 Erik Eliasson <ere@kth.se>
- initial release

