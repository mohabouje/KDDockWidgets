/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "ClassicIndicatorsWindow.h"
#include "kddockwidgets/core/indicators/ClassicDropIndicatorOverlay.h"
#include "core/Utils_p.h"

#include <QQmlContext>
#include <QQuickItem>

using namespace KDDockWidgets;
using namespace KDDockWidgets::Core;
using namespace KDDockWidgets::QtQuick;

namespace KDDockWidgets {

static QString iconName(DropLocation loc, bool active)
{
    QString suffix = active ? QStringLiteral("_active") : QString();

    QString name;
    switch (loc) {
    case DropLocation_Center:
        name = QStringLiteral("center");
        break;
    case DropLocation_Left:
        name = QStringLiteral("inner_left");
        break;
    case DropLocation_Right:
        name = QStringLiteral("inner_right");
        break;
    case DropLocation_Bottom:
        name = QStringLiteral("inner_bottom");
        break;
    case DropLocation_Top:
        name = QStringLiteral("inner_top");
        break;
    case DropLocation_OutterLeft:
        name = QStringLiteral("outter_left");
        break;
    case DropLocation_OutterBottom:
        name = QStringLiteral("outter_bottom");
        break;
    case DropLocation_OutterRight:
        name = QStringLiteral("outter_right");
        break;
    case DropLocation_OutterTop:
        name = QStringLiteral("outter_top");
        break;
    case DropLocation_None:
    case DropLocation_Inner:
    case DropLocation_Outter:
    case DropLocation_Horizontal:
    case DropLocation_Vertical:
        return QString();
    }

    return name + suffix;
}
}


IndicatorWindow::IndicatorWindow(Core::ClassicDropIndicatorOverlay *classicIndicators)
    : QQuickView()
    , m_classicIndicators(classicIndicators)
{
    setFlags(flags() | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint | Qt::Tool);
    setColor(Qt::transparent);

    rootContext()->setContextProperty(QStringLiteral("_window"),
                                      QVariant::fromValue<QObject *>(this));
    setSource(
        QUrl(QStringLiteral("qrc:/kddockwidgets/qtquick/views/qml/ClassicIndicatorsOverlay.qml")));

    connect(classicIndicators, &ClassicDropIndicatorOverlay::indicatorsVisibleChanged, this,
            &IndicatorWindow::indicatorsVisibleChanged);

    connect(classicIndicators, &ClassicDropIndicatorOverlay::hoveredGroupRectChanged, this,
            &IndicatorWindow::hoveredGroupRectChanged);

    connect(classicIndicators, &ClassicDropIndicatorOverlay::currentDropLocationChanged, this,
            &IndicatorWindow::currentDropLocationChanged);

    // Two workarounds for two unrelated bugs:
    if (KDDockWidgets::isOffscreen()) {
        // 1. We need to create the window asap, otherwise, if a drag triggers the indicator window
        // to show, that creates a QOffscreenWindow, which flushes events in the ctor, triggering
        // more hover events which will trigger another QOffscreenWindow.
        // We then end up with a QWindow with two QPlatformWindow, and only one is deleted in
        // at shutdown, meaning some timers aren't unregistered, meaning we get a crash when
        // the timer event is sent to the destroyed QWindow.
        create();
    } else {
        // 2.
        // Small hack to avoid flickering when we drag over a window the first time
        // Not sure why a simply create() doesn't work instead
        // Not if offscreen though, as that QPA is flaky with window activation/focus
        resize(QSize(1, 1));
        show();
        hide();
    }
}

DropLocation IndicatorWindow::hover(QPoint pt)
{
    QQuickItem *item = indicatorForPos(pt);
    const DropLocation loc = item ? locationForIndicator(item) : DropLocation_None;
    classicIndicators()->setDropLocation(loc);
    return loc;
}

