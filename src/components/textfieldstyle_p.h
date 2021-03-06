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

#ifndef TEXTFIELDSTYLE_P_H
#define TEXTFIELDSTYLE_P_H

#include "boxstyle_p.h"

class TextFieldStylePrivate;

class TextFieldStyle : public BoxStyle
{
    Q_OBJECT
    
    Q_PROPERTY(QString passwordCharacter READ passwordCharacter WRITE setPasswordCharacter NOTIFY changed)
    Q_PROPERTY(QColor selectionBackgroundColor READ selectionBackgroundColor WRITE setSelectionBackgroundColor NOTIFY changed)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY changed)
    
public:
    explicit TextFieldStyle(QObject *parent = 0);
    ~TextFieldStyle();
    
    QString passwordCharacter() const;
    void setPasswordCharacter(const QString &c);
    
    QColor selectionBackgroundColor() const;
    void setSelectionBackgroundColor(const QColor &color);
    
    QColor selectionColor() const;
    void setSelectionColor(const QColor &color);
    
signals:
    void changed();
    
protected:
    TextFieldStyle(TextFieldStylePrivate &dd, QObject *parent = 0);
    
    Q_DECLARE_PRIVATE(TextFieldStyle)

private:
    Q_DISABLE_COPY(TextFieldStyle)
};

QML_DECLARE_TYPE(TextFieldStyle)

#endif // TEXTFIELDSTYLE_P_H

