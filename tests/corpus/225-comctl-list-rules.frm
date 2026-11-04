VERSION 5.00
Begin VB.Form Form1
   Caption         =   "List rules"
   Begin MSComctlLib.ListView List1
      Height          =   1200
      Left            =   0
      Top             =   0
      Width           =   3000
      View            =   3
      MultiSelect     =   -1  'True
   End
   Begin MSComctlLib.ListView List2
      Height          =   1200
      Left            =   0
      Top             =   1300
      Width           =   3000
      View            =   0
      Checkboxes      =   0   'False
      LabelEdit       =   1
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Function KeyOf(i As ListItem) As String
    If i Is Nothing Then
        KeyOf = "-"
    Else
        KeyOf = i.Key
    End If
End Function

Private Sub Order(ByVal tag As String, l As ListView)
    Dim i As Long
    Dim s As String
    For i = 1 To l.ListItems.Count
        s = s & l.ListItems(i).Key & " "
    Next i
    Debug.Print tag; ": "; s
End Sub

Private Sub Sel(ByVal tag As String, l As ListView)
    Dim i As Long
    Dim s As String
    For i = 1 To l.ListItems.Count
        s = s & l.ListItems(i).Key & "=" & l.ListItems(i).Selected & " "
    Next i
    Debug.Print tag; ": "; s; "sel="; KeyOf(l.SelectedItem)
End Sub

Private Sub Form_Load()
    Dim li As ListItem
    List1.ColumnHeaders.Add , "c1", "One", 1000
    List1.ColumnHeaders.Add , "c2", "Two", 700
    Set li = List1.ListItems.Add(, "k1", "bravo")
    li.SubItems(1) = "20"
    Set li = List1.ListItems.Add(, "k2", "Alpha")
    li.SubItems(1) = "3"
    Set li = List1.ListItems.Add(, "k3", "ALPHA")
    li.SubItems(1) = "100"
    Set li = List1.ListItems.Add(, "k4", "charlie")
    li.Tag = "tagged"
    Set li = List1.ListItems.Add(2, "k5", "delta")
    Order "added", List1

    ' Sorting: folded case, stable, descending, by a sub item, past the end.
    List1.Sorted = True
    Order "sorted", List1
    List1.SortOrder = lvwDescending
    Order "descending", List1
    List1.SortOrder = lvwAscending
    List1.SortKey = 1
    Order "key 1", List1
    List1.SortKey = 5
    Order "key past", List1
    List1.SortKey = 0
    Order "key 0", List1

    ' SubItems grow on write; negative or non-numeric indexes fail.
    List1.ListItems("k4").SubItems(3) = "far"
    Debug.Print List1.ListItems("k4").ListSubItems.Count; "["; List1.ListItems("k4").SubItems(2); "]"; List1.ListItems("k4").SubItems(3); List1.ListItems("k4").SubItems(0)
    On Error Resume Next
    List1.ListItems("k4").SubItems(-1) = "x"
    Debug.Print "neg put"; Err.Number
    Err.Clear
    Debug.Print List1.ListItems("k4").SubItems(4)
    Debug.Print "get past"; Err.Number
    Err.Clear
    Debug.Print List1.ListItems("k4").SubItems("a")
    Debug.Print "get text"; Err.Number
    Err.Clear
    List1.View = -1
    Debug.Print "view -1"; Err.Number
    Err.Clear
    List1.View = 4
    Debug.Print "view 4"; Err.Number
    Err.Clear
    List1.Left = 5
    List1.ListItems(1).Top = 5
    Debug.Print "readonly"; Err.Number
    Err.Clear
    Set List1.SelectedItem = Me
    Debug.Print "select non item"; Err.Number
    Err.Clear
    Set List1.SelectedItem = List2.ListItems.Add(, "other", "x")
    Debug.Print "select other"; Err.Number
    Err.Clear
    List1.ListItems.Add , , "no key"
    Debug.Print "no key"; Err.Number; List1.ListItems.Count
    Err.Clear
    On Error GoTo 0

    ' Layout of every view, with and without column headers.
    Dim v As Long
    For v = 3 To 0 Step -1
        List1.View = v
        Debug.Print "view"; v; List1.ListItems(1).Left; List1.ListItems(1).Top; List1.ListItems(1).Width; List1.ListItems(1).Height; List1.ListItems(4).Left; List1.ListItems(4).Top
    Next v
    List1.View = 3
    List1.HideColumnHeaders = True
    Debug.Print "no header"; List1.ListItems(1).Top; List1.ListItems(2).Top
    List1.HideColumnHeaders = False

    ' HitTest in report view.
    Dim y As Long
    For y = 0 To 1500 Step 250
        Debug.Print "hit"; y; KeyOf(List1.HitTest(10, y + 5))
    Next y
    Debug.Print "hit x"; KeyOf(List1.HitTest(1699, 300)); KeyOf(List1.HitTest(1701, 300))

    ' FindItem: text, sub items, tag, start item, partial.
    Debug.Print "find"; KeyOf(List1.FindItem("ALPHA")); KeyOf(List1.FindItem("alpha", lvwText, "k3")); KeyOf(List1.FindItem("al", , , lvwPartial)); KeyOf(List1.FindItem("a", , , lvwPartial)); KeyOf(List1.FindItem("tagged", lvwTag)); KeyOf(List1.FindItem("100", lvwSubItem)); KeyOf(List1.FindItem("1", lvwSubItem, , lvwPartial)); KeyOf(List1.FindItem(""))
    On Error Resume Next
    Debug.Print KeyOf(List1.FindItem("x", lvwText, "nope"))
    Debug.Print "find start"; Err.Number
    Err.Clear
    Debug.Print KeyOf(List1.FindItem())
    Debug.Print "find args"; Err.Number
    Err.Clear
    On Error GoTo 0

    ' Selection with MultiSelect.
    List1.ListItems("k1").Selected = True
    List1.ListItems("k4").Selected = True
    Sel "multi", List1
    List1.ListItems("k1").Selected = False
    Sel "off", List1
    Set List1.SelectedItem = List1.ListItems("k2")
    Sel "set", List1
    Set List1.SelectedItem = Nothing
    Sel "nothing", List1

    ' The single selection list.
    List2.ListItems.Add , "x", "X"
    List2.ListItems.Add , "y", "Y"
    List2.ListItems("x").Selected = True
    List2.ListItems("y").Selected = True
    Sel "single", List2
    Debug.Print "first visible"; KeyOf(List2.GetFirstVisible); List2.ListItems("y").EnsureVisible; (List2.ListItems("y").CreateDragImage Is Nothing)
    Order "list2", List2
