// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once

#include "include/cef_client.h"

#include <list>

class SimpleHandler : public CefClient,
                      public CefLifeSpanHandler
{
public:
    explicit SimpleHandler();
    ~SimpleHandler();

    // Provide access to the single global instance of this object.
    static SimpleHandler* GetInstance();

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    // Request that all existing browser windows close.
    void CloseAllBrowsers(bool force_close);

private:
    // List of existing browser windows. Only accessed on the CEF UI thread.
    typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
    BrowserList browser_list_;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(SimpleHandler);
};
