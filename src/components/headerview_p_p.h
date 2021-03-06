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

#ifndef HEADERVIEW_P_P_H
#define HEADERVIEW_P_P_H

#include "item_p_p.h"
#include "headerview_p.h"

class QStringListModel;

class HeaderSectionPrivate
{

public:
    HeaderSectionPrivate(HeaderSection *parent) :
        q_ptr(parent),
        width(0),
        resizeMode(QHeaderView::Fixed),
        index(0),
        visible(true),
        complete(false)
    {
    }

    void _q_onSectionClicked(int i);

    HeaderSection *q_ptr;

    QString text;

    int width;

    QHeaderView::ResizeMode resizeMode;

    int index;

    bool visible;

    bool complete;

    Q_DECLARE_PUBLIC(HeaderSection)
};

class HeaderViewPrivate : public ItemPrivate
{

public:
    HeaderViewPrivate(HeaderView *parent) :
        ItemPrivate(parent)
    {
    }

    static void data_append(QDeclarativeListProperty<QObject> *list, QObject *obj);

    static void children_append(QDeclarativeListProperty<QWidget> *list, QWidget *widget);

    static void actions_append(QDeclarativeListProperty<QObject> *list, QObject *obj);

    static void sections_append(QDeclarativeListProperty<HeaderSection> *list, HeaderSection *section);

    QDeclarativeListProperty<QObject> data();

    QDeclarativeListProperty<QWidget> children();

    QDeclarativeListProperty<QObject> actions();

    QDeclarativeListProperty<HeaderSection> sections();

    QVariant currentIndex() const;
    void setCurrentIndex(const QVariant &index);

    bool isClickable() const;
    void setClickable(bool clickable);

    bool isSortIndicatorShown() const;
    void setSortIndicatorShown(bool shown);

    bool highlightSections() const;
    void setHighlightSections(bool highlight);

    bool stretchLastSection() const;
    void setStretchLastSection(bool stretch);

    bool cascadingSectionResizes() const;
    void setCascadingSectionResizes(bool resizes);

    int defaultSectionSize() const;
    void setDefaultSectionSize(int size);

    int minimumSectionSize() const;
    void setMinimumSectionSize(int size);

    Qt::Alignment defaultAlignment() const;
    void setDefaultAlignment(Qt::Alignment alignment);

    QList<HeaderSection*> sectionList;

    Q_DECLARE_PUBLIC(HeaderView)
};

#endif // HEADERVIEW_P_P_H
