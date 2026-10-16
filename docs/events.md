# Events and mouse support

## Button clicks

Buttons support onclick blocks containing update actions.

~~~pxl
add button clickMe {
    text: "Click me",

    onclick {
        update greeting.text = "The button was clicked!";
        update window.title = "Clicked";
    }
}
~~~

## Triggers

A mouseMove trigger runs whenever the mouse moves.

~~~pxl
trigger(mouseMove) {
    mouseX.text = f"Mouse X: {mouse.x}";
    mouseY.text = f"Mouse Y: {mouse.y}";
}
~~~

A condition trigger runs when a condition changes from false to true.

~~~pxl
trigger(count == 10) {
    status.text = "Count reached ten.";
}
~~~

## Mouse position

mouse.x and mouse.y are read-only positions relative to the Pixelate window.

## Cursor shapes

Change the cursor with:

~~~pxl
update mouse.shape = HAND;
~~~

Supported shapes:

~~~text
ARROW, IBEAM, WAIT, CROSS, UPARROW, SIZE, ICON,
SIZENWSE, SIZENESW, SIZEWE, SIZENS, SIZEALL,
NO, HAND, APPSTARTING, HELP, PIN, PERSON
~~~

PIN and PERSON depend on cursor resources available on Windows.

## Close confirmation

Windows close immediately by default. Add a close trigger for a Yes/No dialog:

~~~pxl
add CLOSE_TRIGGER {
    message: "Do you want to close this app?",
    options: YESNO,
}
~~~

YESNO is currently the only supported option.

