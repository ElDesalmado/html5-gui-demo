#pragma once

#include "include/cef_client.h"
#include "include/cef_render_process_handler.h"

#include <list>

class BrowserManager : public CefClient,
                       public CefLifeSpanHandler
{
public:
    explicit BrowserManager();
    ~BrowserManager();

    // Provide access to the single global instance of this object.
    static BrowserManager* GetInstance();

    // CefClient methods:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

    // CefLifeSpanHandler methods:
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

private:
    // List of existing browser windows. Only accessed on the CEF UI thread.
    typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
    BrowserList m_browser_list;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(BrowserManager);
};
