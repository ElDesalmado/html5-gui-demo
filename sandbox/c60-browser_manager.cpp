#include "c60-browser_manager.h"

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

#include <future>
#include <iostream>

BrowserManager* g_instance = nullptr;

BrowserManager::BrowserManager()
:   m_reloadSecondaryWindow(true)
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

std::vector<CefCookie> cookieStorage;
uint32_t counter = 0;

class MyCefCookieVisitor : public CefCookieVisitor
{
public:
    virtual bool Visit(const CefCookie& cookie,
                       int count,
                       int total,
                       bool& deleteCookie) override
    {
        deleteCookie = false;
        if (total == counter)
        {
            counter = 0;
            return false;
        }

        cookieStorage.push_back(cookie);
        counter++;
        return true;
    }

IMPLEMENT_REFCOUNTING(MyCefCookieVisitor);
};

void BrowserManager::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    CEF_REQUIRE_UI_THREAD();

    std::cout << "OnLoadEnd browser ID: " << browser->GetIdentifier() <<
                 ", current url: " << frame->GetURL() << std::endl;

    if (m_browser_list.size() != 2)
        return;

    if (!m_reloadSecondaryWindow)
        return;

    if (m_browser_list.front()->GetIdentifier() !=
        browser->GetIdentifier())
        return;

    if (frame->GetURL().ToString().find("http://192.168.165.114:8080/faces/_dog_/", 0) ==
        std::string::npos)
        return;

    auto globalManager = CefCookieManager::GetGlobalManager(nullptr);
    globalManager->VisitAllCookies(new MyCefCookieVisitor);

    if (browser->GetIdentifier() !=
        m_browser_list.front()->GetIdentifier())
        return;

    m_reloadSecondaryWindow = false;
    m_browser_list.back()->GetFocusedFrame()->LoadURL("http://192.168.165.114:8080/faces/OopTask/oopPage");
}






