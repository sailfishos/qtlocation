Name:       qt5-plugin-position-geoclue
Summary:    Qt Positioning plugin for Geoclue
Version:    5.4.2
Release:    1%{?dist}
License:    (LGPLv2 or LGPLv3) with exception or GPLv3 or Qt Commercial
URL:        https://www.qt.io/
Source0:    %{name}-%{version}.tar.xz
BuildRequires:  qt5-qtpositioning-devel
BuildRequires:  qt5-qtdbus-devel
BuildRequires:  qt5-qmake
BuildRequires:  qt5-tools
Requires:       geoclue
Obsoletes:      qt5-qtlocation-plugin-position-geoclue <= 5.1.0+git7
Provides:       qt5-qtlocation-plugin-position-geoclue > 5.1.0+git7

%description
This package contains the position plugin for Geoclue


#### Build section

%prep
%setup -q -n %{name}-%{version}

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

# Remove the cmake file
rm %{buildroot}/%{_libdir}/cmake/Qt5Positioning/Qt5Positioning_QGeoPositionInfoSourceFactoryGeoclue.cmake


#### Pre/Post section


#### File section


%files
%defattr(-,root,root,-)
%license LICENSE.LGPLv* LGPL_EXCEPTION.txt
%license LICENSE.FDL LICENSE.GPLv*
%{_libdir}/qt5/plugins/position/*geoclue*


#### No changelog section, separate $pkg.changelog contains the history
