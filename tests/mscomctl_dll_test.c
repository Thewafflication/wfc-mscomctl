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

/* The collections of a TabStrip, a StatusBar and a Toolbar made through COM
 * (ADR-0010 step 7): Add, Count, Item, Remove, the errors of the library
 * (35600 index out of bounds, 35602 key not unique, 35609 too many panels),
 * the first tab selected, SelectedItem, and For Each. */
static HRESULT wfc_call(IDispatch* object, const wchar_t* name, WORD flags,
                        VARIANT* arguments, UINT count, VARIANT* result,
                        EXCEPINFO* exception) {
    /* `arguments` are in the order of the call; COM wants them reversed. */
    OLECHAR* names[1];
    DISPID id = 0;
    DISPID named = DISPID_PROPERTYPUT;
    VARIANT reversed[8];
    DISPPARAMS params;
    UINT i;
    HRESULT found;
    names[0] = (OLECHAR*)name;
    for (i = 0; i < count; ++i) {
        reversed[i] = arguments[count - 1U - i];
    }
    params.rgvarg = reversed;
    params.cArgs = count;
    params.rgdispidNamedArgs = (flags & DISPATCH_PROPERTYPUT) ? &named : NULL;
    params.cNamedArgs = (flags & DISPATCH_PROPERTYPUT) ? 1U : 0U;
    found = object->lpVtbl->GetIDsOfNames(object, &IID_NULL, names, 1, 0, &id);
    if (found != S_OK) {
        return found;
    }
    if (result != NULL) {
        VariantInit(result);
    }
    return object->lpVtbl->Invoke(object, id, &IID_NULL, 0, flags, &params,
                                  result, exception, NULL);
}

static void wfc_missing(VARIANT* value) {
    VariantInit(value);
    value->vt = VT_ERROR;
    value->scode = DISP_E_PARAMNOTFOUND;
}

static void wfc_text(VARIANT* value, const wchar_t* text) {
    VariantInit(value);
    value->vt = VT_BSTR;
    value->bstrVal = SysAllocString(text);
}

static void wfc_number(VARIANT* value, long number) {
    VariantInit(value);
    value->vt = VT_I4;
    value->lVal = number;
}

/* Item(key) of `collection`, or NULL. */
static IDispatch* wfc_item(IDispatch* collection, VARIANT* selector) {
    VARIANT item;
    if (wfc_call(collection, L"Item", DISPATCH_METHOD | DISPATCH_PROPERTYGET,
                 selector, 1, &item, NULL) != S_OK ||
        item.vt != VT_DISPATCH) {
        return NULL;
    }
    return item.pdispVal;
}

static long wfc_property_number(IDispatch* object, const wchar_t* name) {
    VARIANT value;
    VARIANT number;
    VariantInit(&number);
    if (wfc_call(object, name, DISPATCH_PROPERTYGET, NULL, 0, &value, NULL) !=
            S_OK ||
        VariantChangeType(&number, &value, 0, VT_I4) != S_OK) {
        return -1;
    }
    return number.lVal;
}

static int wfc_property_is(IDispatch* object, const wchar_t* name,
                           const wchar_t* text) {
    VARIANT value;
    if (wfc_call(object, name, DISPATCH_PROPERTYGET, NULL, 0, &value, NULL) !=
            S_OK ||
        value.vt != VT_BSTR) {
        return 0;
    }
    return wcscmp(value.bstrVal, text) == 0;
}

/* Calls Add; returns the number of the error, 0 when it worked. */
static long wfc_add(IDispatch* collection, VARIANT* arguments, UINT count,
                    IDispatch** made) {
    VARIANT result;
    EXCEPINFO exception;
    HRESULT outcome;
    memset(&exception, 0, sizeof(exception));
    outcome = wfc_call(collection, L"Add", DISPATCH_METHOD, arguments, count,
                       &result, &exception);
    if (outcome == DISP_E_EXCEPTION) {
        long number = (long)(exception.scode & 0xFFFF);
        wfc_free_exception(&exception);
        return number;
    }
    if (outcome != S_OK) {
        return -1;
    }
    if (made != NULL && result.vt == VT_DISPATCH) {
        *made = result.pdispVal;
    } else {
        VariantClear(&result);
    }
    return 0;
}

static IDispatch* wfc_create(const wchar_t* dll_path, const wchar_t* clsid_text,
                             const char* what) {
    typedef HRESULT(__stdcall * get_class_object)(REFCLSID, REFIID, void**);
    HMODULE module = LoadLibraryW(dll_path);
    get_class_object get;
    CLSID clsid;
    IClassFactory* factory = NULL;
    IDispatch* control = NULL;
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
    factory->lpVtbl->Release(factory);
    return control;
}

