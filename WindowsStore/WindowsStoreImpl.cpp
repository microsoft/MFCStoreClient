// ******************************************************************
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE CODE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.


#include "WindowsStoreImpl.h"
#include <ppltasks.h>
#include <shobjidl.h>
#include <wrl.h>
#include <string>
#include <sstream>     
#include <functional>

using namespace concurrency;
using namespace WinRT;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::Services::Store;
using namespace Windows::Storage::Streams;
using namespace std::placeholders;
using namespace std::placeholders;

WindowsStoreImpl::WindowsStoreImpl()
{

}

WindowsStoreImpl::~WindowsStoreImpl()
{
    if (m_storeContext != nullptr)
    {
        m_storeContext->OfflineLicensesChanged -= m_eventRegistrationToken;
    }
}

WindowsStoreErrorType WindowsStoreImpl::Initialize(HWND hwnd, WindowsStoreCallback licenseChangedCallback, void* userData)
{
    WindowsStoreErrorType result = WINRT_NO_ERROR;

    m_licenseChangedCallback = licenseChangedCallback;
    m_hwnd = hwnd;
    m_userData = userData;
    m_storeContext = StoreContext::GetDefault();
    m_eventRegistrationToken = m_storeContext->OfflineLicensesChanged += ref new TypedEventHandler<StoreContext^, Platform::Object^>(std::bind(&WindowsStoreImpl::OfflineLicensesChanged, this, _1, _2));

    return result;
}

void WindowsStoreImpl::Purchase(WindowsStoreCallback callback, void* userData)
{
    // Assign the app's hwnd to the storeContext
    ComPtr<IInitializeWithWindow> initWindow;
    IUnknown* temp = reinterpret_cast<IUnknown*>(m_storeContext);
    HRESULT hr = temp->QueryInterface(initWindow.GetAddressOf());
    if (SUCCEEDED(hr))
    {
        hr = initWindow->Initialize(m_hwnd);
        if (!SUCCEEDED(hr))
        {
            std::wstringstream ws;
            callback(hr, L"Can't initial StoreContext with hwnd: ", userData);
            return;
        }
    }

    create_task(m_storeContext->GetStoreProductForCurrentAppAsync()).then([this, callback, userData](StoreProductResult^ productResult)
    {
        if (productResult->ExtendedError.Value != S_OK)
        {
            std::wstringstream ws;
            ws << L"GetStoreProductForCurrentAppAsync Error: " << productResult->ExtendedError.Value;
            callback(productResult->ExtendedError.Value, ws.str().c_str(), userData);
            return;
        }

        create_task(m_storeContext->GetAppLicenseAsync()).then([this, productResult, callback, userData](StoreAppLicense^ license)
        {
            if (license->IsTrial)
            {
                create_task(productResult->Product->RequestPurchaseAsync()).then([this, callback, userData](StorePurchaseResult^ result)
                {
                    std::wstringstream ws;
                    switch (result->Status)
                    {
                    case StorePurchaseStatus::AlreadyPurchased:
                        ws << L"You already bought this app and have a fully-licensed version.";
                        break;

                    case StorePurchaseStatus::Succeeded:
                        // License will refresh automatically using the StoreContext.OfflineLicensesChanged event
                        break;

                    case StorePurchaseStatus::NotPurchased:
                        ws << L"Product was not purchased, it may have been canceled";
                        break;

                    case StorePurchaseStatus::NetworkError:
                        ws << L"Product was not purchased due to a Network Error.";
                        break;

                    case StorePurchaseStatus::ServerError:
                        ws << L"Product was not purchased due to a Server Error.";
                        break;

                    default:
                        ws << L"Product was not purchased due to a Unknown Error.";
                        break;
                    }
                    callback(E_FAIL, ws.str().c_str(), userData);

                });
            }
            else
            {
                std::wstringstream ws;
                callback(S_OK, L"You already bought this app and have a fully-licensed version.", userData);

            }
        }, task_continuation_context::get_current_winrt_context());
    }, task_continuation_context::get_current_winrt_context());
}

void WindowsStoreImpl::GetLicenseState(WindowsStoreCallback callback, void* userData)
{
    create_task(m_storeContext->GetAppLicenseAsync()).then([this, callback, userData](StoreAppLicense^ license)
    {
        if (license->IsActive)
        {
            if (license->IsTrial)
            {
                callback(S_OK, L"IsTrial", userData);

            }
            else
            {
                callback(S_OK, L"Full", userData);
            }
        }
        else
        {
            callback(S_OK, L"inactive", userData);
        }
    }, task_continuation_context::get_current_winrt_context());
}

void WindowsStoreImpl::GetPrice(WindowsStoreCallback callback, void* userData)
{
    create_task(m_storeContext->GetStoreProductForCurrentAppAsync()).then([this, callback, userData](StoreProductResult^ result)
    {
        if (result->ExtendedError.Value == S_OK)
        {
            callback(S_OK, result->Product->Price->FormattedPrice->Data(), userData);
        }
        else
        {
            callback(result->ExtendedError.Value, L"Unknown", userData);
        }
    }, task_continuation_context::get_current_winrt_context());
}

void WindowsStoreImpl::OfflineLicensesChanged(StoreContext^ sender, Platform::Object^ args)
{
    if (m_licenseChangedCallback != nullptr)
    {
        GetLicenseState(m_licenseChangedCallback, m_userData);
    }
}




