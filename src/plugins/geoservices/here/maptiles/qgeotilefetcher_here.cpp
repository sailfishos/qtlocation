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

#include "maptiles/qgeotilefetcher_here.h"
#include "maptiles/qgeotiledmapdata_here.h"
#include "maptiles/qgeomapreply_here.h"
#include "maptiles/qgeotiledmappingmanagerengine_here.h"

#include "util/qgeonetworkaccessmanager.h"
#include "util/qgeouriprovider.h"
#include "util/uri_constants.h"

#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QSize>
#include <QDir>
#include <QUrl>
#include <QTime>

#include <map>

QT_BEGIN_NAMESPACE

namespace
{
    QString sizeToStr(const QSize &size)
    {
        if (size.height() >= 512 || size.width() >= 512)
            return QStringLiteral("512");
        else if (size.height() >= 256 || size.width() >= 256)
            return QStringLiteral("256");
        else
            return QStringLiteral("128");   // 128 pixel tiles are deprecated.
    }

    bool isAerialType(const QString mapScheme)
    {
        return mapScheme.startsWith("satellite") || mapScheme.startsWith("hybrid") || mapScheme.startsWith("terrain");
    }
}
QGeoTileFetcherHere::QGeoTileFetcherHere(
        const QMap<QString, QVariant> &parameters,
        QGeoNetworkAccessManager *networkManager,
        QGeoTiledMappingManagerEngine *engine,
        const QSize &tileSize)
        : QGeoTileFetcher(engine),
          m_engineHere(static_cast<QGeoTiledMappingManagerEngineHere *>(engine)),
          m_networkManager(networkManager),
          m_parameters(parameters),
          m_tileSize(tileSize),
          m_copyrightsReply(0),
          m_baseUriProvider(new QGeoUriProvider(this, parameters, "mapping.host", MAP_TILES_HOST)),
          m_aerialUriProvider(new QGeoUriProvider(this, parameters, "mapping.host.aerial", MAP_TILES_HOST_AERIAL))
{
    Q_ASSERT(networkManager);
    m_networkManager->setParent(this);

    qsrand((uint)QTime::currentTime().msec());

    m_applicationId = m_parameters.value("app_id").toString();
    m_applicationCode = m_parameters.value("app_code").toString();
}

QGeoTileFetcherHere::~QGeoTileFetcherHere() {}

QGeoTiledMapReply *QGeoTileFetcherHere::getTileImage(const QGeoTileSpec &spec)
{
    // TODO add error detection for if request.connectivityMode() != QGraphicsGeoMap::OnlineMode
    QString rawRequest = getRequestString(spec);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QGeoTiledMapReply *mapReply = new QGeoMapReplyHere(m_networkManager->get(netRequest), spec);

    return mapReply;
}

QString QGeoTileFetcherHere::getRequestString(const QGeoTileSpec &spec)
{
    static const QString http("http://");
    static const QString path("/maptile/2.1/maptile/newest/");
    static const QChar slash('/');

    QString requestString = http;

    const QString mapScheme = m_engineHere->getScheme(spec.mapId());
    if (isAerialType(mapScheme))
        requestString += m_aerialUriProvider->getCurrentHost();
    else
        requestString += m_baseUriProvider->getCurrentHost();

    requestString += path;
    requestString += mapScheme;
    requestString += slash;
    requestString += QString::number(spec.zoom());
    requestString += slash;
    requestString += QString::number(spec.x());
    requestString += slash;
    requestString += QString::number(spec.y());
    requestString += slash;
    requestString += sizeToStr(m_tileSize);
    static const QString slashpng("/png8");
    requestString += slashpng;

    if (!m_applicationCode.isEmpty() && !m_applicationId.isEmpty()) {
        requestString += "?app_code=";
        requestString += m_applicationCode;
        requestString += "&app_id=";
        requestString += m_applicationId;
    }

    requestString += "&lg=";
    requestString += getLanguageString();

    return requestString;
}

