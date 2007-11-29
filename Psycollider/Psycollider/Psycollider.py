# File: Psycollider.py
# Project: Psycollider
# 
# by:
# Benjamin Golinvaux
# benjamin.golinvaux@euresys.com
# messenger: bgolinvaux@hotmail.com
# 
# currently maintained by:
# Christopher Frauenberger 
# frauenberger@iem.at
# 
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA
# 
# ---------------------------------------------------------------------


import wx
import wx.stc as stc
import wx.html as html
import wx.richtext as richtext
import PySCLang
import os, string, keyword, sys

if wx.Platform == '__WXMSW__':
  faces = { 'times': 'Times New Roman', 'mono' : 'Courier New', 'helv' : 'Arial', 'other': 'Comic Sans MS', 'size' : 10, 'size2': 8, }
  gAppHelpFolder = 'help_windows'
else:
  faces = { 'times': 'Times', 'mono' : 'Courier', 'helv' : 'Helvetica', 'other': 'new century schoolbook', 'size' : 10, 'size2': 8, }
  gAppHelpFolder = 'Help-windows'
  
gHelpFolder = 'Help'
gUserExtensionFolder = '~\\SuperCollider\\Extensions'

#----------------------------------------------------------------------
# wx IDs
ID_NewCodeWin  = wx.NewId()
ID_HTMLtoCode  = wx.NewId()
ID_StartServer = wx.NewId()
ID_StopServer = wx.NewId()
ID_OpenFile = wx.NewId()
ID_SaveFile = wx.NewId()
ID_SaveFileAs = wx.NewId()
ID_CloseCodeWin = wx.NewId() 

ID_Exit = wx.NewId()

ID_EvalSelection = wx.NewId()

ID_Lang_Stop_Server = wx.NewId()
ID_Lang_Run = wx.NewId()
ID_Lang_Stop = wx.NewId()
ID_Lang_CompileLibrary = wx.NewId()
ID_Lang_OpenClassDef = wx.NewId()
ID_Lang_ImplementationsOf = wx.NewId()
ID_Lang_ReferencesTo = wx.NewId()
ID_Lang_ClearLogWindow = wx.NewId()

ID_Go_To_Help_File = wx.NewId()
#----------------------------------------------------------------------

# set SC3_KEYWORDS as global vary.
try:
  file = open("keywords.list","r")
  SC3_KEYWORDS = string.split( file.read() )
  file.close()
except IOError:
  SC3_KEYWORDS = [ "var", "arg", "Server" ]
  print "warning:"
  print "SC3-keywords definition file \"keywords.list\" was not found."
  print "so now, these following words are the KEYWORDS for the meantime."
  print SC3_KEYWORDS

SC3_KEYWORDS.sort()

# ---------------------------------------------------------------------
# HTML Sub Window 
# 
class PsycolliderHTMLSubWin(wx.html.HtmlWindow):

  def __init__ (self,parent):
    wx.html.HtmlWindow.__init__(self,parent)

    
# ---------------------------------------------------------------------
# RTF Sub Window 
# Issues: not editable, does not always (python versions) render rtf correctly
class PsycolliderRTFSubWin(wx.richtext.RichTextCtrl):

  def __init__ (self,parent):
    wx.richtext.RichTextCtrl.__init__(self,parent)
    self.Bind(wx.EVT_CHAR, self.OnChar)

  def OnChar(self,event):
    self.GetParent().GetParent().OnChildCharHook(event)

