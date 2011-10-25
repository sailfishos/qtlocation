/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qplaceuser.h"
#include "qplaceuser_p.h"

QT_USE_NAMESPACE

QPlaceUserPrivate::QPlaceUserPrivate()
    : QSharedData()
{
}

QPlaceUserPrivate::QPlaceUserPrivate(const QPlaceUserPrivate &other)
    : QSharedData(), userId(other.userId), name(other.name)
{
}

QPlaceUserPrivate::~QPlaceUserPrivate()
{
}

bool QPlaceUserPrivate::operator==(const QPlaceUserPrivate &other) const
{
    return userId == other.userId && name == other.name;
}

/*!
    \class QPlaceUser
    \inmodule QtLocation
    \ingroup QtLocation-places
    \since QtLocation 5.0

    \brief The QPlaceUser class represents a user.

    Each QPlaceRating represents a rating object with a count and value.

    QPlaceRating is an in memory representation of a rating object.
*/


/*!
    Constructs a new user.
*/
QPlaceUser::QPlaceUser()
    : d(new QPlaceUserPrivate)
{
}

/*!
    Constructs a copy of \a other
*/
QPlaceUser::QPlaceUser(const QPlaceUser &other)
    :d(other.d)
{
}

/*!
    Destructor.
*/
QPlaceUser::~QPlaceUser()
{
}

/*!
    Assigns \a other to this user and returns a reference to this user.
*/
QPlaceUser &QPlaceUser::operator=(const QPlaceUser &other)
{
    d = other.d;
    return *this;
}


/*!
    Returns true if this user is equal to \a other.
    Otherwise returns false.
*/
bool QPlaceUser::operator==(const QPlaceUser &other) const
{
    return (*d) == *(other.d);
}

/*!
    Returns the user id.
*/
QString QPlaceUser::userId() const
{
    return d->userId;
}

/*!
    Sets the \a userId.
*/
void QPlaceUser::setUserId(const QString &userId)
{
    d->userId = userId;
}

/*!
    Returns the user's name.
*/
QString QPlaceUser::name() const
{
    return d->name;
}

/*!
    Sets the user's \a name.
*/

void QPlaceUser::setName(const QString &name)
{
    d->name = name;
}
