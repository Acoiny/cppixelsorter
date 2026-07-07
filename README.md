# cppixelsorter
A basic pixelsorter programmed in C++.

This project is _heavily_ inspired by
[pixelsortery](https://github.com/Lxtharia/pixelsortery) (heck, it's basically a clone), which in turn
was inspired by [ASDFPixelSort](https://github.com/kimasendorf/ASDFPixelSort).

> [!CAUTION]
> The project is still under active development and may or may currently just
> not work :P

## Usage
### CLI
```bash
./cppixelsorter <input_file> -h <hue_value> -o <output_file>
```

### GUI
`cppixelsorter` now includes a GUI! 🥳
It is completely handwritten using SDL3. To start it simply run the
program without any arguments.
If you want to pass an input file you can pass the `-g` or `--gui`
flag to the program and it will start in GUI mode with the image loaded.

For the GUI it uses the [sui](https://github.com/acoiny/sui) library.

## Building
To build the program a few libraries are required.

- `SDL3` and `SDL3-ttf`

When these libraries are installed, simply run `make` and the project should be
built.

> [!NOTE]
> The program should compile on windows (there is a branch that does this
> inside a docker container) but it hasn't been tested much.
