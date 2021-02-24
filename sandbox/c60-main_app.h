#pragma once

#include "include/cef_app.h"
#include "include/views/cef_window.h"
#include "c60-browser_manager.h"

#include <future>
#include <string>

// Implement application-level callbacks for the browser process.
class MainApp : public CefApp,
                public CefBrowserProcessHandler
{
public:
    using WindowParams = std::vector<std::pair<std::string, uint32_t>>;

public:
    MainApp(const char *relativePath,
            WindowParams urlsAndDisplayIndices = { { "http://www.google.com"  , 0 } })
    :   m_relativePath(relativePath),
        m_urlsAndDisplayIndices(urlsAndDisplayIndices)
    {
        // TODO: remove .exe part in relative path
        auto lastSlash = m_relativePath.rfind('\\');
        assert(lastSlash != std::string::npos && "Failed to find last slash");
        m_relativePath = m_relativePath.substr(0, lastSlash + 1);
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
    WindowParams m_urlsAndDisplayIndices;
    std::string m_relativePath;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(MainApp);
    DISALLOW_COPY_AND_ASSIGN(MainApp);
};