static IDispatch* wfc_collection(IDispatch* control, const wchar_t* name,
                                 const wchar_t* path) {
    VARIANT value;
    if (wfc_call(control, name, DISPATCH_PROPERTYGET, NULL, 0, &value, NULL) !=
            S_OK ||
        value.vt != VT_DISPATCH || value.pdispVal == NULL) {
        wfc_check_fail("the collection is missing in", path);
    }
    return value.pdispVal;
}

static void wfc_check_collections(const wchar_t* dll_path) {
    IDispatch* tabstrip = wfc_create(
        dll_path, L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}", "no TabStrip in");
    IDispatch* tabs = wfc_collection(tabstrip, L"Tabs", dll_path);
    IDispatch* made = NULL;
    IDispatch* second = NULL;
    IDispatch* chosen = NULL;
    VARIANT args[5];
    VARIANT value;
    VARIANT selector;
    EXCEPINFO exception;
    long errors;
    int i;

    memset(&exception, 0, sizeof(exception));
    wfc_missing(&args[0]);
    wfc_text(&args[1], L"a");
    wfc_text(&args[2], L"First");
    if (wfc_add(tabs, args, 3, &made) != 0 || made == NULL) {
        wfc_check_fail("Tabs.Add failed in", dll_path);
    }
    wfc_missing(&args[0]);
    wfc_text(&args[1], L"b");
    wfc_text(&args[2], L"Second");
    if (wfc_add(tabs, args, 3, &second) != 0 || second == NULL) {
        wfc_check_fail("the second Tabs.Add failed in", dll_path);
    }
    if (wfc_property_number(tabs, L"Count") != 2 ||
        wfc_property_number(second, L"Index") != 2 ||
        !wfc_property_is(second, L"Caption", L"Second") ||
        !wfc_property_is(made, L"Key", L"a")) {
        wfc_check_fail("the tabs are not what was added in", dll_path);
    }
    wfc_missing(&args[0]);
    wfc_text(&args[1], L"A");
    wfc_text(&args[2], L"Duplicate");
    if ((errors = wfc_add(tabs, args, 3, NULL)) != 35602) {
        wfc_check_fail("a duplicate key was not error 35602 in", dll_path);
    }
    wfc_number(&args[0], 9);
    wfc_text(&args[1], L"c");
    wfc_text(&args[2], L"Far");
    if ((errors = wfc_add(tabs, args, 3, NULL)) != 35600) {
        wfc_check_fail("index 9 was not error 35600 in", dll_path);
    }
    wfc_number(&args[0], 1);
    wfc_text(&args[1], L"front");
    wfc_text(&args[2], L"Front");
    if (wfc_add(tabs, args, 3, NULL) != 0 ||
        wfc_property_number(second, L"Index") != 3) {
        wfc_check_fail("Add at an index did not insert in", dll_path);
    }
    wfc_text(&selector, L"B");
    chosen = wfc_item(tabs, &selector);
    if (chosen == NULL || !wfc_property_is(chosen, L"Key", L"b")) {
        wfc_check_fail("Item by key (any case) failed in", dll_path);
    }
    chosen->lpVtbl->Release(chosen);

    /* The first tab ever added stays selected; SelectedItem follows. */
    if (wfc_call(tabstrip, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                 &value, NULL) != S_OK ||
        value.vt != VT_DISPATCH || value.pdispVal == NULL ||
        !wfc_property_is(value.pdispVal, L"Key", L"a")) {
        wfc_check_fail("the first tab is not selected in", dll_path);
    }
    VariantClear(&value);
    value.vt = VT_DISPATCH;
    value.pdispVal = second;
    if (wfc_call(tabstrip, L"SelectedItem",
                 DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF, &value, 1,
                 NULL, &exception) != S_OK) {
        wfc_check_fail("SelectedItem could not be set in", dll_path);
    }
    wfc_number(&selector, 3);
    wfc_call(tabs, L"Remove", DISPATCH_METHOD, &selector, 1, NULL, NULL);
    if (wfc_call(tabstrip, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                 &value, NULL) != S_OK ||
        value.vt != VT_DISPATCH || value.pdispVal == NULL ||
        !wfc_property_is(value.pdispVal, L"Key", L"a")) {
        wfc_check_fail(
            "removing the selected tab did not select its neighbour in",
            dll_path);
    }
    VariantClear(&value);
    wfc_number(&selector, 9);
    memset(&exception, 0, sizeof(exception));
    if (wfc_call(tabs, L"Remove", DISPATCH_METHOD, &selector, 1, NULL,
                 &exception) != DISP_E_EXCEPTION ||
        (exception.scode & 0xFFFF) != 35600) {
        wfc_check_fail("Remove(9) was not error 35600 in", dll_path);
    }
    wfc_free_exception(&exception);
    made->lpVtbl->Release(made);
    second->lpVtbl->Release(second);
    tabs->lpVtbl->Release(tabs);
    tabstrip->lpVtbl->Release(tabstrip);

    /* A status bar holds sixteen panels. */
    {
        IDispatch* bar =
            wfc_create(dll_path, L"{11033AF3-05F7-58AE-93B1-596BB33150BB}",
                       "no StatusBar in");
        IDispatch* panels = wfc_collection(bar, L"Panels", dll_path);
        for (i = 0; i < 16; ++i) {
            wfc_missing(&args[0]);
            wfc_missing(&args[1]);
            wfc_text(&args[2], L"Panel");
            if (wfc_add(panels, args, 3, NULL) != 0) {
                wfc_check_fail("Panels.Add failed in", dll_path);
            }
        }
        wfc_missing(&args[0]);
        wfc_missing(&args[1]);
        wfc_text(&args[2], L"One too many");
        if (wfc_add(panels, args, 3, NULL) != 35609 ||
            wfc_property_number(panels, L"Count") != 16) {
            wfc_check_fail("the seventeenth panel was not error 35609 in",
                           dll_path);
        }
        panels->lpVtbl->Release(panels);
        bar->lpVtbl->Release(bar);
    }

    /* A toolbar's buttons keep the style they were added with. */
    {
        IDispatch* bar =
            wfc_create(dll_path, L"{EEBB0901-096F-52EB-A609-86815871912F}",
                       "no Toolbar in");
        IDispatch* buttons = wfc_collection(bar, L"Buttons", dll_path);
        IDispatch* button = NULL;
        IEnumVARIANT* walk = NULL;
        ULONG fetched = 0;
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"save");
        wfc_text(&args[2], L"Save");
        wfc_number(&args[3], 1);
        if (wfc_add(buttons, args, 4, &button) != 0 || button == NULL ||
            wfc_property_number(button, L"Style") != 1) {
            wfc_check_fail("Buttons.Add lost the style in", dll_path);
        }
        if (wfc_call(buttons, L"_NewEnum", DISPATCH_METHOD, NULL, 0, &value,
                     NULL) != S_OK ||
            value.vt != VT_UNKNOWN ||
            value.punkVal->lpVtbl->QueryInterface(
                value.punkVal, &IID_IEnumVARIANT, (void**)&walk) != S_OK ||
            walk->lpVtbl->Next(walk, 1, &value, &fetched) != S_OK ||
            fetched != 1 || value.vt != VT_DISPATCH ||
            !wfc_property_is(value.pdispVal, L"Key", L"save")) {
            wfc_check_fail("For Each over the buttons failed in", dll_path);
        }
        VariantClear(&value);
        walk->lpVtbl->Release(walk);
        button->lpVtbl->Release(button);
        buttons->lpVtbl->Release(buttons);
        bar->lpVtbl->Release(bar);
    }
    /* A ListView has ListItems and ColumnHeaders; an ImageList refuses an
     * image without a picture (481); an ImageCombo selects what it is told. */
    {
        IDispatch* list =
            wfc_create(dll_path, L"{2C1007A3-9018-519F-A902-87E3E466A897}",
                       "no ListView in");
        IDispatch* items = wfc_collection(list, L"ListItems", dll_path);
        IDispatch* columns = wfc_collection(list, L"ColumnHeaders", dll_path);
        IDispatch* item = NULL;
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"k1");
        wfc_text(&args[2], L"One");
        if (wfc_add(items, args, 3, &item) != 0 || item == NULL ||
            !wfc_property_is(item, L"Text", L"One")) {
            wfc_check_fail("ListItems.Add failed in", dll_path);
        }
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"K1");
        wfc_text(&args[2], L"Again");
        if (wfc_add(items, args, 3, NULL) != 35602) {
            wfc_check_fail("a duplicate ListItem key was not 35602 in",
                           dll_path);
        }
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"c1");
        wfc_text(&args[2], L"Name");
        wfc_number(&args[3], 2000);
        if (wfc_add(columns, args, 4, NULL) != 0 ||
            wfc_property_number(columns, L"Count") != 1 ||
            wfc_property_number(items, L"Count") != 1) {
            wfc_check_fail("ColumnHeaders and ListItems are not separate in",
                           dll_path);
        }
        item->lpVtbl->Release(item);
        columns->lpVtbl->Release(columns);
        items->lpVtbl->Release(items);
        list->lpVtbl->Release(list);
    }
    {
        IDispatch* images =
            wfc_create(dll_path, L"{9217D929-9C68-526C-928E-99E4E4D6B815}",
                       "no ImageList in");
        IDispatch* pictures = wfc_collection(images, L"ListImages", dll_path);
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"none");
        wfc_missing(&args[2]);
        if (wfc_add(pictures, args, 3, NULL) != 481) {
            wfc_check_fail("an image without a picture was not 481 in",
                           dll_path);
        }
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"red");
        wfc_text(&args[2], L"picture");
        if (wfc_add(pictures, args, 3, NULL) != 0 ||
            wfc_property_number(pictures, L"Count") != 1) {
            wfc_check_fail("ListImages.Add failed in", dll_path);
        }
        pictures->lpVtbl->Release(pictures);
        images->lpVtbl->Release(images);
    }
    {
        IDispatch* combo =
            wfc_create(dll_path, L"{8E6ABCF0-6E14-5091-9BE5-B04948C170AE}",
                       "no ImageCombo in");
        IDispatch* entries = wfc_collection(combo, L"ComboItems", dll_path);
        IDispatch* red = NULL;
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"red");
        wfc_text(&args[2], L"Red");
        if (wfc_add(entries, args, 3, &red) != 0 || red == NULL) {
            wfc_check_fail("ComboItems.Add failed in", dll_path);
        }
        if (wfc_call(combo, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                     &value, NULL) != S_OK ||
            value.vt != VT_DISPATCH || value.pdispVal != NULL) {
            wfc_check_fail("an ImageCombo selects nothing at first in",
                           dll_path);
        }
        value.vt = VT_DISPATCH;
        value.pdispVal = red;
        if (wfc_call(combo, L"SelectedItem",
                     DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF, &value, 1,
                     NULL, NULL) != S_OK ||
            wfc_call(combo, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                     &value, NULL) != S_OK ||
            value.vt != VT_DISPATCH || value.pdispVal == NULL ||
            !wfc_property_is(value.pdispVal, L"Key", L"red")) {
            wfc_check_fail("SelectedItem of an ImageCombo failed in", dll_path);
        }
        VariantClear(&value);
        red->lpVtbl->Release(red);
        entries->lpVtbl->Release(entries);
        combo->lpVtbl->Release(combo);
    }
    /* A TreeView's Nodes: the Relative and the Relationship place a node. */
    {
        IDispatch* tree =
            wfc_create(dll_path, L"{B10A6F9B-431E-59EF-B149-F118AEA46D00}",
                       "no TreeView in");
        IDispatch* nodes = wfc_collection(tree, L"Nodes", dll_path);
        IDispatch* root = NULL;
        IDispatch* kid = NULL;
        IDispatch* later = NULL;
        IDispatch* found = NULL;
        wfc_missing(&args[0]);
        wfc_missing(&args[1]);
        wfc_text(&args[2], L"r");
        wfc_text(&args[3], L"Root");
        if (wfc_add(nodes, args, 4, &root) != 0 || root == NULL) {
            wfc_check_fail("Nodes.Add failed in", dll_path);
        }
        wfc_text(&args[0], L"r");
        wfc_number(&args[1], 4); /* tvwChild */
        wfc_text(&args[2], L"k");
        wfc_text(&args[3], L"Kid");
        if (wfc_add(nodes, args, 4, &kid) != 0 || kid == NULL) {
            wfc_check_fail("a child node could not be added in", dll_path);
        }
        wfc_text(&args[0], L"k");
        wfc_number(&args[1], 0); /* tvwFirst: before its siblings */
        wfc_text(&args[2], L"k0");
        wfc_text(&args[3], L"First kid");
        if (wfc_add(nodes, args, 4, &later) != 0 || later == NULL) {
            wfc_check_fail("a first sibling could not be added in", dll_path);
        }
        if (wfc_property_number(root, L"Children") != 2 ||
            !wfc_property_is(kid, L"FullPath", L"Root\\Kid")) {
            wfc_check_fail("the tree is not what was added in", dll_path);
        }
        if (wfc_call(root, L"Child", DISPATCH_PROPERTYGET, NULL, 0, &value,
                     NULL) != S_OK ||
            value.vt != VT_DISPATCH ||
            !wfc_property_is(value.pdispVal, L"Key", L"k0")) {
            wfc_check_fail("Child is not the first kid in", dll_path);
        }
        VariantClear(&value);
        if (wfc_call(later, L"Next", DISPATCH_PROPERTYGET, NULL, 0, &value,
                     NULL) != S_OK ||
            value.vt != VT_DISPATCH ||
            !wfc_property_is(value.pdispVal, L"Key", L"k")) {
            wfc_check_fail("Next is not the second kid in", dll_path);
        }
        VariantClear(&value);
        wfc_text(&args[0], L"r");
        wfc_number(&args[1], 9);
        wfc_text(&args[2], L"bad");
        if (wfc_add(nodes, args, 3, NULL) != 35607) {
            wfc_check_fail("relationship 9 was not error 35607 in", dll_path);
        }
        wfc_text(&args[0], L"nope");
        wfc_number(&args[1], 4);
        if (wfc_add(nodes, args, 2, NULL) != 35601) {
            wfc_check_fail("an unknown relative was not error 35601 in",
                           dll_path);
        }
        /* Removing a node removes what is below it. */
        wfc_text(&selector, L"r");
        wfc_call(nodes, L"Remove", DISPATCH_METHOD, &selector, 1, NULL, NULL);
        if (wfc_property_number(nodes, L"Count") != 0) {
            wfc_check_fail("Remove left the descendants in", dll_path);
        }
        found = wfc_item(nodes, &selector);
        if (found != NULL) {
            wfc_check_fail("a removed node was still found in", dll_path);
        }
        later->lpVtbl->Release(later);
        kid->lpVtbl->Release(kid);
        root->lpVtbl->Release(root);
        nodes->lpVtbl->Release(nodes);
        tree->lpVtbl->Release(tree);
    }
    printf(
        "TabStrip, StatusBar, Toolbar, ListView, ImageList, ImageCombo, "
        "TreeView: the collections work through COM\n");
}

