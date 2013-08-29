Name:       qt5-qtlocation-plugin-position-geoclue
Summary:    Qt Positioning plugin for Geoclue
Version:    5.0.0
Release:    1%{?dist}
Group:      Qt/Qt
License:    LGPLv2.1 with exception or GPLv3
URL:        http://qt-project.org/
Source0:    %{name}-%{version}.tar.xz
BuildRequires:  qt5-qtlocation-devel
BuildRequires:  qt5-qtgui-devel
BuildRequires:  qt5-qmake
BuildRequires:  geoclue-devel
BuildRequires:  gconf-devel

%description
This package contains the position plugin for Geoclue


#### Build section

%prep
%setup -q -n %{name}-%{version}/qtlocation

%build
export QTDIR=/usr/share/qt5
touch .git

# Run qmake in project root to run config tests
qmake -qt=5
# Build only the Geoclue plugin
cd src/plugins/position/geoclue
qmake -qt=5
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
cd src/plugins/position/geoclue
%qmake5_install


#### Pre/Post section

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig


#### File section


%files
%defattr(-,root,root,-)
%{_libdir}/qt5/plugins/position/*geoclue*


#### No changelog section, separate $pkg.changelog contains the history
