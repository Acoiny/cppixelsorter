#!/usr/bin/env bash

create-dirs() {
    mkdir -p AppDir/usr/bin
    mkdir -p AppDir/usr/lib
}

create-apprun() {
    cat <<'EOF' > ./AppDir/AppRun
#!/usr/bin/env bash

# so it finds all dependencies
export LD_LIBRARY_PATH=$APPDIR/usr/lib

exec $APPDIR/usr/bin/cppixelsorter "$@"
EOF

chmod +x ./AppDir/AppRun
}

copy-binaries() {
    cp ./cppixelsorter ./AppDir/usr/bin/

    # copying of all dependencies into the appimage
    for l in $(ldd ./cppixelsorter | grep "=>" | awk '{print $3}'); do
        cp $l ./AppDir/usr/lib/
    done
}

copy-assets() {
    cp ./assets/icon.png ./AppDir/cppixelsorter.png
    # create symlink for the diricon
    (cd ./AppDir/ && ln -s cppixelsorter.png .DirIcon)
}

create-desktop-file() {
    cat <<'EOF' > ./AppDir/cppixelsorter.desktop
[Desktop Entry]
Type=Application
Name=Cppixelsorter
Comment=Pixelsorter written in C++
Exec=cppixelsorter -g
Icon=cppixelsorter
Categories=Graphics
EOF
}

package-appimage() {
    ARCH=x86_64 appimagetool ./AppDir cppixelsorter.AppImage
}

main() {
    case $1 in
        clean)
            rm -rf ./AppDir
            rm ./cppixelsorter.AppImage
            exit 0
            ;;
        '')
            ;;
        *)
            echo "Unknown argumetn $1"
            exit 1
            ;;
    esac

    make release

    create-dirs
    create-apprun
    copy-binaries
    create-desktop-file
    copy-assets
    package-appimage
}

main $@
