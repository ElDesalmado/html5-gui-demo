// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "c60-test_simple_app.h"
#include "c60-test_simple_handler.h"

#include <string>
#include <future>
#include <iostream>

#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
template<int x, int y>
class SimpleWindowDelegate : public CefWindowDelegate
{
public:
    explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
    :   browser_view_(browser_view)
    {}

    void OnWindowCreated(CefRefPtr<CefWindow> window) override
    {
        // Add the browser view and show the window.
        window->AddChildView(browser_view_);
        window->Show();

        //window->SetFullscreen(true);
    }

    void OnWindowDestroyed(CefRefPtr<CefWindow> window) override 
    {
        browser_view_ = nullptr;
    }

    bool CanClose(CefRefPtr<CefWindow> window) override 
    {
        // Allow the window to close if the browser says it's OK.
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser)
            return browser->GetHost()->TryCloseBrowser();
        return true;
    }

    CefSize GetPreferredSize(CefRefPtr<CefView> view) override 
    {
        return CefSize(800, 600);
    }

    virtual CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override
    {
        return { x, y, 800, 600 };
    }

private:
    CefRefPtr<CefBrowserView> browser_view_;

    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

class SimpleBrowserViewDelegate : public CefBrowserViewDelegate
{
public:
    SimpleBrowserViewDelegate() {}

private:
    IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

void SimpleApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    std::cout << "SimpleApp::OnContextInitialized" << std::endl;

    CefRefPtr<CefCommandLine> command_line =
            CefCommandLine::GetGlobalCommandLine();

    // SimpleHandler implements browser-level callbacks.
    CefRefPtr<SimpleHandler> handler(new SimpleHandler());

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::string url = "http://www.google.com";

    // Create the BrowserView.
//    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
//            handler, url, browser_settings, nullptr, nullptr,
//            new SimpleBrowserViewDelegate());
//
//    // Create the Window. It will show itself after creation.
//    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate<0, 0>(browser_view));

    CefRefPtr<CefBrowserView> browser_view_ = CefBrowserView::CreateBrowserView(
            handler, std::string("file:///" + relativePath_ + "html/shared-buffer-test.html"),
            browser_settings, nullptr, nullptr,
            new SimpleBrowserViewDelegate());

    // Create the Window. It will show itself after creation.
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate<1600, 10>(browser_view_));
}

class DrawTask : public CefTask
{
public:
    DrawTask(CefRefPtr<CefV8Value> callbackDrawFunc, CefRefPtr<CefV8Context> context)
    :   m_callbackDrawFunc(callbackDrawFunc),
        m_context(context)
    {}

    virtual void Execute() override
    {
        CEF_REQUIRE_RENDERER_THREAD()

//        std::cout << "DrawTask context/callaback: " << m_context.get()
//                  << " " << m_callbackDrawFunc.get() << std::endl;

        m_context->Enter();

        m_callbackDrawFunc->ExecuteFunctionWithContext(m_context, NULL, CefV8ValueList{});

        m_context->Exit();
    }

private:
    CefRefPtr<CefV8Value> m_callbackDrawFunc;
    CefRefPtr<CefV8Context> m_context;

    IMPLEMENT_REFCOUNTING(DrawTask);
    DISALLOW_COPY_AND_ASSIGN(DrawTask);
};

class SwapTask : public CefTask
{
public:
    SwapTask(CefRefPtr<CefV8Value> callbackSwapFunc, CefRefPtr<CefV8Context> context)
    :   m_callbackSwapFunc(callbackSwapFunc),
        m_context(context)
    {}

    virtual void Execute() override
    {
        CEF_REQUIRE_RENDERER_THREAD()

        m_context->Enter();

        m_callbackSwapFunc->ExecuteFunctionWithContext(m_context, NULL, CefV8ValueList{});

        m_context->Exit();
    }

private:
    CefRefPtr<CefV8Value> m_callbackSwapFunc;
    CefRefPtr<CefV8Context> m_context;

    IMPLEMENT_REFCOUNTING(SwapTask);
    DISALLOW_COPY_AND_ASSIGN(SwapTask);
};

class DrawHandler : public CefV8Handler
{
public:
    DrawHandler() = default;

    virtual bool Execute(const CefString& name,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList& arguments,
                         CefRefPtr<CefV8Value>& retval,
                         CefString& exception) override
    {
        if (name == "RegDrawFunction")
        {
            if (arguments.size() == 1 &&
                arguments[0]->IsFunction())
            {
                std::cout << name << std::endl;
                m_drawTask = new DrawTask(arguments[0], CefV8Context::GetCurrentContext());
                return true;
            }
        }

        if (name == "RegSwapBuffers")
        {
            if (arguments.size() == 1 &&
                arguments[0]->IsFunction())
            {
                std::cout << name << std::endl;
                m_swapTask = new SwapTask(arguments[0], CefV8Context::GetCurrentContext());
                return true;
            }
        }

        if (name == "JsDraws")
        {
            //std::cout << "JsDraws" << std::endl;
            m_jsDraws = arguments[0]->GetBoolValue();
            return true;
        }

        // Function does not exist.
        return false;
    }

