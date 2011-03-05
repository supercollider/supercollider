#!/usr/bin/python
# File: Psycollider.py
# Copyright (c) Benjamin Golinvaux
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
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
#  USA
#
# Originally started by
# Benjamin Golinvaux
# benjamin.golinvaux@euresys.com
# messenger: bgolinvaux@hotmail.com
#
# currently maintained by:
# Christopher Frauenberger - frauenberger@iem.at
# John Glover - glover.john@gmail.com
# Martin Victory - martin.victory@gmail.com
#
# Latest changes:
# Nov 2009 (Martin)
#   - Help file navigation
#   - Find and Replace dialogs
#   - Text Formatting
#   - About dialog box

# Jan 2008 (John)
#   - SDI model
#   - Remembers the size and position of the post window
#   - Allows you to save the contents of the post window (file > save/save as)
#   - Automatically stops sc server and swing osc when closing the main (post) window
#   - Allows you to change the default window size
#   - Double clicking on brackets selects the block of text they surround (update by Christopher)
#   - Like the GEdit SC plugin (linux), you can execute a block of text surrounded by round brackets
#     by placing the cursor at the opening bracket and pressing evaluate (ctrl + enter).
#     This only happens if the open bracket is the first character on the line (not including white space)
#   - Disabled word wrap in the text editor
#	- Can toggle displaying of line numbers on/off in code editor
#     (effects all code windows and is saved to config)
#   - added ability to clear the recent file list (file history)
#   - added the option to set the tab size in code windows (saved to config)
#
# ---------------------------------------------------------------------

import PySCLang
import wx
import wx.stc as stc
import wx.html as html
import wx.richtext as richtext
import os, string, keyword, sys, time

if wx.Platform == '__WXMSW__':
  faces = { 'times': 'Times New Roman', 'mono' : 'Courier New', 'helv' : 'Arial', 'other': 'Comic Sans MS', 'size' : 10, 'size2': 8, }
  gAppHelpFolder = 'help_windows'
else:
  faces = { 'times': 'Times', 'mono' : 'Courier', 'helv' : 'Helvetica', 'other': 'new century schoolbook', 'size' : 10, 'size2': 8, }
  gAppHelpFolder = 'Help-windows'

gHelpFolder = 'Help'
gUserExtensionFolder = os.path.join(os.path.expanduser("~"), "SuperCollider\\Extensions")

MAX_HISTORY_FILES = 9
DEFAULT_SIZEX = 500
DEFAULT_SIZEY = 300
DEFAULT_POSX = 100
DEFAULT_POSY = 100

