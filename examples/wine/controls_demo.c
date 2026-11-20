/* A window with the Windows common controls that the MSComctlLib library of
 * WFC wraps (Toolbar, TabStrip, ProgressBar, Slider, TreeView, ListView,
 * StatusBar), in the look of the WFC games: a dark navy window, raised
 * panels with a thin edge and a two-step shadow, teal captions. CI builds it
 * with Wine's winegcc and photographs it under Wine on Linux for the README;
 * it also builds with any Windows C compiler (user32, gdi32, comctl32). */
#include <windows.h>

#include <commctrl.h>
#include <string.h>

/* The colors of the games' theme. */
#define THEME_NAVY RGB(0x11, 0x14, 0x1F)
#define THEME_PANEL RGB(0x1B, 0x20, 0x33)
#define THEME_EDGE RGB(0x40, 0x4A, 0x6B)
#define THEME_SHADOW_NEAR RGB(0x08, 0x0A, 0x0F)
#define THEME_SHADOW_FAR RGB(0x0D, 0x10, 0x17)
#define THEME_TEXT RGB(0xE8, 0xEB, 0xF5)
#define THEME_MUTED RGB(0x98, 0xA0, 0xBE)
#define THEME_TEAL RGB(0x3D, 0xD6, 0xC6)

/* The layout, in pixels. */
#define CLIENT_WIDTH 640
#define CLIENT_HEIGHT 480
#define MARGIN 20
#define GAP 14
#define PAD 12
#define CAPTION 24
#define COLUMN ((CLIENT_WIDTH - 2 * MARGIN - GAP) / 2)
#define TOOLBAR_TOP 72
#define TOOLBAR_HEIGHT 54
#define ROW_TWO (TOOLBAR_TOP + TOOLBAR_HEIGHT + GAP)
#define ROW_TWO_HEIGHT 142
#define ROW_THREE (ROW_TWO + ROW_TWO_HEIGHT + GAP)
#define ROW_THREE_HEIGHT 150
#define LEFT_X (MARGIN + PAD)
#define RIGHT_X (MARGIN + COLUMN + GAP + PAD)
#define INNER (COLUMN - 2 * PAD)

/* Which static text is which, for the colors. */
enum { ID_TITLE = 1, ID_SUBTITLE, ID_CAPTION, ID_SLIDER };

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Card;

static const Card cards[] = {
    {MARGIN, TOOLBAR_TOP, CLIENT_WIDTH - 2 * MARGIN, TOOLBAR_HEIGHT},
    {MARGIN, ROW_TWO, COLUMN, ROW_TWO_HEIGHT},
    {MARGIN + COLUMN + GAP, ROW_TWO, COLUMN, ROW_TWO_HEIGHT},
    {MARGIN, ROW_THREE, COLUMN, ROW_THREE_HEIGHT},
    {MARGIN + COLUMN + GAP, ROW_THREE, COLUMN, ROW_THREE_HEIGHT},
};

static HBRUSH brush_navy;
static HBRUSH brush_panel;
static HBRUSH brush_face;
static HFONT font_body;
static HFONT font_caption;
static HFONT font_title;
static HFONT font_subtitle;

static HFONT make_font(int height, int weight) {
    return CreateFontA(-height, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");
}

static HWND make(const char* class_name, DWORD style, int x, int y, int w,
                 int h, HWND parent) {
    HWND control =
        CreateWindowExA(0, class_name, "", WS_CHILD | WS_VISIBLE | style, x, y,
                        w, h, parent, NULL, GetModuleHandleA(NULL), NULL);
    SendMessageA(control, WM_SETFONT, (WPARAM)font_body, TRUE);
    return control;
}

static void text(HWND parent, int id, int x, int y, int w, int h,
                 const char* caption, HFONT font) {
    HWND label = CreateWindowExA(
        0, "STATIC", caption, WS_CHILD | WS_VISIBLE | SS_LEFT, x, y, w, h,
        parent, (HMENU)(INT_PTR)id, GetModuleHandleA(NULL), NULL);
    SendMessageA(label, WM_SETFONT, (WPARAM)font, TRUE);
}

static void fill(HDC dc, int x, int y, int w, int h, COLORREF color) {
    RECT area;
    HBRUSH brush = CreateSolidBrush(color);
    area.left = x;
    area.top = y;
    area.right = x + w;
    area.bottom = y + h;
    FillRect(dc, &area, brush);
    DeleteObject(brush);
}

/* A raised panel: a two-step shadow, then the card with a thin edge. */
static void paint_card(HDC dc, const Card* card) {
    fill(dc, card->x + 5, card->y + 7, card->width, card->height,
         THEME_SHADOW_FAR);
    fill(dc, card->x + 2, card->y + 3, card->width, card->height,
         THEME_SHADOW_NEAR);
    fill(dc, card->x, card->y, card->width, card->height, THEME_EDGE);
    fill(dc, card->x + 1, card->y + 1, card->width - 2, card->height - 2,
         THEME_PANEL);
}

static LRESULT CALLBACK procedure(HWND window, UINT message, WPARAM w,
                                  LPARAM l) {
    switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_ERASEBKGND: {
            RECT area;
            GetClientRect(window, &area);
            FillRect((HDC)w, &area, brush_navy);
            return 1;
        }
        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window, &paint);
            size_t i;
            for (i = 0; i < sizeof(cards) / sizeof(cards[0]); ++i) {
                paint_card(dc, &cards[i]);
            }
            EndPaint(window, &paint);
            return 0;
        }
        case WM_CTLCOLORSTATIC: {
            HDC dc = (HDC)w;
            const int id = GetDlgCtrlID((HWND)l);
            SetBkMode(dc, OPAQUE);
            if (id == ID_SLIDER) {
                /* The slider takes its background from here; keep it light, as
                 * the other controls are. */
                SetBkColor(dc, GetSysColor(COLOR_BTNFACE));
                return (LRESULT)brush_face;
            }
            if (id == ID_CAPTION) {
                SetTextColor(dc, THEME_TEAL);
                SetBkColor(dc, THEME_PANEL);
                return (LRESULT)brush_panel;
            }
            SetTextColor(dc, id == ID_SUBTITLE ? THEME_MUTED : THEME_TEXT);
            SetBkColor(dc, THEME_NAVY);
            return (LRESULT)brush_navy;
        }
        default:
            break;
    }
    return DefWindowProcA(window, message, w, l);
}

