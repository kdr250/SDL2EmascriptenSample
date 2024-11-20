# SDL2EmscriptenSample
SDL2 + CMake + Emscripten のサンプルです。
Webブラウザ上でSDL2で作ったゲームが動作します。

## ビルド方法
### ネイティブの場合
VSCodeの拡張機能でCMakeを入れて、ビルドして実行する。 

### Webの場合
1. emsdkを有効にする。 `/your/path/to/emsdk/emcmdprompt`を実行。
2. 本プロジェクト直下で `emcmake cmake -B build-web`を実行。
3. 続けて `cmake --build build-web`を実行。
4. Webサーバー起動。 `python -m http.server -d build-web`

## 参考にしたURL
- [SDL Wiki > Emscripten](https://wiki.libsdl.org/SDL2/README/emscripten)
- [WebGPU C++ guide > Building for the Web](https://eliemichel.github.io/LearnWebGPU/appendices/building-for-the-web.html)
- [cmake で依存ライブラリをダウンロードする](https://qiita.com/ousttrue/items/4fa7a786a6c51e9f11f0#sdl2-%E3%81%A8-freetype)
- [CMakeとvcpkgの連携](https://qiita.com/ryutorion/items/8ed357b8c44d31521327#cmake%E3%81%A8vcpkg%E3%81%AE%E9%80%A3%E6%90%BA)
