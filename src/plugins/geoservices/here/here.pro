TARGET = qtgeoservices_here
QT += location-private positioning-private network

PLUGIN_TYPE = geoservices
load(qt_plugin)

include(geocoding/geocoding.pri)
include(maptiles/maptiles.pri)
include(places/places.pri)
include(routing/routing.pri)
include(util/util.pri)

HEADERS += qgeoserviceproviderplugin_here.h
SOURCES += qgeoserviceproviderplugin_here.cpp

RESOURCES += resource.qrc

INCLUDEPATH += $$QT.location.includes
INCLUDEPATH += ../../../location/maps

OTHER_FILES += here_plugin.json
OTHER_FILES += HERE_TERMS_AND_CONDITIONS.txt HERE_TERMS_AND_CONDITIONS.txt.qdocinc
