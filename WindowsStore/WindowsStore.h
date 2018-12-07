// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(DLL_EXPORT)
#define DLL_EXPORT_API extern "C" __declspec(dllexport)
#else
#define DLL_EXPORT_API extern "C" __declspec(dllimport)
#endif

namespace WinRT
{
    enum WindowsStoreErrorType {
        WINRT_NO_ERROR = 0,                         //no error
        WINRT_WINDOWS_RUNTIME_ERROR,                // unable to initialize Windows Runtime
        WINRT_WINDOWS_VERSION_ERROR,                // this version of Windows does not support Windows::Services::Store
        WINRT_APP_PACKAGE_ERROR,                    // app is not running inside of an App Package
        WINRT_INVALID_PARAMETER_ERROR,
        WINRT_MEMORY_ERROR, 
        WINRT_UNSPECIFIED_ERROR
    };

    typedef void* WindowsStorePtr;

    typedef void(*WindowsStoreCallback) (int error, const wchar_t* message, void* userData);

    typedef WindowsStoreErrorType(__cdecl *WindowsStoreInitializeFunc)(WindowsStorePtr* storePtr, HWND window, WindowsStoreCallback licenseChangedCallback, void* userDat);
    DLL_EXPORT_API WindowsStoreErrorType __cdecl store_initialize(WindowsStorePtr* storePtr, HWND window, WindowsStoreCallback licenseChangedCallback, void* userData);
 
    typedef void(__cdecl *WindowsStoreFreeFunc)(WindowsStorePtr storePtr);
    DLL_EXPORT_API void __cdecl store_free(WindowsStorePtr storePtr);

    typedef void(__cdecl *WindowsStorePurchaseFunc)(WindowsStorePtr storePtr, WindowsStoreCallback callback, void* userData);
    DLL_EXPORT_API void __cdecl store_purchase(WindowsStorePtr storePtr, WindowsStoreCallback callback, void* userData);

    typedef void(__cdecl *WindowsStoreLicenseStateFunc)(WindowsStorePtr storePtr, WindowsStoreCallback callback, void* userData);
    DLL_EXPORT_API void __cdecl store_license_state(WindowsStorePtr storePtr, WindowsStoreCallback callback, void* userData);

    typedef void(__cdecl *WindowsStoreGetPriceFunc)(WindowsStorePtr storePtr, WindowsStoreCallback callback, void* userData);
    DLL_EXPORT_API void __cdecl store_get_price(WindowsStorePtr storePtr, WindowsStoreCallback callback, void* userData);

};
 