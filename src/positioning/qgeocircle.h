/****************************************************************************
**
** Copyright (C) 2015 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QGEOCIRCLE_H
#define QGEOCIRCLE_H

#include <QtPositioning/QGeoShape>

QT_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeoCirclePrivate;

class Q_POSITIONING_EXPORT QGeoCircle : public QGeoShape
{
    Q_GADGET
    Q_PROPERTY(QGeoCoordinate center READ center WRITE setCenter)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    QGeoCircle();
    QGeoCircle(const QGeoCoordinate &center, qreal radius = -1.0);
    QGeoCircle(const QGeoCircle &other);
    QGeoCircle(const QGeoShape &other);

    ~QGeoCircle();

    QGeoCircle &operator=(const QGeoCircle &other);

    using QGeoShape::operator==;
    bool operator==(const QGeoCircle &other) const;

    using QGeoShape::operator!=;
    bool operator!=(const QGeoCircle &other) const;

    void setCenter(const QGeoCoordinate &center);
    QGeoCoordinate center() const;

    void setRadius(qreal radius);
    qreal radius() const;

    void translate(double degreesLatitude, double degreesLongitude);
    QGeoCircle translated(double degreesLatitude, double degreesLongitude) const;

    Q_INVOKABLE QString toString() const;

private:
    inline QGeoCirclePrivate *d_func();
    inline const QGeoCirclePrivate *d_func() const;
};

Q_DECLARE_TYPEINFO(QGeoCircle, Q_MOVABLE_TYPE);

// FIXME: Exists to satisfy QMetaType::registerComparators() which is required for
//        QML value type. Remove once QMetaType has been fixed.
inline bool operator<(const QGeoCircle &/*lhs*/, const QGeoCircle &/*rhs*/)
{
    qWarning("'<' operator not valid for QGeoCircle\n");
    return false;
}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QGeoCircle)

#endif