/* ---- Events, sub collections, bound image lists, sorting and the text of an
 * ImageCombo (ADR-0010 step 7, last part). */

/* An event sink: records the dispatch id of each event and, when asked, sets
 * the Cancel argument of a cancellable one. */
static long wfc_sink_events[16];
static int wfc_sink_count = 0;
static int wfc_sink_cancel = 0;

static HRESULT __stdcall wfc_sink_qi(IDispatch* self, REFIID iid,
                                     void** object) {
    if (IsEqualIID(iid, &IID_IUnknown) || IsEqualIID(iid, &IID_IDispatch)) {
        *object = self;
        return S_OK;
    }
    *object = NULL;
    return E_NOINTERFACE;
}

static ULONG __stdcall wfc_sink_ref(IDispatch* self) {
    (void)self;
    return 2;
}

static HRESULT __stdcall wfc_sink_count_types(IDispatch* self, UINT* count) {
    (void)self;
    *count = 0;
    return S_OK;
}

static HRESULT __stdcall wfc_sink_type(IDispatch* self, UINT n, LCID lcid,
                                       ITypeInfo** info) {
    (void)self;
    (void)n;
    (void)lcid;
    *info = NULL;
    return E_NOTIMPL;
}

static HRESULT __stdcall wfc_sink_ids(IDispatch* self, REFIID iid,
                                      LPOLESTR* names, UINT count, LCID lcid,
                                      DISPID* ids) {
    (void)self;
    (void)iid;
    (void)names;
    (void)count;
    (void)lcid;
    (void)ids;
    return DISP_E_UNKNOWNNAME;
}

