# Windows and widgets

## Window options

The window block supports:

| Property | Description |
| --- | --- |
| title | Window title text. |
| width | Window width in pixels. |
| height | Window height in pixels. |
| iconPath | Path to an .ico file. Relative paths are resolved from the current directory, then beside `pixelate.exe`. |
| background | RGB color with channels from 0 to 255. |

Defaults are 800x600, Pixelate App, pixelate.ico, and a white background. Place
the default icon beside `pixelate.exe` when launching Pixelate from `PATH`.

~~~pxl
window {
    title: "My App",
    width: 900,
    height: 650,
    iconPath: "pixelate.ico",
    background: 245 247 252,
}
~~~

## Labels, paragraphs, buttons, textboxes, and textareas

Labels, paragraphs, buttons, textboxes, and textareas support text, x, y, width, height, font, and fontSize.

Font constants include `DEFAULT`, `ARIAL`, `CALIBRI`, `CONSOLAS`, `COURIERNEW`,
`TIMESNEWROMAN`, `TAHOMA`, and `VERDANA`.

The default font size is 16 pixels. Set `fontSize` explicitly to use another size.

`label` is intended for short, single-line text. `par` renders text from the
top-left of its bounds, honors explicit newlines, and wraps long lines.

`textarea` is a multiline editable text input. It accepts Enter for newlines and
shows a vertical scrollbar when the content is taller than the control.

~~~pxl
add label greeting {
    text: "Hello!",
    x: 50,
    y: 50,
    width: 300,
    height: 40,
}

add button clickMe {
    text: "Click me",
    x: 50,
    y: 120,
    width: 200,
    height: 50,
}

add textbox nameInput {
    text: "",
    placeholder: "Type here",
    x: 50,
    y: 190,
    width: 300,
    height: 30,
}

add textarea notesInput {
    text: "",
    placeholder: "Write notes...",
    x: 50,
    y: 240,
    width: 400,
    height: 140,
}
~~~

Text styling can be set in a widget declaration:

~~~pxl
add par body {
    text: "This paragraph uses Arial at 18 pixels.",
    font: ARIAL,
    fontSize: 18,
    x: 50,
    y: 400,
    width: 500,
    height: 80,
}
~~~

It can also be changed at runtime with `update body.font = CONSOLAS;` or
`update body.fontSize = 20;`.

The current font and size can be used in conditions:

~~~pxl
if (body.font == ARIAL) {
    body.text = "This is using Arial.";
}

if (body.fontSize >= 18) {
    body.text = "This is large text.";
}
~~~

Numeric font sizes can be incremented directly, for example
`body.fontSize++;`.

They can also be decremented with `body.fontSize--;`.

Widgets can be declared inside window or added inside script.

Textboxes and textareas are editable while the program is running. The optional
`placeholder` is shown only while the textbox is empty. Read the current
contents with `textboxId.text` or `textareaId.text`, including inside an f-string:

~~~pxl
add label result {
    text: "Waiting for input...",
    x: 50,
    y: 240,
    width: 400,
    height: 35,
}

add button showInput {
    text: "Show input",
    x: 50,
    y: 290,
    width: 180,
    height: 45,
    onclick {
        result.text = f"You entered: {nameInput.text}";
    }
}
~~~

## Updating properties

A single property does not need update:

~~~pxl
greeting.text = "New text";
greeting.x = 100;
greeting.width = 300;
window.title = "New title";
~~~

Use an update block to change several fields:

~~~pxl
update greeting {
    text: "Updated text",
    x: 100,
    y: 120,
    width: 300,
    height: 50,
}
~~~
