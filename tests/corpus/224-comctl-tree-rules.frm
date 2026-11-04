VERSION 5.00
Begin VB.Form Form1
   Caption         =   "Tree rules"
   Begin MSComctlLib.TreeView Tree1
      Height          =   1000
      Left            =   0
      Top             =   0
      Width           =   3000
      Checkboxes      =   -1  'True
   End
   Begin MSComctlLib.TreeView Tree2
      Height          =   2400
      Left            =   3200
      Top             =   0
      Width           =   3000
      SingleSel       =   -1  'True
      LabelEdit       =   1
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Function KeyOf(n As Node) As String
    If n Is Nothing Then
        KeyOf = "-"
    Else
        KeyOf = n.Key
    End If
End Function

Private Function Walk(ByVal n As Node, ByVal depth As Long) As String
    Dim c As Node
    Walk = String(depth, "-") & n.Key & "(" & n.Text & ") "
    Set c = n.Child
    Do While Not c Is Nothing
        Walk = Walk & Walk(c, depth + 1)
        Set c = c.Next
    Loop
End Function

Private Sub Dump(ByVal tag As String, t As TreeView)
    Dim n As Node
    Dim s As String
    Set n = t.Nodes(1)
    Set n = n.FirstSibling
    Do While Not n Is Nothing
        s = s & Walk(n, 0)
        Set n = n.Next
    Loop
    Debug.Print tag; ": "; s; "sel="; KeyOf(t.SelectedItem)
End Sub

Private Sub Form_Load()
    Dim r As Node, a As Node, b As Node, k As Node
    Set r = Tree1.Nodes.Add(, , "r", "Root")
    Set a = Tree1.Nodes.Add("r", tvwChild, "a", "Zulu")
    Set b = Tree1.Nodes.Add(2, tvwNext, "b", "alpha")
    Tree1.Nodes.Add "a", tvwChild, "a1", "Kid"
    Tree1.Nodes.Add "a1", tvwChild, "a11", "Grandkid"
    Tree1.Nodes.Add r, tvwNext, "r2", "Second root"
    Tree1.Nodes.Add "r2", tvwPrevious, "r0", "Zeroth root"
    Dump "built", Tree1

    ' Relationships out of range, or not numbers, and relatives not found.
    On Error Resume Next
    Tree1.Nodes.Add "r", 5, "e1", "x"
    Debug.Print "rel 5"; Err.Number; Err.Description
    Err.Clear
    Tree1.Nodes.Add "r", -1, "e2", "x"
    Debug.Print "rel -1"; Err.Number
    Err.Clear
    Tree1.Nodes.Add "r", "abc", "e3", "x"
    Debug.Print "rel abc"; Err.Number
    Err.Clear
    Tree1.Nodes.Add 99, tvwChild, "e4", "x"
    Debug.Print "relative 99"; Err.Number; Err.Description
    Err.Clear
    Tree1.Nodes.Add "r", tvwChild, 5, "x"
    Debug.Print "key 5"; Err.Number
    Err.Clear
    Tree1.Nodes.Add , , , "no key"
    Debug.Print "no key"; Err.Number; Tree1.Nodes.Count
    Err.Clear
    Tree1.Nodes.Add , tvwChild, "e5", "relationship without relative"
    Debug.Print "rel no relative"; Err.Number; Tree1.Nodes(Tree1.Nodes.Count).Parent Is Nothing
    Err.Clear
    Tree1.Nodes("r").FullPath = "x"
    Debug.Print "readonly"; Err.Number
    Err.Clear
    Tree1.Nodes("r").Children = 3
    Debug.Print "readonly children"; Err.Number
    Err.Clear
    On Error GoTo 0

    ' Paths with a separator, and EnsureVisible.
    Tree1.PathSeparator = " > "
    Debug.Print Tree1.Nodes("a11").FullPath
    Debug.Print Tree1.Nodes("a11").EnsureVisible; Tree1.Nodes("a").Expanded; Tree1.Nodes("r").Expanded
    Debug.Print Tree1.Nodes("a11").EnsureVisible
    Tree1.Scroll = False
    Tree1.Nodes("a").Expanded = False
    Debug.Print Tree1.Nodes("a11").EnsureVisible; Tree1.Nodes("a").Expanded
    Tree1.Scroll = True

    ' Sorting: case folded, stable, per node and for the whole tree.
    Tree1.Nodes("r").Sorted = True
    Dump "node sorted", Tree1
    Tree1.Nodes.Add "r", tvwChild, "m", "Mike"
    Dump "sorted add", Tree1
    Tree1.Sorted = True
    Dump "tree sorted", Tree1

    ' Visible rows: collapsed nodes hide their children.
    Tree1.Nodes("a").Expanded = True
    Debug.Print "visible"; Tree1.GetVisibleCount
    Tree1.Nodes("a").Expanded = False
    Debug.Print "visible collapsed"; Tree1.GetVisibleCount

    ' Hit test: row 240 twips high, indented by depth.
    Tree1.Nodes("a").Expanded = True
    Tree1.Nodes("a1").Expanded = True
    Dim y As Long
    For y = 0 To 1200 Step 240
        Debug.Print "hit"; y; KeyOf(Tree1.HitTest(700, y + 10)); KeyOf(Tree1.HitTest(100, y + 10))
    Next y
    Tree1.Indentation = 500
    Debug.Print "indent"; KeyOf(Tree1.HitTest(400, 10)); KeyOf(Tree1.HitTest(600, 10))
    On Error Resume Next
    Debug.Print Tree1.HitTest(1)
    Debug.Print "hit args"; Err.Number
    Err.Clear
    On Error GoTo 0

    ' Selection, and what removing the selected node and its children does.
    Set Tree1.SelectedItem = Tree1.Nodes("a11")
    Tree1.Nodes.Remove "a"
    Dump "removed", Tree1
    Debug.Print Tree1.Nodes.Count

    ' The second tree: SingleSel, and LabelEdit manual.
    Tree2.Nodes.Add , , "p", "P"
    Tree2.Nodes.Add "p", tvwChild, "p1", "P1"
    Tree2.Nodes.Add , , "q", "Q"
    Tree2.Nodes.Add "q", tvwChild, "q1", "Q1"
    Tree2.Nodes.Add "q1", tvwChild, "q11", "Q11"
    Dump "tree2", Tree2
