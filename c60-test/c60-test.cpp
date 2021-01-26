
#include <helper/DirUtil.hpp>
#include <iostream>

#include <cef_cmake/disable_warnings.h>
#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/wrapper/cef_resource_manager.h>
#include <cef_cmake/reenable_warnings.h>
#include "include/views/cef_browser_view.h"

#include <jsbind.hpp>

// this is only needed so we have a way to break the message loop
class Client : public CefClient, public CefLifeSpanHandler
{
public:
    Client() = default;

    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }

    void OnBeforeClose(CefRefPtr<CefBrowser> /*browser*/) override
    {
        CefQuitMessageLoop();
    }

private:
    IMPLEMENT_REFCOUNTING(Client);
    DISALLOW_COPY_AND_ASSIGN(Client);
};

class TestApp : public CefApp
{
public:
    TestApp() = default;

private:
    IMPLEMENT_REFCOUNTING(TestApp);
    DISALLOW_COPY_AND_ASSIGN(TestApp);
};

int main(int argc, char* argv[])
{
    CefRefPtr<CefCommandLine> commandLine = CefCommandLine::CreateCommandLine();
#if defined(_WIN32)
    CefEnableHighDPISupport();
    CefMainArgs args(GetModuleHandle(NULL));
    commandLine->InitFromString(GetCommandLineW());
#else
    CefMainArgs args(argc, argv);
    commandLine->InitFromArgv(argc, argv);
#endif

    void* windowsSandboxInfo = NULL;

#if defined(CEF_USE_SANDBOX) && defined(_WIN32)
    // Manage the life span of the sandbox information object. This is necessary
    // for sandbox support on Windows. See cef_sandbox_win.h for complete details.
    CefScopedSandboxInfo scopedSandbox;
    windowsSandboxInfo = scopedSandbox.sandbox_info();
#endif

    CefRefPtr<CefApp> app = nullptr;
    std::string appType = commandLine->GetSwitchValue("type");
    app = new TestApp;
    int result = CefExecuteProcess(args, app, windowsSandboxInfo);
    if (result >= 0)
    {
        // child process completed
        return result;
    }

    CefSettings settings;
    settings.remote_debugging_port = 1234;
#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif

    CefInitialize(args, settings, app, windowsSandboxInfo);

    CefWindowInfo windowInfo;

#if defined(_WIN32)
    // On Windows we need to specify certain flags that will be passed to CreateWindowEx().
    windowInfo.SetAsPopup(NULL, "C60 test");
#endif
    CefBrowserSettings browserSettings;
    CefBrowserHost::CreateBrowser(windowInfo, new Client, "http://192.168.166.115:8080", browserSettings, nullptr, nullptr);

    CefRunMessageLoop();

    CefShutdown();

    return 0;
}
