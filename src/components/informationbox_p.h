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

#ifndef INFORMATIONBOX_P_H
#define INFORMATIONBOX_P_H

#include <QMaemo5InformationBox>
#include <QDeclarativeParserStatus>
#include <qdeclarative.h>

class InformationBoxPrivate;

class InformationBox : public QMaemo5InformationBox, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_PROPERTY(QWidget* parent READ parentWidget WRITE setParent NOTIFY parentChanged)
    Q_PROPERTY(int width READ width WRITE setFixedWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setFixedHeight NOTIFY heightChanged)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity NOTIFY opacityChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PRIVATE_PROPERTY(InformationBox::d_func(), QDeclarativeListProperty<QObject> data READ data)
    Q_PRIVATE_PROPERTY(InformationBox::d_func(), QDeclarativeListProperty<QWidget> children READ children)
    Q_PRIVATE_PROPERTY(InformationBox::d_func(), QDeclarativeListProperty<QObject> actions READ actions)
    Q_PRIVATE_PROPERTY(InformationBox::d_func(), QDeclarativeListProperty<QWidget> content READ content)
    Q_PRIVATE_PROPERTY(InformationBox::d_func(), bool focus READ hasFocus WRITE setFocus NOTIFY focusChanged)
    Q_PRIVATE_PROPERTY(InformationBox::d_func(), Style* style READ style WRITE setStyle RESET resetStyle)
    

    Q_ENUMS(Timeout)

    Q_INTERFACES(QDeclarativeParserStatus)

    Q_CLASSINFO("DefaultProperty", "data")

public:
    enum Timeout {
        NoTimeout = QMaemo5InformationBox::NoTimeout,
        DefaultTimeout = QMaemo5InformationBox::DefaultTimeout
    };

    explicit InformationBox(QWidget *parent = 0);
    ~InformationBox();

    qreal opacity() const;
    void setOpacity(qreal opacity);

signals:
    void parentChanged();
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void opacityChanged();
    void visibleChanged();
    void enabledChanged();
    void focusChanged();

protected:
    InformationBox(InformationBoxPrivate &dd, QWidget *parent = 0);
    
    virtual void changeEvent(QEvent *event);
    virtual void moveEvent(QMoveEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);
    virtual void focusInEvent(QFocusEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);

    virtual void classBegin();
    virtual void componentComplete();

    QScopedPointer<InformationBoxPrivate> d_ptr;

    Q_DECLARE_PRIVATE(InformationBox)
    
    Q_PRIVATE_SLOT(d_func(), void _q_onStyleChanged())

private:
    Q_DISABLE_COPY(InformationBox)
};

QML_DECLARE_TYPE(InformationBox)

#endif // INFORMATIONBOX_P_H
