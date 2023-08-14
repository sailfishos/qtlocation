Name:       qt5-qtlocation-source
Summary:    Cross-platform application and UI framework
Version:    5.4.2
Release:    1
License:    (LGPLv2 or LGPLv3) with exception or GPLv3 or Qt Commercial
URL:        https://github.com/sailfishos/qtlocation
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
BuildRequires:  qt5-qmake
BuildRequires:  qt5-tools
BuildRequires:  qt5-qttools-qthelp-devel
BuildRequires:  fdupes

%description
Qt is a cross-platform application and UI framework. Using Qt, you can
write web-enabled applications once and deploy them across desktop,
mobile and embedded systems without rewriting the source code.


%package -n qt5-qtpositioning
Summary:    The QtPositioning library
Requires(post):     /sbin/ldconfig
Requires(postun):   /sbin/ldconfig

%description -n qt5-qtpositioning
This package contains the QtPositioning library


%package -n qt5-qtpositioning-devel
Summary:    Development files for QtPositioning
Requires:   qt5-qtpositioning = %{version}-%{release}

%description -n qt5-qtpositioning-devel
This package contains the files necessary to develop applications
that use the QtPositioning library

%package -n qt5-qtpositioning-doc
Summary:    Documentation for QtPositioning

%description -n qt5-qtpositioning-doc
%{summary}.

%package -n qt5-plugin-position-poll
Summary:    Qt positioning plugin (pollling)
Requires:   qt5-qtpositioning = %{version}-%{release}
Obsoletes:  qt5-qtlocation-plugin-position-poll <= 5.1.0+git7
Provides:   qt5-qtlocation-plugin-position-poll > 5.1.0+git7

%description -n qt5-plugin-position-poll
This package contains the polling positioning plugin. It provides the
generic polling based area monitor.


%package -n qt5-qtdeclarative-import-positioning
Summary:    QtDeclarative positioning import
Requires:   qt5-qtpositioning = %{version}-%{release}
Requires:   qt5-qtdeclarative

%description -n qt5-qtdeclarative-import-positioning
This package contains the Positioning import for QtDeclarative




%package -n qt5-qtlocation
Summary:    The QtLocation library
Requires:   qt5-qtpositioning = %{version}-%{release}
Requires(post):     /sbin/ldconfig
Requires(postun):   /sbin/ldconfig

%description -n qt5-qtlocation
This package contains the QtLocation library


%package -n qt5-qtlocation-devel
Summary:    Development files for QtLocation
Requires:   qt5-qtlocation = %{version}-%{release}
Requires:   qt5-qtpositioning-devel = %{version}-%{release}

%description -n qt5-qtlocation-devel
This package contains the files necessary to develop
applications that use QtLocation

%package -n qt5-qtlocation-doc
Summary:    Documentation for QtLocation

%description -n qt5-qtlocation-doc
%{summary}.

%package -n qt5-plugin-geoservices-here
Summary:    Qt Geoservices plugin using HERE location services
Requires:   qt5-qtlocation = %{version}-%{release}
Obsoletes:  qt5-qtlocation-plugin-geoservices-nokia
Provides:   qt5-qtlocation-plugin-geoservices-here

%description -n qt5-plugin-geoservices-here
This package contains the geoservices plugin using HERE location services


%package -n qt5-plugin-geoservices-osm
Summary:    Qt Geoservices plugin for OpenStreetMaps
Requires:   qt5-qtlocation = %{version}-%{release}
Obsoletes:  qt5-qtlocation-plugin-geoservices-osm <= 5.1.0+git7
Provides:   qt5-qtlocation-plugin-geoservices-osm > 5.1.0+git7

%description -n qt5-plugin-geoservices-osm
This package contains the geoservices plugin for OpenStreetMaps


%package -n qt5-qtdeclarative-import-location
Summary:    QtDeclarative location import
Requires:   qt5-qtlocation = %{version}-%{release}
Requires:   qt5-qtdeclarative


%description -n qt5-qtdeclarative-import-location
This package contains the Location import for QtDeclarative





%prep
%setup -q -n %{name}-%{version}

%build
touch .git
%qmake5
%make_build
%make_build docs


