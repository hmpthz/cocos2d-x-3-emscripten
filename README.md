# Cocos2d

Base version is: **cocos2d-x-3.0alpha0-pre** (commit: 50f2cba03fd7bec4cccba86147003fc83d93e610)

Cocos2d-x abandoned support for emscripten since v3.0. There's an emscripten-port repo: [github.com/emscripten-ports/Cocos2d](github.com/emscripten-ports/Cocos2d). Original repo is old and has several issues while trying to build on windows and emscripten, so we forked the repo and did some minor changes.

Only tested on windows (visual studio 2022) and emscripten, because it makes no sense to use this old version Cocos2d-x if you want to build on platforms other than emscripten.

## Manually building

You can build cocos2d by emscripten with this commands:

```
make PLATFORM=emscripten DEBUG=1 -j10 # (for debug build)
make PLATFORM=emscripten DEBUG=0 -j10 # (for release build)
```

## Using as emscripten-ports

Add flags ```-s USE_COCOS2D=3 -s STB_IMAGE=1 ERROR_ON_UNDEFINED_SYMBOLS=0``` to build.

There are issues on TIFF library (which we don't use), flag ```ERROR_ON_UNDEFINED_SYMBOLS=0``` is used for ignoring them.

Other known issues are written on `Emscripten - TODO.md`.

## External

Download and unzip `cocos2d-x-3-port-external.7z` from repo's release.
