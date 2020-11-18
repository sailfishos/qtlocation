TEMPLATE = app
CONFIG += testcase
TARGET = tst_here_routing

QT += network location testlib
INCLUDEPATH += $$PWD/../../../../src/plugins/geoservices/here

HEADERS += $$PWD/../../../../src/plugins/geoservices/here/qgeonetworkaccessmanager.h
SOURCES += tst_routing.cpp

OTHER_FILES += *.xml

TESTDATA = $$OTHER_FILES
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
