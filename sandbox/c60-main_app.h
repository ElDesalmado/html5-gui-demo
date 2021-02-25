#pragma once

#include "include/cef_app.h"
#include "include/views/cef_window.h"
#include "c60-browser_manager.h"

#include <future>
#include <string>
#include <array>

// Implement application-level callbacks for the browser process.
class MainApp : public CefApp,
                public CefBrowserProcessHandler
{
public:
    static constexpr uint32_t WINDOW_COUNT = 2;

    MainApp(const std::array<std::string, WINDOW_COUNT>& urls)
    :   m_urls(urls)
    {

    }

    // CefApp methods:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

    // CefBrowserProcessHandler methods:
    virtual void OnContextInitialized() override;

private:
    // create window with "url" and open it on the "displayPtr"
    void CreateWindowOnDisplay(const std::string& url, CefRefPtr<CefDisplay> displayPtr,
                               CefRefPtr<BrowserManager> handler, const CefBrowserSettings& settings);

    // add display index derived from console params
    void AddDisplayIndex(uint32_t windowIndex, uint32_t displayIndex);

private:

    std::array<std::string, WINDOW_COUNT> m_urls;
    std::array<uint32_t, WINDOW_COUNT> m_displayNumber;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(MainApp);
    DISALLOW_COPY_AND_ASSIGN(MainApp);
};