# ---------------------------------------------------------------------
# Code Sub Window - plain text window for code
# Issues: 
class PsycolliderCodeSubWin(wx.stc.StyledTextCtrl):

  fold_symbols = 3

  def __init__ (self,parent):
    stc.StyledTextCtrl.__init__(self,parent)
    font = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Courier New")
    self.StyleSetFont(wx.stc.STC_STYLE_DEFAULT, font)
    #self.SetModEventMask(wx.stc.STC_MOD_INSERTTEXT | wx.stc.STC_MOD_DELETETEXT | wx.stc.STC_PERFORMED_USER)
    # bindings
    self.Bind(stc.EVT_STC_CHANGE, self.OnStcChange)
    self.Bind(stc.EVT_STC_UPDATEUI, self.OnUpdateUI)
    self.Bind(stc.EVT_STC_MARGINCLICK, self.OnMarginClick)
    self.Bind(wx.EVT_KEY_DOWN, self.OnKeyPressed)
    self.Bind(wx.EVT_CHAR, self.OnChar)
    
    self.CmdKeyAssign(ord('+'), stc.STC_SCMOD_CTRL, stc.STC_CMD_ZOOMIN)
    self.CmdKeyAssign(ord('-'), stc.STC_SCMOD_CTRL, stc.STC_CMD_ZOOMOUT)

    self.SetLexer(wx.stc.STC_LEX_CPP)             # yssr
    self.SetKeyWords(0, " ".join(SC3_KEYWORDS))   # yssr

    self.SetProperty("fold", "1")
    self.SetProperty("tab.timmy.whinge.level", "1")
    self.SetMargins(1,0) # yssr

    # some settings for appearance
    self.SetViewWhiteSpace(False)
    self.SetViewEOL(False)
    self.SetUseAntiAliasing(True)
    self.SetWrapMode(True)
    self.SetEdgeMode(stc.STC_EDGE_NONE)

    # Setup a margin to hold fold markers
    self.SetMarginType(2, stc.STC_MARGIN_SYMBOL)
    self.SetMarginMask(2, stc.STC_MASK_FOLDERS)
    self.SetMarginSensitive(2, True)
    self.SetMarginWidth(2, 12)

    # Like a flattened tree control using square headers
    self.MarkerDefine(stc.STC_MARKNUM_FOLDEROPEN,    stc.STC_MARK_BOXMINUS,          "white", "#808080")
    self.MarkerDefine(stc.STC_MARKNUM_FOLDER,        stc.STC_MARK_BOXPLUS,           "white", "#808080")
    self.MarkerDefine(stc.STC_MARKNUM_FOLDERSUB,     stc.STC_MARK_VLINE,             "white", "#808080")
    self.MarkerDefine(stc.STC_MARKNUM_FOLDERTAIL,    stc.STC_MARK_LCORNER,           "white", "#808080")
    self.MarkerDefine(stc.STC_MARKNUM_FOLDEREND,     stc.STC_MARK_BOXPLUSCONNECTED,  "white", "#808080")
    self.MarkerDefine(stc.STC_MARKNUM_FOLDEROPENMID, stc.STC_MARK_BOXMINUSCONNECTED, "white", "#808080")
    self.MarkerDefine(stc.STC_MARKNUM_FOLDERMIDTAIL, stc.STC_MARK_TCORNER,           "white", "#808080")

    # Make some styles,  The lexer defines what each style is used for, we
    # just have to define what each style looks like.  This set is adapted from
    # Scintilla sample property files.

    # Global default styles for all languages
    self.StyleSetSpec(stc.STC_STYLE_DEFAULT,     "face:%(mono)s,size:%(size)d" % faces)
    self.StyleClearAll()  # Reset all to be like the default

    # Global default styles for all languages
    self.StyleSetSpec(stc.STC_STYLE_DEFAULT,     "face:%(mono)s,size:%(size)d" % faces)
    #self.StyleSetSpec(stc.STC_STYLE_LINENUMBER,  "back:#C0C0C0,face:%(helv)s,size:%(size2)d" % faces)
    self.StyleSetSpec(stc.STC_STYLE_CONTROLCHAR, "face:%(other)s" % faces)
    self.StyleSetSpec(stc.STC_STYLE_BRACELIGHT,  "fore:#FFFFFF,back:#00FFFF,bold")
    self.StyleSetSpec(stc.STC_STYLE_BRACEBAD,    "fore:#000000,back:#FF3333,bold")

    # C++ styles
    # Default 
    self.StyleSetSpec(stc.STC_C_DEFAULT, "fore:#000000,face:%(mono)s,size:%(size)d" % faces)
    # Comments
    self.StyleSetSpec(stc.STC_C_COMMENTLINE, "fore:#007F00,face:%(mono)s,size:%(size)d" % faces)
    # Number
    self.StyleSetSpec(stc.STC_C_NUMBER, "bold,fore:#333333,size:%(size)d" % faces)
    # String
    self.StyleSetSpec(stc.STC_C_STRING, "italic,fore:#CC0000,face:%(mono)s,size:%(size)d" % faces)
    # Single quoted string
    self.StyleSetSpec(stc.STC_C_CHARACTER, "fore:#CC0000,face:%(mono)s,size:%(size)d" % faces)
    # Keyword
    self.StyleSetSpec(stc.STC_C_WORD, "fore:#00007F,bold,size:%(size)d" % faces)
    # Operators
    # self.StyleSetSpec(stc.STC_C_OPERATOR, "bold,size:%(size)d" % faces)
    # Identifiers
    self.StyleSetSpec(stc.STC_C_IDENTIFIER, "fore:#000000,face:%(mono)s,size:%(size)d" % faces)
    # End of line where string is not closed
    self.StyleSetSpec(stc.STC_C_STRINGEOL, "fore:#000000,face:%(mono)s,back:#E0C0E0,eol,size:%(size)d" % faces)
    #self.StyleSetSpec(stc.STC_C_COMMENTDOC, "fore:#7F7F7F,size:%(size)d" % faces)
    #self.StyleSetSpec(stc.STC_C_COMMENTLINEDOC,  "fore:#7F7F7F,size:%(size)d" % faces)
    #self.StyleSetSpec(stc.STC_C_GLOBALCLASS, "fore:#7F7F7F,size:%(size)d" % faces)
    self.SetCaretForeground("BLACK")
    #wx.MessageBox("OnStcChange")
    #GetParent().setModified

  def OnChar(self,event):
    self.GetParent().GetParent().OnChildCharHook(event)

  def OnStcChange(self, event):
    self.GetParent().OnStcChange(event)
    
  def OnKeyPressed(self, event):