End Sub

Private Sub List1_ItemClick(ByVal Item As MSComctlLib.ListItem)
    Debug.Print "itemclick1 "; Item.Key
End Sub
Private Sub List1_ItemCheck(ByVal Item As MSComctlLib.ListItem)
    Debug.Print "itemcheck1 "; Item.Key; Item.Checked
End Sub
Private Sub List1_Click()
    Debug.Print "click1"
End Sub
Private Sub List1_DblClick()
    Debug.Print "dblclick1"
End Sub
Private Sub List1_ColumnClick(ByVal ColumnHeader As MSComctlLib.ColumnHeader)
    Debug.Print "column1 "; ColumnHeader.Key
End Sub
Private Sub List1_BeforeLabelEdit(Cancel As Integer)
    Debug.Print "before1 "; List1.SelectedItem.Key
    If List1.SelectedItem.Key = "k4" Then Cancel = True
End Sub
Private Sub List1_AfterLabelEdit(Cancel As Integer, NewString As String)
    Debug.Print "after1 ["; NewString; "]"
    If NewString = "bad" Then Cancel = True
End Sub
Private Sub List2_ItemClick(ByVal Item As MSComctlLib.ListItem)
    Debug.Print "itemclick2 "; Item.Key
End Sub
Private Sub List2_BeforeLabelEdit(Cancel As Integer)
    Debug.Print "before2"
End Sub
Private Sub List2_AfterLabelEdit(Cancel As Integer, NewString As String)
    Debug.Print "after2 ["; NewString; "]"
End Sub
