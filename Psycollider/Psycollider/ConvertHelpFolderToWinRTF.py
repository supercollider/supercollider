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

