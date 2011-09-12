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

#ifndef QDECLARATIVESEARCHRESULTMODEL_P_H
#define QDECLARATIVESEARCHRESULTMODEL_P_H

#include "qdeclarativesearchmodelbase.h"
#include "qdeclarativecategory_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeGeoServiceProvider;

class QDeclarativeSearchResultModel : public QDeclarativeSearchModelBase
{
    Q_OBJECT

    Q_PROPERTY(QString searchTerm READ searchTerm WRITE setSearchTerm NOTIFY searchTermChanged)
    Q_PROPERTY(QDeclarativeCategory *searchCategory READ searchCategory WRITE setSearchCategory NOTIFY searchCategoryChanged)
    Q_PROPERTY(int didYouMean READ didYouMean WRITE setDidYouMean NOTIFY didYouMeanChanged);

    Q_ENUMS(SearchResultType)

public:
    enum SearchResultType {
        Place = QPlaceSearchResult::Place,
        DidYouMeanSuggestion = QPlaceSearchResult::DidYouMeanSuggestion,
        UnknownSearchResult = QPlaceSearchResult::UnknownSearchResult
    };

    explicit QDeclarativeSearchResultModel(QObject *parent = 0);
    ~QDeclarativeSearchResultModel();

    QString searchTerm() const;
    void setSearchTerm(const QString &searchTerm);
    Q_INVOKABLE void clearSearchTerm();

    QDeclarativeCategory *searchCategory();
    void setSearchCategory(QDeclarativeCategory *searchCategory);
    Q_INVOKABLE void clearCategories();

    int didYouMean() const;
    void setDidYouMean(int dym);

    void clearData();
    void updateSearchRequest();
    void processReply(QPlaceReply *reply);

    // From QAbstractListModel
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    enum Roles {
        SearchResultType = Qt::UserRole,
        SearchResultRelevance,
        SearchResultDistance,
        SearchResultHeading,
        SearchResultAdditionalData,
        SearchResultPlace,
        SearchResultDidYouMean
    };

signals:
    void searchTermChanged();
    void searchCategoryChanged();
    void didYouMeanChanged();

protected:
    QPlaceReply *sendQuery(QPlaceManager *manager, const QPlaceSearchRequest &request);
    virtual void initializePlugin(QDeclarativeGeoServiceProvider *oldPlugin,
                                  QDeclarativeGeoServiceProvider *newPlugin);

private:
    QList<QPlaceSearchResult> m_results;
    QMap<QString, QDeclarativePlace *> m_places;
    QDeclarativeCategory m_category;
    QPlaceManager *m_placeManager;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESEARCHRESULTMODEL_P_H
