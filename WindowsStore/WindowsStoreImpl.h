// ******************************************************************
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE CODE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
// THE CODE OR THE USE OR OTHER DEALINGS IN THE CODE.
// ******************************************************************
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

