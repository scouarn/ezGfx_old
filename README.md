ezGfx
=====


Semi-private repo for a simple cross-platform graphics library project.
Detailed documentation is not yet available. 



TODO
====

## 3D ext

https://www.youtube.com/watch?v=nBzCS-Y0FcY&t=853s

* get back the Vertex struct
* xTop / xMid = ... * slope => can be simplified ?
* 3D faces drawn on top of each other -> depth buffuring
* textri 
* textri with no perspective correction (PS1 look)

* Bug in 3D demo with 32 bits systems -> bug with alpha blending returning black ? Solved with the normalize bug ? double/float problem with vectors ?

* normal information
* mesh files with material color
* mesh files with uv

* quaternion rotation on vec4f ?
* Other projection matrices

* swap instead of xLeft / xRight => no clipping...



## Misc

* matrix inversion
* github/readme display gif
* port the new triangle routine to draw2D
* check key behaviour linux vs windows
* fullscreen (?)
* Write documentation
* Redo vectors again...


## New Extensions

* GUI : windows, textboxes, buttons, sliders...
* glsl fragment shaders => standalone project
* splines calculation / bezier curves 2D/3D
* voxel engine
* physics

* lua / python / lisp / perl / js / bash / brainfuck / BNF defined / ocaml/ ??
	=> on the fly compiled scripting language ?
	https://github.com/lua/lua





## Platforms

* wasm support
* linux with no WM


## 2D ext

* dithering / demo
* "mode 7" demo



## Sound

* oscilloscope emulator demo
* windows sound
* no alsa
* on the fly file reading (no pcm array but "stream"... block by block)
* oscillo sound viewer
* midi / sequencer
* instruments
* score / midi editor
* FFT
* audio in


## Fonts

* UTF fonts
* TTF fonts

## Formats

* mesh files & materials
* midi
* vst
* png
* jpeg
* font ttf
* config files
* json files