#----------------------------------------------------------------------
# set SC3_KEYWORDS as a global variable.
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
# PsycolliderWindow
#
# Base class for all windows
#   - creates the default menus
#   - asks to save a modified file when closing
#   - adds file history
#   - holds an ID number for each window, for PsycolliderDocument to refer to self
nextPsycolliderWindowId = 1
class PsycolliderWindow(wx.Frame):
    config = None                   # wx.FileConfig object
    menubar = None                  # wx.MenuBar object
    fileMenu = None                 # file menu (wx.Menu object)
    editMenu = None                 # edit menu (wx.Menu) 
    langMenu = None                 # lang menu (wx.Menu)
    optionsMenu = None              # options menu (wx.Menu)
    helpMenu = None                 # help menu (wx.Menu)
    title = ""                      # the window title
    isModified = False              # whether or not window contents have been modified
    filePath = ""                   # path to file being displayed
    windowId = -99

    def __init__(self, parent, id, title="", winStyle=wx.DEFAULT_FRAME_STYLE):
        self.title = title
        self.config = wx.GetApp().config
        wx.Frame.__init__(self, parent, id, title, style=winStyle)
        global nextPsycolliderWindowId
        windowId = nextPsycolliderWindowId
        nextPsycolliderWindowId = nextPsycolliderWindowId + 1
        #sys.stdout.write("windowId in pythonland is ")
        #sys.stdout.write(str(windowId))
        #sys.stdout.write("\n")

        self.config.SetPath("/WindowSettings")
        sizeX = self.config.ReadInt('DefaultSizeX', DEFAULT_SIZEX)
        sizeY = self.config.ReadInt('DefaultSizeY', DEFAULT_SIZEY)
        self.SetSize(wx.Size(sizeX, sizeY))

        self.CreateMenuBar()
        self.Bind(wx.EVT_CLOSE, self.OnCloseWindow)

    # file menu actions
    def OnCloseWindow(self, event):
        if not self.CanCloseWindow():
            event.Veto()

        wx.GetApp().fileHistory.RemoveMenu(self.openRecentMenu)
        wx.GetApp().ClosedWindow(self)
        self.Destroy()

    def OnNewCodeWin(self, event):
        wx.GetApp().NewCodeWindow()

    def OnHtmlToCode(self, event):
        wx.GetApp().HtmlToCode(self)

    def OnOpenFile(self, event):
        wx.GetApp().OpenFile()

    def OnSaveFile(self, event):
        self.SaveFile()

    def OnSaveFileAs(self, event):
        self.SaveFileAs()

    # edit menu actions - need to be overriden by inheriting class
    def OnUndo(self, event):
        pass

    def OnRedo(self, event):
        pass

    def OnCut(self, event):
        pass

    def OnCopy(self, event):
        pass

    def OnPaste(self, event):
        pass

    def OnDelete(self, event):
        pass

    def OnSelectAll(self, event):
        pass

    def OnShowFind(self, event):
        findData = wx.FindReplaceData()
        findDialog = wx.FindReplaceDialog(self, findData, "Find")
        findDialog.findData = findData
        findDialog.Show(True)

    def OnShowFindReplace(self, event):
        findReplaceData = wx.FindReplaceData()
        findReplaceDialog = wx.FindReplaceDialog(self, findReplaceData, "Find & Replace", wx.FR_REPLACEDIALOG)
        findReplaceDialog.findReplaceData = findReplaceData
        findReplaceDialog.Show(True)

    def OnFindClose(self, event):
        event.GetDialog().Destroy()

    def OnFind(self, event):
        pass

    # lang menu actions
    def OnStopServer(self, event):
        wx.GetApp().StopServer()

    def OnRun(self, event):
        wx.GetApp().Run()

    def OnStop(self, event):
        wx.GetApp().Stop()

    def OnCompileLibrary(self, event):
        wx.GetApp().CompileLibrary()

    def OnOpenClassDef(self, event):
        self.OpenClassDef()

    def OnImpOf(self, event):
        self.ImpOf()

    def OnRefsTo(self, event):
        self.RefsTo()

    def OnEval(self, event):
        self.Eval()

    def OnClearPostWindow(self, event):
        wx.GetApp().ClearPostWindow()

    #help menu actions
    def OnScHelp(self, event):
        wx.GetApp().GoToHelpFile(self.GetSelectedTextOrLine())

    def OnBrowseHelp(self, event):
        wx.GetApp().Eval("Help.gui")

    def OnBrowseClasses(self, event):
        wx.GetApp().Eval("Help.browse")

    def OnAbout(self, event):

        description = 'a programming language and engine for real time audio synthesis.'

        info = wx.AboutDialogInfo()

        info.SetIcon(wx.Icon('Help/GUI/Cocoa-GUI/SCImage/icon.supercollider.png', wx.BITMAP_TYPE_PNG))
        info.SetName('SuperCollider')
        info.SetVersion('3.3.2')
        info.SetDescription(description)
        info.SetWebSite('http://supercollider.sourceforge.net')

        wx.AboutBox(info)

    #options menu actions
    def OnSetDefaultWindowSize(self, event):
        size = self.GetSize()
        wx.GetApp().SetDefaultWindowSize(size.x, size.y)

    def OnClearRecentFileList(self, event):
        wx.GetApp().ClearRecentFileList()

    # should be overwritten by inheriting classes
    def SaveFile(self):
        wx.MessageBox("Error: Saving not implemented for this type of window")

    # should be overwritten by inheriting classes
    def SaveFileAs(self):
        wx.MessageBox("Error: Saving not implemented for this type of window")

    def OpenClassDef(self):
        wx.GetApp().OpenClassDef(self.GetSelectedTextOrLine())

    def ImpOf(self):
        wx.GetApp().ImpOf(self.GetSelectedTextOrLine())

    def RefsTo(self):
        wx.GetApp().RefsTo(self.GetSelectedTextOrLine())

    def Eval(self):
        wx.GetApp().Eval(self.GetSelectedTextOrLine())
        self.LineDown()

    # needs to be overwritten by inheriting classes
    def LineDown(self):
        pass

    # should be overwritten by inheriting classes
    def GetSelectedTextOrLine(self):
        return ""

    def CanCloseWindow(self):
        if self.isModified:
            if self.filePath == "":
                dlg = wx.MessageDialog(self,"Do you want to save %s ? " % self.title,"SuperCollider",wx.CANCEL | wx.YES_NO)
                reply = dlg.ShowModal()
                if reply == wx.ID_YES:
                    self.SaveFileAs()
                    return True
                elif reply == wx.ID_NO:
                    return True
                elif reply == wx.ID_CANCEL:
                    return False
            else:
                dlg = wx.MessageDialog(self,"Do you want to save %s ?" % self.filePath,"SuperCollider",wx.CANCEL | wx.YES_NO)
                reply = dlg.ShowModal()
                if reply == wx.ID_YES:
                    self.SaveFile()
                    return True
                elif reply == wx.ID_NO:
                    return True
                elif reply == wx.ID_CANCEL:
                    return False
        else:
            return True

    def CreateMenuBar(self):
        self.fileMenu = wx.Menu()
        self.openRecentMenu = wx.Menu()
        self.editMenu = wx.Menu()
        self.langMenu = wx.Menu()
        self.optionsMenu = wx.Menu()
        self.helpMenu = wx.Menu()
        self.menubar = wx.MenuBar()
        self.menubar.Append(self.fileMenu, "&File")
        self.menubar.Append(self.editMenu, "&Edit")
        self.menubar.Append(self.langMenu, "&Lang")
        self.menubar.Append(self.optionsMenu, "&Options")
        self.menubar.Append(self.helpMenu, "&Help")
        self.SetMenuBar(self.menubar)

        self.newCodeWin = wx.MenuItem(self.fileMenu, -1, '&New\tCtrl+N')
        self.htmlToCode = wx.MenuItem(self.fileMenu, -1, 'H&TML to Code\tCtrl+T')
        self.openFile = wx.MenuItem(self.fileMenu, -1, '&Open...\tCtrl+O')
        self.saveFile = wx.MenuItem(self.fileMenu, -1, '&Save\tCtrl+S')
        self.saveFileAs = wx.MenuItem(self.fileMenu, -1, 'Save &As...\tCtrl+Shift+S')
        self.closeWindow = wx.MenuItem(self.fileMenu, -1, 'Close\tCtrl+W')

        self.undo = wx.MenuItem(self.editMenu, -1, '&Undo\tCtrl+Z')
        self.redo = wx.MenuItem(self.editMenu, -1, '&Redo\tCtrl+Y')
        self.cut = wx.MenuItem(self.editMenu, -1, '&Cut\tCtrl+X')
        self.copy = wx.MenuItem(self.editMenu, -1, 'C&opy\tCtrl+C')
        self.paste = wx.MenuItem(self.editMenu, -1, '&Paste\tCtrl+V')
        self.delete = wx.MenuItem(self.editMenu, -1, '&Delete\tDel')
        self.selectAll = wx.MenuItem(self.editMenu, -1, '&Select All\tCtrl+A')
        self.find = wx.MenuItem(self.editMenu, -1, '&Find\tCtrl+F')
        self.replace = wx.MenuItem(self.editMenu, -1, '&Replace\tCtrl+H')

        self.stopServer = wx.MenuItem(self.langMenu, -1, 'Stop Server')
        self.run = wx.MenuItem(self.langMenu, -1, 'Run\tAlt+R')
        self.stop = wx.MenuItem(self.langMenu, -1, '&Stop\tAlt+.')
        self.compileLibrary = wx.MenuItem(self.langMenu, -1, 'Compile Library\tAlt+K')
        self.openClassDef = wx.MenuItem(self.langMenu, -1, 'Open Class Def\tAlt+J')
        self.impOf = wx.MenuItem(self.langMenu, -1, 'Implementations of\tAlt+Y')
        self.refsTo = wx.MenuItem(self.langMenu, -1, 'References to\tShift+Alt+Y')
        self.eval = wx.MenuItem(self.langMenu, -1, '&Evaluate Selection\tCtrl+Enter')
        self.clearPostWindow = wx.MenuItem(self.langMenu, -1, '&Clear Post Window\tAlt+P')

        self.setDefaultWindowSize = wx.MenuItem(self.optionsMenu, -1, '&Set This Window Size As Default')
        self.clearRecentFileList = wx.MenuItem(self.optionsMenu, -1, '&Clear Recent File List')

        self.scHelp = wx.MenuItem(self.helpMenu, -1, '&SuperCollider Help\tF1')
        self.helpBrowser = wx.MenuItem(self.helpMenu, -1, '&Browse and Search Documentation\t')
        self.classBrowser = wx.MenuItem(self.helpMenu, -1, '&Class Browser\t')
        self.about = wx.MenuItem(self.helpMenu, -1, 'About\t')


        self.fileMenu.AppendItem(self.newCodeWin)
        self.fileMenu.AppendItem(self.openFile)
        self.fileMenu.AppendSubMenu(self.openRecentMenu, "Open Recent")
        self.fileMenu.AppendSeparator()
        self.fileMenu.AppendItem(self.saveFile)
        self.fileMenu.AppendItem(self.saveFileAs)
        self.fileMenu.AppendItem(self.htmlToCode)
        self.fileMenu.AppendSeparator()
        self.fileMenu.AppendItem(self.closeWindow)

        self.editMenu.AppendItem(self.undo)
        self.editMenu.AppendItem(self.redo)
        self.editMenu.AppendSeparator()
        self.editMenu.AppendItem(self.cut)
        self.editMenu.AppendItem(self.copy)
        self.editMenu.AppendItem(self.paste)
        self.editMenu.AppendItem(self.delete)
        self.editMenu.AppendItem(self.selectAll)
        self.editMenu.AppendSeparator()
        self.editMenu.AppendItem(self.find)
        self.editMenu.AppendItem(self.replace)

        self.langMenu.AppendItem(self.stopServer)
        self.langMenu.AppendItem(self.run)
        self.langMenu.AppendItem(self.stop)
        self.langMenu.AppendItem(self.compileLibrary)
        self.langMenu.AppendItem(self.openClassDef)
        self.langMenu.AppendItem(self.impOf)
        self.langMenu.AppendItem(self.refsTo)
        self.langMenu.AppendItem(self.eval)
        self.langMenu.AppendItem(self.clearPostWindow)

        self.optionsMenu.AppendItem(self.setDefaultWindowSize)
        self.optionsMenu.AppendItem(self.clearRecentFileList)

        self.helpMenu.AppendItem(self.scHelp)
        self.helpMenu.AppendItem(self.helpBrowser)
        self.helpMenu.AppendItem(self.classBrowser)
        self.helpMenu.AppendSeparator()
        self.helpMenu.AppendItem(self.about)

        self.Bind(wx.EVT_MENU, self.OnNewCodeWin, id=self.newCodeWin.GetId())
        self.Bind(wx.EVT_MENU, self.OnHtmlToCode, id=self.htmlToCode.GetId())
        self.Bind(wx.EVT_MENU, self.OnOpenFile, id=self.openFile.GetId())
        self.Bind(wx.EVT_MENU, self.OnSaveFile, id=self.saveFile.GetId())
        self.Bind(wx.EVT_MENU, self.OnSaveFileAs, id=self.saveFileAs.GetId())
        self.Bind(wx.EVT_MENU, self.OnCloseWindow, id=self.closeWindow.GetId())

        self.Bind(wx.EVT_MENU, self.OnUndo, id=self.undo.GetId())
        self.Bind(wx.EVT_MENU, self.OnRedo, id=self.redo.GetId())
        self.Bind(wx.EVT_MENU, self.OnCut, id=self.cut.GetId())
        self.Bind(wx.EVT_MENU, self.OnCopy, id=self.copy.GetId())
        self.Bind(wx.EVT_MENU, self.OnPaste, id=self.paste.GetId())
        self.Bind(wx.EVT_MENU, self.OnDelete, id=self.delete.GetId())
        self.Bind(wx.EVT_MENU, self.OnSelectAll, id=self.selectAll.GetId())
        self.Bind(wx.EVT_MENU, self.OnShowFind, id=self.find.GetId())
        self.Bind(wx.EVT_MENU, self.OnShowFindReplace, id=self.replace.GetId())

        self.Bind(wx.EVT_FIND, self.OnFind)
        self.Bind(wx.EVT_FIND_NEXT, self.OnFind)
        self.Bind(wx.EVT_FIND_REPLACE, self.OnFind)
        self.Bind(wx.EVT_FIND_REPLACE_ALL, self.OnFind)
        self.Bind(wx.EVT_FIND_CLOSE, self.OnFindClose)

        self.Bind(wx.EVT_MENU, self.OnStopServer, id=self.stopServer.GetId())
        self.Bind(wx.EVT_MENU, self.OnRun, id=self.run.GetId())
        self.Bind(wx.EVT_MENU, self.OnStop, id=self.stop.GetId())
        self.Bind(wx.EVT_MENU, self.OnCompileLibrary, id=self.compileLibrary.GetId())
        self.Bind(wx.EVT_MENU, self.OnOpenClassDef, id=self.openClassDef.GetId())
        self.Bind(wx.EVT_MENU, self.OnImpOf, id=self.impOf.GetId())
        self.Bind(wx.EVT_MENU, self.OnRefsTo, id=self.refsTo.GetId())
        self.Bind(wx.EVT_MENU, self.OnEval, id=self.eval.GetId())
        self.Bind(wx.EVT_MENU, self.OnClearPostWindow, id=self.clearPostWindow.GetId())

        self.Bind(wx.EVT_MENU, self.OnSetDefaultWindowSize, id=self.setDefaultWindowSize.GetId())
        self.Bind(wx.EVT_MENU, self.OnClearRecentFileList, id=self.clearRecentFileList.GetId())

        self.Bind(wx.EVT_MENU, self.OnScHelp, id=self.scHelp.GetId())
        self.Bind(wx.EVT_MENU, self.OnBrowseHelp, id=self.helpBrowser.GetId())
        self.Bind(wx.EVT_MENU, self.OnBrowseClasses, id=self.classBrowser.GetId())
        self.Bind(wx.EVT_MENU, self.OnAbout, id=self.about.GetId())

        wx.GetApp().fileHistory.UseMenu(self.openRecentMenu)
        wx.GetApp().fileHistory.AddFilesToThisMenu(self.openRecentMenu)
        self.Bind(wx.EVT_MENU_RANGE, wx.GetApp().doFileHistory, id=wx.ID_FILE1, id2=wx.ID_FILE9)


