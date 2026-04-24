# Compiling MacSwear

MacSwear builds as a standard Cocoa app with Xcode. The current source tree is set up for modern macOS and still uses OpenGL.

## Requirements

- Xcode 26.3 or newer
- Current macOS SDK

## Build

For a debug build:

```sh
xcodebuild \
  -project MacSwear.xcodeproj \
  -scheme MacSwear \
  -configuration Debug \
  -sdk macosx \
  -derivedDataPath /tmp/MacSwearDerived \
  build
```

For a release build:

```sh
xcodebuild \
  -project MacSwear.xcodeproj \
  -scheme MacSwear \
  -configuration Release \
  -sdk macosx \
  -derivedDataPath /tmp/MacSwearReleaseDerived \
  build
```

## Notes

- The project uses `OpenGL.framework`; deprecation warnings are expected.
- If you want a distributable zip, use `ditto` so the `.app` bundle metadata is preserved:

```sh
ditto -c -k --sequesterRsrc --keepParent \
  /tmp/MacSwearReleaseDerived/Build/Products/Release/MacSwear.app \
  MacSwear-Release-macOS14.zip
```
