# Architecture and platform backends

Pixelate is split into a platform-neutral UI layer and platform-specific
backends.

## Platform-neutral layer

These parts should remain reusable across Windows, macOS, and Linux:

- `runtime/` — executes PXL programs.
- `language/` — lexer, parser, AST, and interpreter.
- `window/ui/widget.h` — widget geometry, hit testing, and widget behavior.
- `window/ui/label.*`, `window/ui/button.*`, and `window/ui/textbox.*` — public widget implementations.
- `window/ui/draw_context.h` — abstract drawing operations used by widgets.
- `window/platform/platform_types.h` — platform-neutral colors, cursor names, and window options.
- `window/platform/native_textbox.h` — the platform-neutral editable textbox contract.

The generic widgets do not need to know whether they are running on Win32,
Cocoa, or X11/Wayland.

## Current Windows backend

Windows-specific code is under `window/platform/windows/`:

- `win32_draw_context.*` implements drawing with GDI.
- `win32_native_textbox.cpp` implements the native editable Windows textbox.
- `window/app.cpp` currently contains the Win32 window/event-loop adapter.

The `App` public API remains unchanged, so the runtime and PXL programs do not
need to change when a new backend is introduced.

## Adding another backend

A future backend should provide implementations of the drawing context,
native textbox contract, and the App window/event-loop adapter while keeping
the public interfaces and platform-neutral types the same. The Windows files
should remain isolated from the new backend.

The current build is intentionally still Windows-specific. This refactor
creates the boundaries incrementally without changing current behavior.
