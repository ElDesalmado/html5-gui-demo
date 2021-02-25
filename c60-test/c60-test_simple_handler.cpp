// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "c60-test_simple_handler.h"

#include <sstream>
#include <array>
#include <future>
#include <iostream>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

SimpleHandler* g_instance = nullptr;

SimpleHandler::SimpleHandler()
{
    DCHECK(!g_instance);
    g_instance = this;
}

SimpleHandler::~SimpleHandler()
{
    g_instance = nullptr;
}

// static
SimpleHandler* SimpleHandler::GetInstance()
{
    return g_instance;
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    std::cout << "SimpleHandler::OnAfterCreated" << std::endl;

    // Add to the list of existing browsers.
    browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Closing the main window requires special handling. See the DoClose()
    // documentation in the CEF header for a detailed destription of this
    // process.
//    if (m_browser_list.size() == 1)
//    {
//        // Set a flag to indicate that the window close should be allowed.
//        is_closing_ = true;
//    }

    // Allow the close. For windowed browsers this will result in the OS close
    // event being sent.
    return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Remove from the list of existing browsers.
    BrowserList::iterator bit = browser_list_.begin();
    for (; bit != browser_list_.end(); ++bit)
    {
        if ((*bit)->IsSame(browser))
        {
            browser_list_.erase(bit);
            break;
        }
    }

    if (browser_list_.empty())
    {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}

void SimpleHandler::CloseAllBrowsers(bool force_close)
{
    if (!CefCurrentlyOn(TID_UI))
    {
    // Execute on the UI thread.
        CefPostTask(TID_UI, base::Bind(&SimpleHandler::CloseAllBrowsers, this,
                                       force_close));
        return;
    }

    if (browser_list_.empty())
        return;

    BrowserList::const_iterator it = browser_list_.begin();
    for (; it != browser_list_.end(); ++it)
        (*it)->GetHost()->CloseBrowser(force_close);
}




