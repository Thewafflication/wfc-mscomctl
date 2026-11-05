VERSION 5.00
Begin VB.Form Form1
   Caption         =   "Image rules"
   Begin MSComctlLib.TabStrip Tabs1
      Height          =   1000
      Left            =   0
      Top             =   0
      Width           =   3000
   End
   Begin MSComctlLib.ImageList Images1
      Left            =   3200
      Top             =   0
   End
   Begin MSComctlLib.ImageList Images2
      Left            =   3200
      Top             =   400
   End
   Begin MSComctlLib.ImageCombo Combo1
      Height          =   300
      Left            =   0
      Top             =   1100
      Width           =   2400
   End
   Begin MSComctlLib.ImageCombo Combo2
      Height          =   300
      Left            =   0
      Top             =   1500
      Width           =   2400
   End
End
Attribute VB_Name = "Form1"
Option Explicit

Private Sub Try(ByVal tag As String)
    Debug.Print tag; " err="; Err.Number
    Err.Clear
End Sub

Private Function KeyOf(c As ComboItem) As String
    If c Is Nothing Then
        KeyOf = "-"
    Else
        KeyOf = c.Key
    End If
End Function

Private Sub Form_Load()
    Dim p As String
    Dim o As Variant
    p = App.Path & "\226-comctl-image-rules.red.bmp"
    On Error Resume Next
    Images1.ListImages.Add , "a", LoadPicture(p)
    Try "add a"
    Debug.Print "size", Images1.ImageWidth, Images1.ImageHeight
    Images1.ListImages.Add , "b", LoadPicture(p)
    Images1.ListImages.Add , "c", LoadPicture(p)
    Debug.Print "count", Images1.ListImages.Count
    Images1.ListImages.Add , "d"
    Try "no picture"
    Images1.ListImages.Add , "A", LoadPicture(p)
    Try "dup key"
    Images1.ListImages.Add 0, "z", LoadPicture(p)
    Try "index 0"
    Images1.ListImages.Add 9, "z", LoadPicture(p)
    Try "index 9"
    Images1.ListImages.Add "x", "z", LoadPicture(p)
    Try "index text"
    Images1.ListImages.Add 4, "e", LoadPicture(p)
    Try "index end"
    Debug.Print "count", Images1.ListImages.Count, Images1.ListImages(4).Key
    Images1.ImageWidth = 32
    Try "width after add"
    Images2.ImageWidth = 32
    Try "width before add"
    Debug.Print "size2", Images2.ImageWidth, Images2.ImageHeight
    Images2.ListImages.Add , "one", LoadPicture(p)
    Debug.Print "size2", Images2.ImageWidth, Images2.ImageHeight

    o = Images1.Overlay("a", "b")
    Try "overlay keys"
    Debug.Print TypeName(o)
    o = Images1.Overlay(1, 2)
    Try "overlay index"
    Debug.Print TypeName(o)
    o = Images1.Overlay("a")
    Try "overlay one"
    o = Images1.Overlay("a", "nope")
    Try "overlay missing"
    o = Images1.Overlay(0, 1)
    Try "overlay zero"
    o = Images1.Overlay(1, 99)
    Try "overlay far"

    Debug.Print TypeName(Images1.ListImages("c").ExtractIcon)
    Try "extract"
    Debug.Print Images1.ListImages("c").Index, Images1.ListImages("C").Index
    Try "key case"
    Debug.Print Images1.ListImages(0).Key
    Try "item 0"
    Debug.Print Images1.ListImages("").Key
    Try "item empty key"

    Images2.ListImages.Remove "one"
    Try "unbound remove"
    Debug.Print Images2.ListImages.Count
    Images2.ListImages.Add , "one", LoadPicture(p)
    Images2.ListImages.Clear
    Try "unbound clear"
    Debug.Print Images2.ListImages.Count

    Tabs1.ImageList = Images1
    Try "bind"
    Images1.ListImages.Remove "a"
    Try "bound remove"
    Images1.ListImages.Clear
    Try "bound clear"
    Images1.ListImages.Add 1, "front", LoadPicture(p)
    Try "bound front"
    Images1.ListImages.Add 3, "mid", LoadPicture(p)
    Try "bound middle"
    Images1.ListImages.Add 5, "tail", LoadPicture(p)
    Try "bound append"
    Images1.ListImages.Add , "tail2", LoadPicture(p)
    Try "bound append no index"
    Debug.Print "count", Images1.ListImages.Count
    Debug.Print Images1.hImageList <> 0
    Debug.Print Images2.hImageList <> 0

    Tabs1.Tabs.Add , "t1", "One", 1
    Try "tab image index"
    Tabs1.Tabs.Add , "t2", "Two", "b"
    Try "tab image key"
    Debug.Print Tabs1.Tabs(1).Image; Tabs1.Tabs(2).Image
    Try "tab image print"

    Dim ci As ComboItem
    Set ci = Combo1.ComboItems.Add(, "r", "Red")
    Combo1.ComboItems.Add , "g", "green"
    Combo1.ComboItems.Add , "g2", "GREEN"
    Combo1.ComboItems.Add , "b", "Blue", , , 3
    Debug.Print "indent", Combo1.ComboItems("b").Indentation, ci.Indentation
    Combo1.ComboItems("r").Indentation = 2
    Try "set indentation"
    Combo1.ComboItems("r").Indentation = -1
    Try "negative indentation"
    Debug.Print Combo1.ComboItems("r").Indentation
    Combo1.ComboItems("r").Image = 7
    Try "image number"
    Debug.Print Combo1.ComboItems("r").Image, Combo1.ComboItems("r").SelImage

    Combo1.Text = "Green"
    Debug.Print "match", KeyOf(Combo1.SelectedItem)
    Combo1.Text = "BLUE"
    Debug.Print "match", KeyOf(Combo1.SelectedItem)
    Combo1.Text = "Blu"
    Debug.Print "match", KeyOf(Combo1.SelectedItem), "["; Combo1.Text; "]"
    Combo1.Text = ""
    Debug.Print "match", KeyOf(Combo1.SelectedItem), "["; Combo1.Text; "]"

    Combo2.ComboItems.Add , "x", "Xray"
    Set Combo1.SelectedItem = Combo2.ComboItems("x")
    Try "foreign item"
    Debug.Print KeyOf(Combo1.SelectedItem)
    Set Combo1.SelectedItem = Combo1.ComboItems("g")
    Debug.Print KeyOf(Combo1.SelectedItem), Combo1.Text
    Set Combo1.SelectedItem = Nothing
    Try "nothing"
    Debug.Print KeyOf(Combo1.SelectedItem), "["; Combo1.Text; "]"
    Combo1.SelectedItem = 3
    Try "number"

    Combo1.Text = "abcdef"
    Combo1.SelStart = 9
    Combo1.SelLength = 4
    Debug.Print "[" & Combo1.SelText & "]"
    Combo1.SelStart = 4
    Combo1.SelLength = 9
    Debug.Print "[" & Combo1.SelText & "]"
    Combo1.SelStart = -2
    Try "negative start"
    Combo1.SelLength = -2
    Try "negative length"
    Debug.Print "[" & Combo1.SelText & "]", Combo1.SelStart, Combo1.SelLength
    Combo1.SelStart = 2
    Combo1.SelLength = 2
    Combo1.SelText = "-"
    Debug.Print Combo1.Text, Combo1.SelStart, Combo1.SelLength
    Combo1.SelStart = 20
    Combo1.SelLength = 0
    Combo1.SelText = "!"
    Debug.Print Combo1.Text, Combo1.SelStart, Combo1.SelLength

    Combo1.ComboItems.Add 9, "q", "Q"
    Try "combo index 9"
    Combo1.ComboItems.Add 0, "q", "Q"
    Try "combo index 0"
    Combo1.ComboItems.Add , "r", "dup"
    Try "combo dup key"
    Combo1.ComboItems.Add 1, "first", "First"
    Try "combo insert first"
    Debug.Print Combo1.ComboItems(1).Key, Combo1.GetFirstVisible.Key, Combo1.ComboItems.Count
    Combo1.ComboItems.Remove 0
    Try "combo remove 0"
    Combo1.ComboItems.Remove "nope"
    Try "combo remove missing"
    Combo1.ComboItems.Clear
    Debug.Print Combo1.ComboItems.Count, KeyOf(Combo1.GetFirstVisible)
    Try "combo clear"
End Sub
