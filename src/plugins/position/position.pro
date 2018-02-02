TEMPLATE = subdirs

qtHaveModule(dbus):SUBDIRS += geoclue
config_gypsy:SUBDIRS += gypsy
simulator:SUBDIRS += simulator
blackberry:SUBDIRS += blackberry

SUBDIRS += \
    positionpoll
