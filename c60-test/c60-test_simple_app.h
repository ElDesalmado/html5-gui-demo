// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"
#include <future>

// Implement application-level callbacks for the browser process.
class SimpleApp : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler
{
public:
    SimpleApp() = default;

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

    // CefRenderProcessHandler methods:
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) override;

private:
    std::future<void> m_future;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleApp);
    DISALLOW_COPY_AND_ASSIGN(SimpleApp);
};