# ---------------------------------------------------------------------
# PsycolliderCodeSubWin - plain text window for code
class PsycolliderCodeSubWin(wx.stc.StyledTextCtrl):

    fold_symbols = 3

    def __init__ (self,parent):
        stc.StyledTextCtrl.__init__(self,parent)
        self.SetModEventMask(wx.stc.STC_MOD_INSERTTEXT | wx.stc.STC_MOD_DELETETEXT | wx.stc.STC_PERFORMED_USER)

        # bindings
        self.Bind(stc.EVT_STC_CHANGE, self.OnStcChange)
        self.Bind(stc.EVT_STC_UPDATEUI, self.OnUpdateUI)
        self.Bind(stc.EVT_STC_MARGINCLICK, self.OnMarginClick)
        self.Bind(wx.EVT_KEY_DOWN, self.OnKeyPressed)
        self.Bind(wx.EVT_CHAR, self.OnChar)         # this hack is to enable the alt+. shortcut
                                                    # to stop playback, which doesn't seem to work otherwise
                                                    # bug in wx?
        self.Bind(wx.EVT_LEFT_DCLICK, self.OnDoubleClick)

        self.SetLexer(wx.stc.STC_LEX_CPP)             # yssr
        self.SetKeyWords(0, " ".join(SC3_KEYWORDS))   # yssr

        self.SetProperty("fold", "1")
        self.SetProperty("tab.timmy.whinge.level", "1")
        self.SetMargins(1,0) # yssr

        # set end-of-line character to LF
        self.SetEOLMode(wx.stc.STC_EOL_LF);

        # some settings for appearance
        self.SetViewWhiteSpace(False)
        self.SetViewEOL(False)
        self.SetUseAntiAliasing(True)
        self.SetWrapMode(False)
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


    def OnChar(self, event):
        if event.GetKeyCode() == 0x2e and event.AltDown():
            self.GetParent().OnStop(None)
        else:
            event.Skip()

    def OnStcChange(self, event):
        self.GetParent().OnStcChange(event)

    def OnKeyPressed(self, event):
        #if self.CallTipActive():
        #   self.CallTipCancel()
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

    def OnDoubleClick(self, evt):
        braceAtCaret, braceOpposite = self.CheckForMatchingBraces()

        if braceAtCaret != -1  and braceOpposite != -1:
            if braceAtCaret < braceOpposite:
                self.SetSelection(braceAtCaret+1, braceOpposite)
            else:
                self.SetSelection(braceOpposite+1, braceAtCaret)
        else:
            evt.Skip()

    def OnUpdateUI(self, evt):
        braceAtCaret, braceOpposite = self.CheckForMatchingBraces()

        if braceAtCaret != -1  and braceOpposite == -1:
            self.BraceBadLight(braceAtCaret)
        else:
            self.BraceHighlight(braceAtCaret, braceOpposite)

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

    def CheckForMatchingBraces(self):
        braceAtCaret = -1
        braceOpposite = -1
        charAfter = None
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

        return braceAtCaret, braceOpposite

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

    def SetShowLineNumbers(self, value):
        if value:
            self.SetMarginType(2, stc.STC_MARGIN_NUMBER)
        else:
            self.SetMarginType(2, stc.STC_MARGIN_SYMBOL)

    def GetTabSize(self):
        return self.GetTabWidth()

    def SetTabSize(self, tabSize):
        self.SetTabWidth(tabSize)

    def GetText(self):
        # append an extra space, as GetTextUTF8() seems to remove the last character, wx bug?
        self.AppendTextUTF8(" ")
        return self.GetTextUTF8()


