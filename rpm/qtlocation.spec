Name:       qt5-qtlocation-source
Summary:    Cross-platform application and UI framework
Version:    5.0.0
Release:    1%{?dist}
Group:      Qt/Qt
License:    LGPLv2.1 with exception or GPLv3
URL:        http://qt-project.org/
Source0:    %{name}-%{version}.tar.xz
BuildRequires:  qt5-qtcore
BuildRequires:  qt5-qtcore-devel
BuildRequires:  qt5-qtgui
BuildRequires:  qt5-qtgui-devel
BuildRequires:  qt5-qtwidgets
BuildRequires:  qt5-qtwidgets-devel
BuildRequires:  qt5-qtopengl
BuildRequires:  qt5-qtopengl-devel
BuildRequires:  qt5-qtnetwork
BuildRequires:  qt5-qtnetwork-devel
BuildRequires:  qt5-qtdeclarative-devel
BuildRequires:  qt5-qtdeclarative-qtquick-devel
BuildRequires:  qt5-qt3d-devel
BuildRequires:  qt5-qmake
BuildRequires:  qt5-qtv8-devel
BuildRequires:  qt5-tools
BuildRequires:  fdupes

%description
Qt is a cross-platform application and UI framework. Using Qt, you can
write web-enabled applications once and deploy them across desktop,
mobile and embedded systems without rewriting the source code.


%package -n qt5-qtpositioning
Summary:    The QtPositioning library
Group:      Qt/Qt
Requires(post):     /sbin/ldconfig
Requires(postun):   /sbin/ldconfig

%description -n qt5-qtpositioning
This package contains the QtPositioning library


%package -n qt5-qtpositioning-devel
Summary:    Development files for QtPositioning
Group:      Qt/Qt
Requires:   qt5-qtpositioning = %{version}-%{release}

%description -n qt5-qtpositioning-devel
This package contains the files necessary to develop applications
that use the QtPositioning library


%package -n qt5-plugin-position-poll
Summary:    Qt positioning plugin (pollling)
Group:      Qt/Qt
Requires:   qt5-qtpositioning = %{version}-%{release}
Obsoletes:  qt5-qtlocation-plugin-position-poll <= 5.1.0+git7
Provides:   qt5-qtlocation-plugin-position-poll > 5.1.0+git7

%description -n qt5-plugin-position-poll
This package contains the polling positioning plugin. It provides the
generic polling based area monitor.


%package -n qt5-qtdeclarative-import-positioning
Summary:    QtDeclarative positioning import
Group:      Qt/Qt
Requires:   qt5-qtpositioning = %{version}-%{release}
Requires:   qt5-qtdeclarative

%description -n qt5-qtdeclarative-import-positioning
This package contains the Positioning import for QtDeclarative




%package -n qt5-qtlocation
Summary:    The QtLocation library
Group:      Qt/Qt
Requires:   qt5-qtpositioning = %{version}-%{release}
Requires(post):     /sbin/ldconfig
Requires(postun):   /sbin/ldconfig

%description -n qt5-qtlocation
This package contains the QtLocation library


%package -n qt5-qtlocation-devel
Summary:    Development files for QtLocation
Group:      Qt/Qt
Requires:   qt5-qtlocation = %{version}-%{release}
Requires:   qt5-qtpositioning-devel = %{version}-%{release}

%description -n qt5-qtlocation-devel
This package contains the files necessary to develop
applications that use QtLocation


%package -n qt5-plugin-geoservices-nokia
Summary:    Qt Geoservices plugin for Nokia devices
Group:      Qt/Qt
Requires:   qt5-qtlocation = %{version}-%{release}
Obsoletes:  qt5-qtlocation-plugin-geoservices-nokia <= 5.1.0+git7
Provides:   qt5-qtlocation-plugin-geoservices-nokia > 5.1.0+git7

%description -n qt5-plugin-geoservices-nokia
This package contains the geoservices plugin for Nokia devices