QQuickItem *IndicatorWindow::indicatorForPos(QPoint pt) const
{
    const QVector<QQuickItem *> indicators = indicatorItems();
    Q_ASSERT(indicators.size() == 9);

    for (QQuickItem *item : indicators) {
        if (item->isVisible()) {
            QRect rect(0, 0, int(item->width()), int(item->height()));
            rect.moveTopLeft(item->mapToGlobal(QPointF(0, 0)).toPoint());
            if (rect.contains(pt)) {
                return item;
            }
        }
    }

    return nullptr;
}

void IndicatorWindow::updatePositions()
{
    // Not needed to implement, the Indicators use QML anchors
}

QPoint IndicatorWindow::posForIndicator(KDDockWidgets::DropLocation loc) const
{
    QQuickItem *indicator = IndicatorWindow::indicatorForLocation(loc);
    return indicator->mapToGlobal(indicator->boundingRect().center()).toPoint();
}

QString IndicatorWindow::iconName(int loc, bool active) const
{
    return KDDockWidgets::iconName(DropLocation(loc), active);
}

ClassicDropIndicatorOverlay *IndicatorWindow::classicIndicators() const
{
    return m_classicIndicators;
}

QQuickItem *IndicatorWindow::indicatorForLocation(DropLocation loc) const
{
    const QVector<QQuickItem *> indicators = indicatorItems();
    Q_ASSERT(indicators.size() == 9);

    for (QQuickItem *item : indicators) {
        if (locationForIndicator(item) == loc)
            return item;
    }

    qWarning() << Q_FUNC_INFO << "Couldn't find indicator for location" << loc;
    return nullptr;
}

DropLocation IndicatorWindow::locationForIndicator(const QQuickItem *item) const
{
    return DropLocation(item->property("indicatorType").toInt());
}

QVector<QQuickItem *> IndicatorWindow::indicatorItems() const
{
    QVector<QQuickItem *> indicators;
    indicators.reserve(9);

    QQuickItem *root = rootObject();
    const QList<QQuickItem *> items = root->childItems();
    for (QQuickItem *item : items) {
        if (QString::fromLatin1(item->metaObject()->className())
                .startsWith(QLatin1String("ClassicIndicator_QMLTYPE"))) {
            indicators.push_back(item);
        } else if (item->objectName() == QLatin1String("innerIndicators")) {
            const QList<QQuickItem *> innerIndicators = item->childItems();
            for (QQuickItem *innerItem : innerIndicators) {
                if (QString::fromLatin1(innerItem->metaObject()->className())
                        .startsWith(QLatin1String("ClassicIndicator_QMLTYPE"))) {
                    indicators.push_back(innerItem);
                }
            }
        }
    }

    return indicators;
}

void IndicatorWindow::raise()
{
    QWindow::raise();
}

void IndicatorWindow::setGeometry(QRect geo)
{
    QWindow::setGeometry(geo);
}

void IndicatorWindow::setObjectName(const QString &name)
{
    QObject::setObjectName(name);
}

void IndicatorWindow::setVisible(bool is)
{
    QWindow::setVisible(is);
}

void IndicatorWindow::resize(QSize sz)
{
    QWindow::resize(sz);
}

bool IndicatorWindow::isWindow() const
{
    return true;
}

bool IndicatorWindow::innerLeftIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_Left);
}

bool IndicatorWindow::innerRightIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_Right);
}

bool IndicatorWindow::innerTopIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_Top);
}

bool IndicatorWindow::innerBottomIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_Bottom);
}

bool IndicatorWindow::outterLeftIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_OutterLeft);
}

bool IndicatorWindow::outterRightIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_OutterRight);
}

bool IndicatorWindow::outterTopIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_OutterTop);
}

bool IndicatorWindow::outterBottomIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_OutterBottom);
}

bool IndicatorWindow::tabIndicatorVisible() const
{
    return m_classicIndicators->dropIndicatorVisible(DropLocation_Center);
}

QRect IndicatorWindow::hoveredGroupRect() const
{
    return m_classicIndicators->hoveredGroupRect();
}

DropLocation IndicatorWindow::currentDropLocation() const
{
    return m_classicIndicators->currentDropLocation();
}