# ---------------------------------------------------------------------
# Code Window
# accomodates the code sub window
class PsycolliderCodeWin(PsycolliderWindow):

    SHOW_LINE_NUMBERS = False   # default
    TAB_SIZE = 8                # default tab size

    def __init__ (self, parent, id, title, pos=wx.DefaultPosition, size=wx.DefaultSize):
        PsycolliderWindow.__init__(self, parent, id, title)
        self.fileMenu.Remove(self.htmlToCode.GetId())   # Remove unnecessary menu item
        self.codeSubWin = PsycolliderCodeSubWin(self)

        # this will be our default font
        font = wx.Font(10, wx.MODERN, wx.NORMAL, wx.NORMAL, False, "Courier New")
        self.fontSize = font.GetPointSize()
        self.fontFace = font.GetFaceName()

        self.ChangeFont(self.fontFace, self.fontSize)

        # line numbers
        self.config.SetPath("/CodeWindowOptions")
        self.showLineNumbers.Check(self.config.ReadInt('ShowLineNumbers', self.SHOW_LINE_NUMBERS))
        self.codeSubWin.SetShowLineNumbers(self.showLineNumbers.IsChecked())

        # tab size
        self.codeSubWin.SetTabSize(self.config.ReadInt('TabSize', self.TAB_SIZE))

    def OnStcChange(self, event):
        if not self.isModified:
            self.SetTitle(self.GetTitle() + "*")
            self.isModified = True

    def OnShowLineNumbers(self, event):
        for window in wx.GetApp().GetOpenWindows():
            if type(window) == PsycolliderCodeWin:
                window.SetShowLineNumbers(self.showLineNumbers.IsChecked())

        self.config.SetPath("/CodeWindowOptions")
        self.config.WriteInt('ShowLineNumbers', self.showLineNumbers.IsChecked())

    def OnSetTabSize(self, event):
        newTabSize = -1
        getNewTabSize = wx.TextEntryDialog(self, 'Set tab size to:', 'Set Tab Size', str(self.TAB_SIZE))
        getNewTabSize.SetValue(str(self.codeSubWin.GetTabSize()))

        if getNewTabSize.ShowModal() == wx.ID_OK:
            try:
                newTabSize = int(getNewTabSize.GetValue())
                if newTabSize <= 0:
                    raise

                for window in wx.GetApp().GetOpenWindows():
                    if type(window) == PsycolliderCodeWin:
                        window.codeSubWin.SetTabSize(newTabSize)

                self.config.SetPath("/CodeWindowOptions")
                self.config.WriteInt('TabSize', newTabSize)

            except:
                WriteInLogWindow("Invalid tab size, ignoring. Please enter a positive integer\n")
                return

        getNewTabSize.Destroy()

    def GetSelectedText(self):
        return self.codeSubWin.GetSelectedTextUTF8()

    def GetCurLineAsString(self):
        return self.codeSubWin.GetCurLine()

    def SetSubWinFocus(self):
        self.codeSubWin.SetFocus()

    def SelectRange(self,rangeStart,rangeSize):
        self.codeSubWin.SetSelection(rangeStart,rangeStart+rangeSize)

    def SetShowLineNumbers(self, value):
        self.showLineNumbers.Check(value)
        self.codeSubWin.SetShowLineNumbers(value)

    def SaveFile(self):
        if self.filePath == "":
            self.SaveFileAs()
        else:
            try:
                file = open(self.filePath, "w")
                content = self.codeSubWin.GetText()
                file.write(content)
                self.SetTitle(self.filePath)
                self.isModified = False
                file.close()
            except:
                # Todo: better error handling? Just print error message for now
                wx.MessageBox("Error: Could not save file " + self.filePath)

    def SaveFileAs(self):
        fileDlg = wx.FileDialog(self,style=wx.SAVE)
        if fileDlg.ShowModal() == wx.ID_OK:
            self.filePath = fileDlg.GetPath()
            try:
                file = open(self.filePath ,"w")
                content = self.codeSubWin.GetText()
                file.write(content)
                file.close()
            except:
                # Todo: better error handling? Just print error message for now
                wx.MessageBox("Error: Could not save file " + self.filePath)
                return None

            self.SetTitle(self.filePath)
            self.isModified = False
            wx.GetApp().AddFileToHistory(self.filePath)

    def GetSelectedTextOrLine(self):
        """Returns selected text if any. If not, returns the current line"""
        selection = str(self.codeSubWin.GetSelectedTextUTF8())

        if selection == "":
            # get current line, return if not at an open '(' bracket
            currentLine = self.codeSubWin.GetLineUTF8(self.codeSubWin.GetCurrentLine())
            selection = str(currentLine)

            # see if the cursor is at a matched bracket
            x, y = self.codeSubWin.CheckForMatchingBraces()
            if x != -1 and y != -1:
                if chr(self.codeSubWin.GetCharAt(x)) == "(":
                    # make sure the open bracket is the first character in the line
                    if currentLine.strip().find('(') == 0:
                        # get all text up to and including the closing bracket
                        selection = str(self.codeSubWin.GetTextRangeUTF8(x, y+1))

        return selection

    def LineDown(self):
        self.codeSubWin.LineDown()

    def OnOpenFonts(self, event):
        data = wx.FontData()
        data.EnableEffects(False)

        dlg = wx.FontDialog(self, data)
        
        if dlg.ShowModal() == wx.ID_OK:
            data = dlg.GetFontData()
            font = data.GetChosenFont()

            self.fontFace = font.GetFaceName()
            self.fontSize = font.GetPointSize()
            self.ChangeFont(self.fontFace, self.fontSize)

        dlg.Destroy()

    def ChangeFont(self, face, size):
        self.codeSubWin.StyleClearAll()  # Reset all to be like the default
        self.codeSubWin.StyleSetSpec(stc.STC_STYLE_DEFAULT,
            "face:%s, size:%d" % (face, size))

        self.codeSubWin.StyleSetSpec(stc.STC_STYLE_LINENUMBER,
            "back:#C0C0C0,face:%(helv)s,size:%(size2)d" % faces)

        self.codeSubWin.StyleSetSpec(
            stc.STC_STYLE_CONTROLCHAR, "face:%s, size:%d" % (face, size))

        self.codeSubWin.StyleSetSpec(stc.STC_STYLE_BRACELIGHT,
            "fore:#FFFFFF,back:#00FFFF,bold, face:%s, size:%d" % (face, size))

        self.codeSubWin.StyleSetSpec(stc.STC_STYLE_BRACEBAD, 
            "fore:#000000,back:#FF3333,bold,face:%s, size:%d" % (face, size))

        # Default face
        self.codeSubWin.StyleSetSpec(stc.STC_C_DEFAULT,
            "fore:#000000,face:%s,size:%d" % (face, size))
        # Comments
        self.codeSubWin.StyleSetSpec(stc.STC_C_COMMENTLINE,
            "fore:#bf0000,face:%s,size:%d" % (face, size))
        self.codeSubWin.StyleSetSpec(stc.STC_C_COMMENT,
            "fore:#bf0000,face:%s,size:%d" % (face, size))
        # Number
        self.codeSubWin.StyleSetSpec(stc.STC_C_NUMBER,
            "fore:#333333,face:%s,size:%d" % (face, size))
        # String
        self.codeSubWin.StyleSetSpec(stc.STC_C_STRING,
            "italic,fore:#606060,face:%s,size:%d" % (face, size))
        # Single quoted string
        self.codeSubWin.StyleSetSpec(stc.STC_C_CHARACTER,
            "fore:#007300,face:%s,size:%d" % (face, size))
        # Keyword
        self.codeSubWin.StyleSetSpec(stc.STC_C_WORD,
            "fore:#0000bf,face:%s,size:%d" %  (face, size))
        self.codeSubWin.StyleSetSpec(stc.STC_C_WORD2,
            "fore:#0000bf,face:%s,size:%d" %  (face, size))
        # Operators
        self.codeSubWin.StyleSetSpec(stc.STC_C_OPERATOR,
            "face:%s,size:%d" %  (face, size))
        # Identifiers
        self.codeSubWin.StyleSetSpec(stc.STC_C_IDENTIFIER,
            "fore:#000000,face:%s,size:%d" % (face, size))
        self.codeSubWin.StyleSetSpec(stc.STC_C_UUID,
            "fore:#000000,face:%s,size:%d" % (face, size))
        # End of line where string is not closed
        self.codeSubWin.StyleSetSpec(stc.STC_C_STRINGEOL,
            "fore:#000000,face:%s,back:#ffffff,eol,size:%d" % (face, size))

        self.codeSubWin.SetCaretForeground("BLACK")


    def OnBiggerFont(self, event):
        self.fontSize += 1
        self.ChangeFont(self.fontFace, self.fontSize)

    def OnSmallerFont(self, event):
        self.fontSize -= 1
        self.ChangeFont(self.fontFace, self.fontSize)

    def CreateMenuBar(self):
        PsycolliderWindow.CreateMenuBar(self)

        self.showLineNumbers = wx.MenuItem(self.optionsMenu, -1, 'S&how Line Numbers', kind=wx.ITEM_CHECK)
        self.setTabSize = wx.MenuItem(self.optionsMenu, -1, 'S&et Tab Size')
        self.optionsMenu.AppendSeparator()
        self.optionsMenu.AppendItem(self.showLineNumbers)
        self.optionsMenu.AppendItem(self.setTabSize)
        self.Bind(wx.EVT_MENU, self.OnShowLineNumbers, id=self.showLineNumbers.GetId())
        self.Bind(wx.EVT_MENU, self.OnSetTabSize, id=self.setTabSize.GetId())

        # format menu
        formatMenu = wx.Menu()
        self.menubar.Insert(4, formatMenu, "Fo&rmat")
        self.openFonts = wx.MenuItem(formatMenu, -1, '&Show Fonts\tAlt+T')
        self.biggerFont = wx.MenuItem(formatMenu, -1, '&Bigger Font\tCtrl++')
        self.smallerFont = wx.MenuItem(formatMenu, -1, '&Smaller Font\tCtrl+-')
        formatMenu.AppendItem(self.openFonts)
        formatMenu.AppendSeparator()
        formatMenu.AppendItem(self.biggerFont)
        formatMenu.AppendItem(self.smallerFont)
        self.Bind(wx.EVT_MENU, self.OnOpenFonts, id=self.openFonts.GetId())
        self.Bind(wx.EVT_MENU, self.OnBiggerFont, id=self.biggerFont.GetId())
        self.Bind(wx.EVT_MENU, self.OnSmallerFont, id=self.smallerFont.GetId())

    # edit menu actions
    def OnUndo(self, event):
        self.codeSubWin.Undo()

    def OnRedo(self, event):
        self.codeSubWin.Redo()

    def OnCut(self, event):
        self.codeSubWin.Cut()

    def OnCopy(self, event):
        self.codeSubWin.Copy()

    def OnPaste(self, event):
        self.codeSubWin.Paste()

    def OnDelete(self, event):
        self.codeSubWin.Clear()

    def OnSelectAll(self, event):
        self.codeSubWin.SelectAll()

    def OnFind(self, event):
        map = {
            wx.wxEVT_COMMAND_FIND : "FIND",
            wx.wxEVT_COMMAND_FIND_NEXT : "FIND_NEXT",
            wx.wxEVT_COMMAND_FIND_REPLACE : "REPLACE",
            wx.wxEVT_COMMAND_FIND_REPLACE_ALL : "REPLACE_ALL",
            }

        et = event.GetEventType()
        
        length = self.codeSubWin.GetTextLength()
        str = event.GetFindString()

        # find/find next
        if et == wx.wxEVT_COMMAND_FIND or et == wx.wxEVT_COMMAND_FIND_NEXT:
            flags = 0
            if event.GetFlags() & wx.FR_MATCHCASE:
                flags = flags | stc.STC_FIND_MATCHCASE
                print "matching case"

            if event.GetFlags() & wx.FR_WHOLEWORD:
                flags = flags | stc.STC_FIND_WHOLEWORD
                print "searching wholeword"

            if event.GetFlags() & wx.FR_DOWN:
                if et == wx.wxEVT_COMMAND_FIND_NEXT:
                    startPos = self.codeSubWin.GetCurrentPos()
                    endPos = self.codeSubWin.GetTextLength()
                else:
                    startPos = 0
                    endPos = self.codeSubWin.GetTextLength()
            else:
                if et == wx.wxEVT_COMMAND_FIND_NEXT:
                    startPos = self.codeSubWin.GetCurrentPos()-1
                    endPos = 0
                else:
                    startPos = self.codeSubWin.GetTextLength()
                    endPos = 0

            pos = self.codeSubWin.FindText(startPos, endPos, str, flags)

            if pos >= 0:
                self.codeSubWin.SetSelection(pos, pos+len(str))
                self.codeSubWin.EnsureCaretVisible()
            else:
                wx.MessageBox("Reached end of document", "Find", wx.ICON_EXCLAMATION | wx.OK, self.codeSubWin)

        # replace
        elif et == wx.wxEVT_COMMAND_FIND_REPLACE:
            flags = 0
            if event.GetFlags() & wx.FR_MATCHCASE:
                flags = flags | stc.STC_FIND_MATCHCASE

            if event.GetFlags() & wx.FR_WHOLEWORD:
                flags = flags | stc.STC_FIND_WHOLEWORD

            startPos = 0
            endPos = self.codeSubWin.GetTextLength()

            pos = self.codeSubWin.FindText(startPos, endPos, str, flags)
            if pos >= 0:
                self.codeSubWin.SetSelection(pos, pos+len(str))
                self.codeSubWin.ReplaceSelection(event.GetReplaceString())
                self.codeSubWin.EnsureCaretVisible()
            else:
                wx.MessageBox("Reached end of document", "Replace", wx.ICON_EXCLAMATION | wx.OK, self.codeSubWin)

        # replace all
        elif et == wx.wxEVT_COMMAND_FIND_REPLACE_ALL:
            flags = 0
            if event.GetFlags() & wx.FR_MATCHCASE:
                flags = flags | stc.STC_FIND_MATCHCASE

            if event.GetFlags() & wx.FR_WHOLEWORD:
                flags = flags | stc.STC_FIND_WHOLEWORD

            initPos = self.codeSubWin.GetCurrentPos()
            startPos = 0
            endPos = self.codeSubWin.GetTextLength()

            numTokens = 0
            pos = self.codeSubWin.FindText(startPos, endPos, str, flags)
            while pos >= 0:
                numTokens = numTokens+1
                self.codeSubWin.SetSelection(pos, pos+len(str))
                self.codeSubWin.ReplaceSelection(event.GetReplaceString())
                self.codeSubWin.EnsureCaretVisible()
                pos = self.codeSubWin.FindText(pos+len(str), endPos, str, flags)

            self.codeSubWin.GotoPos(initPos)

            wx.MessageBox("%d instance(s) replaced" % (numTokens), "Replace All", wx.ICON_EXCLAMATION | wx.OK, self.codeSubWin)

