#! /bin/sh

if ! [ -x "$(command -v inkscape)" ]; then
    echo "'inkscape' was not found. Please install it."
    exit 1
fi

if ! [ -x "$(command -v optipng)" ]; then
    echo "'optipng' was not found. Please install it."
    exit 1
fi

echo "Generating 'icon_512x512@2.png'"
inkscape "icon/icon_512x512@2.svg" --export-width=1024 --export-filename="icon/icon_512x512@2.png"
optipng -quiet "icon/icon_512x512@2.png"

echo "Generating 'icon_256x256@2.png'"
inkscape "icon/icon_512x512@2.svg" --export-width=512 --export-filename="icon/icon_256x256@2.png"
optipng -quiet "icon/icon_256x256@2.png"

echo "Generating 'icon_128x128@2.png'"
inkscape "icon/icon_512x512@2.svg" --export-width=256 --export-filename="icon/icon_128x128@2.png"
optipng -quiet "icon/icon_128x128@2.png"

echo "Generating 'icon_32x32@2.png'"
inkscape "icon/icon_128x128.svg" --export-width=64 --export-filename="icon/icon_32x32@2.png"
optipng -quiet "icon/icon_32x32@2.png"

echo "Generating 'icon_16x16@2.png'"
inkscape "icon/icon_16x16@2.svg" --export-width=32 --export-filename="icon/icon_16x16@2.png"
optipng -quiet "icon/icon_16x16@2.png"

echo "Generating 'icon_512x512.png'"
inkscape "icon/icon_512x512@2.svg" --export-width=512 --export-filename="icon/icon_512x512.png"
optipng -quiet "icon/icon_512x512.png"

echo "Generating 'icon_256x256.png'"
inkscape "icon/icon_512x512@2.svg" --export-width=256 --export-filename="icon/icon_256x256.png"
optipng -quiet "icon/icon_256x256.png"

echo "Generating 'icon_128x128.png'"
inkscape "icon/icon_128x128.svg" --export-width=128 --export-filename="icon/icon_128x128.png"
optipng -quiet "icon/icon_128x128.png"

echo "Generating 'icon_32x32.png'"
inkscape "icon/icon_16x16@2.svg" --export-width=32 --export-filename="icon/icon_32x32.png"
optipng -quiet "icon/icon_32x32.png"

echo "Generating 'icon_16x16.png'"
inkscape "icon/icon_16x16@2.svg" --export-width=16 --export-filename="icon/icon_16x16.png"
optipng -quiet "icon/icon_16x16.png"

echo "Generating 'icon_256.png'"
inkscape "icon/icon_1024.svg" --export-width=256 --export-filename="icon/icon_256.png"
optipng -quiet "icon/icon_256.png"

echo "Generating 'icon_48.png'"
inkscape "icon/icon_1024.svg" --export-width=48 --export-filename="icon/icon_48.png"
optipng -quiet "icon/icon_48.png"

echo "Generating 'icon_32.png'"
inkscape "icon/icon_1024.svg" --export-width=32 --export-filename="icon/icon_32.png"
optipng -quiet "icon/icon_32.png"

echo "Generating 'icon_16.png'"
inkscape "icon/icon_1024.svg" --export-width=16 --export-filename="icon/icon_16.png"
optipng -quiet "icon/icon_16.png"

if ! [ -x "$(command -v icotool)" ]; then
    echo "'icotool' was not found. Please install it. This is part of icoutils."
    exit 1
fi

echo "Generating 'icon.ico'"
icotool -c -o "icon/icon.ico" "icon/icon_16.png" "icon/icon_32.png" "icon/icon_48.png" "icon/icon_256.png"