#    if self.CallTipActive():
#      self.CallTipCancel()
    key = event.GetKeyCode()
    
    if key == 32 and event.ControlDown():
      pos = self.GetCurrentPos()

      # Tips
      if event.ShiftDown():
        self.CallTipSetBackground("yellow")
        self.CallTipShow(pos, 'we can display tips here')
      # Code completion
      else:
        #lst = []
        #for x in range(50000):
        #  lst.append('%05d' % x)
        #st = " ".join(lst)
        #print len(st)
        #self.AutoCompShow(0, st)

        kw = keyword.kwlist[:]
        #kw.append("zzzzzz?2")

        kw.sort()  # Python sorts are case sensitive
        self.AutoCompSetIgnoreCase(False)  # so this needs to match

        # Images are specified with a appended "?type"
        for i in range(len(kw)):
          if kw[i] in keyword.kwlist:
            kw[i] = kw[i] + "?1"
        self.AutoCompShow(0, " ".join(kw))
    else:
      event.Skip()


  def OnUpdateUI(self, evt):
    # check for matching braces
    
    braceAtCaret = -1
    braceOpposite = -1
    charBefore = None
    caretPos = self.GetCurrentPos()

    if caretPos > 0:
      charBefore = self.GetCharAt(caretPos - 1)
      styleBefore = self.GetStyleAt(caretPos - 1)

    # check before
    if charBefore and chr(charBefore) in "[]{}()" and styleBefore == stc.STC_C_OPERATOR:
      braceAtCaret = caretPos - 1

    # check after
    if braceAtCaret < 0:
      charAfter = self.GetCharAt(caretPos)
      styleAfter = self.GetStyleAt(caretPos)

      if charAfter and chr(charAfter) in "[]{}()" and styleAfter == stc.STC_C_OPERATOR:
        braceAtCaret = caretPos

    if braceAtCaret >= 0:
      braceOpposite = self.BraceMatch(braceAtCaret)

    if braceAtCaret != -1  and braceOpposite == -1:
      self.BraceBadLight(braceAtCaret)
    else:
      self.BraceHighlight(braceAtCaret, braceOpposite)
      #pt = self.PointFromPosition(braceOpposite)
      #self.Refresh(True, wxRect(pt.x, pt.y, 5,5))
      #print pt
      #self.Refresh(False)


  def OnMarginClick(self, evt):
    # fold and unfold as needed
    if evt.GetMargin() == 2:
      if evt.GetShift() and evt.GetControl():
        self.FoldAll()
      else:
        lineClicked = self.LineFromPosition(evt.GetPosition())

        if self.GetFoldLevel(lineClicked) & stc.STC_FOLDLEVELHEADERFLAG:
          if evt.GetShift():
            self.SetFoldExpanded(lineClicked, True)
            self.Expand(lineClicked, True, True, 1)
          elif evt.GetControl():
            if self.GetFoldExpanded(lineClicked):
              self.SetFoldExpanded(lineClicked, False)
              self.Expand(lineClicked, False, True, 0)
            else:
              self.SetFoldExpanded(lineClicked, True)
              self.Expand(lineClicked, True, True, 100)
          else:
            self.ToggleFold(lineClicked)


  def FoldAll(self):
    lineCount = self.GetLineCount()
    expanding = True

    # find out if we are folding or unfolding
    for lineNum in range(lineCount):
      if self.GetFoldLevel(lineNum) & stc.STC_FOLDLEVELHEADERFLAG:
        expanding = not self.GetFoldExpanded(lineNum)
        break;

    lineNum = 0

    while lineNum < lineCount:
      level = self.GetFoldLevel(lineNum)
      if level & stc.STC_FOLDLEVELHEADERFLAG and \
         (level & stc.STC_FOLDLEVELNUMBERMASK) == stc.STC_FOLDLEVELBASE:

        if expanding:
          self.SetFoldExpanded(lineNum, True)
          lineNum = self.Expand(lineNum, True)
          lineNum = lineNum - 1
        else:
          lastChild = self.GetLastChild(lineNum, -1)
          self.SetFoldExpanded(lineNum, False)

          if lastChild > lineNum:
            self.HideLines(lineNum+1, lastChild)

      lineNum = lineNum + 1


  def Expand(self, line, doExpand, force=False, visLevels=0, level=-1):
    lastChild = self.GetLastChild(line, level)
    line = line + 1

    while line <= lastChild:
      if force:
        if visLevels > 0:
          self.ShowLines(line, line)
        else:
          self.HideLines(line, line)
      else:
        if doExpand:
          self.ShowLines(line, line)

      if level == -1:
        level = self.GetFoldLevel(line)

      if level & stc.STC_FOLDLEVELHEADERFLAG:
        if force:
          if visLevels > 1:
            self.SetFoldExpanded(line, True)
          else:
            self.SetFoldExpanded(line, False)

          line = self.Expand(line, doExpand, force, visLevels-1)

        else:
          if doExpand and self.GetFoldExpanded(line):
            line = self.Expand(line, True, force, visLevels-1)
          else:
            line = self.Expand(line, False, force, visLevels-1)
      else:
        line = line + 1;

    return line
      
    # this is where unicode -> string conversion occurs...
    PySCLang.setCmdLine(str(sel))
    PySCLang.sendMain(methodName)


# ---------------------------------------------------------------------
# Code Window
# accomodates the sub code window
class PsycolliderCodeWin(wx.MDIChildFrame):

  def __init__ (self,parent,ID,title,pos=wx.DefaultPosition,size=wx.DefaultSize):
    wx.MDIChildFrame.__init__(self,parent,ID,title,pos,size)
    self.codeSubWin = PsycolliderCodeSubWin(self)
    self.isModified = 0
    self.originalFilePath = ""
    self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)  
    # TODO : have users able to directly type in the window when it's created

  def GetSelectedText(self):
    return self.codeSubWin.GetSelectedText()

  def GetCurLineAsString(self):
    return self.codeSubWin.GetCurLine()
    
  def OnStcChange(self, event):
    if not self.isModified:
      self.originalTitle = self.GetTitle()
      self.SetTitle(self.originalTitle + "*")
      self.isModified = 1

  def OnCloseWindow(self,event):
    if not self.GetParent().IsAllowedToCloseWindow(self):
      event.Veto()
    else:
      self.Destroy()

  def SetSubWinFocus(self):
    self.codeSubWin.SetFocus()

  def SelectRange(self,rangeStart,rangeSize):
    self.codeSubWin.SetSelection(rangeStart,rangeStart+rangeSize)
    