# ---------------------------------------------------------------------
# HTML Sub Window
#
class PsycolliderHTMLSubWin(wx.html.HtmlWindow):

    def __init__ (self,parent):
        wx.html.HtmlWindow.__init__(self,parent)
        self.parent = parent
        self.Bind(wx.EVT_CHAR, self.OnChar)         # this hack is to enable the alt+. shortcut
        self.Bind(html.EVT_HTML_LINK_CLICKED, self.OnClicked)
        self.titles = [parent.GetTitle()]
        self.titlePos = 0

    def OnChar(self, event):
        if event.GetKeyCode() == 0x2e and event.AltDown():
            self.GetParent().OnStop(None)
        elif event.GetKeyCode() == wx.WXK_LEFT and event.AltDown():
            self.HistoryBack()
        elif event.GetKeyCode() == wx.WXK_RIGHT and event.AltDown():
            self.HistoryForward()
        else:
            event.Skip()

    # this allows us to correctly set the title of the parent window
    def OnClicked(self, event):

        # clicking on a link effectively removes forward history
        self.titles = self.titles[:self.titlePos+1]

        info = event.GetLinkInfo()
        href = info.GetHref()
        self.LoadPage(href)

        pageTitle = os.path.splitext(os.path.basename(href))[0]
        self.parent.SetTitle(pageTitle)
        self.titles.append(pageTitle)
        self.titlePos += 1

    def GoForward(self, event):
        if self.HistoryCanForward():
            self.HistoryForward()
            self.titlePos += 1
            self.parent.SetTitle(self.titles[self.titlePos])

    def GoBack(self, event):
        if self.HistoryCanBack():
            self.HistoryBack()
            self.titlePos -= 1
            self.parent.SetTitle(self.titles[self.titlePos])

    def GoHome(self, event):
        filePath = os.path.join(gHelpFolder,"Help.html")
        self.parent.SetTitle("Help")
        self.LoadPage(filePath)
        self.titles.append("Help")
        self.titlePos += 1