static HRESULT __stdcall wfc_sink_invoke(IDispatch* self, DISPID id, REFIID iid,
                                         LCID lcid, WORD flags,
                                         DISPPARAMS* params, VARIANT* result,
                                         EXCEPINFO* exception, UINT* arg) {
    (void)self;
    (void)iid;
    (void)lcid;
    (void)flags;
    (void)result;
    (void)exception;
    (void)arg;
    if (wfc_sink_count < 16) {
        wfc_sink_events[wfc_sink_count++] = (long)id;
    }
    if (wfc_sink_cancel && params->cArgs == 1 &&
        params->rgvarg[0].vt == (VT_I2 | VT_BYREF)) {
        *params->rgvarg[0].piVal = -1;
    }
    return S_OK;
}

static IDispatchVtbl wfc_sink_vtbl = {
    wfc_sink_qi,   wfc_sink_ref, wfc_sink_ref,    wfc_sink_count_types,
    wfc_sink_type, wfc_sink_ids, wfc_sink_invoke,
};
static IDispatch wfc_sink = {&wfc_sink_vtbl};

static void wfc_listen(IDispatch* control, const wchar_t* path) {
    IConnectionPointContainer* container = NULL;
    IConnectionPoint* point = NULL;
    DWORD cookie = 0;
    if (control->lpVtbl->QueryInterface(control, &IID_IConnectionPointContainer,
                                        (void**)&container) != S_OK ||
        container->lpVtbl->FindConnectionPoint(container, &IID_IDispatch,
                                               &point) != S_OK ||
        point->lpVtbl->Advise(point, (IUnknown*)&wfc_sink, &cookie) != S_OK) {
        wfc_check_fail("cannot listen to the events of a control in", path);
    }
    point->lpVtbl->Release(point);
    container->lpVtbl->Release(container);
}

