// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_app.h"
#include <future>

#include <string>

// Implement application-level callbacks for the browser process.
class SimpleApp : public CefApp,
                  public CefBrowserProcessHandler,
                  public CefRenderProcessHandler
{
public:
    SimpleApp(const char *relativePath)
        : relativePath_(relativePath)
    {
        // TODO: remove .exe part in relative path
        auto lastSlash = relativePath_.rfind('\\');
        assert(lastSlash != std::string::npos && "Failed to find last slash");
        relativePath_ = relativePath_.substr(0, lastSlash + 1);
    }

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

    // CefRenderProcessHandler methods:
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefRefPtr<CefV8Context> context) override;

private:
    uint32_t w1_number,
             w2_number;
    std::string relativePath_;
    std::future<void> m_future_{};

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleApp);
    DISALLOW_COPY_AND_ASSIGN(SimpleApp);
};
