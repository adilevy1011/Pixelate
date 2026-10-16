# Windows and widgets

## Window options

The window block supports:

| Property | Description |
| --- | --- |
| title | Window title text. |
| width | Window width in pixels. |
| height | Window height in pixels. |
| iconPath | Path to an .ico file. |
| background | RGB color with channels from 0 to 255. |

Defaults are 800x600, Pixelate App, window\\pixelate.ico, and a white background.

~~~pxl
window {
    title: "My App",
    width: 900,
    height: 650,
    iconPath: "window\\pixelate.ico",
    background: 245 247 252,
}
~~~

## Labels, buttons, and textboxes

Labels, buttons, and textboxes support text, x, y, width, and height.

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
~~~

Widgets can be declared inside window or added inside script.

Textboxes are editable while the program is running. The optional
`placeholder` is shown only while the textbox is empty. Read the current
contents with `textboxId.text`, including inside an f-string:

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
