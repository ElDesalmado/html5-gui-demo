// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "c60-main_app.h"

#include "include/views/cef_browser_view.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_request_context_handler.h"
#include "include/cef_cookie.h"

#include <string>
#include <future>
#include <iostream>

// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
class FullscreenWindowDelegate : public CefWindowDelegate
{
public:
    explicit FullscreenWindowDelegate(CefRefPtr<CefBrowserView> browser_view,
                                      CefRefPtr<CefDisplay> display = nullptr)
    : m_browser_view(browser_view),
      m_display(display)
    {}

    void OnWindowCreated(CefRefPtr<CefWindow> window) override
    {
        // Add the browser view and show the window.
        window->AddChildView(m_browser_view);
        window->Show();

        window->SetFullscreen(true);
    }

    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override 
    {
        m_browser_view = nullptr;
    }

    bool CanClose(CefRefPtr<CefWindow> window) override 
    {
        // Allow the window to close if the browser says it's OK.
        CefRefPtr<CefBrowser> browser = m_browser_view->GetBrowser();
        if (browser)
            return browser->GetHost()->TryCloseBrowser();
        return true;
    }

    virtual CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override
    {
        if (m_display != nullptr)
            return m_display->GetBounds();
        else return CefDisplay::GetPrimaryDisplay()->GetBounds();
    }

private:
    CefRefPtr<CefBrowserView> m_browser_view;

    IMPLEMENT_REFCOUNTING(FullscreenWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(FullscreenWindowDelegate);

    CefRefPtr<CefDisplay> m_display;
};

class FullscreenBrowserViewDelegate : public CefBrowserViewDelegate
{
public:
    FullscreenBrowserViewDelegate() {}

private:
    IMPLEMENT_REFCOUNTING(FullscreenBrowserViewDelegate);
    DISALLOW_COPY_AND_ASSIGN(FullscreenBrowserViewDelegate);
};

constexpr uint32_t MainApp::WINDOW_COUNT;

void MainApp::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line)
{
    std::vector<CefString> argv;
    command_line->GetArgv(argv);
    for(auto iter = argv.begin(); iter != argv.end(); ++iter)
    {
        std::string paramName = iter->ToString();
        if (paramName.size() == 2 &&
            paramName[0] == 'w' &&
            std::isdigit(paramName[1]))
        {
            uint32_t index = paramName[1] - 48;
            try
            {
                if (++iter != argv.end())
                    AddDisplayIndex(index, std::stoi(iter->ToString()));
            }
            catch(std::exception& e)
            {
                std::cout << e.what() << std::endl;
            }
        }

    }
}

void MainApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    std::cout << "MainApp::OnContextInitialized" << std::endl;

    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();

    // BrowserManager implements browser-level callbacks.
    CefRefPtr<BrowserManager> handler(new BrowserManager());

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::vector<CefRefPtr<CefDisplay>> displays;
    CefDisplay::GetAllDisplays(displays);

    for(size_t i = 0; i < WINDOW_COUNT; ++i)
    {
        auto displayNumber = m_displayNumber[i];
        auto displayPtr = displays.size() > displayNumber ?
                          displays[displayNumber] : nullptr;
        CreateWindowOnDisplay(m_urls[i], displayPtr, handler, browser_settings);
    }
}

void MainApp::AddDisplayIndex(uint32_t windowIndex, uint32_t displayIndex)
{
    if (WINDOW_COUNT > windowIndex)
        m_displayNumber[windowIndex] = displayIndex;
}

void MainApp::CreateWindowOnDisplay(const std::string &url, CefRefPtr<CefDisplay> displayPtr,
                                    CefRefPtr<BrowserManager> handler, const CefBrowserSettings& settings)
{
    // static MyCefRequestContext staticContex{};

    // Create the BrowserView.
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
            handler, url, settings, nullptr, nullptr,
            new FullscreenBrowserViewDelegate());

    // Create the Window. It will show itself after creation.
    CefWindow::CreateTopLevelWindow(new FullscreenWindowDelegate(browser_view, displayPtr));
}

