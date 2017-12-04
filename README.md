## Build

1. Make sure you have Qt 5.9.3 and Qt Creator
2. Open the project in Qt creator
3. build it
4. do ``cp -R /Applications/Stremio.app/Contents/Resources/WCjs/lib/ build-stremio-Desktop_Qt_5_7_0_clang_64bit-Debug/stremio.app/Contents/MacOS/lib``



## Command line to build:

```
( cd deps/qBreakpad/handler ; qmake ; make )
qmake
make
```

## Releasing a version

1. Bump the version in the `stremio.pro` file
2. Create a git tag with the corresponding version
