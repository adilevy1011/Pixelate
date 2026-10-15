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
    title: "My App"
    width: 900
    height: 650
    iconPath: "window\\pixelate.ico"
    background: 245 247 252
}
~~~

## Labels and buttons

The current widget types are label and button. Both support text, x, y, width, and height.

~~~pxl
add label greeting {
    text: "Hello!"
    x: 50
    y: 50
    width: 300
    height: 40
}

add button clickMe {
    text: "Click me"
    x: 50
    y: 120
    width: 200
    height: 50
}
~~~

Widgets can be declared inside window or added inside script.

## Updating properties

A single property does not need update:

~~~pxl
greeting.text = "New text"
greeting.x = 100
greeting.width = 300
window.title = "New title"
~~~

Use an update block to change several fields:

~~~pxl
update greeting {
    text: "Updated text"
    x: 100
    y: 120
    width: 300
    height: 50
}
~~~
