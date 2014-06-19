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

#include "dialog_p.h"
#include "dialog_p_p.h"
#include <QAbstractButton>
#include <QActionGroup>
#include <QMoveEvent>
#include <QResizeEvent>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    d_ptr(new DialogPrivate(this))
{
    Q_D(Dialog);

    d->contentItem = new Item(this);
    d->grid = new QGridLayout(this);
    d->buttonBox = new QDialogButtonBox(Qt::Vertical, this);
}

Dialog::Dialog(DialogPrivate &dd, QWidget *parent) :
    QDialog(parent),
    d_ptr(&dd)
{
}

Dialog::~Dialog() {}

bool Dialog::showingProgressIndicator() const {
    return this->testAttribute(Qt::WA_Maemo5ShowProgressIndicator);
}

void Dialog::showProgressIndicator(bool show) {
    this->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, show);
}

AnchorLine Dialog::left() const {
    Q_D(const Dialog);

    return d->left;
}

AnchorLine Dialog::right() const {
    Q_D(const Dialog);

    return d->right;
}

AnchorLine Dialog::top() const {
    Q_D(const Dialog);

    return d->top;
}

AnchorLine Dialog::bottom() const {
    Q_D(const Dialog);

    return d->bottom;
}

AnchorLine Dialog::horizontalCenter() const {
    Q_D(const Dialog);

    return d->horizontalCenter;
}

AnchorLine Dialog::verticalCenter() const {
    Q_D(const Dialog);

    return d->verticalCenter;
}

void Dialog::changeEvent(QEvent *event) {
    switch (event->type()) {
    case QEvent::ParentChange:
        emit parentChanged();
        break;
    default:
        break;
    }

    QDialog::changeEvent(event);
}

void Dialog::moveEvent(QMoveEvent *event) {
    if (event->pos().x() != event->oldPos().x()) {
        emit xChanged();
    }

    if (event->pos().y() != event->oldPos().y()) {
        emit yChanged();
    }

    QDialog::moveEvent(event);
}

void Dialog::resizeEvent(QResizeEvent *event) {
    if (event->size().width() != event->oldSize().width()) {
        emit widthChanged();
    }

    if (event->size().height() != event->oldSize().height()) {
        emit heightChanged();
    }

    QDialog::resizeEvent(event);
}

void Dialog::classBegin() {}

void Dialog::componentComplete() {
    Q_D(Dialog);

    d->componentComplete();

    if (this->parentWidget()) {
        this->setWindowFlags(Qt::Dialog);
    }

    d->_q_onOrientationChanged(Screen::instance()->currentOrientation());

    this->connect(Screen::instance(), SIGNAL(currentOrientationChanged(Screen::Orientation)), this, SLOT(_q_onOrientationChanged(Screen::Orientation)));
}

void DialogPrivate::data_append(QDeclarativeListProperty<QObject> *list, QObject *obj) {
    if (!obj) {
        return;
    }

    if (Dialog *dialog = qobject_cast<Dialog*>(list->object)) {
        dialog->d_func()->dataList.append(obj);
    }
}

void DialogPrivate::actions_append(QDeclarativeListProperty<QObject> *list, QObject *obj) {
    if (!obj) {
        return;
    }

    if (Dialog *dialog = qobject_cast<Dialog*>(list->object)) {
        dialog->d_func()->actionList.append(obj);

        if (!dialog->d_func()->complete) {
            return;
        }

        if (QAction *action = qobject_cast<QAction*>(obj)) {
            dialog->addAction(action);
        }
        else if (QActionGroup *group = qobject_cast<QActionGroup*>(obj)) {
            dialog->addActions(group->actions());
        }
    }
}

void DialogPrivate::content_append(QDeclarativeListProperty<QObject> *list, QObject *obj) {
    if (!obj) {
        return;
    }

    if (Dialog *dialog = qobject_cast<Dialog*>(list->object)) {
        dialog->d_func()->contentList.append(obj);
        obj->setParent(dialog->d_func()->contentItem);
    }
}

void DialogPrivate::buttons_append(QDeclarativeListProperty<QObject> *list, QObject *obj) {
    if (!obj) {
        return;
    }

    if (Dialog *dialog = qobject_cast<Dialog*>(list->object)) {
        dialog->d_func()->buttonList.append(obj);

        if (QAbstractButton *button = qobject_cast<QAbstractButton*>(obj)) {
            dialog->d_func()->buttonBox->addButton(button, QDialogButtonBox::ActionRole);
        }
    }
}

QDeclarativeListProperty<QObject> DialogPrivate::data() {
    return QDeclarativeListProperty<QObject>(q_func(), 0, DialogPrivate::data_append);
}

QDeclarativeListProperty<QObject> DialogPrivate::actions() {
    return QDeclarativeListProperty<QObject>(q_func(), 0, DialogPrivate::actions_append);
}

QDeclarativeListProperty<QObject> DialogPrivate::content() {
    return QDeclarativeListProperty<QObject>(q_func(), 0, DialogPrivate::content_append);
}

QDeclarativeListProperty<QObject> DialogPrivate::buttons() {
    return QDeclarativeListProperty<QObject>(q_func(), 0, DialogPrivate::buttons_append);
}

void DialogPrivate::_q_onOrientationChanged(Screen::Orientation orientation) {
    Q_Q(Dialog);

    int h = 0;

    switch (orientation) {
    case Screen::PortraitOrientation:
        buttonBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        if (grid->isEmpty()) {
            grid->addWidget(contentItem, 0, 0);
            grid->addWidget(buttonBox, 1, 0);
        }
        else {
            grid->removeWidget(buttonBox);
            grid->addWidget(buttonBox, 1, 0);
        }

        return;
    default:
        buttonBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

        if (grid->isEmpty()) {
            grid->addWidget(contentItem, 0, 0);
            grid->addWidget(buttonBox, 0, 1);
        }
        else {
            grid->removeWidget(buttonBox);
            grid->addWidget(buttonBox, 0, 1);
        }

        return;
    }
}

#include "moc_dialog_p.cpp"
