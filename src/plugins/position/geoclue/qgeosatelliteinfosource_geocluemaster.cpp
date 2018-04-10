/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd, author: Aaron McCarthy <aaron.mccarthy@jollamobile.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtPositioning module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeosatelliteinfosource_geocluemaster.h"

#include <geoclue_interface.h>
#include <satellite_interface.h>

#include <QtDBus/QDBusPendingCallWatcher>

#define MINIMUM_UPDATE_INTERVAL 1000

QT_BEGIN_NAMESPACE

QGeoSatelliteInfoSourceGeoclueMaster::QGeoSatelliteInfoSourceGeoclueMaster(QObject *parent)
:   QGeoSatelliteInfoSource(parent), m_master(new QGeoclueMaster(this)), m_provider(0), m_sat(0),
    m_error(NoError), m_satellitesChangedConnected(false), m_running(false)
{
    qDBusRegisterMetaType<QGeoSatelliteInfo>();
    qDBusRegisterMetaType<QList<QGeoSatelliteInfo> >();

    connect(m_master, SIGNAL(positionProviderChanged(QString,QString,QString,QString)),
            this, SLOT(positionProviderChanged(QString,QString,QString,QString)));
    m_requestTimer.setSingleShot(true);
    connect(&m_requestTimer, SIGNAL(timeout()), this, SLOT(requestUpdateTimeout()));
}

QGeoSatelliteInfoSourceGeoclueMaster::~QGeoSatelliteInfoSourceGeoclueMaster()
{
    cleanupSatelliteSource();
}

int QGeoSatelliteInfoSourceGeoclueMaster::minimumUpdateInterval() const
{
    return MINIMUM_UPDATE_INTERVAL;
}

void QGeoSatelliteInfoSourceGeoclueMaster::setUpdateInterval(int msec)
{
    QGeoSatelliteInfoSource::setUpdateInterval(qMax(MINIMUM_UPDATE_INTERVAL, msec));
}

QGeoSatelliteInfoSource::Error QGeoSatelliteInfoSourceGeoclueMaster::error() const
{
    return m_error;
}

void QGeoSatelliteInfoSourceGeoclueMaster::startUpdates()
{
    if (m_running)
        return;

    m_running = true;

    // Start Geoclue provider.
    if (!m_master->hasMasterClient())
        configureSatelliteSource();

    m_requestTimer.start(qMax(updateInterval(), minimumUpdateInterval()));
}

void QGeoSatelliteInfoSourceGeoclueMaster::stopUpdates()
{
    if (!m_running)
        return;

    if (m_sat) {
        disconnect(m_sat, SIGNAL(SatelliteChanged(int,int,int,QList<int>,QList<QGeoSatelliteInfo>)),
                   this, SLOT(satelliteChanged(int,int,int,QList<int>,QList<QGeoSatelliteInfo>)));
    }

    m_running = false;

    // Only stop positioning if single update not requested.
    if (!m_requestTimer.isActive()) {
        cleanupSatelliteSource();
        m_master->releaseMasterClient();
    }
}

void QGeoSatelliteInfoSourceGeoclueMaster::requestUpdate(int timeout)
{
    if (timeout < minimumUpdateInterval() && timeout != 0) {
        emit requestTimeout();
        return;
    }

    if (m_requestTimer.isActive())
        return;

    if (!m_master->hasMasterClient())
        configureSatelliteSource();

    m_requestTimer.start(qMax(timeout, minimumUpdateInterval()));

    if (m_sat) {
        QDBusPendingReply<int, int, int, QList<int>, QList<QGeoSatelliteInfo> > reply =
            m_sat->GetSatellite();
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                this, SLOT(getSatelliteFinished(QDBusPendingCallWatcher*)));
    }
}

void QGeoSatelliteInfoSourceGeoclueMaster::updateSatelliteInfo(int timestamp, int satellitesUsed,
                                                               int satellitesVisible,
                                                               const QList<int> &usedPrn,
                                                               const QList<QGeoSatelliteInfo> &satInfos)
{
    m_requestTimer.stop();
    Q_UNUSED(timestamp)

    QList<QGeoSatelliteInfo> inUse;

    foreach (const QGeoSatelliteInfo &si, satInfos)
        if (usedPrn.contains(si.satelliteIdentifier()))
            inUse.append(si);

    if (satInfos.length() != satellitesVisible) {
        qWarning("QGeoSatelliteInfoSourceGeoclueMaster number of in view QGeoSatelliteInfos (%d) "
                 "does not match expected number of in view satellites (%d).", satInfos.length(),
                 satellitesVisible);
    }

    if (inUse.length() != satellitesUsed) {
        qWarning("QGeoSatelliteInfoSourceGeoclueMaster number of in use QGeoSatelliteInfos (%d) "
                 "does not match expected number of in use satellites (%d).", inUse.length(),
                 satellitesUsed);
    }

    m_inView = satInfos;
    emit satellitesInViewUpdated(m_inView);

    m_inUse = inUse;
    emit satellitesInUseUpdated(m_inUse);
}