# ---------------------------------------------------------------------
# Log Window 
# the post window to which the PySCLang module posts
class PsycolliderLogWindow(wx.MDIChildFrame):

  def __init__ (self,parent,ID,title,pos=wx.DefaultPosition,size=wx.DefaultSize):
    wx.MDIChildFrame.__init__(self,parent,ID,title,pos,size)
    self.logSubWin = wx.TextCtrl(self,style = wx.TE_MULTILINE | wx.TE_READONLY )
    self.Bind(wx.EVT_CLOSE, self.OnClose)

  def OnClose(self, event):
    self.logSubWin.AppendText("Sorry, not closing the log window...\n")
  
# ---------------------------------------------------------------------
# RTF Window
# accomodates RTF sub window
class PsycolliderRTFWin(wx.MDIChildFrame):

  """ this class is not really able to display the SC help files properly """

  def __init__ (self,parent,ID,title,pos=wx.DefaultPosition,size=wx.DefaultSize):
    wx.MDIChildFrame.__init__(self,parent,ID,title,pos,size)
    self.rtfSubWin = PsycolliderRTFSubWin(self)

  def GetSelectedText(self):
    return self.rtfSubWin.GetStringSelection()

  def GetCurLineAsString(self):
    posInText = self.rtfSubWin.GetInsertionPoint()
    (x,y) = self.rtfSubWin.PositionToXY(posInText)
    return self.rtfSubWin.GetLineText(y)

  def SetSubWinFocus(self):
    self.rtfSubWin.SetFocus()
   
# ---------------------------------------------------------------------
# HTML Window
# accomodates HTML sub window
class PsycolliderHTMLWin(wx.MDIChildFrame):

  def __init__ (self,parent,ID,title,pos=wx.DefaultPosition,size=wx.DefaultSize):
    wx.MDIChildFrame.__init__(self,parent,ID,title,pos,size)
    self.htmlSubWin = PsycolliderHTMLSubWin(self)

  def GetSelectedText(self):
    return self.htmlSubWin.SelectionToText()

  def GetCurLineAsString(self):
    posInText = self.htmlSubWin.GetInsertionPoint()
    (x,y) = self.htmlSubWin.PositionToXY(posInText)
    return self.htmlSubWin.SelectLine(y)

  def SetSubWinFocus(self):
    self.htmlSubWin.SetFocus()

# ---------------------------------------------------------------------
# Main window
class PsycolliderMainFrame(wx.MDIParentFrame):

  def __init__ (self,parent,ID,title,pos=wx.DefaultPosition,size=wx.DefaultSize, style = wx.DEFAULT_FRAME_STYLE | wx.VSCROLL | wx.HSCROLL, name = "frame"):
    wx.MDIParentFrame.__init__(self,parent,ID,title,pos,size,style)
    self.winCount = 0

    # configuration 
    self.config  = wx.Config("Psycollider")

    # menu creation 
    menu = wx.Menu()
    menu.Append(ID_NewCodeWin, "&New Code Window...\tCtrl+N")
    menu.Append(ID_HTMLtoCode, "&HTML to Code Window...\tCtrl+T")
#    spacing seems unequal here, but comes out okay when compiled, any ideas??
#    menu.Append(ID_StartServer, "&Start Server")
#    menu.Append(ID_StopServer, "&Stop Server")
    menu.Append(ID_OpenFile, "&Open File...\tCtrl+O")
    menu.Append(ID_SaveFile, "&Save File...\tCtrl+S")
    menu.Append(ID_SaveFileAs, "Save File &As...\tCtrl+Shift+S")
    
    menu.AppendSeparator()
    
    menu.Append(ID_CloseCodeWin, "Close &Window...\tCtrl+W") # yssr

    menu.AppendSeparator()
    menu.Append(ID_Exit, "E&xit\tAlt+F4")

    # File History
    self.file_history = wx.FileHistory()
    self.file_history.UseMenu(menu)
    self.config.SetPath("/RecentFiles")
    self.file_history.Load(self.config)
    self.Bind(wx.EVT_MENU_RANGE, self.doFileHistory, id=wx.ID_FILE1, id2=wx.ID_FILE9)

    menuLang = wx.Menu()
    menuLang.Append(ID_Lang_Stop_Server,"Stop Server")
    menuLang.Append(ID_Lang_Run,"Run\tAlt+R")
    menuLang.Append(ID_Lang_Stop,"Stop\tAlt+.")
    menuLang.Append(ID_Lang_CompileLibrary,"Compile Library\tAlt+K")
    menuLang.Append(ID_Lang_OpenClassDef,"Open Class Def\tAlt+J")
    menuLang.Append(ID_Lang_ImplementationsOf,"Implementations of\tAlt+Y")
    menuLang.Append(ID_Lang_ReferencesTo,"References to\tShift+Alt+Y")
    menuLang.Append(ID_EvalSelection , "&Evaluate Selection\tCtrl+Enter")
    menuLang.Append(ID_Go_To_Help_File , "&Go To Help File\tF1")
    menuLang.Append(ID_Lang_ClearLogWindow, "&Clear Post Window\tAlt+P")
    
    menubar = wx.MenuBar()
    menubar.Append(menu, "&File")
    menubar.Append(menuLang, "&Lang")
    self.SetMenuBar(menubar)
    
    self.CreateStatusBar()

    self.Bind(wx.EVT_MENU, self.OnNewCodeWindow, id=ID_NewCodeWin)
    self.Bind(wx.EVT_MENU, self.OnHTMLtoCode, id=ID_HTMLtoCode)
    self.Bind(wx.EVT_MENU, self.OnExit, id=ID_Exit)