End Sub

Private Sub Tree1_Expand(ByVal Node As MSComctlLib.Node)
    Debug.Print "expand1 "; Node.Key
End Sub
Private Sub Tree1_Collapse(ByVal Node As MSComctlLib.Node)
    Debug.Print "collapse1 "; Node.Key
End Sub
Private Sub Tree1_NodeClick(ByVal Node As MSComctlLib.Node)
    Debug.Print "nodeclick1 "; Node.Key
End Sub
Private Sub Tree1_NodeCheck(ByVal Node As MSComctlLib.Node)
    Debug.Print "check1 "; Node.Key; Node.Checked
End Sub
Private Sub Tree1_BeforeLabelEdit(Cancel As Integer)
    Debug.Print "before1 "; Tree1.SelectedItem.Key
    If Tree1.SelectedItem.Key = "r0" Then Cancel = True
End Sub
Private Sub Tree1_AfterLabelEdit(Cancel As Integer, NewString As String)
    Debug.Print "after1 ["; NewString; "]"
    If NewString = "bad" Then Cancel = True
End Sub
Private Sub Tree2_Expand(ByVal Node As MSComctlLib.Node)
    Debug.Print "expand2 "; Node.Key
End Sub
Private Sub Tree2_Collapse(ByVal Node As MSComctlLib.Node)
    Debug.Print "collapse2 "; Node.Key
End Sub
Private Sub Tree2_NodeClick(ByVal Node As MSComctlLib.Node)
    Debug.Print "nodeclick2 "; Node.Key
End Sub
Private Sub Tree2_BeforeLabelEdit(Cancel As Integer)
    Debug.Print "before2"
End Sub
