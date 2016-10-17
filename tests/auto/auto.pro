TEMPLATE = subdirs

qtHaveModule(location) {

    #Place unit tests
    SUBDIRS += qplace \
           qplaceattribute \
           qplacecategory \
           qplacecontactdetail \
           qplacecontentrequest \
           qplacedetailsreply \
           qplaceeditorial \
           qplacematchreply \
           qplacematchrequest \
           qplaceimage \
           qplaceratings \
           qplaceresult \
           qproposedsearchresult \
           qplacereply \
           qplacereview \
           qplacesearchrequest \
           qplacesupplier \
           qplacesearchresult \
           qplacesearchreply \
           qplacesearchsuggestionreply \
           qplaceuser \
           qplacemanager \
           qplacemanager_here \
           qplacemanager_unsupported \
           placesplugin_unsupported

    #misc tests
    SUBDIRS +=  qmlinterface \
           cmake \
           doublevectors

    #Map and Navigation tests
    SUBDIRS += geotestplugin \
           qgeocodingmanagerplugins \
           qgeocameracapabilities\
           qgeocameradata \
           qgeocodereply \
           qgeocodingmanager \
           qgeomaneuver \
           qgeomapscene \
           qgeoroute \
           qgeoroutereply \
           qgeorouterequest \
           qgeoroutesegment \
           qgeoroutingmanager \
           qgeoroutingmanagerplugins \
           qgeotilespec \
           qgeoroutexmlparser \
           qgeomapcontroller \
           maptype \
           here_services \
           qgeocameratiles

    qtHaveModule(quick) {
        SUBDIRS += declarative_core \
                declarative_geoshape

        !mac: SUBDIRS += declarative_ui
    }
}


SUBDIRS += \
           positionplugin \
           positionplugintest \
           qgeoaddress \
           qgeoareamonitor \
           qgeoshape \
           qgeorectangle \
           qgeocircle \
           qgeocoordinate \
           qgeolocation \
           qgeopositioninfo \
           qgeopositioninfosource \
           qgeosatelliteinfo \
           qgeosatelliteinfosource \
           qnmeapositioninfosource
