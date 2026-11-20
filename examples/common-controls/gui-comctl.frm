VERSION 5.00
Begin VB.Form frmCommon
   BackColor       =   &H001F1411&
   Caption         =   "Windows Common Controls"
   ClientHeight    =   7200
   ClientWidth     =   9600
   BeginProperty Font
      Name            =   "Segoe UI"
      Size            =   9
   EndProperty
   Begin VB.Timer Timer1
      Interval        =   2500
   End
   Begin VB.Label lblTitle
      BackColor       =   &H001F1411&
      Caption         =   "Windows Common Controls"
      ForeColor       =   &H00F5EBE8&
      Height          =   420
      Left            =   300
      Top             =   210
      Width           =   6000
      BeginProperty Font
         Name            =   "Segoe UI"
         Size            =   15
         Bold            =   -1
      EndProperty
   End
   Begin VB.Label lblSub
      BackColor       =   &H001F1411&
      Caption         =   "The MSComctlLib controls of WFC, in one form"
      ForeColor       =   &H00BEA098&
      Height          =   270
      Left            =   300
      Top             =   630
      Width           =   6900
      BeginProperty Font
         Name            =   "Segoe UI"
         Size            =   9
         Bold            =   0
      EndProperty
   End
   Begin VB.Label lblTabs
      BackColor       =   &H0033201B&
      Caption         =   "TabStrip"
      ForeColor       =   &H00C6D63D&
      Height          =   270
      Left            =   480
      Top             =   2220
      Width           =   2250
      BeginProperty Font
         Name            =   "Segoe UI"
         Size            =   9
         Bold            =   -1
      EndProperty
   End
   Begin VB.Label lblRange
      BackColor       =   &H0033201B&
      Caption         =   "ProgressBar, Slider, ImageCombo"
      ForeColor       =   &H00C6D63D&
      Height          =   270
      Left            =   5085
      Top             =   2220
      Width           =   3000
      BeginProperty Font
         Name            =   "Segoe UI"
         Size            =   9
         Bold            =   -1
      EndProperty
   End
   Begin VB.Label lblTree
      BackColor       =   &H0033201B&
      Caption         =   "TreeView"
      ForeColor       =   &H00C6D63D&
      Height          =   270
      Left            =   480
      Top             =   4560
      Width           =   2250
      BeginProperty Font
         Name            =   "Segoe UI"
         Size            =   9
         Bold            =   -1
      EndProperty
   End
   Begin VB.Label lblList
      BackColor       =   &H0033201B&
      Caption         =   "ListView"
      ForeColor       =   &H00C6D63D&
      Height          =   270
      Left            =   5085
      Top             =   4560
      Width           =   2250
      BeginProperty Font
         Name            =   "Segoe UI"
         Size            =   9
         Bold            =   -1
      EndProperty
   End
   Begin VB.Shape cardT
      BorderColor     =   &H006B4A40&
      FillColor       =   &H0033201B&
      FillStyle       =   0
      Height          =   810
      Left            =   300
      Shape           =   0
      Top             =   1080
      Width           =   9000
   End
   Begin VB.Shape shaT
      BorderStyle     =   0
      FillColor       =   &H000F0A08&
      FillStyle       =   0
      Height          =   810
      Left            =   330
      Shape           =   0
      Top             =   1125
      Width           =   9000
   End
   Begin VB.Shape shbT
      BorderStyle     =   0
      FillColor       =   &H00170D10&
      FillStyle       =   0
      Height          =   810
      Left            =   375
      Shape           =   0
      Top             =   1185
      Width           =   9000
   End
   Begin VB.Shape cardL
      BorderColor     =   &H006B4A40&
      FillColor       =   &H0033201B&
      FillStyle       =   0
      Height          =   2130
      Left            =   300
      Shape           =   0
      Top             =   2100
      Width           =   4395
   End
   Begin VB.Shape shaL
      BorderStyle     =   0
      FillColor       =   &H000F0A08&
      FillStyle       =   0
      Height          =   2130
      Left            =   330
      Shape           =   0
      Top             =   2145
      Width           =   4395
   End
   Begin VB.Shape shbL
      BorderStyle     =   0
      FillColor       =   &H00170D10&
      FillStyle       =   0
      Height          =   2130
      Left            =   375
      Shape           =   0
      Top             =   2205
      Width           =   4395
   End
   Begin VB.Shape cardR
      BorderColor     =   &H006B4A40&
      FillColor       =   &H0033201B&
      FillStyle       =   0
      Height          =   2130
      Left            =   4905
      Shape           =   0
      Top             =   2100
      Width           =   4395
   End
   Begin VB.Shape shaR
      BorderStyle     =   0
      FillColor       =   &H000F0A08&
      FillStyle       =   0
      Height          =   2130
      Left            =   4935
      Shape           =   0
      Top             =   2145
      Width           =   4395
   End
   Begin VB.Shape shbR
      BorderStyle     =   0
      FillColor       =   &H00170D10&
      FillStyle       =   0
      Height          =   2130
      Left            =   4980
      Shape           =   0
      Top             =   2205
      Width           =   4395
   End
   Begin VB.Shape cardC
      BorderColor     =   &H006B4A40&
      FillColor       =   &H0033201B&
      FillStyle       =   0
      Height          =   2250
      Left            =   300
      Shape           =   0
      Top             =   4440
      Width           =   4395
   End
   Begin VB.Shape shaC
      BorderStyle     =   0
      FillColor       =   &H000F0A08&
      FillStyle       =   0
      Height          =   2250
      Left            =   330
      Shape           =   0
      Top             =   4485
      Width           =   4395
   End
   Begin VB.Shape shbC
      BorderStyle     =   0
      FillColor       =   &H00170D10&
      FillStyle       =   0
      Height          =   2250
      Left            =   375
      Shape           =   0
      Top             =   4545
      Width           =   4395
   End
   Begin VB.Shape cardD
      BorderColor     =   &H006B4A40&
      FillColor       =   &H0033201B&
      FillStyle       =   0
      Height          =   2250
      Left            =   4905
      Shape           =   0
      Top             =   4440
      Width           =   4395
   End
   Begin VB.Shape shaD
      BorderStyle     =   0
      FillColor       =   &H000F0A08&
      FillStyle       =   0
      Height          =   2250
      Left            =   4935
      Shape           =   0
      Top             =   4485
      Width           =   4395
   End
   Begin VB.Shape shbD
      BorderStyle     =   0
      FillColor       =   &H00170D10&
      FillStyle       =   0
      Height          =   2250
      Left            =   4980
      Shape           =   0
      Top             =   4545
      Width           =   4395
   End
   Begin MSComctlLib.Toolbar Toolbar1
      Height          =   570
      Left            =   480
      Top             =   1200
      Width           =   8640
   End
   Begin MSComctlLib.TabStrip TabStrip1
      Height          =   1530
      Left            =   480
      Top             =   2520
      Width           =   4035
   End
   Begin MSComctlLib.ProgressBar ProgressBar1
      Height          =   270
      Left            =   5085
      Top             =   2550
      Width           =   4035
      Value           =   65
   End
   Begin MSComctlLib.Slider Slider1
      Height          =   510
      Left            =   5085
      Top             =   2940
      Width           =   4035
      Max             =   10
      Value           =   4
   End
   Begin MSComctlLib.ImageCombo ImageCombo1
      Height          =   360
      Left            =   5085
      Top             =   3570
      Width           =   4035
   End
   Begin MSComctlLib.TreeView TreeView1
      Height          =   1650
      Left            =   480
      Top             =   4860
      Width           =   4035
   End
   Begin MSComctlLib.ListView ListView1
      Height          =   1650
      Left            =   5085
      Top             =   4860
      Width           =   4035
      View            =   3
   End
   Begin MSComctlLib.StatusBar StatusBar1
      Height          =   330
      Left            =   0
      Top             =   6870
      Width           =   9600
   End
   Begin MSComctlLib.ImageList Images1
      Left            =   9150
      Top             =   210
   End
