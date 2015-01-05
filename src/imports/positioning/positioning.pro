QT += quick-private positioning-private qml-private core-private

INCLUDEPATH *= $$PWD
DEFINES += TOUCH_EVENT_WORKAROUND

HEADERS += qdeclarativeposition_p.h \
           qdeclarativepositionsource_p.h \
           locationsingleton.h \
           error_messages.h \
           qdeclarativegeocoordinateanimation_p.h

SOURCES += qdeclarativeposition.cpp \
           positioning.cpp \
           qdeclarativepositionsource.cpp \
           locationsingleton.cpp \
           error_messages.cpp \
           qdeclarativegeocoordinateanimation.cpp

load(qml_plugin)

OTHER_FILES += \
    plugin.json \
    qmldir