# ---------------------------------------------------------------------
# HTML Window
# accomodates HTML sub window
class PsycolliderHTMLWin(PsycolliderWindow):

    def __init__ (self,parent,id,title,pos=wx.DefaultPosition,size=wx.DefaultSize):
        PsycolliderWindow.__init__(self, parent, id, title)
        self.fileMenu.Remove(self.saveFile.GetId())     # Remove unnecessary menu items
        self.fileMenu.Remove(self.saveFileAs.GetId())
        self.htmlSubWin = PsycolliderHTMLSubWin(self)

        # Remove edit menu
        self.menubar.Remove(1)

        # Add navigation menu to HTML windows
        self.navMenu = wx.Menu()
        self.menubar.Insert(3, self.navMenu, "&Navigation")
        self.home = wx.MenuItem(self.navMenu, -1, '&Home')
        self.forward = wx.MenuItem(self.navMenu, -1, '&Forward\tAlt+Right')
        self.back = wx.MenuItem(self.navMenu, -1, '&Back\tAlt+Left')

        self.navMenu.AppendItem(self.home)
        self.navMenu.AppendItem(self.forward)
        self.navMenu.AppendItem(self.back)
        self.SetMenuBar(self.menubar)

        self.Bind(wx.EVT_MENU, self.htmlSubWin.GoHome, id=self.home.GetId())
        self.Bind(wx.EVT_MENU, self.htmlSubWin.GoForward, id=self.forward.GetId())
        self.Bind(wx.EVT_MENU, self.htmlSubWin.GoBack, id=self.back.GetId())

    def GetSelectedText(self):
        return self.htmlSubWin.SelectionToText()

    def GetCurLineAsString(self):
        posInText = self.htmlSubWin.GetInsertionPoint()
        (x,y) = self.htmlSubWin.PositionToXY(posInText)
        return self.htmlSubWin.SelectLine(y)

    def GetSelectedTextOrLine(self):
        """Returns selected text"""
        return str(self.GetSelectedText())

    def SetSubWinFocus(self):
        self.htmlSubWin.SetFocus()


