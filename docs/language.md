# PXL language reference

## Program structure

Every program requires one window block and may contain a script block.

~~~pxl
window {
    title: "Example",
}

script {
    # commands and declarations go here
}
~~~

Comments begin with #.

## Shapes

Windows can contain filled rectangles, circles, and triangles. Rectangle angles
are in degrees and increase clockwise. Circle `x` and `y` specify its center;
triangle points use absolute window coordinates, and triangle angles rotate the
calculated triangle clockwise around its centroid.

Triangles may instead omit all three points to create an upward-pointing
equilateral triangle with side length 10. If `x` and `y` are supplied with a
complete point set, the calculated triangle is translated so its centroid is
at that position. `p1`, `p2`, and `p3` must be supplied together, as must `x`
and `y`.

Shape coordinates can be changed or incremented with normal widget properties,
for example `marker.x++`. For triangles, `x` and `y` translate all three points.

~~~pxl
window {
    rect panel {
        x: 50,
        y: 30,
        width: 200,
        height: 100,
        angle: 90,
    }

    circle dot {
        x: 300,
        y: 150,
        radius: 10,
    }

    triangle marker {
        p1: (400, 100),
        p2: (450, 200),
        p3: (350, 200),
    }
}
~~~

## Multiple windows

A single window may omit its id. If a file contains multiple windows, every
window must have a unique string id. The window with id `main` is selected
first; otherwise the first window is selected.

~~~pxl
window {
    id: "main",
    title: "Main",
}

window {
    id: "settings",
    title: "Settings",
}
~~~

Switch views from a script or button action. By default the old window is
destroyed:

~~~pxl
switch_view("settings");
switch_view("main", destroy_window = false);
~~~

## Variables

Supported types are int, short, long, float, double, bool, char, and String.

~~~pxl
int count = 5;
short smallNumber = 2;
long largeNumber = 100000;
float ratio = 0.5;
double preciseRatio = 1.25;
bool enabled = true;
char grade = 'A';
String name = "Pixelate";
~~~

Variables can be used in conditions and formatted strings.

## Conditions

Conditions support `!` negation and these comparison operators: `==`, `!=`, `<`, `<=`, `>`, and `>=`.

~~~pxl
if (count == 5) {
    status.text = "The count is five.";
} else {
    status.text = "The count is not five.";
}

if (!enabled) {
    status.text = "The feature is disabled.";
}

Conditions can combine comparisons and boolean values with `&&` and `||`.
`&&` binds more tightly than `||`; parentheses can group subconditions.

~~~pxl
if (score >= 60 && (passed == true || bonus == true)) {
    update status.text = "Passed";
}
~~~

## Loops

~~~pxl
while (count < 8) {
    count++;
}

for (int i = 0; i < 3; i++) {
    status.text = "The loop is running.";
}
~~~

Loops currently use numeric variables and ++ increments. A safety limit prevents runaway loops.

## Formatted strings

Use f before a string and put expressions inside braces.

~~~pxl
String appName = "Pixelate";
int version = 1;

info.text = f"{appName} version {version}";
~~~

Mouse values can also be formatted:

~~~pxl
mouseLabel.text = f"X: {mouse.x}, Y: {mouse.y}";
~~~

