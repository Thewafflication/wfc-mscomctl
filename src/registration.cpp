// Registration of the Windows Common Controls library with Windows: the entry
// points regsvr32 calls. The work is in wfc/ocx/registration.hpp. Internal to
// the WFC libraries.
//
// The class table lists the controls of the library: WFC's own ProgIDs and
// class ids, and the Microsoft ones used when WFC_REGISTER_MICROSOFT_IDS=1.
// The library does not create the controls itself yet (DllGetClassObject
// reports that the class is not available); the registration records which
// library and which version serves them.

#include <wfc/ocx/registration.hpp>
#include "registration_info.hpp"

#include <iterator>

namespace {

const wfc::registration::Library kLibrary{L"mscomctl", L"" WFC_LIB_VERSION,
                                          L"" WFC_LIB_TARGET};

const wfc::registration::ComClass kClasses[] = {
    {L"TabStrip", L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
     L"WFC.MSComctlLib.TabStrip", L"{1EFB6596-857C-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.TabStrip.2"},
    {L"Toolbar", L"{EEBB0901-096F-52EB-A609-86815871912F}",
     L"WFC.MSComctlLib.Toolbar", L"{66833FE6-8583-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.Toolbar.2"},
    {L"StatusBar", L"{11033AF3-05F7-58AE-93B1-596BB33150BB}",
     L"WFC.MSComctlLib.StatusBar", L"{8E3867A3-8586-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.SBarCtrl.2"},
    {L"ProgressBar", L"{FA3BFA10-D0F7-579F-A62B-339D7E21E9ED}",
     L"WFC.MSComctlLib.ProgressBar", L"{35053A22-8589-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.ProgCtrl.2"},
    {L"TreeView", L"{B10A6F9B-431E-59EF-B149-F118AEA46D00}",
     L"WFC.MSComctlLib.TreeView", L"{C74190B6-8589-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.TreeCtrl.2"},
    {L"ListView", L"{2C1007A3-9018-519F-A902-87E3E466A897}",
     L"WFC.MSComctlLib.ListView", L"{BDD1F04B-858B-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.ListViewCtrl.2"},
    {L"ImageList", L"{9217D929-9C68-526C-928E-99E4E4D6B815}",
     L"WFC.MSComctlLib.ImageList", L"{2C247F23-8591-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.ImageListCtrl.2"},
    {L"Slider", L"{43A1BE27-BBCC-5436-958B-45521EE99844}",
     L"WFC.MSComctlLib.Slider", L"{F08DF954-8592-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.Slider.2"},
    {L"ImageCombo", L"{8E6ABCF0-6E14-5091-9BE5-B04948C170AE}",
     L"WFC.MSComctlLib.ImageCombo", L"{DD9DA666-8594-11D1-B16A-00C0F0283628}",
     L"MSComctlLib.ImageComboCtl.2"},
};

HMODULE this_module() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&this_module), &module);
    return module;
}

}  // namespace

extern "C" {

HRESULT __stdcall DllRegisterServer() {
    return wfc::registration::register_library(this_module(), kLibrary,
                                               kClasses, std::size(kClasses));
}

HRESULT __stdcall DllUnregisterServer() {
    return wfc::registration::unregister_library(
        kLibrary, kClasses, std::size(kClasses), this_module());
}

HRESULT __stdcall DllCanUnloadNow() {
    return S_OK;
}

HRESULT __stdcall DllGetClassObject(REFCLSID, REFIID, void** object) {
    if (object != nullptr) {
        *object = nullptr;
    }
    return CLASS_E_CLASSNOTAVAILABLE;
}

}  // extern "C"
