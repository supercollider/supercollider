// Copyright 2003 Maurizio Umberto Puxeddu

#ifndef _Supercollider_libraryConfig_h_
#define _Supercollider_libraryConfig_h_

class LibraryConfig {
 public:
  LibraryConfig(void);
  virtual ~LibraryConfig();
  
  char **includedDirectories(void);
  char **excludedDirectories(void);

  void postExcludedDirectories(void);
  bool forEachIncludedDirectory(bool (*func)(char *, int));

  bool pathIsExcluded(const char *path);

  void addIncludedDirectory(char *name);
  void addExcludedDirectory(char *name);

 private:
  int m_nIncludedDirectories;
  char **m_includedDirectories;
  int m_nExcludedDirectories;
  char **m_excludedDirectories;
};

extern char *unixStandardizePath(const char *path, char *newpath);

#endif // _Supercollider_libraryConfig_h_
