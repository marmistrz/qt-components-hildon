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

#include "tableview_p.h"
#include "tableview_p_p.h"
#include "varianttablemodel_p.h"
#include <QHeaderView>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QActionGroup>
#include <QScrollBar>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeComponent>
#include <QDeclarativeInfo>
#include <QGraphicsOpacityEffect>
#include <QTimer>

TableView::TableView(QWidget *parent) :
    QTableView(parent),
    d_ptr(new TableViewPrivate(this))
{
    Q_D(TableView);

    d->scrollTimer = new QTimer(this);
    d->scrollTimer->setInterval(500);
    d->scrollTimer->setSingleShot(true);

    this->connect(d->scrollTimer, SIGNAL(timeout()), this, SLOT(_q_onScrollingStopped()));
    this->connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(_q_onHorizontalScrollPositionChanged()));
    this->connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(_q_onVerticalScrollPositionChanged()));
}

TableView::TableView(TableViewPrivate &dd, QWidget *parent) :
    QTableView(parent),
    d_ptr(&dd)
{
    Q_D(TableView);

    d->scrollTimer = new QTimer(this);
    d->scrollTimer->setInterval(500);
    d->scrollTimer->setSingleShot(true);

    this->connect(d->scrollTimer, SIGNAL(timeout()), this, SLOT(_q_onScrollingStopped()));
    this->connect(this->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(_q_onHorizontalScrollPositionChanged()));
    this->connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(_q_onVerticalScrollPositionChanged()));
}

TableView::~TableView() {}

void TableView::setX(int x) {
    if (x != this->x()) {
        this->move(x, this->y());
    }
}

void TableView::setY(int y) {
    if (y != this->y()) {
        this->move(this->x(), y);
    }
}

qreal TableView::opacity() const {
    if (QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect*>(this->graphicsEffect())) {
        return effect->opacity();
    }

    return 1.0;
}

void TableView::setOpacity(qreal opacity) {
    QGraphicsOpacityEffect *effect = qobject_cast<QGraphicsOpacityEffect*>(this->graphicsEffect());

    if (!effect) {
        effect = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(effect);
    }

    if (opacity != effect->opacity()) {
        effect->setOpacity(opacity);
        emit opacityChanged();
    }
}

AnchorLine TableView::left() const {
    Q_D(const TableView);

    return d->left;
}

AnchorLine TableView::right() const {
    Q_D(const TableView);

    return d->right;
}

AnchorLine TableView::top() const {
    Q_D(const TableView);

    return d->top;
}

AnchorLine TableView::bottom() const {
    Q_D(const TableView);

    return d->bottom;
}

AnchorLine TableView::horizontalCenter() const {
    Q_D(const TableView);

    return d->horizontalCenter;
}

AnchorLine TableView::verticalCenter() const {
    Q_D(const TableView);

    return d->verticalCenter;
}

bool TableView::interactive() const {
    Q_D(const TableView);

    return d->kineticScroller->isEnabled();
}

void TableView::setInteractive(bool interactive) {
    if (interactive != this->interactive()) {
        Q_D(TableView);
        d->kineticScroller->setEnabled(interactive);
        emit interactiveChanged();
    }
}

bool TableView::moving() const {
    Q_D(const TableView);

    if (d->scrollTimer->isActive()) {
        return true;
    }

    switch (d->kineticScroller->state()) {
    case QAbstractKineticScroller::Pushing:
    case QAbstractKineticScroller::AutoScrolling:
        return true;
    default:
        return false;
    }
}

bool TableView::atXBeginning() const {
    return this->horizontalScrollBar()->value() == this->horizontalScrollBar()->minimum();
}

bool TableView::atXEnd() const {
    return this->horizontalScrollBar()->value() == this->horizontalScrollBar()->maximum();
}

bool TableView::atYBeginning() const {
    return this->verticalScrollBar()->value() == this->verticalScrollBar()->minimum();
}

bool TableView::atYEnd() const {
    return this->verticalScrollBar()->value() == this->verticalScrollBar()->maximum();
}

int TableView::contentX() const {
    return this->horizontalScrollBar()->value();
}

void TableView::setContentX(int x) {
    this->horizontalScrollBar()->setValue(x);
}

