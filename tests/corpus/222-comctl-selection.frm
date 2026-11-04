VERSION 5.00
Begin VB.Form Form1
   Caption         =   "Selection and limits"
   Begin MSComctlLib.TabStrip Tabs1
      Height          =   2000
      Left            =   120
      Top             =   120
      Width           =   4000
   End
   Begin MSComctlLib.StatusBar Status1
      Height          =   300
      Left            =   0
      Top             =   2700
      Width           =   4800
   End
   Begin MSComctlLib.ImageList Images1
      Left            =   4200
      Top             =   0
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Function Chosen() As String
    If Tabs1.SelectedItem Is Nothing Then
        Chosen = "none"
    Else
        Chosen = Tabs1.SelectedItem.Key
    End If
End Function

Private Sub Form_Load()
    Dim i As Long
    ' The first tab added is selected, later ones are not.
    Tabs1.Tabs.Add , "a", "A"
    Debug.Print "first", Chosen()
    Tabs1.Tabs.Add , "b", "B"
    Tabs1.Tabs.Add , "c", "C"
    Tabs1.Tabs.Add , "d", "D"
    Debug.Print "added", Chosen(), Tabs1.Tabs.Count
    ' Removing the selected first tab selects the next one.
    Tabs1.Tabs.Remove "a"
    Debug.Print "remove first", Chosen()
    ' Removing a selected tab in the middle selects the previous one.
    Tabs1.Tabs("c").Selected = True
    Tabs1.Tabs.Remove "c"
    Debug.Print "remove middle", Chosen()
    ' Removing a tab that is not selected leaves the selection alone.
    Tabs1.Tabs.Remove "d"
    Debug.Print "remove other", Chosen(), Tabs1.Tabs.Count
    ' Removing the last tab leaves nothing selected.
    Tabs1.Tabs.Remove "b"
    Debug.Print "remove last", Chosen(), Tabs1.Tabs.Count
    ' Clear leaves nothing selected; the next Add selects again.
    Tabs1.Tabs.Add , "x", "X"
    Tabs1.Tabs.Add , "y", "Y"
    Tabs1.Tabs.Clear
    Debug.Print "clear", Chosen(), Tabs1.Tabs.Count
    Tabs1.Tabs.Add , "z", "Z"
    Debug.Print "after clear", Chosen()
    ' Removing the last of several selects the previous one.
    Tabs1.Tabs.Add , "w", "W"
    Tabs1.Tabs("w").Selected = True
    Tabs1.Tabs.Remove "w"
    Debug.Print "remove end", Chosen()

    ' A status bar holds at most sixteen panels.
    On Error Resume Next
    For i = 1 To 17
        Status1.Panels.Add , "p" & i, "P" & i
        If Err.Number <> 0 Then
            Debug.Print "panel", i, Err.Number, Err.Description
            Err.Clear
        End If
    Next i
    Debug.Print "panels", Status1.Panels.Count
    On Error GoTo 0

    ' Images are inserted anywhere until the ImageList is bound to a control,
    ' and only appended after that.
    Images1.ListImages.Add , "one", LoadPicture(App.Path & "\222-comctl-selection.red.bmp")
    Images1.ListImages.Add 1, "front", LoadPicture(App.Path & "\222-comctl-selection.red.bmp")
    Debug.Print "images", Images1.ListImages.Count, Images1.ListImages(1).Key
    Tabs1.ImageList = Images1
    On Error Resume Next
    Images1.ListImages.Add 1, "late", LoadPicture(App.Path & "\222-comctl-selection.red.bmp")
    Debug.Print "bound insert", Err.Number, Err.Description
    Err.Clear
    Images1.ListImages.Add , "end", LoadPicture(App.Path & "\222-comctl-selection.red.bmp")
    Debug.Print "bound append", Err.Number, Images1.ListImages.Count
    On Error GoTo 0
End Sub
