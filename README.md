# Sprite / Layer2 / Music Example for zx next

Forked from https://github.com/ncot-technology/specnext-sprites-example/

This is a "playground" project in order to test the development environment

You may need to change the paths in cmake/next.cmake to the place your z88dk is installed.


```
set(CMAKE_FIND_ROOT_PATH  /Users/mike/src/stuff/z88dk/)
```

# Building 

## CMAKE

```
mkdir build && cd build
cmake ..
make
make install
```

This will produce a build in the "bin" directory at the root of your project which you can sync to the next or copy the entire folder to your SD card.

## Command line
Alternatively You can build the current version using the following command

```
zcc +zxn  -subtype=nex -vn -SO3 --list -m -s -clib=new -Iinclude -startup=1 -Cz"--clean" -create-app  src/main.c src/layer2.c src/sprites.c src/stuff.asm src/mysong.asm -o build/zxn_sprite_1.nex  -L/Users/mike/src/zxnext_layer2/lib/sccz80/
```

You will need the files from the "assets" folder copied to the same place as the resulting .nex file


# Running the example

After copying the files from ./bin to your next directory, run the "zxn_sprite_1.nex" binary

Music should start playing, and a layer2 image (offset) should be drawn.

Upon pressing any key, fish sprites will spawn and bounce around the screen to the currently sprite limit (128).

The border illustrates some of the time the code takes to run. 

