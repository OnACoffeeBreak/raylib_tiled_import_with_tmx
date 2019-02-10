# Example of Importing Tiled Map into Raylib with TMX C Loader

This example uses [TMX C Loader library](https://github.com/baylej/tmx) to read [Tiled Map Editor](https://www.mapeditor.org/) file stored in its native [TMX file format](https://doc.mapeditor.org/en/stable/reference/tmx-map-format/). It then allows for camera navigation of the imported map.

TMX Loader has dependencies on libxml2 and zlib. A lighter-weight option might be to export a Tiled map into JSON format and use a much simpler library to load the file. See https://github.com/OnACoffeeBreak/raylib_tiled_import_with_json/

# Build

Tested in MinGW64 on Windows 10. Steps to build from a virgin Windows 10 install:

1. Download 64-bit install from http://www.msys2.org/
1. Install into default location C:\msys64 and **follow installation instructions from http://www.msys2.org/**
1. Install mingw-w64 toolchain: ```pacman -S mingw-w64-x86_64-toolchain```
1. Install git: ```pacman -S git```
1. Install libxml2: ```pacman -S mingw-w64-x86_64-libxml2```
1. Clone this repo: ```git clone https://github.com/OnACoffeeBreak/raylib_tiled_import_with_json.git```
1. ```cd raylib_tiled_import_with_json```
1. ```mingw32-make```
1. ```./raylib_tiled_import.exe```

# To Do

- [ ] Consider using a CC0 tileset for more engaging map. On the other hand, this will make tiles harder to see.
- [ ] Test on other platforms. How?
- [ ] Use a pixel graphics tileset instead of 64x64 and use camera zoom to create the pixel graphics aesthetic.
- [ ] Add camera boundary detection. Too much for a simple demo?
- [ ] Integrate into raylib exmples. raysan5 doesn't want examples to depend on large libraries. [raylib_tiled_import_with_json](https://github.com/OnACoffeeBreak/raylib_tiled_import_with_json/) with a single-header JSON lib might be more palatable.
