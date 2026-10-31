/* A window with the Windows common controls that the MSComctlLib library of
 * WFC wraps (TabStrip, Toolbar, ProgressBar, Slider, TreeView, ListView,
 * StatusBar). CI builds it with Wine's winegcc and photographs it under Wine
 * on Linux for the README. */
#include <windows.h>
#include <commctrl.h>
#include <string.h>

static HWND make(const char* class_name, DWORD style, int x, int y, int w,
                 int h, HWND parent) {
    return CreateWindowExA(0, class_name, "", WS_CHILD | WS_VISIBLE | style, x,
                           y, w, h, parent, NULL, GetModuleHandleA(NULL), NULL);
}

static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM w,
                                  LPARAM l) {
    if (message == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(window, message, w, l);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command,
                   int show) {
    WNDCLASSA type;
    HWND window;
    HWND control;
    TCITEMA tab;
    TBBUTTON buttons[3];
    TVINSERTSTRUCTA node;
    HTREEITEM root;
    LVCOLUMNA column;
    LVITEMA row;
    INITCOMMONCONTROLSEX init;
    int parts[2] = {200, -1};
    const char* names[] = {"One", "Two"};
    const char* sizes[] = {"1 KB", "2 KB"};
    int i;
    MSG message;
    (void)previous;
    (void)command;
    (void)show;
    init.dwSize = sizeof(init);
    init.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES |
                 ICC_TREEVIEW_CLASSES | ICC_LISTVIEW_CLASSES |
                 ICC_PROGRESS_CLASS | ICC_UPDOWN_CLASS;
    InitCommonControlsEx(&init);
    ZeroMemory(&type, sizeof(type));
    type.lpfnWndProc = procedure;
    type.hInstance = instance;
    type.hCursor = LoadCursorA(NULL, IDC_ARROW);
    type.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    type.lpszClassName = "WfcControlsDemo";
    RegisterClassA(&type);
    window = CreateWindowExA(0, "WfcControlsDemo", "Common controls",
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE, 20, 20, 590, 420,
                             NULL, NULL, instance, NULL);

    control = make(WC_TABCONTROLA, 0, 10, 40, 200, 110, window);
    tab.mask = TCIF_TEXT;
    tab.pszText = (char*)"General";
    SendMessageA(control, TCM_INSERTITEMA, 0, (LPARAM)&tab);
    tab.pszText = (char*)"Advanced";
    SendMessageA(control, TCM_INSERTITEMA, 1, (LPARAM)&tab);

    control = make(TOOLBARCLASSNAMEA, TBSTYLE_LIST | TBSTYLE_FLAT, 0, 0, 0, 0,
                   window);
    SendMessageA(control, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    memset(buttons, 0, sizeof(buttons));
    for (i = 0; i < 3; ++i) {
        const char* label[] = {"New", "Open", "Bold"};
        buttons[i].iBitmap = I_IMAGENONE;
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
        buttons[i].iString = (INT_PTR)SendMessageA(control, TB_ADDSTRINGA, 0,
                                                   (LPARAM)label[i]);
    }
    SendMessageA(control, TB_ADDBUTTONSA, 3, (LPARAM)buttons);

    control = make(PROGRESS_CLASSA, 0, 230, 45, 200, 18, window);
    SendMessageA(control, PBM_SETPOS, 60, 0);
    control = make(TRACKBAR_CLASSA, TBS_AUTOTICKS, 230, 75, 200, 36, window);
    SendMessageA(control, TBM_SETPOS, TRUE, 4);

    control = make(WC_TREEVIEWA, TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
                   10, 170, 190, 150, window);
    ZeroMemory(&node, sizeof(node));
    node.hParent = TVI_ROOT;
    node.hInsertAfter = TVI_LAST;
    node.item.mask = TVIF_TEXT;
    node.item.pszText = (char*)"Root";
    root = (HTREEITEM)SendMessageA(control, TVM_INSERTITEMA, 0, (LPARAM)&node);
    node.hParent = root;
    node.item.pszText = (char*)"Alpha";
    SendMessageA(control, TVM_INSERTITEMA, 0, (LPARAM)&node);
    node.item.pszText = (char*)"Beta";
    SendMessageA(control, TVM_INSERTITEMA, 0, (LPARAM)&node);
    SendMessageA(control, TVM_EXPAND, TVE_EXPAND, (LPARAM)root);

    control = make(WC_LISTVIEWA, LVS_REPORT, 215, 170, 200, 150, window);
    ZeroMemory(&column, sizeof(column));
    column.mask = LVCF_TEXT | LVCF_WIDTH;
    column.cx = 90;
    column.pszText = (char*)"Name";
    SendMessageA(control, LVM_INSERTCOLUMNA, 0, (LPARAM)&column);
    column.pszText = (char*)"Size";
    SendMessageA(control, LVM_INSERTCOLUMNA, 1, (LPARAM)&column);
    for (i = 0; i < 2; ++i) {
        ZeroMemory(&row, sizeof(row));
        row.mask = LVIF_TEXT;
        row.iItem = i;
        row.pszText = (char*)names[i];
        SendMessageA(control, LVM_INSERTITEMA, 0, (LPARAM)&row);
        row.iSubItem = 1;
        row.pszText = (char*)sizes[i];
        SendMessageA(control, LVM_SETITEMA, 0, (LPARAM)&row);
    }

    control = CreateWindowExA(0, STATUSCLASSNAMEA, "", WS_CHILD | WS_VISIBLE, 0,
                              0, 0, 0, window, NULL, instance, NULL);
    SendMessageA(control, SB_SETPARTS, 2, (LPARAM)parts);
    SendMessageA(control, SB_SETTEXTA, 0, (LPARAM)"Ready");
    SendMessageA(control, SB_SETTEXTA, 1, (LPARAM)"3 items");

    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return 0;
}
