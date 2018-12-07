# MFCStoreClient

MFCStoreClient is an example of how to access Windows Store APIs from a C++ MFC app. The sample demonstrates the following:

* Detect if your app is running inside of an App Package (required to use the Windows Store APIs)

* Detect if your app is running on Windows 10. (required to use the Windows Store APIs)

* Puts all of the Windows 10 Store code in a DLL that is only loaded if you app meets the above requirements.

* Demonstrates how to implement a trial version of your app with option to purchase.

* How to use a [Packaging Project](https://docs.microsoft.com/en-us/windows/uwp/porting/desktop-to-uwp-packaging-dot-net) to package your app for the Windows Store

Note: This sample is based on the Trial App scenario in the [UWP Store Sample](https://github.com/Microsoft/Windows-universal-samples/tree/fe8567faf2efdea3672c2ba642ba7b925ff6467e/Samples/Store) 
from the [Microsoft Windows Universal Samples](https://github.com/Microsoft/Windows-universal-samples)

## Requirements

* Visual Studio 2017 with the following workloads installed

	* Universal Windows Platform development. Make sure you add the C++ workload.
	
## Setup Instructions

Before running this sample, you must associate it with an app in the Microsoft Store. If you do not, then the Windows Store calls will return errors.

1. [Register as an app developer](https://developer.microsoft.com/store/register).
2. Follow the instructions in
[Testing apps that use the Windows.Services.Store namespace](https://msdn.microsoft.com/windows/uwp/monetize/in-app-purchases-and-trials#testing-apps-that-use-the-windows-services-store-namespace).

## Build and run MFCClient

1. Open MFCClient.sln

1. Make sure you have selected the x86 or x64 configuration. No not use the AnyCPU configuration.

1. Set the Packaging Project as the startup project

1. Press F5 to build and run the app.

1. A Win32 MFC Window will appear. You can interact with the app to test the Store Trial/Purchase APIs

## Debugging the MFCClient App

Since we are using the Packaging project to package and start the MFC app, we need to change the Debug properties of the Packaging project to use the Native Debugger.

* Right-click on the Packaging project.

* Select the Debug Tab

* Set Debugger Type | Application process to Native

* Set Debugger Type | Background task process to Native


## Copying the WindowsStore DLL to your App Package

In this sample, WindowsStore.DLL is manually loaded by the MFC app only if it is running on Windows 10. Since we are using a Packaging Project, we need to manually add the WindowsStore.DLL
to the correct location in the App Package. We can do this by modifying the XML of the PackagingProject.

Add the following XML near the end of the Packaging.wapproj (you can open this file with a text editor or Unload/Edit/Reload the file in Visual Studio)

```xml
  <ItemGroup>
    <Content Include="$(SolutionDir)$(Platform)/$(Configuration)/WindowsStore.dll">
    <Content Include="$(SolutionDir)$(Platform)/$(Configuration)/WindowsStore.dll">
      <Link>MFCClient/WindowsStore.dll</Link>
      <CopyToOutputDirectory>Always</CopyToOutputDirectory>
    </Content>
  </ItemGroup>
```

Note: You can use this technique to add other files to your App Package that are missed by the Packaging Project

This XML will automatically copy the correct build of the WindowsStore.DLL to your App Package.

## Accessing Windows 10 APIs from a Win32 Desktop DLL

The Windows 10 Store code is placed in a Win32 Desktop DLL that is enabled to access the Windows 10 APIs. In order to enable the access to the Windows 10 APIs, do the following:

The file MRAppService/MRAppServiceConnection.h contains 2 defines that allow the Win32 app to connect to the AppService

Right click on your Win32 Desktop DLL Project and select Properties

* Under C/C++ | General add the following to the Addition #using Directories

```c++
$(VC_LibraryPath_VC_x86_Store)\references\;C:\Program Files (x86)\Windows Kits\10\UnionMetadata\\$(TargetPlatformVersion);C:\Program Files (x86)\Windows Kits\10\UnionMetadata;C:\Program Files (x86)\Windows Kits\10\References\Windows.Foundation.UniversalApiContract\3.0.0.0;C:\Program Files (x86)\Windows Kits\10\References\Windows.Foundation.FoundationContract\3.0.0.0;%(AdditionalUsingDirectories) 
```

* Under C/C++ | General set Consume Windows Runtime Extension to Yes(/ZW)

* Under C/C++ | Command Line add /Zc:twoPhase-

Make sure you make these property changes for All Configurations and All Platforms
 
## How to Detect if your app is running on Windows 10

Before your app can load the Win32 Desktop DLL containing the Windows 10 APIs it must check if it is running on Windows 10.
Starting with Windows 8.1, the following Win32 version checking functions will return version 6.2.0.0 for Windows 8.1 and above:
 
* [GetVersion()](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724439(v=vs.85).aspx)

* [GetVersionEx()](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx)

* [VerifyVersionInfo()](https://msdn.microsoft.com/en-us/library/windows/desktop/ms725492(v=vs.85).aspx)


GetVersion() and GetVersionEx() have also been deprecated in Windows 10. In order to correctly obtain the Windows OS version your application is running on, you can do one of the following strategies:

1. [Add an application manifest](#application-manifest) to your application and use VerifyVersionInfo() to test for Windows 10.
1. [Get the version of kernel32.dll](#kernel32-method). This method does not require adding an application manifest to ensure correct Windows 8.1/10 version numbers.

If you do not check for Windows 10 and attempt to load the winrtmidi DLL on Windows 7 or 8, your application will quit with an error. 
You can use one of the following methods to detect if your app is running on Windows 10.


## Testing for Windows 10 Using an Application Manifest

1. Create a file called app.manifest.
1. Add the following XML to the app.manifest file:

``` xml
<?xml version="1.0" encoding="utf-8" standalone="yes"?>
<assembly manifestVersion="1.0" xmlns="urn:schemas-microsoft-com:asm.v1" xmlns:asmv3="urn:schemas-microsoft-com:asm.v3">
  <compatibility xmlns="urn:schemas-microsoft-com:compatibility.v1">
    <application>
      <!-- Windows 10 -->
      <supportedOS Id="{8e0f7a12-bfb3-4fe8-b9a5-48fd50a15a9a}"/>
      <!-- Windows 8.1 -->
      <supportedOS Id="{1f676c76-80e1-4239-95bb-83d0f6d0da78}"/>
      <!-- Windows Vista -->
      <supportedOS Id="{e2011457-1546-43c5-a5fe-008deee3d3f0}"/>
      <!-- Windows 7 -->
      <supportedOS Id="{35138b9a-5d96-4fbd-8e2d-a2440225f93a}"/>
      <!-- Windows 8 -->
      <supportedOS Id="{4a2f28e3-53b9-4441-ba9c-d69d4a4a6e38}"/>
    </application>
  </compatibility>
</assembly>
```
1. Right click on your application project and select **Properties**.

1. Select the **Linker | Manifest Tool | Input and Output** option and enter the path to the app.manifest file, for example $(ProjectDir)app.manifest

1. Use the following function to test for Windows 10 in your application. If the function returns true, it is safe to load the winrtmidi DLL.
This method will work with Visual Studio 2010-2017. This method is also provided in MFCClient\WindowsVersionHelper.h which you can add to your project.

``` c++
bool windows10orGreaterWithManifest()
{
    OSVERSIONINFOEX  osvi;
    DWORDLONG dwlConditionMask = 0;
    int op = VER_GREATER_EQUAL;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    osvi.dwMajorVersion = 10;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);

    BOOL result = VerifyVersionInfo(&osvi, VER_MAJORVERSION ,dwlConditionMask);
    return result ? true : false;
}
```


### Reference

[Windows.Services.Store namespace](https://msdn.microsoft.com/library/windows/apps/windows.services.store.aspx)  
[Monetization, customer engagement, and Microsoft Store services](https://msdn.microsoft.com/windows/uwp/monetize/)  

##  Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.


## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft Security
Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). You should
receive a response within 24 hours. If for some reason you do not, please follow up via
email to ensure we received your original message. Further information, including the
[MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can be found in
the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).
