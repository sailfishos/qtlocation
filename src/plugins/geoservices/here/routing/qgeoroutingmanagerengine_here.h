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

#ifndef QGEOROUTINGMANAGER_HERE_H
#define QGEOROUTINGMANAGER_HERE_H

#include <QtLocation/QGeoServiceProvider>
#include <QtLocation/QGeoRoutingManagerEngine>

QT_BEGIN_NAMESPACE

class QGeoNetworkAccessManager;
class QGeoUriProvider;

class QGeoRoutingManagerEngineHere : public QGeoRoutingManagerEngine
{
    Q_OBJECT
public:
    QGeoRoutingManagerEngineHere(QGeoNetworkAccessManager *networkInterface,
                                  const QMap<QString, QVariant> &parameters,
                                  QGeoServiceProvider::Error *error,
                                  QString *errorString);
    ~QGeoRoutingManagerEngineHere();

    QGeoRouteReply *calculateRoute(const QGeoRouteRequest &request);
    QGeoRouteReply *updateRoute(const QGeoRoute &route, const QGeoCoordinate &position);

private Q_SLOTS:
    void routeFinished();
    void routeError(QGeoRouteReply::Error error, const QString &errorString);

private:
    QStringList calculateRouteRequestString(const QGeoRouteRequest &request);
    QStringList updateRouteRequestString(const QGeoRoute &route, const QGeoCoordinate &position);
    QString routeRequestString(const QGeoRouteRequest &request) const;
    bool checkEngineSupport(const QGeoRouteRequest &request,
                            QGeoRouteRequest::TravelModes travelModes) const;
    QString modesRequestString(const QGeoRouteRequest &request,
                               QGeoRouteRequest::TravelModes travelModes,
                               const QString &optimization) const;
    static QString trimDouble(double degree, int decimalDigits = 10);

    QGeoNetworkAccessManager *m_networkManager;
    QGeoUriProvider *m_uriProvider;
    QString m_appId;
    QString m_appCode;
};

QT_END_NAMESPACE

#endif
