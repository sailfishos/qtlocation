/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QROUTEXMLPARSER_H
#define QROUTEXMLPARSER_H

#include <QtLocation/QGeoManeuver>
#include <QtLocation/QGeoRouteSegment>
#include <QtLocation/QGeoRouteRequest>

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>

QT_BEGIN_NAMESPACE
class QIODevice;
class QXmlStreamReader;
class QGeoRoute;
class QGeoCoordinate;
class QGeoRectangle;

class QGeoManeuverContainer
{
public:
    QGeoManeuver maneuver;
    QString id;
    QString toId;
};

class QGeoRouteSegmentContainer
{
public:
    QGeoRouteSegment segment;
    QString id;
    QString maneuverId;
};

class QGeoDynamicSpeedInfoContainer
{
public:
    QGeoDynamicSpeedInfoContainer();

public:
    double trafficSpeed;
    double baseSpeed;
    int trafficTime;
    int baseTime;
};

class QGeoRouteXmlParser
{
public:
    QGeoRouteXmlParser(const QGeoRouteRequest &request);
    ~QGeoRouteXmlParser();

    bool parse(QIODevice *source);
    QList<QGeoRoute> results() const;
    QString errorString() const;

private:
    bool parseRootElement();
    bool parseRoute(QGeoRoute *route);
    //bool parseWaypoint(QGeoRoute *route);
    bool parseCoordinates(QGeoCoordinate &coord);
    bool parseMode(QGeoRoute *route);
    bool parseSummary(QGeoRoute *route);
    bool parseGeoPoints(const QString &strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName);
    bool parseLeg();
    bool parseManeuver();
    bool parseLink();
    bool postProcessRoute(QGeoRoute *route);

    bool parseBoundingBox(QGeoRectangle &bounds);
    bool parseDynamicSpeedInfo(QGeoDynamicSpeedInfoContainer &speedInfo);

    QGeoRouteRequest m_request;
    QScopedPointer<QXmlStreamReader> m_reader;
    QList<QGeoRoute> m_results;
    QString m_errorString;
    QList<QGeoManeuverContainer> m_maneuvers;
    QList<QGeoRouteSegmentContainer> m_segments;
};

QT_END_NAMESPACE

#endif
