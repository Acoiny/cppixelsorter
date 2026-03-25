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
It is completely handwritten using SDL3.
To start it, pass the `-g` or `--gui` flag to the program.

## Building
To build the program a few libraries are required.

- `SDL3` and `SDL3-ttf`
- `libdbus1`

When these libraries are installed, simply run `make` and the project should be
built.

> [!WARNING]
> Currently, the project only builds on Linux systems and uses
> dbus for the file-picker!
