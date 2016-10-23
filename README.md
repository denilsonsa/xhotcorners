THIS IS A VERY INCOMPLETE PROTOTYPE!

Objective: Create a small 1x1 window at one of the screen corners, and then execute a shell command whenever the mouse pointer enters that window (and/or when the user clicks there).

In other words, to implement a "hot corner" feature, useful for window managers that don't have native support for it (e.g. Openbox).

Secondary objectives: Be very small, with as few dependencies as possible, and be very efficient at runtime.


Current status: Very early. I can open a window, I can detect some events. And nothing more.


Prerequisites:

    apt-get install build-essential libxcb-icccm4-dev libxcb-ewmh-dev


Next steps:

* Implement command-line parameters.
* Implement some kind of config file for easy setup.
* Automatically detect the screen size and position the window at the correct location.
* Automatically reposition the window whenever the screen size changes.
* Add support for selecting a corner in a specific display, in case of multiple displays.
* Support multiple corners from the same config file. This means keeping track and opening multiple windows.

Question:

What is the "correct" way to make a window STICKY?
I tried adding `_NET_WM_STATE_STICKY` to `_NET_WM_STATE`, but it was ignored (or maybe even removed, when I looked at `xprop`). I tested this in Openbox.

What worked for me was changing the window desktop to -1, and only after the window was mapped. Is this portable to multiple WMs or is this a specific quirk of Openbox?