End
Attribute VB_Name = "frmCommon"
Option Explicit

' The look of the games: a dark navy form, raised panels with a thin edge
' and a two-step shadow, teal captions. The controls themselves are the
' system common controls.

Private Sub Form_Load()
    Dim n As Node
    Dim folder As Node
    Dim p As String
    Dim c As Object
    ' VB6 starts a TabStrip and a StatusBar with one item; start empty.
    TabStrip1.Tabs.Clear
    StatusBar1.Panels.Clear
    p = App.Path & "\gui-comctl."
    Images1.ListImages.Add , "doc", LoadPicture(p & "doc.bmp")
    Images1.ListImages.Add , "folder", LoadPicture(p & "folder.bmp")
    Images1.ListImages.Add , "star", LoadPicture(p & "star.bmp")
    Set Toolbar1.ImageList = Images1
    Set TabStrip1.ImageList = Images1
    Set TreeView1.ImageList = Images1
    Set ListView1.SmallIcons = Images1
    Set ListView1.Icons = Images1
    Set ImageCombo1.ImageList = Images1
    Toolbar1.Buttons.Add , "new", "New", , "doc"
    Toolbar1.Buttons.Add , "open", "Open", tbrDropdown, "folder"
    Toolbar1.Buttons.Add , "sep", , tbrSeparator
    Toolbar1.Buttons.Add , "bold", "Bold", tbrCheck
    Toolbar1.Buttons.Add , "star", "Favorite", , "star"
    TabStrip1.Tabs.Add , "gen", "General", 1
    TabStrip1.Tabs.Add , "adv", "Advanced"
    TabStrip1.Tabs.Add , "log", "Log"
    Set n = TreeView1.Nodes.Add(, , "root", "Project", "folder")
    Set folder = TreeView1.Nodes.Add(n, tvwChild, "src", "Sources", "folder")
    TreeView1.Nodes.Add folder, tvwChild, "main", "Main.bas", "doc"
    TreeView1.Nodes.Add folder, tvwChild, "form", "Form1.frm", "doc"
    TreeView1.Nodes.Add n, tvwChild, "docs", "Docs", "folder"
    n.Expanded = True
    folder.Expanded = True
    ListView1.ColumnHeaders.Add , , "Name", 1700
    ListView1.ColumnHeaders.Add , , "Size", 1000
    ListView1.ListItems.Add(, "x", "Main.bas", , "doc").SubItems(1) = "4 KB"
    ListView1.ListItems.Add(, "y", "Form1.frm", , "doc").SubItems(1) = "12 KB"
    ListView1.ListItems.Add(, "z", "Readme.txt", , "doc").SubItems(1) = "1 KB"
    ImageCombo1.ComboItems.Add , "t", "Teal", "star"
    ImageCombo1.ComboItems.Add , "a", "Amber", "star"
    ImageCombo1.ComboItems.Add , "n", "Navy", "star"
    Set ImageCombo1.SelectedItem = ImageCombo1.ComboItems("t")
    StatusBar1.Panels.Add , "msg", "Ready"
    StatusBar1.Panels.Add , "n", "3 files"
    On Error Resume Next
    For Each c In Controls
        If TypeName(c) <> "Label" And TypeName(c) <> "Shape" Then
            c.Font.Name = "Segoe UI"
        End If
    Next c
End Sub

Private Sub Timer1_Timer()
    Debug.Print "closing "; TabStrip1.Tabs.Count; TreeView1.Nodes.Count; ListView1.ListItems.Count; ProgressBar1.Value
    Unload Me
End Sub