void QGeoSatelliteInfoSourceGeoclueMaster::requestUpdateTimeout()
{
    // If we end up here, there has not been a valid satellite info update.
    if (m_running) {
        m_inView.clear();
        m_inUse.clear();
        emit satellitesInViewUpdated(m_inView);
        emit satellitesInUseUpdated(m_inUse);
    } else {
        emit requestTimeout();

        // Only stop satellite info if regular updates not active.
        cleanupSatelliteSource();
        m_master->releaseMasterClient();
    }
}

void QGeoSatelliteInfoSourceGeoclueMaster::getSatelliteFinished(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<int, int, int, QList<int>, QList<QGeoSatelliteInfo> > reply = *watcher;
    watcher->deleteLater();

    if (reply.isError())
        return;

    updateSatelliteInfo(reply.argumentAt<0>(), reply.argumentAt<1>(), reply.argumentAt<2>(),
                        reply.argumentAt<3>(), reply.argumentAt<4>());
}

void QGeoSatelliteInfoSourceGeoclueMaster::satelliteChanged(int timestamp, int satellitesUsed, int satellitesVisible, const QList<int> &usedPrn, const QList<QGeoSatelliteInfo> &satInfos)
{
    updateSatelliteInfo(timestamp, satellitesUsed, satellitesVisible, usedPrn, satInfos);
}

void QGeoSatelliteInfoSourceGeoclueMaster::positionProviderChanged(const QString &name,
                                                                   const QString &description,
                                                                   const QString &service,
                                                                   const QString &path)
{
    Q_UNUSED(name)
    Q_UNUSED(description)

    cleanupSatelliteSource();

    QString providerService;
    QString providerPath;

    if (service.isEmpty() || path.isEmpty()) {
        // No valid position provider has been selected. This probably means that the GPS provider
        // has not yet obtained a position fix. It can still provide satellite information though.
        if (!m_satellitesChangedConnected) {
            QDBusConnection conn = QDBusConnection::sessionBus();
            conn.connect(QString(), QString(), QStringLiteral("org.freedesktop.Geoclue.Satellite"),
                         QStringLiteral("SatelliteChanged"), this,
                         SLOT(satelliteChanged(QDBusMessage)));
            m_satellitesChangedConnected = true;
            return;
        }
    } else {
        if (m_satellitesChangedConnected) {
            QDBusConnection conn = QDBusConnection::sessionBus();
            conn.disconnect(QString(), QString(),
                            QStringLiteral("org.freedesktop.Geoclue.Satellite"),
                            QStringLiteral("SatelliteChanged"), this,
                            SLOT(satelliteChanged(QDBusMessage)));
            m_satellitesChangedConnected = false;
        }

        providerService = service;
        providerPath = path;
    }

    if (providerService.isEmpty() || providerPath.isEmpty()) {
        m_error = AccessError;
        emit QGeoSatelliteInfoSource::error(m_error);
        return;
    }

    m_provider = new OrgFreedesktopGeoclueInterface(providerService, providerPath, QDBusConnection::sessionBus());
    m_provider->AddReference();

    m_sat = new OrgFreedesktopGeoclueSatelliteInterface(providerService, providerPath, QDBusConnection::sessionBus());

    if (m_running) {
        connect(m_sat, SIGNAL(SatelliteChanged(int,int,int,QList<int>,QList<QGeoSatelliteInfo>)),
                this, SLOT(satelliteChanged(int,int,int,QList<int>,QList<QGeoSatelliteInfo>)));
    }
}

void QGeoSatelliteInfoSourceGeoclueMaster::satelliteChanged(const QDBusMessage &message)
{
    QVariantList arguments = message.arguments();
    if (arguments.length() != 5)
        return;

    int timestamp = arguments.at(0).toInt();
    int usedSatellites = arguments.at(1).toInt();
    int visibleSatellites = arguments.at(2).toInt();

    QDBusArgument dbusArgument = arguments.at(3).value<QDBusArgument>();

    QList<int> usedPrn;
    dbusArgument >> usedPrn;

    dbusArgument = arguments.at(4).value<QDBusArgument>();

    QList<QGeoSatelliteInfo> satelliteInfos;
    dbusArgument >> satelliteInfos;

    satelliteChanged(timestamp, usedSatellites, visibleSatellites, usedPrn, satelliteInfos);
}

void QGeoSatelliteInfoSourceGeoclueMaster::configureSatelliteSource()
{
    if (!m_master->createMasterClient(Accuracy::Detailed, QGeoclueMaster::ResourceGps)) {
        m_error = UnknownSourceError;
        emit QGeoSatelliteInfoSource::error(m_error);
    }
}

void QGeoSatelliteInfoSourceGeoclueMaster::cleanupSatelliteSource()
{
    if (m_provider)
        m_provider->RemoveReference();
    delete m_provider;
    m_provider = 0;
    delete m_sat;
    m_sat = 0;

    m_requestTimer.stop();
}

QT_END_NAMESPACE
