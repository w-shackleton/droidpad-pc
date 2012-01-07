#!/bin/sh
cd data

# Create resized versions
convert droidpad.png -resize 16x16 icon-16.png
convert droidpad.png -resize 32x32 icon-32.png
convert droidpad.png -resize 64x64 icon-64.png
convert droidpad.png -resize 128x128 icon-128.png

# Problem with transparency, using online converter.
convert icon-16.png icon-32.png icon-64.png icon-128.png droidpad.png -colors 256 -alpha on -background none -flatten icon.ico

convert droidpad.png -resize 128x128 iconlarge.xpm
convert droidpad.png -resize 32x32 icon.xpm

rm icon-16.png
rm icon-32.png
rm icon-64.png
rm icon-128.png
