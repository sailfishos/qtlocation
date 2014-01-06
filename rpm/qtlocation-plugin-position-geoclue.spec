Name:       qt5-plugin-position-geoclue
Summary:    Qt Positioning plugin for Geoclue
Version:    5.0.0
Release:    1%{?dist}
Group:      Qt/Qt
License:    LGPLv2.1 with exception or GPLv3
URL:        http://qt-project.org/
Source0:    %{name}-%{version}.tar.xz
BuildRequires:  qt5-qtpositioning-devel
BuildRequires:  qt5-qtdbus-devel
BuildRequires:  qt5-qmake
BuildRequires:  qt5-tools
BuildRequires:  geoclue-devel
Obsoletes:      qt5-qtlocation-plugin-position-geoclue <= 5.1.0+git7
Provides:       qt5-qtlocation-plugin-position-geoclue > 5.1.0+git7

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


#### File section


%files
%defattr(-,root,root,-)
%{_libdir}/qt5/plugins/position/*geoclue*


#### No changelog section, separate $pkg.changelog contains the history
