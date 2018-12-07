// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WindowsStore.h"
#include "WindowsVersionHelper.h"

#include <iostream>
#include <string>
#include <conio.h>

#define USING_APP_MANIFEST
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mutex>

using namespace WinRT;

// WindowsStore DLL function pointers
WindowsStoreInitializeFunc gWindowsStoreInitFunc = nullptr;
WindowsStorePurchaseFunc gWindowsStorePurchaseFunc = nullptr;
WindowsStoreLicenseStateFunc gWindowsStoreLicenseStateFunc = nullptr;
WindowsStoreGetPriceFunc gWindowsStoreGetPriceFunc = nullptr;
WindowsStoreFreeFunc gWindowsStoreFreeFunc = nullptr;

std::mutex s_mutex; // used to control output to Console window

void storePurchaseCallback(int error, const wchar_t* message, void* userData)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    std::wcout << L"storePurchaseCallback: error: " << error << L" message: " << message << std::endl;
}

void storeLicenseStateChangedCallback(int error, const wchar_t* message, void* userData)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    std::wcout << L"storeLicenseStateChangedCallback: error: " << error << L" message: " << message << std::endl;
}

void storeLicenseStateCallback(int error, const wchar_t* message, void* userData)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    std::wcout << L"storeLicenseStateCallback: error: " << error << L" message: " << message << std::endl;
}

void storeGetPriceCallback(int error, const wchar_t* message, void* userData)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    std::wcout << L"storeGetPriceCallback: error: " << error << L" message: " << message << std::endl;
}

int main()
{
HINSTANCE dllHandle = NULL;
    WindowsStorePtr storePtr = nullptr;
    WindowsStoreErrorType result = WINRT_NO_ERROR;

    //Load the WindowsStore dll
#ifdef USING_APP_MANIFEST
    if (windows10orGreaterWithManifest())
    {
        dllHandle = LoadLibrary(L"WindowsStore.dll");
    }
#else
    if (windows10orGreater())
    {
        dllHandle = LoadLibrary(L"WindowsStore.dll");
    }
#endif

    if (NULL == dllHandle)
    {
        std::cout << "Unable to load WindowsStore.dll" << std::endl;
        goto cleanup;
    }

    // Get WindowsStore DLL function pointers. Error checking needs to be added!
    gWindowsStoreInitFunc = reinterpret_cast<WindowsStoreInitializeFunc>(::GetProcAddress(dllHandle, "store_initialize"));
    gWindowsStorePurchaseFunc = reinterpret_cast<WindowsStorePurchaseFunc>(::GetProcAddress(dllHandle, "store_purchase"));
    gWindowsStoreLicenseStateFunc = reinterpret_cast<WindowsStoreLicenseStateFunc>(::GetProcAddress(dllHandle, "store_license_state"));
    gWindowsStoreGetPriceFunc = reinterpret_cast<WindowsStoreGetPriceFunc>(::GetProcAddress(dllHandle, "store_get_price"));
    gWindowsStoreFreeFunc = reinterpret_cast<WindowsStoreFreeFunc>(::GetProcAddress(dllHandle, "store_free"));
    
    // initialize Windows Store functionality
    result = gWindowsStoreInitFunc(&storePtr, GetConsoleWindow(), storeLicenseStateChangedCallback, nullptr);
    if (result != WINRT_NO_ERROR)
    {
        std::cout << "Cannot initialize Windows Store." << std::endl;
        switch (result)
        {
        case WINRT_WINDOWS_RUNTIME_ERROR:
            std::cout << "Unable to initialize Windows Runtime" << std::endl;
            break;

        case WINRT_WINDOWS_VERSION_ERROR:
            std::cout << "This version of Windows does not support Windows::Services::Store" << std::endl;
            break;

        case WINRT_APP_PACKAGE_ERROR:
            std::cout << "This app is not running inside of an App Package" << std::endl;
            break;

        default:
            std::cout << "Unable to initialize Windows Store" << std::endl;
            break;
        }
        goto cleanup;
    }

    // get Windows Store trial license info
    gWindowsStoreGetPriceFunc(storePtr, storeGetPriceCallback, nullptr);
    gWindowsStoreLicenseStateFunc(storePtr, storeLicenseStateCallback, nullptr);
    gWindowsStorePurchaseFunc(storePtr, storePurchaseCallback, nullptr);

cleanup:
    // wait for user to exit...
    std::cout << "Press any key to exit..." << std::endl;
    char c = _getch();

    // release the WindowsStore object
    if (gWindowsStoreFreeFunc)
    {
        gWindowsStoreFreeFunc(storePtr);
    }

    //Free the library:
    if (dllHandle)
    {
        FreeLibrary(dllHandle);
    }

    return 0;
}

