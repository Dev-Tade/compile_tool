# Compile Tool
Simple and easy to use GUI tool to build C source code built on top of [Raylib](https://github.com/raysan5/raylib) and [Raygui](https://github.com/raysan5/raygui)

---

<img src="screenshots/Screenshot0.png" width=260 height=210></img>
<img src="screenshots/Screenshot1.png" width=260 height=210></img>
<br>
<img src="screenshots/Screenshot2.png" width=260 height=210></img>
<img src="screenshots/Screenshot3.png" width=260 height=210></img>

---

## Building
The provided makefile makes easy to build the project, but it requires to have raylib (either built from source or prebuilt) and raygui
* Raylib from source code (You already should have built raylib)
```console
make RAYLIB_PATH=<path to raylib directory> RAYGUI_INCLUDE=<path to directory containing raygui header file>
```
* Raylib precompiled
```console
make RAYLIB_INCLUDE=<path to directory containing raylib header files> RAYLIB_LIBRARY=<path to directory containing raylib library files> RAYGUI_INCLUDE=<path to directory containing raygui header file>
```

---

### Features
1. Drag and drop files to track them
2. Recursively tracks dropped directories
3. Trims the path to the local one
4. Custom window title bar (it does not use system native one)

### Limitations
* Right now the `only compiler is gcc` because is **hardcoded**
* You can't see compiler output if an error ocurred
* May be really slow for a large amount of files

---

### Future

Application layout was done using [rGuiLayout](https://github.com/raysan5/rguilayout), but it will be replaced with a manually coded one (or loaded from rGuiLayout file, it would be better for user since it would be able to customize the layout)

[rini](https://github.com/raysan5/rini) will be used for saving project build configurations