static void add_tab(HWND tabs, int index, const char* label) {
    TCITEMA tab;
    ZeroMemory(&tab, sizeof(tab));
    tab.mask = TCIF_TEXT;
    tab.pszText = (char*)label;
    SendMessageA(tabs, TCM_INSERTITEMA, (WPARAM)index, (LPARAM)&tab);
}

static HTREEITEM add_node(HWND tree, HTREEITEM parent, const char* label) {
    TVINSERTSTRUCTA node;
    ZeroMemory(&node, sizeof(node));
    node.hParent = parent;
    node.hInsertAfter = TVI_LAST;
    node.item.mask = TVIF_TEXT;
    node.item.pszText = (char*)label;
    return (HTREEITEM)SendMessageA(tree, TVM_INSERTITEMA, 0, (LPARAM)&node);
}

static void add_column(HWND list, int index, int width, const char* label) {
    LVCOLUMNA column;
    ZeroMemory(&column, sizeof(column));
    column.mask = LVCF_TEXT | LVCF_WIDTH;
    column.cx = width;
    column.pszText = (char*)label;
    SendMessageA(list, LVM_INSERTCOLUMNA, (WPARAM)index, (LPARAM)&column);
}

static void add_row(HWND list, int index, const char* name, const char* size) {
    LVITEMA row;
    ZeroMemory(&row, sizeof(row));
    row.mask = LVIF_TEXT;
    row.iItem = index;
    row.pszText = (char*)name;
    SendMessageA(list, LVM_INSERTITEMA, 0, (LPARAM)&row);
    row.iSubItem = 1;
    row.pszText = (char*)size;
    SendMessageA(list, LVM_SETITEMA, 0, (LPARAM)&row);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command,
                   int show) {
    WNDCLASSA type;
    RECT frame;
    HWND window;
    HWND control;
    TBBUTTON buttons[5];
    HTREEITEM project;
    HTREEITEM sources;
    INITCOMMONCONTROLSEX init;
    int parts[2] = {200, -1};
    const char* labels[] = {"New", "Open", "", "Bold", "Favorite"};
    const char* colors[] = {"Teal", "Amber", "Navy"};
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
    brush_navy = CreateSolidBrush(THEME_NAVY);
    brush_panel = CreateSolidBrush(THEME_PANEL);
    brush_face = GetSysColorBrush(COLOR_BTNFACE);
    font_body = make_font(13, FW_NORMAL);
    font_caption = make_font(13, FW_BOLD);
    font_title = make_font(24, FW_BOLD);
    font_subtitle = make_font(13, FW_NORMAL);
    ZeroMemory(&type, sizeof(type));
    type.lpfnWndProc = procedure;
    type.hInstance = instance;
    type.hCursor = LoadCursorA(NULL, IDC_ARROW);
    type.hbrBackground = brush_navy;
    type.lpszClassName = "WfcControlsDemo";
    RegisterClassA(&type);
    frame.left = 0;
    frame.top = 0;
    frame.right = CLIENT_WIDTH;
    frame.bottom = CLIENT_HEIGHT;
    AdjustWindowRect(&frame, WS_OVERLAPPEDWINDOW, FALSE);
    window =
        CreateWindowExA(0, "WfcControlsDemo", "Windows Common Controls",
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE, 20,
                        20, frame.right - frame.left, frame.bottom - frame.top,
                        NULL, NULL, instance, NULL);

    text(window, ID_TITLE, MARGIN, 12, 400, 32, "Windows Common Controls",
         font_title);
    text(window, ID_SUBTITLE, MARGIN, 44, 500, 20,
         "The controls of wfc-mscomctl, in one window", font_subtitle);
    text(window, ID_CAPTION, LEFT_X, ROW_TWO + 8, 150, 18, "TabStrip",
         font_caption);
    text(window, ID_CAPTION, RIGHT_X, ROW_TWO + 8, 250, 18,
         "ProgressBar, Slider, ComboBox", font_caption);
    text(window, ID_CAPTION, LEFT_X, ROW_THREE + 8, 150, 18, "TreeView",
         font_caption);
    text(window, ID_CAPTION, RIGHT_X, ROW_THREE + 8, 150, 18, "ListView",
         font_caption);

    control =
        make(TOOLBARCLASSNAMEA,
             TBSTYLE_LIST | CCS_NOPARENTALIGN | CCS_NORESIZE | CCS_NODIVIDER,
             LEFT_X, TOOLBAR_TOP + 12, CLIENT_WIDTH - 2 * LEFT_X, 30, window);
    SendMessageA(control, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessageA(control, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
    memset(buttons, 0, sizeof(buttons));
    for (i = 0; i < 5; ++i) {
        buttons[i].iBitmap = I_IMAGENONE;
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
        if (i == 1) {
            buttons[i].fsStyle |= BTNS_DROPDOWN;
        }
        if (i == 2) {
            buttons[i].fsStyle = BTNS_SEP;
        } else {
            buttons[i].iString = (INT_PTR)SendMessageA(control, TB_ADDSTRINGA,
                                                       0, (LPARAM)labels[i]);
        }
        if (i == 3) {
            buttons[i].fsStyle |= BTNS_CHECK;
        }
    }
    SendMessageA(control, TB_ADDBUTTONSA, 5, (LPARAM)buttons);

    control =
        make(WC_TABCONTROLA, WS_CLIPSIBLINGS, LEFT_X, ROW_TWO + CAPTION + 4,
             INNER, ROW_TWO_HEIGHT - CAPTION - PAD - 4, window);
    add_tab(control, 0, "General");
    add_tab(control, 1, "Advanced");
    add_tab(control, 2, "Log");

    control = make(PROGRESS_CLASSA, 0, RIGHT_X, ROW_TWO + CAPTION + 6, INNER,
                   18, window);
    SendMessageA(control, PBM_SETPOS, 65, 0);
    control = make(TRACKBAR_CLASSA, TBS_AUTOTICKS, RIGHT_X,
                   ROW_TWO + CAPTION + 32, INNER, 34, window);
    SetWindowLongPtrA(control, GWLP_ID, ID_SLIDER);
    SendMessageA(control, TBM_SETRANGE, TRUE, MAKELONG(0, 10));
    SendMessageA(control, TBM_SETPOS, TRUE, 4);
    control = make("COMBOBOX", CBS_DROPDOWNLIST | WS_VSCROLL, RIGHT_X,
                   ROW_TWO + CAPTION + 74, INNER, 120, window);
    for (i = 0; i < 3; ++i) {
        SendMessageA(control, CB_ADDSTRING, 0, (LPARAM)colors[i]);
    }
    SendMessageA(control, CB_SETCURSEL, 0, 0);

    control =
        make(WC_TREEVIEWA, TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT,
             LEFT_X, ROW_THREE + CAPTION + 4, INNER,
             ROW_THREE_HEIGHT - CAPTION - PAD - 4, window);
    project = add_node(control, TVI_ROOT, "Project");
    sources = add_node(control, project, "Sources");
    add_node(control, sources, "Main.bas");
    add_node(control, sources, "Form1.frm");
    add_node(control, project, "Docs");
    SendMessageA(control, TVM_EXPAND, TVE_EXPAND, (LPARAM)project);
    SendMessageA(control, TVM_EXPAND, TVE_EXPAND, (LPARAM)sources);

    control = make(WC_LISTVIEWA, LVS_REPORT, RIGHT_X, ROW_THREE + CAPTION + 4,
                   INNER, ROW_THREE_HEIGHT - CAPTION - PAD - 4, window);
    add_column(control, 0, 150, "Name");
    add_column(control, 1, 90, "Size");
    add_row(control, 0, "Main.bas", "4 KB");
    add_row(control, 1, "Form1.frm", "12 KB");
    add_row(control, 2, "Readme.txt", "1 KB");

    control = CreateWindowExA(0, STATUSCLASSNAMEA, "", WS_CHILD | WS_VISIBLE, 0,
                              0, 0, 0, window, NULL, instance, NULL);
    SendMessageA(control, WM_SETFONT, (WPARAM)font_body, TRUE);
    SendMessageA(control, SB_SETPARTS, 2, (LPARAM)parts);
    SendMessageA(control, SB_SETTEXTA, 0, (LPARAM) "Ready");
    SendMessageA(control, SB_SETTEXTA, 1, (LPARAM) "3 files");

    while (GetMessageA(&message, NULL, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return 0;
}
