// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "c60-test_simple_app.h"
#include "c60-test_simple_handler.h"

#include <string>

#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
template<int x, int y>
class SimpleWindowDelegate : public CefWindowDelegate
{
public:
    explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
    :   browser_view_(browser_view)
    {}

    void OnWindowCreated(CefRefPtr<CefWindow> window) override
    {
        // Add the browser view and show the window.
        window->AddChildView(browser_view_);
        window->Show();

        window->SetFullscreen(true);
    }

    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override 
    {
        browser_view_ = nullptr;
    }

    bool CanClose(CefRefPtr<CefWindow> window) override 
    {
        // Allow the window to close if the browser says it's OK.
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser)
            return browser->GetHost()->TryCloseBrowser();
        return true;
    }

    CefSize GetPreferredSize(CefRefPtr<CefView> view) override 
    {
        return CefSize(800, 600);
    }

    virtual CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override
    {
        return { x, y, 800, 600 };
    }

private:
    CefRefPtr<CefBrowserView> browser_view_;

    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

class SimpleBrowserViewDelegate : public CefBrowserViewDelegate
{
public:
    SimpleBrowserViewDelegate() {}

private:
    IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

void SimpleApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();

    // SimpleHandler implements browser-level callbacks.
    CefRefPtr<SimpleHandler> handler(new SimpleHandler());

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::string url = "http://www.google.com";

    // Create the BrowserView.
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
            handler, url, browser_settings, nullptr, nullptr,
            new SimpleBrowserViewDelegate());

    // Create the Window. It will show itself after creation.
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate<0, 0>(browser_view));

    CefRefPtr<CefBrowserView> browser_view_ = CefBrowserView::CreateBrowserView(
            handler, url, browser_settings, nullptr, nullptr,
            new SimpleBrowserViewDelegate());

    // Create the Window. It will show itself after creation.
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate<1600, 10>(browser_view_));
}