/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2019-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

import 'package:KDDockWidgets/View_mixin.dart';
import 'package:KDDockWidgets/PositionedWidget.dart';
import 'package:KDDockWidgetsBindings/Bindings.dart';
import 'package:KDDockWidgetsBindings/Bindings_KDDWBindingsCore.dart'
    as KDDWBindingsCore;
import 'package:KDDockWidgetsBindings/Bindings_KDDWBindingsFlutter.dart'
    as KDDWBindingsFlutter;
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart' hide View;
import 'package:flutter/material.dart' as material show TabBar;
import 'package:flutter/rendering.dart';

class TabBar extends KDDWBindingsFlutter.TabBar with View_mixin {
  late final KDDWBindingsCore.TabBar m_controller;

  TabBar(KDDWBindingsCore.TabBar? tabBar, KDDWBindingsCore.View? parent)
      : super(tabBar, parent: parent) {
    m_fillsParent = true;
    m_controller = tabBar!;
    initMixin(this);
  }

  Widget createFlutterWidget() {
    return TabBarWidget(kddwView, this, key: widgetKey);
  }

  @override
  onRebuildRequested() {}

  void tabsForTabBar(RenderBox rb, List<RenderBox> result) {
    rb.visitChildren((child) {
      final rbChild = child as RenderBox?;
      if (rbChild != null) {
        if (rbChild.runtimeType.toString() == "_TabLabelBarRenderer") {
          rbChild.visitChildren((child) {
            final tab = child as RenderBox?;
            if (tab != null) {
              result.add(tab);
            }
          });
        } else {
          tabsForTabBar(rbChild, result);
        }
      }
    });
  }

  void dumpRenderBox(RenderBox rb, int level) {
    print(
        "${"".padLeft(level * 4)} $rb size=${rb.size} type=${rb.runtimeType.toString()}");

    rb.visitChildren((child) {
      final rbChild = child as RenderBox?;
      if (rbChild != null) {
        dumpRenderBox(rbChild, level + 1);
      }
    });
  }

  @override
  int tabAt(QPoint localPos) {
    final localX = localPos.x().toDouble();
    final localY = localPos.y().toDouble();

    final renderBox = widgetKey.currentContext?.findRenderObject() as RenderBox;
    final windowCoords = renderBox.localToGlobal(Offset(localX, localY));

    List<RenderBox> tabs = [];
    tabsForTabBar(renderBox, tabs);

    for (int i = 0; i < tabs.length; ++i) {
      final tab = tabs[i];
      final tabWindowPos = tab.localToGlobal(Offset.zero);

      final tabWindowRect = Rect.fromLTWH(
          tabWindowPos.dx, tabWindowPos.dy, tab.size.width, tab.size.height);

      if (tabWindowRect.contains(windowCoords)) return i;
    }

    return -1;
  }
}

class TabBarWidget extends PositionedWidget {
  final TabBar TabBarView;
  TabBarWidget(var kddwView, this.TabBarView, {Key? key})
      : super(kddwView, key: key);

  @override
  State<PositionedWidget> createState() {
    return TabBarPositionedWidgetState(kddwView, TabBarView);
  }
}

class TabBarPositionedWidgetState extends PositionedWidgetState {
  final TabBar m_tabBarView;

  TabBarPositionedWidgetState(var kddwView, this.m_tabBarView)
      : super(kddwView);

  @override
  Widget buildContents(BuildContext ctx) {
    final int numTabs = m_tabBarView.m_controller.numDockWidgets();
    final tabs = <Widget>[];
    for (var i = 0; i < numTabs; ++i) {
      final dw = m_tabBarView.m_controller.dockWidgetAt_2(i);
      tabs.add(Tab(text: "${dw.title().toDartString()}"));
    }

    return Listener(
        onPointerDown: (event) {
          kddwView.onFlutterMouseEvent(event);
        },
        onPointerUp: (event) {
          kddwView.onFlutterMouseEvent(event);
        },
        onPointerMove: (event) {
          if (event.buttons != kPrimaryButton) return;
          kddwView.onFlutterMouseEvent(event);
        },
        child: SizedBox(
            height: 50,
            child: DefaultTabController(
              length: numTabs,
              child: material.TabBar(
                tabs: tabs,
                labelColor: Colors.black,
              ),
            )));
  }
}
