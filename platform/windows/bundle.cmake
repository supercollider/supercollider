include(BundleUtilities)

foreach(exe scide sclang scsynth)
  fixup_bundle(
    "${SC_INSTALL_PREFIX}/SuperCollider/${exe}.exe"
    ""
    "${SC_DEPENDENCY_DIRS}"
  )
endforeach()
