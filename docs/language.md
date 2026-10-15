# PXL language reference

## Program structure

Every program requires one window block and may contain a script block.

~~~pxl
window {
    title: "Example"
}

script {
    # commands and declarations go here
}
~~~

Comments begin with #.

## Variables

Supported types are int, short, long, float, double, bool, char, and String.

~~~pxl
int count = 5
short smallNumber = 2
long largeNumber = 100000
float ratio = 0.5
double preciseRatio = 1.25
bool enabled = true
char grade = 'A'
String name = "Pixelate"
~~~

Variables can be used in conditions and formatted strings.

## Conditions

Supported comparison operators are ==, !=, <, <=, >, and >=.

~~~pxl
if (count == 5) {
    status.text = "The count is five."
}
~~~

## Loops

~~~pxl
while (count < 8) {
    count++
}

for (int i = 0; i < 3; i++) {
    status.text = "The loop is running."
}
~~~

Loops currently use numeric variables and ++ increments. A safety limit prevents runaway loops.

## Formatted strings

Use f before a string and put expressions inside braces.

~~~pxl
String appName = "Pixelate"
int version = 1

info.text = f"{appName} version {version}"
~~~

Mouse values can also be formatted:

~~~pxl
mouseLabel.text = f"X: {mouse.x}, Y: {mouse.y}"
~~~

