/* Black-box check of wfcmscomctl.dll: exports, version resource and the
 * registration round trip (see dll_check.h). Run by CTest with the DLL,
 * the version and the target as arguments. */
#include "dll_check.h"

#include <ocidl.h>

static void wfc_free_exception(EXCEPINFO* exception) {
    SysFreeString(exception->bstrSource);
    SysFreeString(exception->bstrDescription);
    memset(exception, 0, sizeof(*exception));
}

static const struct wfc_check_class kClasses[] = {
    {L"WFC.MSComctlLib.TabStrip", L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
     L"MSComctlLib.TabStrip.2", L"{1EFB6596-857C-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.Toolbar", L"{EEBB0901-096F-52EB-A609-86815871912F}",
     L"MSComctlLib.Toolbar.2", L"{66833FE6-8583-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.StatusBar", L"{11033AF3-05F7-58AE-93B1-596BB33150BB}",
     L"MSComctlLib.SBarCtrl.2", L"{8E3867A3-8586-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ProgressBar", L"{FA3BFA10-D0F7-579F-A62B-339D7E21E9ED}",
     L"MSComctlLib.ProgCtrl.2", L"{35053A22-8589-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.TreeView", L"{B10A6F9B-431E-59EF-B149-F118AEA46D00}",
     L"MSComctlLib.TreeCtrl.2", L"{C74190B6-8589-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ListView", L"{2C1007A3-9018-519F-A902-87E3E466A897}",
     L"MSComctlLib.ListViewCtrl.2", L"{BDD1F04B-858B-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ImageList", L"{9217D929-9C68-526C-928E-99E4E4D6B815}",
     L"MSComctlLib.ImageListCtrl.2", L"{2C247F23-8591-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.Slider", L"{43A1BE27-BBCC-5436-958B-45521EE99844}",
     L"MSComctlLib.Slider.2", L"{F08DF954-8592-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ImageCombo", L"{8E6ABCF0-6E14-5091-9BE5-B04948C170AE}",
     L"MSComctlLib.ImageComboCtl.2", L"{DD9DA666-8594-11D1-B16A-00C0F0283628}"},
};

/* The behaviour of a Slider and a ProgressBar made through COM: the defaults,
 * Value outside Min and Max refused with error 380, and Value brought inside
 * a narrower range. */
static double wfc_range_get(IDispatch* control, const wchar_t* name) {
    OLECHAR* names[1];
    DISPID id = 0;
    DISPPARAMS none = {NULL, NULL, 0, 0};
    VARIANT value;
    VARIANT number;
    names[0] = (OLECHAR*)name;
    VariantInit(&value);
    VariantInit(&number);
    if (control->lpVtbl->GetIDsOfNames(control, &IID_NULL, names, 1, 0, &id) !=
            S_OK ||
        control->lpVtbl->Invoke(control, id, &IID_NULL, 0, DISPATCH_PROPERTYGET,
                                &none, &value, NULL, NULL) != S_OK ||
        VariantChangeType(&number, &value, 0, VT_R8) != S_OK) {
        return -1e30;
    }
    return number.dblVal;
}

static HRESULT wfc_range_put(IDispatch* control, const wchar_t* name,
                             double number, EXCEPINFO* exception) {
    OLECHAR* names[1];
    DISPID id = 0;
    DISPID named = DISPID_PROPERTYPUT;
    VARIANT value;
    DISPPARAMS put;
    names[0] = (OLECHAR*)name;
    VariantInit(&value);
    value.vt = VT_R8;
    value.dblVal = number;
    put.rgvarg = &value;
    put.rgdispidNamedArgs = &named;
    put.cArgs = 1;
    put.cNamedArgs = 1;
    if (control->lpVtbl->GetIDsOfNames(control, &IID_NULL, names, 1, 0, &id) !=
        S_OK) {
        return DISP_E_UNKNOWNNAME;
    }
    return control->lpVtbl->Invoke(control, id, &IID_NULL, 0,
                                   DISPATCH_PROPERTYPUT, &put, NULL, exception,
                                   NULL);
}

static void wfc_check_range(const wchar_t* dll_path, const wchar_t* clsid_text,
                            double default_max, const char* what) {
    typedef HRESULT(__stdcall * get_class_object)(REFCLSID, REFIID, void**);
    HMODULE module = LoadLibraryW(dll_path);
    get_class_object get;
    CLSID clsid;
    IClassFactory* factory = NULL;
    IDispatch* control = NULL;
    EXCEPINFO exception;
    if (module == NULL) {
        wfc_check_fail("cannot load", dll_path);
    }
    get = (get_class_object)(void*)GetProcAddress(module, "DllGetClassObject");
    CLSIDFromString(clsid_text, &clsid);
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (get == NULL ||
        get(&clsid, &IID_IClassFactory, (void**)&factory) != S_OK ||
        factory->lpVtbl->CreateInstance(factory, NULL, &IID_IDispatch,
                                        (void**)&control) != S_OK) {
        wfc_check_fail(what, dll_path);
    }
    if (wfc_range_get(control, L"Min") != 0.0 ||
        wfc_range_get(control, L"Max") != default_max ||
        wfc_range_get(control, L"Value") != 0.0) {
        wfc_check_fail("the defaults of the range control are wrong in",
                       dll_path);
    }
    memset(&exception, 0, sizeof(exception));
    if (wfc_range_put(control, L"Value", default_max + 1.0, &exception) !=
            DISP_E_EXCEPTION ||
        (exception.scode & 0xFFFF) != 380) {
        wfc_check_fail("Value above Max was not error 380 in", dll_path);
    }
    wfc_free_exception(&exception);
    if (wfc_range_put(control, L"Value", 4.0, NULL) != S_OK ||
        wfc_range_get(control, L"Value") != 4.0) {
        wfc_check_fail("Value inside the range was refused in", dll_path);
    }
    if (wfc_range_put(control, L"Max", 3.0, NULL) != S_OK ||
        wfc_range_get(control, L"Value") != 3.0) {
        wfc_check_fail("Value was not brought inside a narrower range in",
                       dll_path);
    }
    if (wfc_range_put(control, L"Min", 5.0, &exception) != DISP_E_EXCEPTION) {
        wfc_check_fail("Min above Max was accepted in", dll_path);
    }
    wfc_free_exception(&exception);
    control->lpVtbl->Release(control);
    factory->lpVtbl->Release(factory);
    printf("%s: the range is kept through COM\n", what);
}

int main(int argc, char** argv) {
    int checked = wfc_check_dll(argc, argv, L"mscomctl", kClasses,
                                sizeof(kClasses) / sizeof(kClasses[0]));
    if (checked != 0) {
        return checked;
    }
    wfc_check_range(wfc_check_widen(argv[1]),
                    L"{43A1BE27-BBCC-5436-958B-45521EE99844}", 10.0, "Slider");
    wfc_check_range(wfc_check_widen(argv[1]),
                    L"{FA3BFA10-D0F7-579F-A62B-339D7E21E9ED}", 100.0,
                    "ProgressBar");
    return wfc_check_create(wfc_check_widen(argv[1]),
                            L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
                            L"Enabled", L"Refresh");
}