    bool Draw()
    {
        if (!m_jsDraws)
        {
            if (m_drawTask != nullptr && m_swapTask != nullptr)
            {
                m_jsDraws = true;
                CefPostTask(TID_RENDERER, m_swapTask);
                CefPostTask(TID_RENDERER, m_drawTask);
                return true;
            }
        }
        return false;
    }

    CefRefPtr<DrawTask> m_drawTask;
    CefRefPtr<SwapTask> m_swapTask;

    bool m_jsDraws = false;
    // Provide the reference counting implementation for this class.

    IMPLEMENT_REFCOUNTING(DrawHandler);
    DISALLOW_COPY_AND_ASSIGN(DrawHandler);
};

class MyRelease : public CefV8ArrayBufferReleaseCallback
{
public:
    virtual void ReleaseBuffer(void* buffer)
    {
        delete[] buffer;
    }

    IMPLEMENT_REFCOUNTING(MyRelease);
};

void DoAsyncUpdate(uint8_t *firstSharedArray, uint8_t *secondSharedArray,
                   size_t sharedArraySize, uint8_t increment, CefRefPtr<CefV8Handler> handler)
{
    uint8_t* arrayToUpdate = firstSharedArray;
    DrawHandler* drawHandler = static_cast<DrawHandler*>(handler.get());
    uint8_t prevColor = arrayToUpdate[0];
    while(true)
    {
        if (drawHandler->Draw())
        {
            uint8_t newColor = prevColor + increment;

            for (size_t i = 0; i < sharedArraySize / 4; ++i)
            {
                arrayToUpdate[i * 4 + 0] = newColor;
                arrayToUpdate[i * 4 + 1] = newColor;
                arrayToUpdate[i * 4 + 2] = newColor;
            }
            prevColor = newColor;
            arrayToUpdate = arrayToUpdate != firstSharedArray ?
                            firstSharedArray : secondSharedArray;
        }

    }
}

void SimpleApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefV8Context> context)
{
    CEF_REQUIRE_RENDERER_THREAD()

    std::cout << "SimpleApp::OnContextCreated" << std::endl;

    CefRefPtr<CefV8Value> object = context->GetGlobal();

    // Create a new V8 string value. See the "Basic JS Types" section below.
    size_t pixelArraySize = 800 * 600 * 4;
    uint8_t* pixelArray0 = new uint8_t[pixelArraySize];
    for(size_t i = 0; i < pixelArraySize / 4; ++i)
    {
        pixelArray0[i * 4 + 0] = 255;
        pixelArray0[i * 4 + 1] = 0;
        pixelArray0[i * 4 + 2] = 0;
        pixelArray0[i * 4 + 3] = 255;
    }

    uint8_t* pixelArray1 = new uint8_t[pixelArraySize];
//    for(size_t i = 0; i < pixelArraySize / 4; ++i)
//    {
//        pixelArray1[i * 4 + 0] = 0;
//        pixelArray1[i * 4 + 1] = 255;
//        pixelArray1[i * 4 + 2] = 0;
//        pixelArray1[i * 4 + 3] = 255;
//    }
    std::copy(pixelArray0, (pixelArray0 + pixelArraySize), pixelArray1);

    CefRefPtr<CefV8Value> sharedArray0 = CefV8Value::CreateArrayBuffer(pixelArray0, pixelArraySize, new MyRelease);
    object->SetValue("sharedArray0", sharedArray0, V8_PROPERTY_ATTRIBUTE_NONE);

    CefRefPtr<CefV8Value> sharedArray1 = CefV8Value::CreateArrayBuffer(pixelArray1, pixelArraySize, new MyRelease);
    object->SetValue("sharedArray1", sharedArray1, V8_PROPERTY_ATTRIBUTE_NONE);

    // Create an instance of my CefV8Handler object.

    CefRefPtr<CefV8Handler> handler = new DrawHandler;

    CefRefPtr<CefV8Value> regDrawFunction = CefV8Value::CreateFunction("RegDrawFunction", handler);
    object->SetValue("RegDrawFunction", regDrawFunction, V8_PROPERTY_ATTRIBUTE_NONE);

    CefRefPtr<CefV8Value> regSwapBuffers = CefV8Value::CreateFunction("RegSwapBuffers", handler);
    object->SetValue("RegSwapBuffers", regSwapBuffers, V8_PROPERTY_ATTRIBUTE_NONE);

    CefRefPtr<CefV8Value> jsDraws = CefV8Value::CreateFunction("JsDraws", handler);
    object->SetValue("JsDraws", jsDraws, V8_PROPERTY_ATTRIBUTE_NONE);

    m_future_ = std::async(std::launch::async, DoAsyncUpdate,
                          pixelArray0, pixelArray1,
                          pixelArraySize, 1, handler);
}