%install
%qmake5_install
# Remove unneeded .la files
rm -f %{buildroot}/%{_libdir}/*.la
# We don't need qt5/Qt/
rm -rf %{buildroot}/%{_qt5_includedir}/Qt
# Duke libqtposition_geoclue.so
rm -f %{buildroot}/%{_qt5_plugindir}/position/libqtposition_geoclue.so

# Fix wrong path in pkgconfig files
find %{buildroot}%{_libdir}/pkgconfig -type f -name '*.pc' \
-exec perl -pi -e "s, -L%{_builddir}/?\S+,,g" {} \;
# Fix wrong path in prl files
find %{buildroot}%{_libdir} -type f -name '*.prl' \
-exec sed -i -e "/^QMAKE_PRL_BUILD_DIR/d;s/\(QMAKE_PRL_LIBS =\).*/\1/" {} \;

#install docs
mkdir -p %{buildroot}/%{_docdir}/qtlocation/
mkdir -p %{buildroot}/%{_docdir}/qtpositioning/
cp -R doc/qtlocation/* %{buildroot}/%{_docdir}/qtlocation/
cp doc/qtlocation.qch %{buildroot}/%{_docdir}/qtlocation/
cp -R doc/qtpositioning/* %{buildroot}/%{_docdir}/qtpositioning/
cp doc/qtpositioning.qch %{buildroot}/%{_docdir}/qtpositioning/

%fdupes %{buildroot}/%{_includedir}

%post -n qt5-qtpositioning -p /sbin/ldconfig
%postun -n qt5-qtpositioning -p /sbin/ldconfig

%post -n qt5-qtlocation -p /sbin/ldconfig
%postun -n qt5-qtlocation -p /sbin/ldconfig

%files -n qt5-qtpositioning
%defattr(-,root,root,-)
%license LICENSE.LGPLv* LGPL_EXCEPTION.txt
%license LICENSE.FDL LICENSE.GPLv*
%{_qt5_libdir}/libQt5Positioning.so.5
%{_qt5_libdir}/libQt5Positioning.so.5.*

%files -n qt5-qtpositioning-devel
%defattr(-,root,root,-)
%{_qt5_libdir}/libQt5Positioning.so
%{_qt5_libdir}/libQt5Positioning.prl
%{_qt5_libdir}/pkgconfig/Qt5Positioning.pc
%{_qt5_includedir}/QtPositioning
%{_qt5_archdatadir}/mkspecs/modules/qt_lib_positioning.pri
%{_qt5_archdatadir}/mkspecs/modules/qt_lib_positioning_private.pri
%{_qt5_libdir}/cmake/Qt5Positioning/

%files -n qt5-qtpositioning-doc
%defattr(-,root,root,-)
%{_docdir}/qtpositioning/*

%files -n qt5-plugin-position-poll
%defattr(-,root,root,-)
%{_qt5_plugindir}/position/*positionpoll*

%files -n qt5-qtdeclarative-import-positioning
%defattr(-,root,root,-)
%{_qt5_archdatadir}/qml/QtPositioning/



%files -n qt5-qtlocation
%defattr(-,root,root,-)
%{_qt5_libdir}/libQt5Location.so.5
%{_qt5_libdir}/libQt5Location.so.5.*

%files -n qt5-qtlocation-devel
%defattr(-,root,root,-)
%{_qt5_libdir}/libQt5Location.so
%{_qt5_libdir}/libQt5Location.prl
%{_qt5_libdir}/pkgconfig/Qt5Location.pc
%{_qt5_includedir}/QtLocation
%{_qt5_archdatadir}/mkspecs/modules/qt_lib_location.pri
%{_qt5_archdatadir}/mkspecs/modules/qt_lib_location_private.pri
%{_qt5_libdir}/cmake/Qt5Location/

%files -n qt5-qtlocation-doc
%defattr(-,root,root,-)
%{_docdir}/qtlocation/*

%files -n qt5-plugin-geoservices-here
%defattr(-,root,root,-)
%{_qt5_plugindir}/geoservices/*here*

%files -n qt5-plugin-geoservices-osm
%defattr(-,root,root,-)
%{_qt5_plugindir}/plugins/geoservices/*osm*

%files -n qt5-qtdeclarative-import-location
%defattr(-,root,root,-)
%{_qt5_archdatadir}/qml/QtLocation/