# ---------------------------------------------------------------------
# Psycollider Post Window
class PsycolliderPostWindow(PsycolliderWindow):
    log = None          # The wx.TextCtrl object that displays post info

    def __init__(self, parent, id, title):
        # init (no maximise button)
        PsycolliderWindow.__init__(self, parent, id, title, wx.MINIMIZE_BOX | wx.CLOSE_BOX |wx.RESIZE_BORDER | wx.SYSTEM_MENU | wx.CAPTION)

        self.config.SetPath("/WindowSettings")
        sizeX = self.config.ReadInt('PostWindow-sizeX', DEFAULT_SIZEX)
        sizeY = self.config.ReadInt('PostWindow-sizeY', DEFAULT_SIZEY)
        posX = self.config.ReadInt('PostWindow-posX', DEFAULT_POSX)
        posY = self.config.ReadInt('PostWindow-posY', DEFAULT_POSY)

        # check that position is > 0
        if posX < 0:
            posX = DEFAULT_POSX
        if posY < 0:
            posY = DEFAULT_POSY

        self.SetSize(wx.Size(sizeX, sizeY))
        self.SetPosition(wx.Point(posX, posY))

        self.fileMenu.Remove(self.htmlToCode.GetId())   # Remove unnecessary menu items
        self.langMenu.Remove(self.run.GetId())
        self.langMenu.Remove(self.stop.GetId())
        self.langMenu.Remove(self.openClassDef.GetId())
        self.langMenu.Remove(self.impOf.GetId())
        self.langMenu.Remove(self.refsTo.GetId())
        self.langMenu.Remove(self.eval.GetId())
        self.menubar.Remove(1)

        mainPanel = wx.Panel(self, -1)
        mainSizer = wx.BoxSizer(wx.VERTICAL)
        mainPanel.SetSizer(mainSizer)

        self.log = wx.TextCtrl(mainPanel, -1, style = wx.TE_MULTILINE | wx.TE_READONLY)
        mainSizer.Add(self.log, proportion = 1, flag = wx.EXPAND)

        self.Show(True)

    def OnCloseWindow(self, event):
        dlg = wx.MessageDialog(self, "This will shut down SuperCollider, stop all servers and close all code windows.\n Do you want to quit?")
        reply = dlg.ShowModal()
        dlg.Destroy()
        if reply == wx.ID_OK:
            PsycolliderWindow.OnCloseWindow(self, event)
            size = self.GetSize()
            pos = self.GetPosition()
            self.config.SetPath("/WindowSettings")
            self.config.WriteInt('PostWindow-sizeX', size.x)
            self.config.WriteInt('PostWindow-sizeY', size.y)
            self.config.WriteInt('PostWindow-posX', pos.x)
            self.config.WriteInt('PostWindow-posY', pos.y)

            wx.GetApp().Shutdown()
	else:
		# No need?  wx.MessageBox("Canceled");
		pass

    def SaveFile(self):
        if self.filePath == "":
            self.SaveFileAs()
        else:
            try:
                file = open(self.filePath, "w")
                content = self.log.GetRange(0, self.log.GetLastPosition())
                file.write(content)
                file.close()
            except:
                # Todo: better error handling? Just print error message for now
                wx.MessageBox("Error: Could not save file " + filePath)

    def SaveFileAs(self):
        fileDlg = wx.FileDialog(self,style=wx.SAVE)
        if fileDlg.ShowModal() == wx.ID_OK:
            self.filePath = fileDlg.GetPath()
            try:
                file = open(self.filePath ,"w")
                content = self.log.GetRange(0, self.log.GetLastPosition())
                file.write(content)
                file.close()
            except:
                # Todo: better error handling? Just print error message for now
                wx.MessageBox("Error: Could not save file " + self.filePath)


