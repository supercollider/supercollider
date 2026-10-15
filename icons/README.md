# Icons

## Apple *Squircle* icon

macOS 26 introduced *squircle* icons (see `sc_ide.icon`) which can be edited by using the [*Icon composer*](https://developer.apple.com/documentation/xcode/creating-your-app-icon-using-icon-composer).
Currently only the IDE has such a *squircle* icon.

The *compiled* files are checked into the repo, but if the icon should change, the folowing files need to be re-generated

* the `car` file (see `sc_ide_squircle/Assets.car`)
* a `icns` file (see `sc_ide_squircle/sc_ide.icns`)

`actool` requires to create a `sc_ide_icon.plist`, but we already wire everything up via CMake, so we can simply (git)ignore this file.



```shell
actool sc_ide.icon \
    --compile sc_ide_squircle \
    --app-icon sc_ide \
    --platform macosx \
    --output-partial-info-plist sc_ide_icon.plist \
    --minimum-deployment-target 12.0
```
