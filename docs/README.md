# Getting started with Pixelate

Pixelate is a small Windows UI library and interpreted language for creating graphical applications with .pxl files.

This repository includes the interpreter executable, example programs, and a VS Code extension prototype.

## Requirements

- Windows
- A cloned copy of this GitHub repository
- VS Code, if you want syntax highlighting and the run button

No compiler is required just to run the included pixelate.exe.

## Run your first program

Clone or download the repository, open PowerShell in the repository folder, and run:

~~~powershell
.\pixelate.exe programs\demo.pxl
~~~

You can run the other examples with:

~~~powershell
.\pixelate.exe programs\control_flow.pxl
.\pixelate.exe programs\example.pxl
~~~

## Create a program

Create a file such as programs/my_app.pxl:

~~~pxl
window {
    title: "My First Pixelate App"
    width: 800
    height: 600
    background: 240 245 250
}

script {
    add label greeting {
        text: "Hello from Pixelate!"
        x: 50
        y: 50
        width: 300
        height: 40
    }
}
~~~

Run it with:

~~~powershell
.\pixelate.exe programs\my_app.pxl
~~~

## VS Code extension

The pixelate folder contains the extension source. To use it during development:

1. Open the repository in VS Code.
2. Open the pixelate folder as an extension project, or open the repository and press F5 to launch an Extension Development Host.
3. Open a .pxl file in the development host.
4. Use the Run Pixelate button or the Command Palette command Pixelate: Run Current File.

The extension expects the executable at:

~~~text
${workspaceFolder}/pixelate.exe
~~~

## Documentation

- [Language reference](language.md)
- [Windows and widgets](ui.md)
- [Events and mouse support](events.md)
- [VS Code extension](vscode.md)

## Project examples

- programs/demo.pxl is a general feature demonstration.
- programs/control_flow.pxl demonstrates conditions, loops, triggers, and live mouse coordinates.
- programs/example.pxl is a smaller language example.

Pixelate is under development, so syntax and runtime behavior may change.