# ---------------------------------------------------------------------
# Psycollider
class Psycollider(wx.App):
    theMainFrame = None             # Points to the post window object
    openWindows = []                # List of all windows currently open
    config = None                   # Main wx.Config object, used by all windows
    fileHistory = None              # wx.FileHistory object

    def OnInit(self):
        self.config = wx.Config()

        # File history
        self.fileHistory = wx.FileHistory()
        self.config.SetPath("/RecentFiles")
        self.fileHistory.Load(self.config)

        # Create post window
        self.theMainFrame = PsycolliderPostWindow(None, -1, "SuperCollider (Post Window)")
        self.theMainFrame.Show(True)
        self.SetTopWindow(self.theMainFrame)

        # enable images for html
        wx.InitAllImageHandlers()

        # Set the log sink function (writes to post window)
        # On windows, we can write directly to it, and using the PostText function
        # causes the post window to be updated slightly later which doesn't look too nice.
        #
        # Can't do this on Linux, as gtk is not thread safe, so must use PostText.
        if(os.name == 'nt'):
            PySCLang.setSCLogSink(WriteInLogWindow)
        else:
            PySCLang.setSCLogSink(PostText)

        PySCLang.setPyPrOpenWinTextFile(OpenTextFile)

        if not self.ChangeDirToSCClassLibraryFolder():
            return False

        PySCLang.start()

        (addr, port) = self.GetServerAddressAndPort()

        return True

    def doFileHistory(self, event):
        """Open a file from file history"""
        fileNumber = event.GetId() - wx.ID_FILE1
        filename = self.fileHistory.GetHistoryFile(fileNumber)
        newWindow = self.OpenFile(filename)
        if newWindow != None:
            self.openWindows.append(newWindow)
            self.AddFileToHistory(filename) # add it back to the history so it will be moved up the list

    def GetServerAddressAndPort(self):
        return ("127.1.0.1", "57110")

    def ChangeDirToSCClassLibraryFolder(self):
        # first, we check if the current working folder
        # contains an item called 'SCClassLibrary'
        curPath = os.getcwd()
        listOfFolders = os.listdir(curPath)

        # if the cwd contains 'SCClassLibrary', we're done
        if 'SCClassLibrary' in listOfFolders:
            return True

        # uniqueName is what gets returned from config.Read(...)
        # if nothing was stored in the config yet
        uniqueName = "{1FB0EC09-A883-4684-AD73-1D49A98A89DE}"
        self.config.SetPath("/GeneralSettings")
        classLibPath = self.config.Read("SCClassLibraryPath", uniqueName)
        leafName = (os.path.split(classLibPath))[1]

        # if the folder stored in the config is actually an existing
        # folder called 'SCClassLibrary', we change cwd to that
        # folder and we're done
        if os.path.isdir(classLibPath) and leafName == 'SCClassLibrary':
            classLibPath_split = os.path.split(classLibPath)
            classLibParentFolder = classLibPath_split[0]
            os.chdir(classLibParentFolder)
            return True

        # if something was stored in the config, but does not exist
        # anymore or is not correct, let's warn the user
        if classLibPath != uniqueName:
            wx.MessageBox("The path stored in the application preferences is not a valid SCClassLibrary folder. You will now be requested to select an existing SCClassLibrary folder","Error", wx.OK | wx.ICON_ERROR)

        # ask the user to locate the folder
        continueLookingForFolder = True
        classLibFolderFound = False
        while continueLookingForFolder:
            dlg = wx.DirDialog(None, "Please locate the SCClassLibrary")
            if dlg.ShowModal() == wx.ID_CANCEL:
                wx.MessageBox("Sorry. No class library available. SuperCollider will not work correctly","Error", wx.OK | wx.ICON_ERROR)
                continueLookingForFolder = False
            else:
                classLibPath = dlg.GetPath()
                leafName = (os.path.split(classLibPath))[1]
                if leafName != 'SCClassLibrary':
                    wx.MessageBox("The folder needs to be called SCClassLibrary for SuperCollider to work correctly", "Error", wx.OK | wx.ICON_ERROR)
                else:
                    continueLookingForFolder = False
                    classLibFolderFound = True

        # only if a valid SCClassLibrary folder was found, then
        # set the current folder as its parent
        if classLibFolderFound:
            self.config.Write("SCClassLibraryPath",classLibPath)
            classLibPath_split = os.path.split(classLibPath)
            classLibParentFolder = classLibPath_split[0]
            os.chdir(classLibParentFolder)
            return True
        else:
            return False

    def NewCodeWindow(self):
        window = PsycolliderCodeWin(self.theMainFrame, -1, "Untitled %d" % (len(self.openWindows)+1))
        self.openWindows.append(window)
        window.Show(True)
        window.SetSubWinFocus()
        return window

    def NewHTMLWindow(self, filepath):
        window = PsycolliderHTMLWin(self.theMainFrame, -1, 
            os.path.splitext(os.path.basename(filepath))[0])
        self.openWindows.append(window)
        window.Show(True)
        window.SetSubWinFocus()
        return window

    def ClosedWindow(self, window):
        try:
            self.openWindows.remove(window)
        except:
            pass

    def HtmlToCode(self, window):
        if type(window) == PsycolliderHTMLWin:
            text = window.htmlSubWin.ToText()
            window.Destroy()
            newWindow = PsycolliderCodeWin(self.theMainFrame, -1, "Untitled %d" % (len(self.openWindows)+1))
            self.openWindows.append(newWindow)
            newWindow.codeSubWin.AddText(text)
            newWindow.Show(True)
            newWindow.SetSubWinFocus()
            return newWindow

    def OpenFile(self, path=''):
        if path == '':
            fileDlg = wx.FileDialog(self.theMainFrame, style=wx.OPEN)
            if not fileDlg.ShowModal() == wx.ID_OK:
                return

            path = fileDlg.GetPath()
            self.AddFileToHistory(path)

        try:
            file = open(path ,"r")
            textContent = file.read()
            file.close()
        except:
            # Todo: better error handling? Just print error message for now
            wx.MessageBox("Psycollider Error: Could not open file " + path)
            return None

        if textContent[0:5] == '{\\rtf':
            win = self.NewCodeWindow()
            win.codeSubWin.AddTextUTF8('Sorry, still no RTF support, wxRichTextControl does not yet support reading RTF files...')
            win.isModified = False
            return win

        elif (textContent.find('<html') >= 0 or textContent.find('<HTML') >= 0):
            win = self.NewHTMLWindow(path)
            win.htmlSubWin.LoadPage(path)
            return win

        else:
            # everything else is plain text code window
            win = self.NewCodeWindow()
            win.codeSubWin.AddTextUTF8(textContent)
            win.filePath = path
            win.SetTitle(path)
            win.isModified = 0
            return win

    def StopServer(self):
        if PySCLang.compiledOK():
            PySCLang.setCmdLine('s.sendMsg("/quit");')
            PySCLang.sendMain("interpretPrintCmdLine")

    def StopSwingOSC(self):
        if PySCLang.compiledOK():
            PySCLang.setCmdLine('SwingOSC.default.sendMsg("/quit");')
            PySCLang.sendMain("interpretPrintCmdLine")

    def Run(self):
        PySCLang.sendMain("run");

    def Stop(self):
        PySCLang.sendMain("stop");

    def CompileLibrary(self):
        self.StopServer()
        self.StopSwingOSC()
        time.sleep(1)
        PySCLang.compileLibrary()

    def OpenClassDef(self, text):
        PySCLang.setCmdLine(text)
        PySCLang.sendMain("openWinCodeFile")

    def ImpOf(self, text):
        PySCLang.setCmdLine(text)
        PySCLang.sendMain("methodTemplates")

    def RefsTo(self, text):
        PySCLang.setCmdLine(text)
        PySCLang.sendMain("methodReferences")

    def Eval(self, text):
        PySCLang.setCmdLine(text)
        PySCLang.sendMain("interpretPrintCmdLine")

    def GoToHelpFile(self, sel=""):
        # TODO : test this : most help files don't open. remove trailing and leading spaces from sel, too, since wxTextCtrl is behaving strangely
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
            for helpFolder in [gHelpFolder, gUserExtensionFolder]:
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

    def ClearPostWindow(self):
        self.theMainFrame.log.Clear()

    def SetDefaultWindowSize(self, sizeX, sizeY):
        self.config.SetPath("/WindowSettings")
        self.config.WriteInt('DefaultSizeX', sizeX)
        self.config.WriteInt('DefaultSizeY', sizeY)
        WriteInLogWindow("Set default window size to " + str(sizeX) + " x " + str(sizeY) + "\n")

    def ClearRecentFileList(self):
        numFiles = self.fileHistory.GetCount()
        for i in range(numFiles):
            self.fileHistory.RemoveFileFromHistory(0)   # remove the first file every time

    def AddFileToHistory(self, path):
        self.fileHistory.AddFileToHistory(path)

    def GetOpenWindows(self):
        return self.openWindows

    def Shutdown(self):
        # Recent file list
        self.config.SetPath("/RecentFiles")
        self.fileHistory.Save(self.config)
        del self.fileHistory

        # stop scsynth and swingosc
        self.StopServer()
        self.StopSwingOSC()


# ---------------------------------------------------------------------
# WriteInLogWindow
def WriteInLogWindow(text):
    if wx.GetApp().theMainFrame == None:
        sys.stdout.write(text)
    else:
        wx.GetApp().theMainFrame.log.AppendText(text)

def PostText(text):
    wx.CallAfter(WriteInLogWindow, text)

# ---------------------------------------------------------------------
def OpenTextFile(path, rangeStart, rangeSize):
    if wx.GetApp().theMainFrame == None:
        wx.MessageBox("Cannot open %s since the main window is not created yet","Error",wx.OK | wx.ICON_ERROR)
    else:
        codeWin = wx.GetApp().OpenFile(path)
        #codeWin.SelectRange(rangeStart,rangeSize)
        return codeWin

# ---------------------------------------------------------------------
# Main
app = Psycollider(0)
app.MainLoop()
