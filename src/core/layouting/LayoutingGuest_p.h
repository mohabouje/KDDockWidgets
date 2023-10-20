/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "kddockwidgets/docks_export.h"
#include "QtCompat_p.h"

#include <kdbindings/signal.h>

namespace KDDockWidgets {

namespace Core {

class Item;
class LayoutingHost;

/// The interface graphical components need to implement in order to be hosted by a layout
/// The layout engine doesn't know about any GUI, only about LayoutingHost and LayoutingGuest
/// This allows to keep the layouting engine separate from the rest of KDDW and even
/// reused by non-KDDW projects

class DOCKS_EXPORT LayoutingGuest
{
public:
    virtual ~LayoutingGuest();
    virtual void setLayoutItem(Item *) = 0;
    virtual Size minSize() const = 0;
    virtual Size maxSizeHint() const = 0;
    virtual void setGeometry(Rect r) = 0;
    virtual void setVisible(bool is) const = 0;
    virtual Rect guestGeometry() const = 0;
    virtual QString debugName() const = 0;
    virtual void setHost(LayoutingHost *parent) = 0;
    virtual LayoutingHost *host() const = 0;
    virtual QString id() const = 0;
    virtual bool freed() const
    {
        return false;
    }

#ifdef DOCKS_DEVELOPER_MODE
    virtual void setMinimumSize(Size) = 0;
    virtual void setMaximumSize(Size) = 0;
    virtual Size size() const = 0;
#endif

    KDBindings::Signal<LayoutingHost *> hostChanged;
    KDBindings::Signal<> beingDestroyed;
    KDBindings::Signal<> layoutInvalidated;
};

}

}
