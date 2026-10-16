# VS Code extension

The pixelate folder contains the VS Code extension prototype.

## Features

- PXL syntax highlighting
- Diagnostics for common syntax mistakes
- Hover documentation for keywords and widgets
- A run button and command for the current .pxl file
- Pixelate file icons

## Development use

Open the repository in VS Code and press F5 to launch an Extension Development Host. Open a .pxl file in the new host window.

Run the current file with the Pixelate: Run Current File command or the run button.

The extension defaults to:

~~~text
${workspaceFolder}/pixelate.exe
~~~

Change the executable path through the `pixelate.executablePath` setting when
necessary. On Linux, point it at the CMake-built `pixelate` executable, for
example `${workspaceFolder}/build/pixelate`.

