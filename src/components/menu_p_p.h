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

#ifndef MENU_P_P_H
#define MENU_P_P_H

#include "menu_p.h"
#include "style_p.h"

class MenuPrivate
{

public:
    MenuPrivate(Menu *parent) :
        q_ptr(parent),
        qmlStyle(0)
    {
    }

    virtual ~MenuPrivate() {}

    static void data_append(QDeclarativeListProperty<QObject> *list, QObject *obj);

    QDeclarativeListProperty<QObject> data();

    bool hasFocus() const;
    void setFocus(bool focus);
    
    Style* style() const;
    void setStyle(Style *style);
    void resetStyle();
    
    void _q_onStyleChanged();

    void componentComplete();

    Menu *q_ptr;
    
    Style *qmlStyle;

    bool complete;

    QList<QObject*> dataList;

    Q_DECLARE_PUBLIC(Menu)
};

#endif // MENU_P_P_H