#    self.Bind(wx.EVT_MENU, self.OnStartServer, id=ID_StartServer)
#    self.Bind(wx.EVT_MENU, self.OnStopServer, id=ID_StopServer)
    self.Bind(wx.EVT_MENU, self.OnOpenFile, id=ID_OpenFile)
    self.Bind(wx.EVT_MENU, self.OnSaveFile, id=ID_SaveFile)
    self.Bind(wx.EVT_MENU, self.OnSaveFileAs, id=ID_SaveFileAs)
    self.Bind(wx.EVT_MENU, self.OnCloseCodeWin, id=ID_CloseCodeWin) # yssr


    self.Bind(wx.EVT_MENU, self.OnEvalSelection, id=ID_EvalSelection)
    self.Bind(wx.EVT_MENU, self.OnGotoHelpFile, id=ID_Go_To_Help_File)

    self.Bind(wx.EVT_MENU, self.OnStopServer, id=ID_Lang_Stop_Server)
    self.Bind(wx.EVT_MENU, self.On_Lang_Run, id=ID_Lang_Run)
    self.Bind(wx.EVT_MENU, self.On_Lang_Stop, id=ID_Lang_Stop)
    self.Bind(wx.EVT_MENU, self.On_Lang_CompileLibrary, id=ID_Lang_CompileLibrary)
    self.Bind(wx.EVT_MENU, self.On_Lang_OpenClassDef, id=ID_Lang_OpenClassDef)
    self.Bind(wx.EVT_MENU, self.On_Lang_ImplementationsOf, id=ID_Lang_ImplementationsOf)
    self.Bind(wx.EVT_MENU, self.On_Lang_ReferencesTo, id=ID_Lang_ReferencesTo)
    self.Bind(wx.EVT_MENU, self.On_Lang_ClearLogWindow, id=ID_Lang_ClearLogWindow)
    
    self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)  
    
    accelTableEntries = []
    # FILE
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL,ord('N'),ID_NewCodeWin))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL,ord('T'),ID_HTMLtoCode))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL,ord('O'),ID_OpenFile))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL,ord('S'),ID_SaveFile))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL,ord('W'),ID_CloseCodeWin)) # yssr
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL | wx.ACCEL_SHIFT ,ord('S'),ID_SaveFileAs))
    # EVAL & HELP
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL ,wx.WXK_RETURN,ID_EvalSelection))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_CTRL ,wx.WXK_NUMPAD_ENTER,ID_EvalSelection))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_NORMAL,wx.WXK_DECIMAL,ID_Lang_Stop))
    accelTableEntries.append(wx.AcceleratorEntry(wx.ACCEL_NORMAL,wx.WXK_F1,ID_Go_To_Help_File))
    
    accelTable = wx.AcceleratorTable(accelTableEntries)
    self.SetAcceleratorTable(accelTable)
    
    try:
      # CUSTOMIZE : add a list of files to open here. just modify the next line
      # with another file name (and duplicate the line for more file names)
      self.OpenFile("C:\\SOMEWHERE\\test.sc")
    except:
      pass

  def OnChildCharHook(self,event):
    #if event.GetKeyCode() == 0x0a and event.ControlDown( ):  # CTRL+RETURN == 0x0a
    #  self.Eval( ) # evt is useless here
    #elif event.GetKeyCode() == 0x2e and event.AltDown( ):
    
    if event.GetKeyCode() == "r" and event.AltDown( ): # '.'
      self.On_Lang_Run(None) 
    if event.GetKeyCode() == 0x2e and event.AltDown( ):
      self.On_Lang_Stop(None) 
    if event.GetKeyCode() == "k" and event.AltDown( ):
      self.On_Lang_CompileLibrary(None) 
    if event.GetKeyCode() == "j" and event.AltDown( ):
      self.On_Lang_OpenClassDef(None) 
    if event.GetKeyCode() == "y" and event.AltDown( ):
      self.On_Lang_ImplementationsOf(None) 
    if event.GetKeyCode() == "y" and event.AltDown( ) and event.ShiftDown( ):
      self.On_Lang_ReferencesTo(None) 
    else:
      event.Skip( )
    

  def OnChar(self,event):
  #  if event.GetKeyCode() == 0x0a and event.ControlDown( ):  # CTRL+RETURN == 0x0a
  #    self.Eval( ) # evt is useless here
  #  elif event.GetKeyCode() == 0x2e and event.AltDown( ):
  #    self.LangStop( )
  #  else:
    event.Skip( )
    pass
  
  def doFileHistory(self, event):
    """Open a file from file history"""
    file_number = event.GetId() - wx.ID_FILE1
    filename = self.file_history.GetHistoryFile(file_number)
    self.OpenFile(filename)
    
  def Eval(self):
    self.SendSelection("interpretPrintCmdLine")
  
  def On_Lang_Run(self,evt):
    PySCLang.sendMain("run");

  def On_Lang_Stop(self,evt):
    PySCLang.sendMain("stop");

  def On_Lang_CompileLibrary(self,evt):
    PySCLang.compileLibrary()

  def On_Lang_ClearLogWindow(self,evt):
    wx.GetApp().theMainFrame.logWin.logSubWin.Clear()
	  
  def On_Lang_OpenClassDef(self,evt):
    self.SendSelection("openWinCodeFile")

  def On_Lang_ImplementationsOf(self,evt):
    self.SendSelection("methodTemplates")

  def On_Lang_ReferencesTo(self,evt):
    self.SendSelection("methodReferences")
    
  def SendSelection(self,methodName):
    if not PySCLang.compiledOK():
      return None
    activeChild = self.GetActiveChild()
    if activeChild == self.logWin:
      return None
    sel = activeChild.GetSelectedText();
    if sel == "" :
      # this means nothing was selected... retrieve current line instead
      selTuple = activeChild.GetCurLineAsString( )
      sel = selTuple[0]  
    # this is where unicode -> string conversion occurs...
    PySCLang.setCmdLine(str(sel))
    PySCLang.sendMain(methodName)
      
  def OnOpenFile(self,evt):
    fileDlg = wx.FileDialog(self,style=wx.OPEN)
    if fileDlg.ShowModal( ) == wx.ID_OK:
      path = fileDlg.GetPath()
      self.file_history.AddFileToHistory(path)
      self.OpenFile(path)

  def OnCloseCodeWin(self,evt): # yssr -added new def
    activeWin = self.GetActiveChild( )
    if activeWin == None:
      # TODO : disable menu items dynamically instead 
      wx.MessageBox("ERROR : No document is active")
    elif not activeWin == self.logWin:
      if self.IsAllowedToCloseWindow(activeWin):
        activeWin.Destroy();

  def IsAllowedToCloseWindow(self,win):
    if win.isModified:
      if win.originalFilePath == "":
        dlg = wx.MessageDialog(self,"Do you want to save %s ? " % win.originalTitle,"Psycollider",wx.CANCEL | wx.YES_NO)
        reply = dlg.ShowModal( );
        if reply == wx.ID_YES:
          return self.OnSaveFileAsInWin(None,win)
        elif reply == wx.ID_NO:
          return 1
        elif reply == wx.ID_CANCEL:
          return 0
      else:
        dlg = wx.MessageDialog(self,"Do you want to save %s ?" % win.originalFilePath,"Psycollider",wx.CANCEL | wx.YES_NO)
        reply = dlg.ShowModal( );
        if reply == wx.ID_YES:
          #try:
          self.OnSaveFileInWin(None,win)
          #except:
          #  wx.MessageBox("Cannot save file","Error",wx.OK | wx.ICON_ERROR)
          #  return 0
          return 1
        elif reply == wx.ID_NO:
          return 1
        elif reply == wx.ID_CANCEL:
          return 0
    else:
      return 1                                  
    
  def OpenFile(self,path):
      child = self.GetActiveChild()
      if child == None:
        maximizeNewWindow = False
      else:
        maximizeNewWindow = child.IsMaximized()
      file = open(path ,"r")
      textContent = file.read()
      file.close()
      if textContent[0:5] == '{\\rtf':
	win = self.OnNewRTFWindow(None)
	# that is still not working, needs more tweaking...
        #win.rtfSubWin.LoadFile(path)
	win.rtfSubWin.AppendText('Sorry, still no RTF support, wxRichTextControl does not yet support reading RTF files...')
      elif (textContent.find('<html') >= 0 or textContent.find('<HTML') >= 0):
      	win = self.OnNewHTMLWindow(None)
	win.htmlSubWin.LoadPage(path)
      else:
       	# everything else is plain text code window
        win = self.OnNewCodeWindow(None)
        win.codeSubWin.AddText(textContent)
        
      win.originalFilePath = path 
      win.SetTitle(path)
      if maximizeNewWindow :
        win.Maximize(True)
      win.isModified = 0
      win.SetSubWinFocus()
      return win
  
  def OnSaveFile(self,evt):
    activeWin = self.GetActiveChild( );
    self.OnSaveFileInWin(evt,activeWin) 

  def OnSaveFileInWin(self,evt,activeWin):
    if activeWin == self.logWin: return None 
    if activeWin == None:
      # TODO : disable menu items dynamically instead 
      wx.MessageBox("ERROR : No document is active")
    else:
      # fileDlg = wx.FileDialog(self,style=wx.SAVE)
      path = activeWin.originalFilePath
      # path = fileDlg.GetPath()
      if path == "":
        self.OnSaveFileAsInWin(evt,activeWin)
      else:
        file = open(path ,"w" )
        content = activeWin.codeSubWin.GetText()
        file.write(content)
        file.close()
        activeWin.SetTitle(path) # yssr
        activeWin.isModified = 0 # yssr

  def OnSaveFileAs(self,evt):
    activeWin = self.GetActiveChild( );
    self.OnSaveFileAsInWin(evt,activeWin)

  def CreateLogWindow(self):
    self.logWin = PsycolliderLogWindow(self, -1, "SC Log")
    self.logWin.Show(True)
    PySCLang.setSCLogSink(WriteInLogWindow)

  def OnSaveFileAsInWin(self,evt,activeWin):
    if activeWin == None:
      # TODO : disable menu items dynamically instead 
      wx.MessageBox("ERROR : No document is active")
    else:
      fileDlg = wx.FileDialog(self,style=wx.SAVE)
      if fileDlg.ShowModal( ) == wx.ID_OK:
        path = fileDlg.GetPath()
        file = open(path ,"w" )
        content = activeWin.codeSubWin.GetText()
        file.write(content)
        file.close()
        activeWin.originalFilePath = path # yssr
        activeWin.SetTitle(path) # yssr
        activeWin.isModified = 0 # yssr

  def OnEvalSelection(self,evt):
    self.Eval()

  def OnLangStop(self,evt):
    self.Stop()
    
  def OnGotoHelpFile(self,evt):
    self.GoToHelpFile()
  
  def OnExit(self, evt):
    self.Close(True)

  def OnCloseWindow(self, event):
    # TODO : make sure users are asked to save docs
    # deal with rtf and html file saving
    shouldVeto = 0
    for win in self.GetChildren():
      if type(win) == PsycolliderCodeWin:
        if not self.IsAllowedToCloseWindow(win):
          shouldVeto = 1
          break
      
    if shouldVeto:
      event.Veto()
    else:
      self.config.SetPath("/RecentFiles")
      self.file_history.Save(self.config)
      self.Destroy()

  def OnStartServer(self, event):
    self.StartServer()

  def OnStopServer(self, event):
    self.StopServer()
  
  def StartServer(self):
    pass

  def StopServer(self):
    PySCLang.setCmdLine('s.sendMsg("/quit");')
    PySCLang.sendMain("interpretPrintCmdLine")
    pass
    
  def OnArrangeWindows(self, evt):
    currentlyActive = self.GetActiveChild()
    clientRect = self.GetClientSize()
    codeLeft = 0
    codeWidth = clientRect[0] - 4
    codeTop = 0
    codeHeight = (clientRect[1]*2)/3
    logLeft = 0
    logWidth = clientRect[0] - 4
    logTop = (clientRect[1]*2)/3
    logHeight = (clientRect[1])/3 - 3
    for win in self.GetChildren():
      if type(win) == PsycolliderLogWindow:
        win.SetDimensions(logLeft,logTop,logWidth,logHeight)
      else:
        win.SetDimensions(codeLeft,codeTop,codeWidth,codeHeight)

  def OnNewCodeWindow(self, evt):
    self.winCount = self.winCount + 1
    win = PsycolliderCodeWin(self, -1, "Untitled %d" % self.winCount)
    win.Show(True)
    win.SetSubWinFocus()
    return win

  def OnHTMLtoCode(self, evt):
    activeChild = self.GetActiveChild()
    if type(activeChild) == PsycolliderHTMLWin:
      text = activeChild.htmlSubWin.ToText()
      activeChild.Destroy()
      win = PsycolliderCodeWin(self, -1, "Child Window: %d" % self.winCount)
      win.codeSubWin.AddText(text)
      win.Show(True)
      win.SetSubWinFocus()
      return win
    return
    
  def OnNewRTFWindow(self, evt):
    self.winCount = self.winCount + 1
    win = PsycolliderRTFWin(self, -1, "Child Window: %d" % self.winCount)
    #canvas = ScrolledWindow.MyCanvas(win)
    win.Show(True)
    win.SetSubWinFocus()
    return win

  def OnNewHTMLWindow(self, evt):
    self.winCount = self.winCount + 1
    win = PsycolliderHTMLWin(self, -1, "Child Window: %d" % self.winCount)
    win.Show(True)
    win.SetSubWinFocus()
    return win

  def GoToHelpFile(self):
    # TODO : test this : most help files don't open. remove trailing and leading spaces from sel, too, since wxTextCtrl is behaving strangely
    activeChild = self.GetActiveChild()
    if not activeChild == self.logWin: # yssr
      sel = string.strip(str(activeChild.GetSelectedText()))
    else : sel = ""                    # yssr
    foundFilePath = ""
    filePath = ""                                                                                                         
    if   sel == "-" : sel = "subtraction"				# "subtraction.rtf"
    elif sel == "/" : sel = "division"					# "division.rtf"
    elif sel == "*" : sel = "multiplication"   			# from "*.rtf"
    elif sel == "**": sel = "exponentiation"        	# from "**.rtf"
    elif sel == "<" : sel = "lessthan"             		# from "<.rtf"
    elif sel == "<=": sel = "lessorequalthan"    		# from "<=.rtf"
    elif sel == ">" : sel = "greaterthan"          		# from ">.rtf"
    elif sel == ">=": sel = "greaterorequalthan" 		# from ">=.rtf"
    elif sel == "%" : sel = "modulo"					# from "%.rtf"
    if sel != "":
      for helpFolder in [gHelpFolder, os.path.expanduser(gUserExtensionFolder)]:
        for folderPath, foldersInPath, fileNamesInFolder in os.walk(helpFolder):
          # don't visit CVS directories
          if 'CVS' in foldersInPath:
            foldersInPath.remove('CVS')  
          # don't visit .svn directories
          if '.svn' in foldersInPath:
            foldersInPath.remove('.svn')  
          for fileName in fileNamesInFolder:
            filePath = os.path.join(folderPath, fileName)
            if fileName == sel + ".help.html":
              foundFilePath = filePath
              break
            if fileName == sel + ".html":
              foundFilePath = filePath
              break
          # for fileName
          # if file is found, let's break
          if foundFilePath != "":
            break
        # for folderPath, ....
        # if file is found, let's break
        if foundFilePath != "":
          break
    if foundFilePath == "":     
      foundFilePath = os.path.join(gHelpFolder,"Help.html")
    self.OpenFile(foundFilePath)
    

