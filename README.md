# Sprite / Layer2 / Music Example for zx next

Forked from https://github.com/ncot-technology/specnext-sprites-example/

Use the following command to compile, since the cmake / make currently does not work

```
zcc +zxn  -subtype=nex -v -SO3 --list -m -s -clib=new -Iinclude -startup=1 -Cz"--clean" -create-app  src/main.c src/layer2.c src/sprites.c -o build/zxn_sprite_1.nex  -L/Users/mike/src/zxnext_layer2/lib/sccz80/
```

You will need the files from the "assets" folder copied to the same place as the resulting .nex file

