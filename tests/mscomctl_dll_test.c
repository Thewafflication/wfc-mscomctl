/* Black-box check of wfcmscomctl.dll: exports, version resource and the
 * registration round trip (see dll_check.h). Run by CTest with the DLL,
 * the version and the target as arguments. */
#include "dll_check.h"

static const struct wfc_check_class kClasses[] = {
    {L"WFC.MSComctlLib.TabStrip",
     L"{05B6AB6B-6444-5494-AFE6-1D8431CB104B}",
     L"MSComctlLib.TabStrip.2",
     L"{1EFB6596-857C-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.Toolbar",
     L"{EEBB0901-096F-52EB-A609-86815871912F}",
     L"MSComctlLib.Toolbar.2",
     L"{66833FE6-8583-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.StatusBar",
     L"{11033AF3-05F7-58AE-93B1-596BB33150BB}",
     L"MSComctlLib.SBarCtrl.2",
     L"{8E3867A3-8586-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ProgressBar",
     L"{FA3BFA10-D0F7-579F-A62B-339D7E21E9ED}",
     L"MSComctlLib.ProgCtrl.2",
     L"{35053A22-8589-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.TreeView",
     L"{B10A6F9B-431E-59EF-B149-F118AEA46D00}",
     L"MSComctlLib.TreeCtrl.2",
     L"{C74190B6-8589-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ListView",
     L"{2C1007A3-9018-519F-A902-87E3E466A897}",
     L"MSComctlLib.ListViewCtrl.2",
     L"{BDD1F04B-858B-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ImageList",
     L"{9217D929-9C68-526C-928E-99E4E4D6B815}",
     L"MSComctlLib.ImageListCtrl.2",
     L"{2C247F23-8591-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.Slider",
     L"{43A1BE27-BBCC-5436-958B-45521EE99844}",
     L"MSComctlLib.Slider.2",
     L"{F08DF954-8592-11D1-B16A-00C0F0283628}"},
    {L"WFC.MSComctlLib.ImageCombo",
     L"{8E6ABCF0-6E14-5091-9BE5-B04948C170AE}",
     L"MSComctlLib.ImageComboCtl.2",
     L"{DD9DA666-8594-11D1-B16A-00C0F0283628}"},
};

int main(int argc, char** argv) {
    return wfc_check_dll(argc, argv, L"mscomctl", kClasses,
                         sizeof(kClasses) / sizeof(kClasses[0]));
}