%package -n qt5-plugin-geoservices-osm
Summary:    Qt Geoservices plugin for OpenStreetMaps
Group:      Qt/Qt
Requires:   qt5-qtlocation = %{version}-%{release}
Obsoletes:  qt5-qtlocation-plugin-geoservices-osm <= 5.1.0+git7
Provides:   qt5-qtlocation-plugin-geoservices-osm > 5.1.0+git7

%description -n qt5-plugin-geoservices-osm
This package contains the geoservices plugin for OpenStreetMaps


%package -n qt5-qtdeclarative-import-location
Summary:    QtDeclarative location import
Group:      Qt/Qt
Requires:   qt5-qtlocation = %{version}-%{release}
Requires:   qt5-qtdeclarative


%description -n qt5-qtdeclarative-import-location
This package contains the Location import for QtDeclarative





%prep
%setup -q -n %{name}-%{version}

%build
export QTDIR=/usr/share/qt5
touch .git
%qmake5
make %{?_smp_mflags}


%install
rm -rf %{buildroot}
%qmake5_install
# Remove unneeded .la files
rm -f %{buildroot}/%{_libdir}/*.la
# We don't need qt5/Qt/
rm -rf %{buildroot}/%{_includedir}/qt5/Qt

# Fix wrong path in pkgconfig files
find %{buildroot}%{_libdir}/pkgconfig -type f -name '*.pc' \
-exec perl -pi -e "s, -L%{_builddir}/?\S+,,g" {} \;
# Fix wrong path in prl files
find %{buildroot}%{_libdir} -type f -name '*.prl' \
-exec sed -i -e "/^QMAKE_PRL_BUILD_DIR/d;s/\(QMAKE_PRL_LIBS =\).*/\1/" {} \;

%fdupes %{buildroot}/%{_includedir}



%post -n qt5-qtpositioning -p /sbin/ldconfig
%postun -n qt5-qtpositioning -p /sbin/ldconfig

%post -n qt5-qtlocation -p /sbin/ldconfig
%postun -n qt5-qtlocation -p /sbin/ldconfig



%files -n qt5-qtpositioning
%defattr(-,root,root,-)
%{_libdir}/libQt5Positioning.so.5
%{_libdir}/libQt5Positioning.so.5.*

%files -n qt5-qtpositioning-devel
%defattr(-,root,root,-)
%{_libdir}/libQt5Positioning.so
%{_libdir}/libQt5Positioning.prl
%{_libdir}/pkgconfig/Qt5Positioning.pc
%{_includedir}/qt5/QtPositioning
%{_datadir}/qt5/mkspecs/modules/qt_lib_positioning.pri
%{_datadir}/qt5/mkspecs/modules/qt_lib_positioning_private.pri
%{_libdir}/cmake/Qt5Positioning/

%files -n qt5-plugin-position-poll
%defattr(-,root,root,-)
%{_libdir}/qt5/plugins/position/*positionpoll*

%files -n qt5-qtdeclarative-import-positioning
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/QtPositioning/



%files -n qt5-qtlocation
%defattr(-,root,root,-)
%{_libdir}/libQt5Location.so.5
%{_libdir}/libQt5Location.so.5.*

%files -n qt5-qtlocation-devel
%defattr(-,root,root,-)
%{_libdir}/libQt5Location.so
%{_libdir}/libQt5Location.prl
%{_libdir}/pkgconfig/Qt5Location.pc
%{_includedir}/qt5/QtLocation
%{_datadir}/qt5/mkspecs/modules/qt_lib_location.pri
%{_datadir}/qt5/mkspecs/modules/qt_lib_location_private.pri
%{_libdir}/cmake/Qt5Location/

%files -n qt5-plugin-geoservices-nokia
%defattr(-,root,root,-)
%{_libdir}/qt5/plugins/geoservices/*nokia*

%files -n qt5-plugin-geoservices-osm
%defattr(-,root,root,-)
%{_libdir}/qt5/plugins/geoservices/*osm*

%files -n qt5-qtdeclarative-import-location
%defattr(-,root,root,-)
%{_libdir}/qt5/qml/QtLocation/


