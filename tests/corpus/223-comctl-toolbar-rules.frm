VERSION 5.00
Begin VB.Form Form1
   Caption         =   "Toolbar rules"
   Begin MSComctlLib.Toolbar Tool1
      Height          =   330
      Left            =   0
      Top             =   0
      Width           =   2000
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Show(ByVal tag As String)
    Dim b As Button
    Dim s As String
    For Each b In Tool1.Buttons
        s = s & b.Key & "=" & b.Value & IIf(b.Visible, "", "h") & " "
    Next b
    Debug.Print tag; ": "; s
End Sub

Private Sub Form_Load()
    Dim b As Button
    Dim i As Long
    Tool1.Buttons.Add , "off", "Off"
    Tool1.Buttons("off").Enabled = False
    Tool1.Buttons.Add , "sep", , tbrSeparator
    Tool1.Buttons.Add , "ph", , tbrPlaceholder
    Tool1.Buttons.Add , "chk", "Check", tbrCheck
    Tool1.Buttons.Add , "ga", "A", tbrButtonGroup
    Tool1.Buttons.Add , "gb", "B", tbrButtonGroup
    Tool1.Buttons.Add , "plain", "Plain"
    Set b = Tool1.Buttons.Add(, "dd", "Drop", tbrDropdown)
    b.ButtonMenus.Add , "m1", "One"
    b.ButtonMenus.Add , "mdis", "Disabled"
    b.ButtonMenus("mdis").Enabled = False
    Set b = Tool1.Buttons.Add(, "ddoff", "DropOff", tbrDropdown)
    b.Enabled = False
    Tool1.Buttons.Add , "x1", "X", tbrCheck
    Tool1.Buttons.Add , "x2", "Y", tbrButtonGroup
    Tool1.Buttons.Add , "x3", "Z", tbrButtonGroup
    Show "start"

    ' Values and styles outside their ranges fail with 380.
    On Error Resume Next
    For i = -1 To 6 Step 7
        Tool1.Buttons("plain").Style = i
        Debug.Print "style"; i; Err.Number; Err.Description
        Err.Clear
    Next i
    Tool1.Buttons("chk").Value = 2
    Debug.Print "value 2"; Err.Number
    Err.Clear
    Tool1.Buttons("chk").Value = -1
    Debug.Print "value -1"; Err.Number
    Err.Clear
    Tool1.Buttons("chk").Style = "abc"
    Debug.Print "style abc"; Err.Number
    Err.Clear
    Tool1.Buttons("nope").Value = 1
    Debug.Print "no button"; Err.Number; Err.Description
    Err.Clear
    Tool1.Buttons(0).Value = 1
    Debug.Print "index 0"; Err.Number
    Err.Clear
    On Error GoTo 0

    ' A group presses one button and releases its neighbours.
    Tool1.Buttons("ga").Value = tbrPressed
    Tool1.Buttons("gb").Value = tbrPressed
    Show "group"
    Tool1.Buttons("x2").Value = tbrPressed
    Show "second group"
    ' Pressing a group button by value 0 releases it only.
    Tool1.Buttons("x2").Value = tbrUnpressed
    Show "release"
    ' A check button toggles on its own.
    Tool1.Buttons("x1").Value = tbrPressed
    Tool1.Buttons("chk").Value = tbrPressed
    Show "check"

    ' Visibility moves the buttons that follow.
    Debug.Print "left"; Tool1.Buttons("plain").Left > Tool1.Buttons("chk").Left
    Tool1.Buttons("chk").Visible = False
    Debug.Print "hidden"; Tool1.Buttons("chk").Width; Tool1.Buttons("chk").Left
    Tool1.Buttons("chk").Visible = True
    Tool1.Buttons("sep").Width = 300
    Debug.Print "sep"; Tool1.Buttons("sep").Width

    ' The toolbar wraps when it is wrappable and the row is full.
    Tool1.Wrappable = True
    Debug.Print "wrap"; Tool1.Buttons(Tool1.Buttons.Count).Top > Tool1.Buttons(1).Top
    Tool1.Wrappable = False
    Debug.Print "nowrap"; Tool1.Buttons(Tool1.Buttons.Count).Top = Tool1.Buttons(1).Top

    ' Save and restore keep the visible flags of the buttons by key.
    Tool1.Buttons("x1").Visible = False
    Tool1.SaveToolbar "app223", "tb", "rules"
    Tool1.Buttons("x1").Visible = True
    Tool1.Buttons("plain").Visible = False
    Tool1.RestoreToolbar "app223", "tb", "rules"
    Show "restore"
    Tool1.RestoreToolbar "app223", "tb", "missing"
    Show "restore missing"
    On Error Resume Next
    Tool1.SaveToolbar "app223", "tb"
    Debug.Print "save args"; Err.Number; Err.Description
    Err.Clear
    On Error GoTo 0
End Sub

Private Sub Tool1_ButtonClick(ByVal Button As MSComctlLib.Button)
    Debug.Print "click "; Button.Key; Button.Value
End Sub

Private Sub Tool1_ButtonDropDown(ByVal Button As MSComctlLib.Button)
    Debug.Print "dropdown "; Button.Key
End Sub

Private Sub Tool1_ButtonMenuClick(ByVal ButtonMenu As MSComctlLib.ButtonMenu)
    Debug.Print "menu "; ButtonMenu.Key; ButtonMenu.Text
End Sub