int TableView::contentY() const {
    return this->horizontalScrollBar()->value();
}

void TableView::setContentY(int y) {
    this->verticalScrollBar()->setValue(y);
}

qreal TableView::flickDeceleration() const {
    Q_D(const TableView);

    return d->kineticScroller->decelerationFactor();
}

void TableView::setFlickDeceleration(qreal deceleration) {
    if (deceleration != this->flickDeceleration()) {
        Q_D(TableView);
        d->kineticScroller->setDecelerationFactor(deceleration);
        emit flickDecelerationChanged();
    }
}

qreal TableView::maximumFlickVelocity() const {
    Q_D(const TableView);

    return d->kineticScroller->maximumVelocity();
}

void TableView::setMaximumFlickVelocity(qreal maximum) {
    if (maximum != this->maximumFlickVelocity()) {
        Q_D(TableView);
        d->kineticScroller->setMaximumVelocity(maximum);
        emit maximumFlickVelocityChanged();
    }
}

bool TableView::rowNumbersVisible() const {
    Q_D(const TableView);

    return d->rowNumbersVisible;
}

void TableView::setRowNumbersVisible(bool visible) {
    if (visible != this->rowNumbersVisible()) {
        Q_D(TableView);
        d->rowNumbersVisible = visible;
        this->verticalHeader()->setVisible(visible);
        emit rowNumbersVisibleChanged();
    }
}

void TableView::setHeader(QHeaderView *header) {
    if (header != this->horizontalHeader()) {
        this->setHorizontalHeader(header);
        this->connect(header, SIGNAL(sectionClicked(int)), this, SLOT(selectColumn(int)), Qt::UniqueConnection);
        emit headerChanged();
    }
}

void TableView::positionViewAtBeginning(bool immediate) {
    if (this->model()) {
        if (immediate) {
            this->scrollTo(this->model()->index(0, 0, this->rootIndex()), QTableView::PositionAtTop);
        }
        else {
            Q_D(TableView);
            d->kineticScroller->scrollTo(QPoint(this->horizontalScrollBar()->minimum(), this->verticalScrollBar()->minimum()));
        }
    }
}

void TableView::positionViewAtEnd(bool immediate) {
    if (this->model()) {
        if (immediate) {
            this->scrollTo(this->model()->index(model()->rowCount() - 1, 0, this->rootIndex()), QTableView::PositionAtBottom);
        }
        else {
            Q_D(TableView);
            d->kineticScroller->scrollTo(QPoint(this->horizontalScrollBar()->maximum(), this->verticalScrollBar()->maximum()));
        }
    }
}

void TableView::positionViewAtIndex(const QModelIndex &index, ScrollHint mode, bool immediate) {
    if (immediate) {
        this->scrollTo(index, mode);
    }
    else {
        Q_D(TableView);
        QRect rect = this->visualRect(index);
        const QRect viewRect = this->viewport()->rect();
        const int wDiff = viewRect.width() - rect.width();
        const int hDiff = viewRect.height() - rect.height();
        
        switch (mode) {
        case PositionAtTop:
            break;
        case PositionAtBottom:
            rect.translate(-wDiff, -hDiff);
            break;
        case PositionAtCenter:
            rect.translate(wDiff ? -(wDiff / 2) : 0, hDiff ? -(hDiff / 2) : 0);
            break;
        default:
            d->kineticScroller->ensureVisible(rect.topLeft(), rect.height(), rect.height());
            return;
        }
        
        d->kineticScroller->scrollTo(QPoint(qBound(0, rect.left() + this->horizontalScrollBar()->value(),
                                                   this->horizontalScrollBar()->maximum()),
                                            qBound(0, rect.top() + this->verticalScrollBar()->value(), 
                                                   this->verticalScrollBar()->maximum())));
    }
}

void TableView::currentChanged(const QModelIndex &current, const QModelIndex &previous) {
    emit currentIndexChanged();
    QTableView::currentChanged(current, previous);
}

void TableView::changeEvent(QEvent *event) {
    switch (event->type()) {
    case QEvent::ParentChange:
        emit parentChanged();
        break;
    case QEvent::EnabledChange:
        emit enabledChanged();
        break;
    default:
        break;
    }

    QTableView::changeEvent(event);
}

