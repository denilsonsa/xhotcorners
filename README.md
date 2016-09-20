THIS IS A WORK-IN-PROGRESS! THIS IS STILL INCOMPLETE!

Objective: Create a small 1x1 window at one of the screen corners, and then execute a shell command whenever the mouse pointer enters that window (and/or when the user clicks there).

In other words, to implement a "hot corner" feature, useful for window managers that don't have native support for it (e.g. Openbox).

Secondary objectives: Be very small, with as few dependencies as possible, and be very efficient at runtime.


Current status: Very early. I can open a window, I can detect some events. And nothing more.


Prerequisites:

    apt-get install build-essential libxcb-icccm4-dev libxcb-ewmh-dev

