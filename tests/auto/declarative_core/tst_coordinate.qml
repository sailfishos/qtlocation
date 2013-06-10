/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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
****************************************************************************/

import QtQuick 2.0
import QtTest 1.0
import QtLocation 5.0

Item {
    id: item

    property variant empty: QtLocation.coordinate()
    property variant base: QtLocation.coordinate(1.0, 1.0, 5.0)
    property variant zero: QtLocation.coordinate(0, 0)
    property variant plusone: QtLocation.coordinate(0, 1)
    property variant minusone: QtLocation.coordinate(0, -1)
    property variant north: QtLocation.coordinate(3, 0)

    SignalSpy { id: coordSpy; target: item; signalName: "baseChanged" }

    TestCase {
        name: "Coordinate"

        function test_validity() {
            compare(empty.isValid, false)

            empty.longitude = 0.0;
            empty.latitude = 0.0;

            compare(empty.isValid, true)
        }

        function test_accessors() {
            compare(base.longitude, 1.0)
            compare(base.latitude, 1.0)
            compare(base.altitude, 5.0)

            coordSpy.clear();

            base.longitude = 2.0;
            base.latitude = 3.0;
            base.altitude = 6.0;

            compare(base.longitude, 2.0)
            compare(base.latitude, 3.0)
            compare(base.altitude, 6.0)
            compare(coordSpy.count, 3)
        }

        function test_distance() {
            compare(zero.distanceTo(plusone), zero.distanceTo(minusone))
            compare(2*plusone.distanceTo(zero), plusone.distanceTo(minusone))
            compare(zero.distanceTo(plusone) > 0, true)
        }

        function test_azimuth() {
            compare(zero.azimuthTo(north), 0)
            compare(zero.azimuthTo(plusone), 90)
            compare(zero.azimuthTo(minusone), 270)
            compare(minusone.azimuthTo(plusone), 360 - plusone.azimuthTo(minusone))
        }

        function test_atDistanceAndAzimuth() {
            // 112km is approximately one degree of arc

            var coord_0d = zero.atDistanceAndAzimuth(112000, 0)
            compare(coord_0d.latitude > 0.95, true)
            compare(coord_0d.latitude < 1.05, true)
            compare(coord_0d.longitude < 0.05, true)
            compare(coord_0d.longitude > -0.05, true)
            compare(zero.distanceTo(coord_0d), 112000)
            compare(zero.azimuthTo(coord_0d), 0)

            var coord_90d = zero.atDistanceAndAzimuth(112000, 90)
            compare(coord_90d.longitude > 0.95, true)
            compare(coord_90d.longitude < 1.05, true)
            compare(coord_90d.latitude < 0.05, true)
            compare(coord_90d.latitude > -0.05, true)
            compare(zero.distanceTo(coord_90d), 112000)
            compare(zero.azimuthTo(coord_90d), 90)

            var coord_30d = zero.atDistanceAndAzimuth(20000, 30)
            compare(coord_30d.longitude > 0, true)
            compare(coord_30d.latitude > 0, true)
            compare(zero.distanceTo(coord_30d), 20000)
            compare(zero.azimuthTo(coord_30d), 30)

            var coord_30d2 = coord_30d.atDistanceAndAzimuth(200, 30)
            compare(zero.distanceTo(coord_30d2), 20200)

        }

    }
}