QString QGeoTileFetcherHere::getLanguageString() const
{
    if (!m_engineHere)
        return QStringLiteral("ENG");

    QLocale locale = m_engineHere.data()->locale();

    // English is the default, where no ln is specified. We hardcode the languages
    // here even though the entire list is updated automagically from the server.
    // The current languages are Arabic, Chinese, Simplified Chinese, English
    // French, German, Italian, Polish, Russian and Spanish. The default is English.
    // These are acually available from the same host under the URL: /maptiler/v2/info

    switch (locale.language()) {
    case QLocale::Arabic:
        return QStringLiteral("ARA");
    case QLocale::Chinese:
        if (locale.script() == QLocale::TraditionalChineseScript)
            return QStringLiteral("CHI");
        else
            return QStringLiteral("CHT");
    case QLocale::Dutch:
        return QStringLiteral("DUT");
    case QLocale::French:
        return QStringLiteral("FRE");
    case QLocale::German:
        return QStringLiteral("GER");
    case QLocale::Gaelic:
        return QStringLiteral("GLE");
    case QLocale::Greek:
        return QStringLiteral("GRE");
    case QLocale::Hebrew:
        return QStringLiteral("HEB");
    case QLocale::Hindi:
        return QStringLiteral("HIN");
    case QLocale::Indonesian:
        return QStringLiteral("IND");
    case QLocale::Italian:
        return QStringLiteral("ITA");
    case QLocale::Persian:
        return QStringLiteral("PER");
    case QLocale::Polish:
        return QStringLiteral("POL");
    case QLocale::Portuguese:
        return QStringLiteral("POR");
    case QLocale::Russian:
        return QStringLiteral("RUS");
    case QLocale::Sinhala:
        return QStringLiteral("SIN");
    case QLocale::Spanish:
        return QStringLiteral("SPA");
    case QLocale::Thai:
        return QStringLiteral("THA");
    case QLocale::Turkish:
        return QStringLiteral("TUR");
    case QLocale::Ukrainian:
        return QStringLiteral("UKR");
    case QLocale::Urdu:
        return QStringLiteral("URD");
    case QLocale::Vietnamese:
        return QStringLiteral("VIE");

    default:
        return QStringLiteral("ENG");
    }
    // No "lg" param means that we want English.
}

QString QGeoTileFetcherHere::applicationCode() const
{
    return m_applicationCode;
}

QString QGeoTileFetcherHere::applicationId() const
{
    return m_applicationId;
}

void QGeoTileFetcherHere::copyrightsFetched()
{
    if (m_engineHere && m_copyrightsReply->error() == QNetworkReply::NoError) {
        QMetaObject::invokeMethod(m_engineHere.data(),
                                  "loadCopyrightsDescriptorsFromJson",
                                  Qt::QueuedConnection,
                                  Q_ARG(QByteArray, m_copyrightsReply->readAll()));
    }

    m_copyrightsReply->deleteLater();
}

void QGeoTileFetcherHere::versionFetched()
{
    if (m_engineHere && m_versionReply->error() == QNetworkReply::NoError) {
        QMetaObject::invokeMethod(m_engineHere.data(),
                                  "parseNewVersionInfo",
                                  Qt::QueuedConnection,
                                  Q_ARG(QByteArray, m_versionReply->readAll()));
    }

    m_versionReply->deleteLater();
}

void QGeoTileFetcherHere::fetchCopyrightsData()
{
    QString copyrightUrl = QStringLiteral("http://");

    copyrightUrl += m_baseUriProvider->getCurrentHost();
    copyrightUrl += QStringLiteral("/maptile/2.1/copyright/newest?output=json");

    if (!applicationCode().isEmpty()) {
        copyrightUrl += QStringLiteral("&app_code=");
        copyrightUrl += applicationCode();
    }

    if (!applicationId().isEmpty()) {
        copyrightUrl += QStringLiteral("&app_id=");
        copyrightUrl += applicationId();
    }

    QNetworkRequest netRequest((QUrl(copyrightUrl)));
    m_copyrightsReply = m_networkManager->get(netRequest);
    if (m_copyrightsReply->error() != QNetworkReply::NoError) {
        qWarning() << __FUNCTION__ << m_copyrightsReply->errorString();
        m_copyrightsReply->deleteLater();
        return;
    }

    if (m_copyrightsReply->isFinished()) {
        copyrightsFetched();
    } else {
        connect(m_copyrightsReply, &QNetworkReply::finished,
                this, &QGeoTileFetcherHere::copyrightsFetched);
    }
}

void QGeoTileFetcherHere::fetchVersionData()
{
    QString versionUrl = QStringLiteral("http://");

    versionUrl += m_baseUriProvider->getCurrentHost();
    versionUrl += QStringLiteral("/maptile/2.1/version");

    if (!applicationCode().isEmpty()) {
        versionUrl += QStringLiteral("?app_code=");
        versionUrl += applicationCode();
    }

    if (!applicationId().isEmpty()) {
        versionUrl += QStringLiteral("&app_id=");
        versionUrl += applicationId();
    }

    QNetworkRequest netRequest((QUrl(versionUrl)));
    m_versionReply = m_networkManager->get(netRequest);

    if (m_versionReply->error() != QNetworkReply::NoError) {
        qWarning() << __FUNCTION__ << m_versionReply->errorString();
        m_versionReply->deleteLater();
        return;
    }

    if (m_versionReply->isFinished()) {
        versionFetched();
    } else {
        connect(m_versionReply, &QNetworkReply::finished,
                this, &QGeoTileFetcherHere::versionFetched);
    }
}

QT_END_NAMESPACE
