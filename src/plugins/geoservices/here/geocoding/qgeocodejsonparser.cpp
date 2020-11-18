/****************************************************************************
**
** Copyright (C) 2016 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
** This file is part of the HERE geoservices plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file HERE_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the HERE services
** plugin source code.
**
****************************************************************************/

#include "geocoding/qgeocodejsonparser.h"

#include <QtPositioning/QGeoShape>
#include <QtPositioning/QGeoRectangle>
#include <QtPositioning/QGeoCircle>
#include <QtPositioning/QGeoLocation>
#include <QtPositioning/QGeoAddress>
#include <QtPositioning/QGeoCoordinate>

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonParseError>
#include <QtCore/QVariantMap>
#include <QtCore/QIODevice>

#include <QtDebug>

QT_BEGIN_NAMESPACE

namespace {

/*
    Checks that the given Location object contains the information
    we need and is not malformed in any way.  We expect a Location
    object of the following form:

    "Location": {
        "Address": {
            "AdditionalData": [
                {
                    "key": "CountryName",
                    "value": "Australia"
                },
                {
                    "key": "StateName",
                    "value": "New South Wales"
                }
            ],
            "City": "Sydney",
            "Country": "AUS",
            "District": "Casula",
            "Label": "Casula, Sydney, NSW, Australia",
            "PostalCode": "2170",
            "State": "NSW"
        },
        "DisplayPosition": {
            "Latitude": -33.949509999999997,
            "Longitude": 150.90386000000001
        },
        "LocationId": "NT_5UQ89lKoiI4DIYbOrIR0-D",
        "LocationType": "area",
        "MapReference": {
            "CityId": "1469266800",
            "CountryId": "1469256839",
            "DistrictId": "1469267758",
            "MapId": "NXAM16130",
            "MapReleaseDate": "2016-10-05",
            "MapVersion": "Q1/2016",
            "ReferenceId": "868383156",
            "SideOfStreet": "neither",
            "StateId": "1469256831"
        },
        "MapView": {
            "BottomRight": {
                "Latitude": -33.966839999999998,
                "Longitude": 150.91875999999999
            },
            "TopLeft": {
                "Latitude": -33.937440000000002,
                "Longitude": 150.87457000000001
            }
        }
    }

*/
bool checkLocation(const QJsonObject &loc, QString *errorString)
{
    QJsonObject::const_iterator ait = loc.constFind(QLatin1String("Address"));
    if (ait == loc.constEnd()) {
        *errorString = QLatin1String("Expected Address element within Location object");
        return false;
    } else if (!ait.value().isObject()) {
        *errorString = QLatin1String("Expected Address object within Location object");
        return false;
    }

    QJsonObject::const_iterator dpit = loc.constFind(QLatin1String("DisplayPosition"));
    if (dpit == loc.constEnd()) {
        *errorString = QLatin1String("Expected DisplayPosition element within Location object");
        return false;
    } else if (!dpit.value().isObject()) {
        *errorString = QLatin1String("Expected DisplayPosition object within Location object");
        return false;
    }
    QJsonObject displayPosition = dpit.value().toObject();
    QJsonObject::const_iterator latit = displayPosition.constFind(QLatin1String("Latitude"));
    if (latit == displayPosition.constEnd()) {
        *errorString = QLatin1String("Expected Latitude element within Location.DisplayPosition object");
        return false;
    } else if (!latit.value().isDouble()) {
        *errorString = QLatin1String("Expected Latitude double within Location.DisplayPosition object");
        return false;
    }
    QJsonObject::const_iterator lonit = displayPosition.constFind(QLatin1String("Longitude"));
    if (lonit == displayPosition.constEnd()) {
        *errorString = QLatin1String("Expected Longitude element within Location.DisplayPosition object");
        return false;
    } else if (!lonit.value().isDouble()) {
        *errorString = QLatin1String("Expected Longitude double within Location.DisplayPosition object");
        return false;
    }

    QJsonObject::const_iterator mvit = loc.constFind(QLatin1String("MapView"));
    if (mvit == loc.constEnd()) {
        *errorString = QLatin1String("Expected MapView element within Location object");
        return false;
    } else if (!mvit.value().isObject()) {
        *errorString = QLatin1String("Expected MapView object within Location object");
        return false;
    }
    QJsonObject mapView = mvit.value().toObject();
    QJsonObject::const_iterator brit = mapView.constFind(QLatin1String("BottomRight"));
    if (brit == mapView.constEnd()) {
        *errorString = QLatin1String("Expected BottomRight element within Location.MapView object");
        return false;
    } else if (!brit.value().isObject()) {
        *errorString = QLatin1String("Expected BottomRight object within Location.MapView object");
        return false;
    }
    QJsonObject bottomRight = brit.value().toObject();
    QJsonObject::const_iterator brlatit = bottomRight.constFind(QLatin1String("Latitude"));
    if (brlatit == bottomRight.constEnd()) {
        *errorString = QLatin1String("Expected Latitude element within Location.MapView.BottomRight object");
        return false;
    } else if (!brlatit.value().isDouble()) {
        *errorString = QLatin1String("Expected Latitude double within Location.MapView.BottomRight object");
        return false;
    }
    QJsonObject::const_iterator brlonit = bottomRight.constFind(QLatin1String("Longitude"));
    if (brlonit == bottomRight.constEnd()) {
        *errorString = QLatin1String("Expected Longitude element within Location.MapView.BottomRight object");
        return false;
    } else if (!brlonit.value().isDouble()) {
        *errorString = QLatin1String("Expected Longitude double within Location.MapView.BottomRight object");
        return false;
    }
    QJsonObject::const_iterator tlit = mapView.constFind(QLatin1String("TopLeft"));
    if (tlit == mapView.constEnd()) {
        *errorString = QLatin1String("Expected TopLeft element within Location.MapView object");
        return false;
    } else if (!tlit.value().isObject()) {
        *errorString = QLatin1String("Expected TopLeft object within Location.MapView object");
        return false;
    }
    QJsonObject topLeft = tlit.value().toObject();
    QJsonObject::const_iterator tllatit = topLeft.constFind(QLatin1String("Latitude"));
    if (tllatit == topLeft.constEnd()) {
        *errorString = QLatin1String("Expected Latitude element within Location.MapView.TopLeft object");
        return false;
    } else if (!tllatit.value().isDouble()) {
        *errorString = QLatin1String("Expected Latitude double within Location.MapView.TopLeft object");
        return false;
    }
    QJsonObject::const_iterator tllonit = topLeft.constFind(QLatin1String("Longitude"));
    if (tllonit == bottomRight.constEnd()) {
        *errorString = QLatin1String("Expected Longitude element within Location.MapView.TopLeft object");
        return false;
    } else if (!tllonit.value().isDouble()) {
        *errorString = QLatin1String("Expected Longitude double within Location.MapView.TopLeft object");
        return false;
    }

    return true;
}

/*
    Checks that the given document contains the required information
    and is not malformed in any way.  We expect a document like the
    following:

    {
        "Response": {
            "MetaInfo": {
                "Timestamp": "2016-10-18T08:42:04.369+0000"
            },
            "View": [
                {
                    "ViewId": 0,
                    "_type": "SearchResultsViewType",
                    "Result": [
                        {
                            "Direction": 72.099999999999994,
                            "Distance": -1885.2,
                            "Location": {
                                // OMITTED FOR BREVITY
                            },
                            "MatchLevel": "district",
                            "MatchQuality": {
                                "City": 1,
                                "Country": 1,
                                "District": 1,
                                "PostalCode": 1,
                                "State": 1
                            },
                            "Relevance": 1
                        }
                    ]
                }
            ]
        }
    }
*/
bool checkDocument(const QJsonDocument &doc, QString *errorString)
{
    if (!doc.isObject()) {
        *errorString = QLatin1String("Expected JSON document containing object");
        return false;
    }

    QJsonObject rootObject = doc.object();
    QJsonObject::const_iterator it = rootObject.constFind(QLatin1String("Response"));
    if (it == rootObject.constEnd()) {
        *errorString = QLatin1String("Expected Response element within root object");
        return false;
    } else if (!it.value().isObject()) {
        *errorString = QLatin1String("Expected Response object within root object");
        return false;
    }

    QJsonObject response = it.value().toObject();
    QJsonObject::const_iterator rit = response.constFind(QLatin1String("View"));
    if (rit == response.constEnd()) {
        *errorString = QLatin1String("Expected View element within Response object");
        return false;
    } else if (!rit.value().isArray()) {
        *errorString = QLatin1String("Expected View array within Response object");
        return false;
    }

    QJsonArray view = rit.value().toArray();
    for (const QJsonValue &viewElement : view) {
        if (!viewElement.isObject()) {
            *errorString = QLatin1String("Expected View array element to be object");
            return false;
        }

        QJsonObject viewObject = viewElement.toObject();
        QJsonObject::const_iterator voit = viewObject.constFind(QLatin1String("Result"));
        if (voit == viewObject.constEnd()) {
            *errorString = QLatin1String("Expected Result element within View array object element");
            return false;
        } else if (!voit.value().isArray()) {
            *errorString = QLatin1String("Expected Result array within View array object element");
            return false;
        }

        QJsonArray result = voit.value().toArray();
        for (const QJsonValue &resultElement : result) {
            if (!resultElement.isObject()) {
                *errorString = QLatin1String("Expected Result array element to be object");
                return false;
            }

            QJsonObject resultObject = resultElement.toObject();
            QJsonObject::const_iterator roit = resultObject.constFind("Location");
            if (roit == resultObject.constEnd()) {
                *errorString = QLatin1String("Expected Location element in Result array element object");
                return false;
            } else if (!roit.value().isObject()) {
                *errorString = QLatin1String("Expected Location object in Result array element object");
                return false;
            }

            QJsonObject location = roit.value().toObject();
            if (!checkLocation(location, errorString)) {
                return false;
            }
        }
    }

    return true;
}

bool parseLocation(const QJsonObject &obj, QGeoLocation *loc)
{
    QGeoAddress address;
    QJsonObject addr = obj.value("Address").toObject();
    address.setCountryCode(addr.value("Country").toString());
    address.setState(addr.value("State").toString());
    address.setCounty(addr.value("County").toString());
    address.setCity(addr.value("City").toString());
    address.setDistrict(addr.value("District").toString());
    QString houseNumber = addr.value("HouseNumber").toString();
    QString street = addr.value("Street").toString();
    address.setStreet(houseNumber.isEmpty() ? street : QString("%1 %2").arg(houseNumber, street));
    address.setPostalCode(addr.value("PostalCode").toString());
    QString label = addr.value("Label").toString().trimmed();
    if (!label.isEmpty()) {
        address.setText(label);
    }
    QJsonArray additionalData = addr.value("AdditionalData").toArray();
    for (const QJsonValue &adv : additionalData) {
        if (adv.isObject()) {
            const QJsonObject &ado(adv.toObject());
            if (ado.value("key").toString() == QLatin1String("CountryName")) {
                address.setCountry(ado.value("value").toString());
            }
        }
    }

    QJsonObject displayPosition = obj.value("DisplayPosition").toObject();
    QGeoCoordinate coordinate = QGeoCoordinate(displayPosition.value("Latitude").toDouble(), displayPosition.value("Longitude").toDouble());

    QGeoRectangle boundingBox;
    QJsonObject mapView = obj.value("MapView").toObject();
    QJsonObject bottomRight = mapView.value("BottomRight").toObject();
    QJsonObject topLeft = mapView.value("TopLeft").toObject();
    boundingBox.setBottomRight(QGeoCoordinate(bottomRight.value("Latitude").toDouble(), bottomRight.value("Longitude").toDouble()));
    boundingBox.setTopLeft(QGeoCoordinate(topLeft.value("Latitude").toDouble(), topLeft.value("Longitude").toDouble()));

    loc->setAddress(address);
    loc->setCoordinate(coordinate);
    loc->setBoundingBox(boundingBox);

    return true;
}

bool parseDocument(const QJsonDocument &doc, QList<QGeoLocation> *locs)
{
    QJsonArray view = doc.object().value("Response").toObject().value("View").toArray();
    for (const QJsonValue &viewElement : view) {
        QJsonArray result = viewElement.toObject().value("Result").toArray();
        for (const QJsonValue &resultElement : result) {
            QGeoLocation location;
            if (parseLocation(resultElement.toObject().value("Location").toObject(), &location)) {
                locs->append(location);
            }
        }
    }

    return true;
}

} // namespace

QGeoCodeJsonParser::QGeoCodeJsonParser()
{
}

QGeoCodeJsonParser::~QGeoCodeJsonParser()
{
}

bool QGeoCodeJsonParser::parse(QIODevice *source)
{
    // parse the document.
    QJsonParseError error;
    QByteArray data = source->readAll();
    m_document = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        m_errorString = error.errorString();
        return false;
    }

    // ensure that the response is valid and contains the information we need.
    if (!checkDocument(m_document, &m_errorString)) {
        return false;
    }

    // extract the location results from the response.
    return parseDocument(m_document, &m_results);
}

QList<QGeoLocation> QGeoCodeJsonParser::results() const
{
    return m_results;
}

QString QGeoCodeJsonParser::errorString() const
{
    return m_errorString;
}

QT_END_NAMESPACE
