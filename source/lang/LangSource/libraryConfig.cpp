#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/param.h>
#include <unistd.h>

#include "SCBase.h"
#include "libraryConfig.h"

void LibraryConfig::postExcludedDirectories(void) {
  char **directories = m_excludedDirectories;
  if (directories != 0) {
    while (directories[0] != 0) {
      post("\texcluding dir: '%s'\n", directories[0]);
      directories += 1;
    }
  }
}

bool LibraryConfig::forEachIncludedDirectory(bool (*func)(char *, int)) {
  char **directories = m_includedDirectories;
  if (directories != 0) {
    while (directories[0] != 0) {
      if (!func(directories[0], 0)) return false;
      directories += 1;
    }
  }
  return true;
}

LibraryConfig::LibraryConfig(void) :
  m_nIncludedDirectories(0),
  m_nExcludedDirectories(0)
{
  m_includedDirectories = (char **)malloc(sizeof(char *) * 1);
  m_excludedDirectories = (char **)malloc(sizeof(char *) * 1);
  
  m_includedDirectories[0] = 0;
  m_excludedDirectories[0] = 0;
}

LibraryConfig::~LibraryConfig() {
  int i;

  for (i = 0; i <= m_nIncludedDirectories; ++i)
    free(m_includedDirectories[i]);
  for (i = 0; i <= m_nExcludedDirectories; ++i)
    free(m_excludedDirectories[i]);

  free(m_includedDirectories);
  free(m_excludedDirectories);
}

char **LibraryConfig::includedDirectories(void) {
  return m_includedDirectories;
}

char **LibraryConfig::excludedDirectories(void) {
  return m_excludedDirectories;
}

bool LibraryConfig::pathIsExcluded(const char *path) {
  int i;

  if (m_nExcludedDirectories != 0)
    for (i = 0; i < m_nExcludedDirectories; ++i)
      if (strcmp(path, m_excludedDirectories[i]) == 0) return true;
  
  return false;
}

void LibraryConfig::addIncludedDirectory(char *path) {
  char **includedDirectories = m_includedDirectories;

  if (path == 0) return;

  m_includedDirectories = (char **)realloc(m_includedDirectories, (m_nIncludedDirectories + 2) * sizeof(char *));
  if (m_includedDirectories == 0) {
    m_includedDirectories = includedDirectories;
  } else {
    m_includedDirectories[m_nIncludedDirectories] = path;
    m_includedDirectories[m_nIncludedDirectories + 1] = 0;
    m_nIncludedDirectories += 1;
  }
}

void LibraryConfig::addExcludedDirectory(char *path) {
  char **excludedDirectories = m_excludedDirectories;

  if (path == 0) return;

  m_excludedDirectories = (char **)realloc(m_excludedDirectories, (m_nExcludedDirectories + 2) * sizeof(char *));
  if (m_excludedDirectories == 0) {
    m_excludedDirectories = excludedDirectories;
  } else {
    m_excludedDirectories[m_nExcludedDirectories] = path;
    m_excludedDirectories[m_nExcludedDirectories + 1] = 0;
    m_nExcludedDirectories += 1;
  }
}

char *unixStandardizePath(const char *path, char *newpath2) {
  char newpath1[MAXPATHLEN + 1];

  newpath1[0] = '\0';

  newpath2[0] = '\0';
  
  if (path[0] == '~' && path[1] == '/') {
    const char *home = getenv("HOME");

    if (home != 0) {
      strcpy(newpath1, home);
      strcat(newpath1, path + 1);
    } else {
      strcpy(newpath1, path + 2);
    }
  } else {
    strcpy(newpath1, path);
  }
  
  if (realpath(newpath1, newpath2) != newpath2)
    return 0;

  return newpath2;
}