# ---------------------------------------------------------------------
# Every wxWindows application must have a class derived from wxApp
class PsycolliderApp(wx.App):
  # wxWindows calls this method to initialize the application
  def OnInit(self):
    # Create an instance of our customized Frame class
    frame = PsycolliderMainFrame(None, -1, "PsyCollider", wx.DefaultPosition, wx.Size(400,400))
    frame.Show(1)

    # Tell wxWindows that this is our main window
    self.SetTopWindow(frame)
    
    # enable images for html
    wx.InitAllImageHandlers()
    
    #this creates a log window and tells PySCLang about it
    frame.CreateLogWindow( )
        
    self.theMainFrame = frame
    
    PySCLang.setPyPrOpenWinTextFile(OpenTextFile)
    
    if self.ChangeDirToSCClassLibraryFolder() == False: 
      return 0

    # start the sc lang interpreter
    PySCLang.start( )    
    
    (addr,port) = self.GetServerAddressAndPort()
    
    # Return a success flag
    return 1

  def OnProut(self):
    pass
    
    
  def GetServerAddressAndPort(self):
    return ("127.1.0.1","57110")
     
  def ChangeDirToSCClassLibraryFolder(self):
    # first, we check if the current working folder
    # contains an item called 'SCClassLibrary'
    curPath = os.getcwd()
    listOfFolders = os.listdir(curPath)

    # if the cwd contains 'SCClassLibrary', we're done
    if 'SCClassLibrary' in listOfFolders:
      return None
      
    # then, we try to retrieve the SCClassLibrary folder
    # from the registry
    config  = wx.Config("Psycollider")

    # uniqueName is what gets returned from config.Read(...) 
    # if nothing was stored in the config yet
    uniqueName = "{1FB0EC09-A883-4684-AD73-1D49A98A89DE}" 
    classLibPath = config.Read("SCClassLibraryPath",uniqueName)
    leafName = (os.path.split(classLibPath))[1]

    # if the folder stored in the reg is actually an existing
    # folder called 'SCClassLibrary', we change cwd to that 
    # folder and we're done
    if os.path.isdir(classLibPath) and leafName == 'SCClassLibrary':
      classLibPath_split = os.path.split(classLibPath)
      classLibParentFolder = classLibPath_split[0]
      os.chdir(classLibParentFolder)
      return None
      
    # if something was stored in the registry, but does not exist
    # anymore or is not correct, let's warn the user
    if classLibPath != uniqueName:
      wx.MessageBox("The path stored in the application preferences is not a valid SCClassLibrary folder. You will now be requested to select an existing SCClassLibrary folder","Error", wx.OK | wx.ICON_ERROR)

    # ask the user to locate the folder
    continueLookingForFolder = True
    classLibFolderFound = False
    while continueLookingForFolder:
      dlg = wx.DirDialog(None,"Please locate the SCClassLibrary")
      if dlg.ShowModal( ) == wx.ID_CANCEL:
        wx.MessageBox("Sorry. No class library available. Psycollider will not work correctly","Error", wx.OK | wx.ICON_ERROR)
        continueLookingForFolder = False
      else:
        classLibPath = dlg.GetPath( )
        leafName = (os.path.split(classLibPath))[1]
        if leafName != 'SCClassLibrary':
          wx.MessageBox("The folder needs to be called SCClassLibrary for Psycollider to work correctly","Error", wx.OK | wx.ICON_ERROR)
        else:
          continueLookingForFolder = False
          classLibFolderFound = True

    # only if a valid SCClassLibrary folder was found, then 
    # set the current folder as its parent
    if classLibFolderFound:
      config.Write("SCClassLibraryPath",classLibPath)
      classLibPath_split = os.path.split(classLibPath)
      classLibParentFolder = classLibPath_split[0]
      os.chdir(classLibParentFolder)
    else:
      return False
    return True
      
                     
def OpenTextFile(path,rangeStart,rangeSize):
  sys.stdout.write("Calling OpenTextFile in python");
  if wx.GetApp().theMainFrame == None:
    wx.MessageBox("Cannot open %s since the main window is not created yet","Error",wx.OK | wx.ICON_ERROR)
  else:
    codeWin = wx.GetApp().theMainFrame.OpenFile(path)
    codeWin.SelectRange(rangeStart,rangeSize)

def WriteInLogWindow(text):
  if wx.GetApp().theMainFrame == None:
    sys.stdout.write(text)
  else:
    wx.GetApp().theMainFrame.logWin.logSubWin.AppendText(text)


# ---------------------------------------------------------------------
app = PsycolliderApp(0)     # Create an instance of the application class
app.MainLoop()     # Tell it to start processing events