static void wfc_expect_events(const wchar_t* path, const char* what, int count,
                              long first, long second) {
    if (wfc_sink_count != count || (count > 0 && wfc_sink_events[0] != first) ||
        (count > 1 && wfc_sink_events[1] != second)) {
        wfc_check_fail(what, path);
    }
    wfc_sink_count = 0;
}

static void wfc_check_more(const wchar_t* dll_path) {
    VARIANT args[5];
    VARIANT value;
    VARIANT selector;
    IDispatch* made = NULL;

    /* A selection made by the program raises BeforeClick (id 1, which can be
     * cancelled) and Click (id -600) on a TabStrip. */
    {
        IDispatch* strip =
            wfc_create(dll_path, L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
                       "no TabStrip in");
        IDispatch* tabs = wfc_collection(strip, L"Tabs", dll_path);
        IDispatch* first = NULL;
        IDispatch* second = NULL;
        wfc_listen(strip, dll_path);
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"a");
        wfc_text(&args[2], L"A");
        wfc_add(tabs, args, 3, &first);
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"b");
        wfc_text(&args[2], L"B");
        wfc_add(tabs, args, 3, &second);
        wfc_expect_events(dll_path, "Add raised an event in", 0, 0, 0);
        value.vt = VT_BOOL;
        value.boolVal = VARIANT_TRUE;
        wfc_call(second, L"Selected", DISPATCH_PROPERTYPUT, &value, 1, NULL,
                 NULL);
        wfc_expect_events(
            dll_path, "Selected = True did not raise BeforeClick and Click in",
            2, 1, -600);
        wfc_call(second, L"Selected", DISPATCH_PROPERTYPUT, &value, 1, NULL,
                 NULL);
        wfc_expect_events(
            dll_path, "selecting the selected tab raised events in", 0, 0, 0);
        wfc_sink_cancel = 1;
        wfc_call(first, L"Selected", DISPATCH_PROPERTYPUT, &value, 1, NULL,
                 NULL);
        wfc_sink_cancel = 0;
        wfc_expect_events(
            dll_path, "a cancelled BeforeClick did not still raise Click in", 2,
            1, -600);
        if (wfc_call(strip, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                     &value, NULL) != S_OK ||
            !wfc_property_is(value.pdispVal, L"Key", L"b")) {
            wfc_check_fail("a cancelled BeforeClick changed the selection in",
                           dll_path);
        }
        VariantClear(&value);
        first->lpVtbl->Release(first);
        second->lpVtbl->Release(second);
        tabs->lpVtbl->Release(tabs);
        strip->lpVtbl->Release(strip);
    }

    /* An ImageCombo's Text follows its selection and the other way round. */
    {
        IDispatch* combo =
            wfc_create(dll_path, L"{8E6ABCF0-6E14-5091-9BE5-B04948C170AE}",
                       "no ImageCombo in");
        IDispatch* entries = wfc_collection(combo, L"ComboItems", dll_path);
        IDispatch* red = NULL;
        wfc_listen(combo, dll_path);
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"red");
        wfc_text(&args[2], L"Red");
        wfc_add(entries, args, 3, &red);
        wfc_text(&value, L"RED");
        wfc_call(combo, L"Text", DISPATCH_PROPERTYPUT, &value, 1, NULL, NULL);
        wfc_expect_events(dll_path, "setting Text did not raise Change in", 1,
                          1, 0);
        if (wfc_call(combo, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                     &value, NULL) != S_OK ||
            value.pdispVal == NULL ||
            !wfc_property_is(value.pdispVal, L"Key", L"red")) {
            wfc_check_fail("Text did not select the item in", dll_path);
        }
        VariantClear(&value);
        wfc_text(&value, L"custom");
        wfc_call(combo, L"Text", DISPATCH_PROPERTYPUT, &value, 1, NULL, NULL);
        wfc_sink_count = 0;
        if (wfc_call(combo, L"SelectedItem", DISPATCH_PROPERTYGET, NULL, 0,
                     &value, NULL) != S_OK ||
            value.pdispVal != NULL) {
            wfc_check_fail("an unknown Text left an item selected in",
                           dll_path);
        }
        wfc_number(&value, 1);
        wfc_call(combo, L"SelStart", DISPATCH_PROPERTYPUT, &value, 1, NULL,
                 NULL);
        wfc_number(&value, 3);
        wfc_call(combo, L"SelLength", DISPATCH_PROPERTYPUT, &value, 1, NULL,
                 NULL);
        if (!wfc_property_is(combo, L"SelText", L"ust")) {
            wfc_check_fail("SelText is wrong in", dll_path);
        }
        value.vt = VT_DISPATCH;
        value.pdispVal = red;
        wfc_call(combo, L"SelectedItem",
                 DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF, &value, 1,
                 NULL, NULL);
        wfc_expect_events(dll_path,
                          "SelectedItem did not raise Click and Change in", 2,
                          -600, 1);
        if (!wfc_property_is(combo, L"Text", L"Red")) {
            wfc_check_fail("SelectedItem did not set Text in", dll_path);
        }
        red->lpVtbl->Release(red);
        entries->lpVtbl->Release(entries);
        combo->lpVtbl->Release(combo);
    }

    /* SubItems, ListSubItems and ButtonMenus. */
    {
        IDispatch* list =
            wfc_create(dll_path, L"{2C1007A3-9018-519F-A902-87E3E466A897}",
                       "no ListView in");
        IDispatch* items = wfc_collection(list, L"ListItems", dll_path);
        IDispatch* item = NULL;
        IDispatch* subs = NULL;
        VARIANT sub;
        EXCEPINFO exception;
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"k");
        wfc_text(&args[2], L"Item");
        wfc_add(items, args, 3, &item);
        wfc_number(&args[0], 2);
        wfc_text(&args[1], L"two");
        wfc_call(item, L"SubItems", DISPATCH_PROPERTYPUT, args, 2, NULL, NULL);
        wfc_number(&args[0], 2);
        if (wfc_call(item, L"SubItems", DISPATCH_PROPERTYGET, args, 1, &sub,
                     NULL) != S_OK ||
            sub.vt != VT_BSTR || wcscmp(sub.bstrVal, L"two") != 0) {
            wfc_check_fail("SubItems(2) is wrong in", dll_path);
        }
        wfc_number(&args[0], 9);
        memset(&exception, 0, sizeof(exception));
        if (wfc_call(item, L"SubItems", DISPATCH_PROPERTYGET, args, 1, &sub,
                     &exception) != DISP_E_EXCEPTION ||
            (exception.scode & 0xFFFF) != 35600) {
            wfc_check_fail("SubItems(9) was not error 35600 in", dll_path);
        }
        wfc_free_exception(&exception);
        if (wfc_call(item, L"ListSubItems", DISPATCH_PROPERTYGET, NULL, 0, &sub,
                     NULL) != S_OK ||
            sub.vt != VT_DISPATCH ||
            wfc_property_number(sub.pdispVal, L"Count") != 2) {
            wfc_check_fail("ListSubItems does not hold the sub items in",
                           dll_path);
        }
        VariantClear(&sub);
        item->lpVtbl->Release(item);
        items->lpVtbl->Release(items);
        list->lpVtbl->Release(list);

        list = wfc_create(dll_path, L"{EEBB0901-096F-52EB-A609-86815871912F}",
                          "no Toolbar in");
        items = wfc_collection(list, L"Buttons", dll_path);
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"b");
        wfc_text(&args[2], L"Button");
        wfc_number(&args[3], 5);
        wfc_add(items, args, 4, &item);
        if (wfc_call(item, L"ButtonMenus", DISPATCH_PROPERTYGET, NULL, 0, &sub,
                     NULL) != S_OK ||
            sub.vt != VT_DISPATCH) {
            wfc_check_fail("a button has no ButtonMenus in", dll_path);
        }
        subs = sub.pdispVal;
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"m");
        wfc_text(&args[2], L"Menu");
        if (wfc_add(subs, args, 3, NULL) != 0 ||
            wfc_property_number(subs, L"Count") != 1) {
            wfc_check_fail("ButtonMenus.Add failed in", dll_path);
        }
        subs->lpVtbl->Release(subs);
        item->lpVtbl->Release(item);
        items->lpVtbl->Release(items);
        list->lpVtbl->Release(list);
    }

    /* An ImageList given to a control keeps its images (35615), and its size
     * is read-only once there are images (35614). */
    {
        IDispatch* images =
            wfc_create(dll_path, L"{9217D929-9C68-526C-928E-99E4E4D6B815}",
                       "no ImageList in");
        IDispatch* pictures = wfc_collection(images, L"ListImages", dll_path);
        IDispatch* strip =
            wfc_create(dll_path, L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
                       "no TabStrip in");
        EXCEPINFO exception;
        memset(&exception, 0, sizeof(exception));
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"one");
        wfc_text(&args[2], L"picture");
        wfc_add(pictures, args, 3, NULL);
        value.vt = VT_DISPATCH;
        value.pdispVal = images;
        wfc_call(strip, L"ImageList",
                 DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF, &value, 1,
                 NULL, NULL);
        wfc_text(&selector, L"one");
        if (wfc_call(pictures, L"Remove", DISPATCH_METHOD, &selector, 1, NULL,
                     &exception) != DISP_E_EXCEPTION ||
            (exception.scode & 0xFFFF) != 35615) {
            wfc_check_fail("removing from a bound ImageList was not 35615 in",
                           dll_path);
        }
        wfc_free_exception(&exception);
        wfc_number(&args[0], 1);
        wfc_text(&args[1], L"front");
        wfc_text(&args[2], L"picture");
        if (wfc_add(pictures, args, 3, NULL) != 35615) {
            wfc_check_fail("inserting in a bound ImageList was not 35615 in",
                           dll_path);
        }
        wfc_missing(&args[0]);
        wfc_text(&args[1], L"end");
        wfc_text(&args[2], L"picture");
        if (wfc_add(pictures, args, 3, NULL) != 0) {
            wfc_check_fail("appending to a bound ImageList failed in",
                           dll_path);
        }
        wfc_number(&value, 32);
        if (wfc_call(images, L"ImageWidth", DISPATCH_PROPERTYPUT, &value, 1,
                     NULL, &exception) != DISP_E_EXCEPTION ||
            (exception.scode & 0xFFFF) != 35614) {
            wfc_check_fail("ImageWidth after images was not 35614 in",
                           dll_path);
        }
        wfc_free_exception(&exception);
        strip->lpVtbl->Release(strip);
        pictures->lpVtbl->Release(pictures);
        images->lpVtbl->Release(images);
    }

    /* A sorted TreeView, and its PathSeparator. */
    {
        IDispatch* tree =
            wfc_create(dll_path, L"{B10A6F9B-431E-59EF-B149-F118AEA46D00}",
                       "no TreeView in");
        IDispatch* nodes = wfc_collection(tree, L"Nodes", dll_path);
        IDispatch* kid = NULL;
        value.vt = VT_BOOL;
        value.boolVal = VARIANT_TRUE;
        wfc_call(tree, L"Sorted", DISPATCH_PROPERTYPUT, &value, 1, NULL, NULL);
        wfc_text(&value, L"/");
        wfc_call(tree, L"PathSeparator", DISPATCH_PROPERTYPUT, &value, 1, NULL,
                 NULL);
        wfc_missing(&args[0]);
        wfc_missing(&args[1]);
        wfc_text(&args[2], L"b");
        wfc_text(&args[3], L"Beta");
        wfc_add(nodes, args, 4, NULL);
        wfc_missing(&args[0]);
        wfc_missing(&args[1]);
        wfc_text(&args[2], L"a");
        wfc_text(&args[3], L"alpha");
        wfc_add(nodes, args, 4, &made);
        wfc_text(&args[0], L"a");
        wfc_number(&args[1], 4);
        wfc_text(&args[2], L"c");
        wfc_text(&args[3], L"Child");
        wfc_add(nodes, args, 4, &kid);
        if (wfc_call(made, L"FirstSibling", DISPATCH_PROPERTYGET, NULL, 0,
                     &value, NULL) != S_OK ||
            value.vt != VT_DISPATCH ||
            !wfc_property_is(value.pdispVal, L"Key", L"a")) {
            wfc_check_fail("a sorted TreeView did not put alpha first in",
                           dll_path);
        }
        VariantClear(&value);
        if (!wfc_property_is(kid, L"FullPath", L"alpha/Child")) {
            wfc_check_fail("PathSeparator is not used by FullPath in",
                           dll_path);
        }
        kid->lpVtbl->Release(kid);
        made->lpVtbl->Release(made);
        nodes->lpVtbl->Release(nodes);
        tree->lpVtbl->Release(tree);
    }
    printf(
        "events, sub collections, bound image lists and sorting work "
        "through COM\n");
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
    wfc_check_collections(wfc_check_widen(argv[1]));
    wfc_check_more(wfc_check_widen(argv[1]));
    return wfc_check_create(wfc_check_widen(argv[1]),
                            L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
                            L"Enabled", L"Refresh");
}
