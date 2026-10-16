# Getting started with Pixelate

Pixelate is a small Windows UI library and interpreted language for creating graphical applications with .pxl files.

This repository includes the interpreter executable, example programs, and a VS Code extension prototype.

## Requirements

- Windows
- A cloned copy of this GitHub repository
- VS Code, if you want syntax highlighting and the run button

No compiler is required just to run the included `bin\pixelate.exe`.

## Run your first program

Clone or download the repository, open PowerShell in the repository folder, and run:

~~~powershell
.\bin\pixelate.exe programs\demos\01_variables.pxl
~~~

You can run the other examples with:

~~~powershell
.\bin\pixelate.exe programs\demos\02_conditions.pxl
.\bin\pixelate.exe programs\demos\09_multiple_windows.pxl
~~~

## Create a program

Create a file such as programs/my_app.pxl:

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

Run it with:

~~~powershell
.\bin\pixelate.exe programs\my_app.pxl
~~~

## VS Code extension

The pixelate folder contains the extension source. To use it during development:

1. Open the repository in VS Code.
2. Open the pixelate folder as an extension project, or open the repository and press F5 to launch an Extension Development Host.
3. Open a .pxl file in the development host.
4. Use the Run Pixelate button or the Command Palette command Pixelate: Run Current File.

The VS Code extension runs `pixelate.exe` from the system `PATH` by default:

~~~text
pixelate.exe
~~~

## Documentation

- [Language reference](language.md)
- [Windows and widgets](ui.md)
- [Events and mouse support](events.md)
- [VS Code extension](vscode.md)
- [Architecture and platform backends](architecture.md)

## Project examples

- programs/01_variables.pxl demonstrates primitive variables and formatted strings.
- programs/02_conditions.pxl demonstrates if/else and boolean conditions.
- programs/03_loops.pxl demonstrates while loops, for loops, and increments.
- programs/04_updates.pxl demonstrates single-field and block updates.
- programs/05_buttons.pxl demonstrates button actions and state changes.
- programs/06_mouse.pxl demonstrates mouse coordinates and cursor shapes.
- programs/07_triggers.pxl demonstrates live condition triggers.
- programs/08_textbox.pxl demonstrates editable textbox input and reading its contents.
- programs/10_textarea.pxl demonstrates multiline textarea input and reading its contents.
- programs/11_fonts.pxl demonstrates widget font and font-size styling.
- programs/09_multiple_windows.pxl demonstrates identified windows and `switch_view`.
- programs/demo.pxl is a general feature demonstration combining several concepts.
- programs/control_flow.pxl is a larger combined example of conditions, loops, triggers, and mouse coordinates.
- programs/example.pxl is a smaller language example.

Pixelate is under development, so syntax and runtime behavior may change.