void TableView::moveEvent(QMoveEvent *event) {
    if (event->pos().x() != event->oldPos().x()) {
        emit xChanged();
    }

    if (event->pos().y() != event->oldPos().y()) {
        emit yChanged();
    }

    QTableView::moveEvent(event);
}

void TableView::resizeEvent(QResizeEvent *event) {
    if (event->size().width() != event->oldSize().width()) {
        emit widthChanged();
    }

    if (event->size().height() != event->oldSize().height()) {
        emit heightChanged();
    }

    QTableView::resizeEvent(event);
}

void TableView::showEvent(QShowEvent *event) {
    emit visibleChanged();
    QTableView::showEvent(event);
}

void TableView::hideEvent(QHideEvent *event) {
    emit visibleChanged();
    QTableView::hideEvent(event);
}

void TableView::focusInEvent(QFocusEvent *event) {
    emit focusChanged();
    QTableView::focusInEvent(event);
}

void TableView::focusOutEvent(QFocusEvent *event) {
    emit focusChanged();
    QTableView::focusOutEvent(event);
}

void TableView::classBegin() {}

void TableView::componentComplete() {
    Q_D(TableView);

    d->componentComplete();

    if (this->parentWidget()) {
        this->setWindowFlags(Qt::Widget);
    }

    if (d->qmlVisible()) {
        this->show();
    }

    if (!this->rowNumbersVisible()) {
        this->verticalHeader()->hide();
    }
}

void TableViewPrivate::data_append(QDeclarativeListProperty<QObject> *list, QObject *obj) {
    if (!obj) {
        return;
    }

    if (TableView *view = qobject_cast<TableView*>(list->object)) {
        view->d_func()->dataList.append(obj);

        if (obj->isWidgetType()) {
            view->d_func()->childrenList.append(qobject_cast<QWidget*>(obj));
        }
    }
}

void TableViewPrivate::children_append(QDeclarativeListProperty<QWidget> *list, QWidget *widget) {
    if (!widget) {
        return;
    }

    if (TableView *view = qobject_cast<TableView*>(list->object)) {
        view->d_func()->childrenList.append(widget);
        view->d_func()->dataList.append(widget);
    }
}

void TableViewPrivate::actions_append(QDeclarativeListProperty<QObject> *list, QObject *obj) {
    if (!obj) {
        return;
    }

    if (TableView *view = qobject_cast<TableView*>(list->object)) {
        view->d_func()->actionList.append(obj);
        view->d_func()->dataList.append(obj);

        if (!view->d_func()->complete) {
            return;
        }

        if (QAction *action = qobject_cast<QAction*>(obj)) {
            view->addAction(action);
        }
        else if (QActionGroup *group = qobject_cast<QActionGroup*>(obj)) {
            view->addActions(group->actions());
        }
    }
}

QDeclarativeListProperty<QObject> TableViewPrivate::data() {
    return QDeclarativeListProperty<QObject>(q_func(), 0, TableViewPrivate::data_append);
}

QDeclarativeListProperty<QWidget> TableViewPrivate::children() {
    return QDeclarativeListProperty<QWidget>(q_func(), 0, TableViewPrivate::children_append);
}

QDeclarativeListProperty<QObject> TableViewPrivate::actions() {
    return QDeclarativeListProperty<QObject>(q_func(), 0, TableViewPrivate::actions_append);
}

QVariant TableViewPrivate::model() const {
    return modelVariant;
}

void TableViewPrivate::setModel(const QVariant &model) {
    Q_Q(TableView);

    if (model == this->model()) {
        return;
    }

    VariantTableModel *oldModel = variantModel;
    variantModel = 0;
    modelVariant = model;
    QObject *obj = qvariant_cast<QObject*>(model);
    QAbstractItemModel *itemModel = 0;

    if ((obj) && (itemModel = qobject_cast<QAbstractItemModel*>(obj))) {
        q->setModel(itemModel);
    }
    else {
        variantModel = new VariantTableModel(q);
        variantModel->setVariant(model);
        q->setModel(variantModel);
    }

    if (oldModel) {
        delete oldModel;
    }

    emit q->modelChanged();
}

QVariant TableViewPrivate::rootIndex() const {
    Q_Q(const TableView);

    return QVariant::fromValue<QModelIndex>(q->rootIndex());
}

