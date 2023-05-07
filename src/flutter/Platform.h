/*
  This file is part of KDDockWidgets.

  SPDX-FileCopyrightText: 2020-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Sérgio Martins <sergio.martins@kdab.com>

  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#pragma once

#include "kddockwidgets/core/Platform.h"

#include <optional>

namespace KDDockWidgets {

namespace Core {
class MainWindow;
}

namespace flutter {

/// @brief implements functions specific to a particular platform
/// A platform can be for example qtwidgets, qtquick, etc.
class DOCKS_EXPORT Platform : public Core::Platform
{
public:
    Platform();
    ~Platform() override;

    static Platform *platformFlutter()
    {
        return static_cast<Platform *>(Platform::instance());
    }

    const char *name() const override;
    bool hasActivePopup() const override;
    Core::ViewFactory *createDefaultViewFactory() override;
    std::shared_ptr<Core::Window> windowAt(QPoint globalPos) const override;

    int screenNumberFor(Core::View *) const override;
    QSize screenSizeFor(Core::View *) const override;

    Core::View *createView(Core::Controller *controller, Core::View *parent = nullptr) const override;
    bool inDisallowedDragView(QPoint globalPos) const override;
    bool usesFallbackMouseGrabber() const override;
    void ungrabMouse() override;
    QVector<std::shared_ptr<Core::Screen>> screens() const override;
    std::shared_ptr<Core::Screen> primaryScreen() const override;

    void onFloatingWindowCreated(Core::FloatingWindow *) override;
    void onFloatingWindowDestroyed(Core::FloatingWindow *) override;

#ifdef DOCKS_DEVELOPER_MODE
    void tests_initPlatform_impl() override;
    void tests_deinitPlatform_impl() override;
    Core::View *tests_createView(Core::CreateViewOptions, Core::View *parent = nullptr) override;
    std::shared_ptr<Core::Window> tests_createWindow() override;
    Core::View *tests_createFocusableView(Core::CreateViewOptions, Core::View *parent = nullptr) override;
    Core::View *tests_createNonClosableView(Core::View *parent = nullptr) override;
    Core::MainWindow *
    createMainWindow(const QString &uniqueName, Core::CreateViewOptions viewOpts,
                     MainWindowOptions options = MainWindowOption_HasCentralFrame,
                     Core::View *parent = nullptr, Qt::WindowFlags flags = {}) const override;

    bool tests_waitForWindowActive(std::shared_ptr<Core::Window>, int timeout) const override;
    bool tests_waitForResize(Core::View *, int timeout) const override;
    bool tests_waitForResize(Core::Controller *, int timeout) const override;
    bool tests_waitForEvent(QObject *w, Event::Type type, int timeout) const override;
    bool tests_waitForEvent(Core::View *, Event::Type type, int timeout) const override;
    bool tests_waitForEvent(std::shared_ptr<Core::Window>, Event::Type type, int timeout) const override;
    bool tests_waitForDeleted(Core::View *, int timeout) const override;
    bool tests_waitForDeleted(QObject *, int timeout) const override;
    void tests_sendEvent(std::shared_ptr<Core::Window> window, Event *ev) const override;
    void tests_wait(int ms) override;
    void installMessageHandler() override;
    void uninstallMessageHandler() override;

    /// Pauses execution, so we can attach Dart's debugger
    virtual void pauseForDartDebugger() = 0;

    typedef int (*RunTestsFunc)();
    static RunTestsFunc s_runTestsFunc;
    std::optional<int> m_testsResult;
    void runTests();
    std::optional<int> testsResult() const;
#endif
protected:
    void init();

    // Platform interface
public:
    std::shared_ptr<Core::View> focusedView() const override;
    QVector<std::shared_ptr<Core::Window>> windows() const override;
    void sendEvent(Core::View *, Event *) const override;
    int screenNumberFor(std::shared_ptr<Core::Window>) const override;
    bool isProcessingAppQuitEvent() const override;
    QString applicationName() const override;
    QString organizationName() const override;
    void setMouseCursor(Qt::CursorShape) override;
    void restoreMouseCursor() override;
    DisplayType displayType() const override;
    bool isLeftMouseButtonPressed() const override;
    QPoint cursorPos() const override;
    void setCursorPos(QPoint) override;
    void setFocusedView(std::shared_ptr<Core::View>);

    std::shared_ptr<Core::View> m_focusedView;
};

}

}
