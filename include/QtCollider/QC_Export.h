#ifndef QC_EXPORT_H
#define QC_EXPORT_H

#if defined _WIN32 || defined __CYGWIN__
  #define QT_COLLIDER_IMPORT __declspec(dllimport)
  #define QT_COLLIDER_EXPORT __declspec(dllexport)
#else
  #if __GNUC__ >= 4
    #define QT_COLLIDER_IMPORT __attribute__ ((visibility("default")))
    #define QT_COLLIDER_EXPORT __attribute__ ((visibility("default")))
  #else
    #define QT_COLLIDER_IMPORT
    #define QT_COLLIDER_EXPORT
  #endif
#endif

#ifdef QT_COLLIDER_EXPORTING // if QtCollider is being built, instead of used
  #define QC_PUBLIC QT_COLLIDER_EXPORT
#else
  #define QC_PUBLIC QT_COLLIDER_IMPORT
#endif

#endif // QC_EXPORT_H
