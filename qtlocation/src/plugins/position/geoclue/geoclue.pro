TARGET = qtposition_geoclue
QT += location gui

PLUGIN_TYPE = position
load(qt_plugin)

HEADERS += \
    qgeopositioninfosource_geocluemaster_p.h \
    qgeopositioninfosourcefactory_geoclue.h \
    qgeocluemaster.h

SOURCES += \
    qgeopositioninfosource_geocluemaster.cpp \
    qgeopositioninfosourcefactory_geoclue.cpp \
    qgeocluemaster.cpp

config_geoclue-satellite {
    DEFINES += HAS_SATELLITE

    HEADERS += qgeosatelliteinfosource_geocluemaster.h
    SOURCES += qgeosatelliteinfosource_geocluemaster.cpp
}

INCLUDEPATH += $$QT.location.includes

CONFIG += link_pkgconfig
PKGCONFIG += geoclue

OTHER_FILES += \
    plugin.json \
    plugin-satellite.json
