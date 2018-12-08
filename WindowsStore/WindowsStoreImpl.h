// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "WindowsStore.h"
#include <memory>
#include <string>
#include <Windows.h>

namespace WinRT
{
    class WindowsStoreImpl
    {
    public:
        WindowsStoreImpl();
        ~WindowsStoreImpl();
        WindowsStoreErrorType Initialize(HWND hwnd, WindowsStoreCallback licenseChangedCallback, void* userData);
        void Purchase(WindowsStoreCallback callback, void* userData);
        void GetLicenseState(WindowsStoreCallback callback, void* userData);
        void GetPrice(WindowsStoreCallback callback, void* userData);

    private:
        void OfflineLicensesChanged(Windows::Services::Store::StoreContext^ sender, Platform::Object^ args);
        Windows::Services::Store::StoreContext^ m_storeContext;
        Windows::Foundation::EventRegistrationToken m_eventRegistrationToken;
        WindowsStoreCallback m_licenseChangedCallback;
        void* m_userData;
        HWND m_hwnd;
    };
};

