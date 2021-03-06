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

#include "geocoding/qgeocodereply_here.h"
#include "geocoding/qgeocodejsonparser.h"

#include <QtPositioning/QGeoShape>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtDebug>

QT_BEGIN_NAMESPACE

QGeoCodeReplyHere::QGeoCodeReplyHere(QNetworkReply *reply, int limit, int offset,
                                     const QGeoShape &viewport, bool manualBoundsRequired,
                                     QObject *parent)
        : QGeoCodeReply(parent)
        , m_reply(reply)
        , m_manualBoundsRequired(manualBoundsRequired)
{
    connect(m_reply, &QNetworkReply::finished,
            this, &QGeoCodeReplyHere::networkFinished);

    setLimit(limit);
    setOffset(offset);
    setViewport(viewport);
}

QGeoCodeReplyHere::~QGeoCodeReplyHere()
{
    if (m_reply)
        m_reply->deleteLater();
}

void QGeoCodeReplyHere::abort()
{
    if (!m_reply)
        return;

    m_reply->abort();

    m_reply->deleteLater();
    m_reply = 0;
}

void QGeoCodeReplyHere::networkFinished()
{
    if (!m_reply)
        return;

    if (m_reply->error() != QNetworkReply::NoError) {
        setError(QGeoCodeReply::CommunicationError, m_reply->errorString());
        m_reply->deleteLater();
        m_reply = 0;
        return;
    }

    QGeoCodeJsonParser parser;
    if (parser.parse(m_reply)) {
        QList<QGeoLocation> locations = parser.results();
        QGeoShape bounds = viewport();
        if (m_manualBoundsRequired && bounds.isValid()) {
            for (int i = locations.size() - 1; i >= 0; --i) {
                if (!bounds.contains(locations[i].coordinate())) {
                    locations.removeAt(i);
                }
            }
        }
        setLocations(locations);
        setFinished(true);
    } else {
        setError(QGeoCodeReply::ParseError, parser.errorString());
    }

    m_reply->deleteLater();
    m_reply = 0;
}

QT_END_NAMESPACE
