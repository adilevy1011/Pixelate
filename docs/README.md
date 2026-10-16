# Getting started with Pixelate

Pixelate is an early-stage UI library and interpreted language for creating graphical applications with `.pxl` files. Windows remains the original
platform; Linux support is experimental and behavior may still change.

## Requirements

- A cloned copy of this repository
- A C++17 compiler and CMake when building from source
- VS Code, if you want syntax highlighting and the run button

### Windows

The Windows backend uses Win32 and GDI. A release may include a prebuilt
`pixelate.exe`, so a compiler is not required just to run that release.

### Linux (experimental)

Install the build and GTK development dependencies:

~~~bash
sudo apt update
sudo apt install build-essential cmake pkg-config libgtk-3-dev libcairo2-dev
~~~

The Linux backend uses GTK 3, Cairo, and Pango. Font appearance depends on
the fonts installed on the system; Windows font names may use Linux fallbacks.

## Build from source

From the repository root:

~~~bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
~~~

The parser smoke test checks all demo programs. GUI behavior is still best
checked by running the demos manually on a desktop session.

## Run your first program

On Windows, open PowerShell in the repository folder and run:

~~~powershell
.\bin\pixelate.exe programs\demos\01_variables.pxl
~~~
or add pixelate.exe to your path and run it simply with:
~~~powershell
pixelate yourProgram.pxl
~~~
On Linux, run the CMake-built executable:

~~~bash
./build/pixelate programs/demos/01_variables.pxl
~~~

You can run the other examples with the same platform-specific command.

## Create a program

Create a file such as `programs/my_app.pxl`:

~~~pxl
window {
    title: "My First Pixelate App",
    width: 800,
    height: 600,
    background: 240 245 250,
}

script {
    add label greeting {
        text: "Hello from Pixelate!",
        x: 50,
        y: 50,
        width: 300,
        height: 40,
    }
}
~~~

Run it with the appropriate command above. For Linux:

~~~bash
./build/pixelate programs/my_app.pxl
~~~

## Installation

After building, install the interpreter, demos, and documentation with:

~~~bash
cmake --install build --prefix "$HOME/.local"
~~~

The executable will be installed under `$HOME/.local/bin`. The installed
demos are under `$HOME/.local/share/pixelate`, and the documentation is under
`$HOME/.local/share/doc/pixelate`.

## VS Code extension

The pixelate folder contains the extension source. To use it during
development, open the repository in VS Code and press F5 to launch an
Extension Development Host. Open a `.pxl` file in the development host and
use the run button or the command `Pixelate: Run Current File`.

The extension's `pixelate.executablePath` setting can point to the platform
executable. On Linux, use `${workspaceFolder}/build/pixelate` during
development.

## Documentation

- [Language reference](language.md)
- [Windows, Linux, and widgets](ui.md)
- [Events and mouse support](events.md)
- [VS Code extension](vscode.md)
- [Architecture and platform backends](architecture.md)

## Project examples

- `programs/demos/01_variables.pxl` through `11_fonts.pxl` demonstrate the
  current language and UI features.
- `programs/demos/09_multiple_windows.pxl` demonstrates identified windows
  and `switch_view`.

Pixelate is under early development. Syntax, APIs, rendering, and platform
behavior may change without compatibility guarantees.
