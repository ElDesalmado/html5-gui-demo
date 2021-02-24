#include "c60-browser_manager.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <future>
#include <iostream>

BrowserManager* g_instance = nullptr;

BrowserManager::BrowserManager()
{
    DCHECK(!g_instance);
    g_instance = this;
}

BrowserManager::~BrowserManager()
{
    g_instance = nullptr;
}

BrowserManager* BrowserManager::GetInstance()
{
    return g_instance;
}

void BrowserManager::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    std::cout << "BrowserManager::OnAfterCreated" << std::endl;

    // Add to the list of existing browsers.
    m_browser_list.push_back(browser);
}

void BrowserManager::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();

    // Remove from the list of existing browsers.
    BrowserList::iterator bit = m_browser_list.begin();
    for (; bit != m_browser_list.end(); ++bit)
    {
        if ((*bit)->IsSame(browser))
        {
            m_browser_list.erase(bit);
            break;
        }
    }

    if (m_browser_list.empty())
    {
        // All browser windows have closed. Quit the application message loop.
        CefQuitMessageLoop();
    }
}