void TableViewPrivate::setRootIndex(const QVariant &index) {
    if (index != this->rootIndex()) {
        Q_Q(TableView);
        q->setRootIndex(index.value<QModelIndex>());
        emit q->rootIndexChanged();
    }
}

QVariant TableViewPrivate::currentIndex() const {
    Q_Q(const TableView);

    return QVariant::fromValue<QModelIndex>(q->currentIndex());
}

void TableViewPrivate::setCurrentIndex(const QVariant &index) {
    if (index != this->currentIndex()) {
        Q_Q(TableView);
        q->setCurrentIndex(index.value<QModelIndex>());
        emit q->currentIndexChanged();
    }
}

QVariantList TableViewPrivate::selectedIndexes() const {
    Q_Q(const TableView);
    QVariantList indexes;
    
    foreach(QModelIndex index, q->selectedIndexes()) {
        indexes.append(QVariant::fromValue(index));
    }
    
    return indexes;
}

void TableViewPrivate::setSelectedIndexes(const QVariantList &indexes) {
    Q_Q(TableView);
    
    if (!q->selectionModel()) {
        return;
    }
    
    q->clearSelection();
    
    foreach (QVariant index, indexes) {
        q->selectionModel()->select(index.value<QModelIndex>(), QItemSelectionModel::Select);
    }
}

QDeclarativeComponent* TableViewPrivate::delegate() const {
    return delegateComponent;
}

void TableViewPrivate::setDelegate(QDeclarativeComponent *delegate) {
    if (delegate != delegateComponent) {
        Q_Q(TableView);
        QDeclarativeComponent *oldDelegate = delegateComponent;
        delegateComponent = delegate;

        if (delegateComponent) {
            delegateComponent->setParent(q);

            QDeclarativeContext *creationContext = delegateComponent->creationContext();
            QDeclarativeContext *context = new QDeclarativeContext(creationContext ? creationContext : qmlContext(q));

            if (QObject *obj = delegateComponent->create(context)) {
                if (QAbstractItemDelegate *delegate = qobject_cast<QAbstractItemDelegate*>(obj)) {
                    context->setParent(delegate);
                    context->setContextProperty("view", q);
                    q->setItemDelegate(delegate);
                }
                else {
                    qmlInfo(q) << TableView::tr("Component is not a valid item delegate.");
                    delete obj;
                    delete context;
                }
            }
            else {
                delete context;
            }
        }

        if (oldDelegate) {
            delete oldDelegate;
        }

        emit q->delegateChanged();
    }
}

void TableViewPrivate::_q_onHorizontalScrollPositionChanged() {
    Q_Q(TableView);

    if (!scrollTimer->isActive()) {
        emit q->movingChanged();

        if (atXBeginning) {
            atXBeginning = false;
            emit q->atXBeginningChanged();
        }

        if (atXEnd) {
            atXEnd = false;
            emit q->atXEndChanged();
        }
    }

    emit q->contentXChanged();

    scrollTimer->start();
}

void TableViewPrivate::_q_onVerticalScrollPositionChanged() {
    Q_Q(TableView);

    if (!scrollTimer->isActive()) {
        emit q->movingChanged();

        if (atYBeginning) {
            atYBeginning = false;
            emit q->atYBeginningChanged();
        }

        if (atXEnd) {
            atXEnd = false;
            emit q->atYEndChanged();
        }
    }

    emit q->contentYChanged();

    scrollTimer->start();
}

void TableViewPrivate::_q_onScrollingStopped() {
    Q_Q(TableView);

    bool xb = q->atXBeginning();
    bool xe = q->atYEnd();
    bool yb = q->atYBeginning();
    bool ye = q->atYEnd();

    if (xb != atXBeginning) {
        atXBeginning = xb;
        emit q->atXBeginningChanged();
    }

    if (xe != atXEnd) {
        atXEnd = xe;
        emit q->atXEndChanged();
    }

    if (yb != atYBeginning) {
        atYBeginning = yb;
        emit q->atYBeginningChanged();
    }

    if (ye != atYEnd) {
        atYEnd = ye;
        emit q->atYEndChanged();
    }

    emit q->movingChanged();
}

#include "moc_tableview_p.cpp"
