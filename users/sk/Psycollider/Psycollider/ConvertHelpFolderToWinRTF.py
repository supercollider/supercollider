import win32com.client
import os
import sys

def ConvertFile(wordApp,filePath):
  wordApp.Documents.Open(filePath)
  wordApp.Run("SCMacRTFToWinRTF")
  wordApp.ActiveDocument.Save()
  wordApp.ActiveDocument.Close()

wordApplication = win32com.client.Dispatch('Word.application')

#ProcessFile(wordApplication,"C:\\__p4depot\\rnd\\usersCorner\\golinvauxb\\_own\\_cvs_supco\\SuperCollider3\\build\\Help_windows\\Help.help.rtf")

helpFolder = os.getcwd()

for folderPath, foldersInPath, fileNamesInFolder in os.walk(helpFolder):
  for fileName in fileNamesInFolder:
    if fileName[-4:] == ".rtf":
      filePath = os.path.join(folderPath, fileName)
      sys.stdout.write("Processing " + str(filePath) + "\n")
      ConvertFile(wordApplication,filePath)

# THE FOLLOWING MACRO NEEDS TO BE ADDED TO WINWORD.EXE :

#Sub SCMacRTFToWinRTF()
#'
#' Macro1 Macro
#' Macro enregistrée le 16/10/2004 par  Benjamin Golinvaux
#'
    
#' Helvetica --> Arial
#    Selection.Find.ClearFormatting
#    Selection.Find.Replacement.ClearFormatting
#    With Selection.Find.Font
#        .Name = "Helvetica"
#    End With
#    With Selection.Find
#        .Text = ""
#        .Replacement.Text = ""
#        .Replacement.Font.Name = "Arial"
#        .Forward = True
#        .Wrap = wdFindContinue
#        .Format = True
#        .MatchCase = False
#        .MatchWholeWord = False
#        .MatchWildcards = False
#        .MatchSoundsLike = False
#        .MatchAllWordForms = False
#    End With
#    Selection.Find.Execute Replace:=wdReplaceAll

#' Helvetica-Bold --> Arial bold
#    Selection.Find.ClearFormatting
#    Selection.Find.Replacement.ClearFormatting
#    With Selection.Find.Font
#        .Name = "Helvetica-bold"
#    End With
#    With Selection.Find
#        .Text = ""
#        .Replacement.Text = ""
#        .Replacement.Font.Name = "Arial"
#        .Replacement.Font.Bold = True
#        .Forward = True
#        .Wrap = wdFindContinue
#        .Format = True
#        .MatchCase = False
#        .MatchWholeWord = False
#        .MatchWildcards = False
#        .MatchSoundsLike = False
#        .MatchAllWordForms = False
#    End With
#    Selection.Find.Execute Replace:=wdReplaceAll

#' Monaco --> Courier New
#    Selection.Find.ClearFormatting
#    Selection.Find.Replacement.ClearFormatting
#    With Selection.Find.Font
#        .Name = "Monaco"
#    End With
#    With Selection.Find
#        .Text = ""
#        .Replacement.Text = ""
#        .Replacement.Font.Name = "Courier New"
#        .Forward = True
#        .Wrap = wdFindContinue
#        .Format = True
#        .MatchCase = False
#        .MatchWholeWord = False
#        .MatchWildcards = False
#        .MatchSoundsLike = False
#        .MatchAllWordForms = False
#    End With
#    Selection.Find.Execute Replace:=wdReplaceAll
#End Sub



