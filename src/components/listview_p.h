/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef LISTVIEW_P_H
#define LISTVIEW_P_H

#include "anchors_p.h"
#include <QListView>
#include <QDeclarativeParserStatus>
#include <qdeclarative.h>

class ItemDelegate;

class ListViewPrivate;

class ListView : public QListView, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QWidget* parent READ parentWidget WRITE setParent NOTIFY parentChanged)
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(int width READ width WRITE setFixedWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setFixedHeight NOTIFY heightChanged)
    Q_PRIVATE_PROPERTY(ListView::d_func(), Anchors* anchors READ anchors CONSTANT FINAL)
    Q_PROPERTY(AnchorLine left READ left CONSTANT FINAL)
    Q_PROPERTY(AnchorLine right READ right CONSTANT FINAL)
    Q_PROPERTY(AnchorLine top READ top CONSTANT FINAL)
    Q_PROPERTY(AnchorLine bottom READ bottom CONSTANT FINAL)
    Q_PROPERTY(AnchorLine horizontalCenter READ horizontalCenter CONSTANT FINAL)
    Q_PROPERTY(AnchorLine verticalCenter READ verticalCenter CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(ListView::d_func(), QDeclarativeListProperty<QObject> data READ data)
    Q_PRIVATE_PROPERTY(ListView::d_func(), QDeclarativeListProperty<QObject> actions READ actions)
    Q_PRIVATE_PROPERTY(ListView::d_func(), bool visible READ qmlVisible WRITE setQmlVisible)
    Q_PRIVATE_PROPERTY(ListView::d_func(), QVariant model READ model WRITE setModel)
    Q_PRIVATE_PROPERTY(ListView::d_func(), QVariant rootIndex READ rootIndex WRITE setRootIndex)
    Q_PRIVATE_PROPERTY(ListView::d_func(), QVariant currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_PRIVATE_PROPERTY(ListView::d_func(), ItemDelegate* delegate READ delegate WRITE setDelegate RESET resetDelegate)

    Q_INTERFACES(QDeclarativeParserStatus)

    Q_CLASSINFO("DefaultProperty", "data")

public:
    explicit ListView(QWidget *parent = 0);
    ~ListView();

    void setX(int x);
    void setY(int y);

    AnchorLine left() const;
    AnchorLine right() const;
    AnchorLine top() const;
    AnchorLine bottom() const;
    AnchorLine horizontalCenter() const;
    AnchorLine verticalCenter() const;

signals:
    void parentChanged();
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();

private:
    ListView(ListViewPrivate &dd, QWidget *parent = 0);

    void changeEvent(QEvent *event);
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    void classBegin();
    void componentComplete();

    QScopedPointer<ListViewPrivate> d_ptr;

    Q_DISABLE_COPY(ListView)
    Q_DECLARE_PRIVATE(ListView)
};

QML_DECLARE_TYPE(ListView)
Q_DECLARE_METATYPE(QModelIndex)

#endif // LISTVIEW_P_H
