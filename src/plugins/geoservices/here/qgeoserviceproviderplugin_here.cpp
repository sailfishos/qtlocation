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

#include "qgeoserviceproviderplugin_here.h"

#include "geocoding/qgeocodingmanagerengine_here.h"
#include "routing/qgeoroutingmanagerengine_here.h"
#include "maptiles/qgeotiledmappingmanagerengine_here.h"
#include "places/qplacemanagerengine_here.h"

#include "util/qgeointrinsicnetworkaccessmanager.h"
#include "util/qgeoerror_messages.h"

#include <QtPlugin>
#include <QNetworkProxy>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE

namespace
{
    bool isValidParameter(const QString &param)
    {
        if (param.isEmpty())
            return false;

        if (param.length() > 512)
            return false;

        foreach (QChar c, param) {
            if (!c.isLetterOrNumber() && c.toLatin1() != '%' && c.toLatin1() != '-' &&
                c.toLatin1() != '+' && c.toLatin1() != '_') {
                return false;
            }
        }
        return true;
    }

    QGeoNetworkAccessManager *tryGetNetworkAccessManager(const QVariantMap &parameters)
    {
        return static_cast<QGeoNetworkAccessManager *>(qvariant_cast<void *>(parameters.value(QStringLiteral("nam"))));
    }

    void checkUsageTerms(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    {
        const QString appId = parameters.value(QLatin1String("app_id")).toString();
        const QString appCode = parameters.value(QLatin1String("app_code")).toString();

        if (!isValidParameter(appId) || !isValidParameter(appCode)) {
            *error = QGeoServiceProvider::MissingRequiredParameterError;
            *errorString = QCoreApplication::translate(HERE_PLUGIN_CONTEXT_NAME, MISSED_CREDENTIALS);
        }
    }

    template<class TInstance>
    TInstance * CreateInstanceOf(const QVariantMap &parameters, QGeoServiceProvider::Error *error, QString *errorString)
    {
        checkUsageTerms(parameters, error, errorString);

        if (*error != QGeoServiceProvider::NoError)
            return 0;

        QGeoNetworkAccessManager *networkManager = tryGetNetworkAccessManager(parameters);
        if (!networkManager)
            networkManager = new QGeoIntrinsicNetworkAccessManager(parameters);

        return new TInstance(networkManager, parameters, error, errorString);
    }
}

QGeoCodingManagerEngine *QGeoServiceProviderFactoryHere::createGeocodingManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QGeoCodingManagerEngineHere>(parameters, error, errorString);
}

QGeoMappingManagerEngine *QGeoServiceProviderFactoryHere::createMappingManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QGeoTiledMappingManagerEngineHere>(parameters, error, errorString);
}

QGeoRoutingManagerEngine *QGeoServiceProviderFactoryHere::createRoutingManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QGeoRoutingManagerEngineHere>(parameters, error, errorString);
}

QPlaceManagerEngine *QGeoServiceProviderFactoryHere::createPlaceManagerEngine(
        const QVariantMap &parameters,
        QGeoServiceProvider::Error *error,
        QString *errorString) const
{
    return CreateInstanceOf<QPlaceManagerEngineHere>(parameters, error, errorString);
}

QT_END_NAMESPACE
