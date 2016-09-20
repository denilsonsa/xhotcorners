// Documentation on XCB:
// https://xcb.freedesktop.org/manual/index.html
// https://rosettacode.org/wiki/Window_creation/X11#XCB
// https://en.wikibooks.org/wiki/X_Window_Programming/XCB
// https://bugs.freedesktop.org/show_bug.cgi?id=69417
// https://cgit.freedesktop.org/xcb/util-wm/tree/icccm/icccm.c
// https://tronche.com/gui/x/xlib/ICC/client-to-window-manager/wm-normal-hints.html
// http://xcb-util.sourcearchive.com/
// http://thetudor.ddns.net/res/docs/xcb/util-wm/modules.html
// https://standards.freedesktop.org/wm-spec/wm-spec-latest.html

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xcb/xcb.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>

int main(int argc, char** argv) {
	xcb_connection_t* c;
	xcb_screen_t* screen;
	xcb_drawable_t win;
	uint32_t mask = 0;
	int error;

	int width = 1;
	int height = 1;

	char title[] = "xhotcorners";
	uint8_t title_len = strlen(title);

	uint32_t values[4];
	xcb_size_hints_t size_hints;
	xcb_atom_t atoms[4];
	xcb_ewmh_connection_t ewmh_c;
	xcb_intern_atom_cookie_t* atom_cookie;

	// Connect to X server.
	c = xcb_connect(NULL, NULL);
	error = xcb_connection_has_error(c);
	if (error) {
		fprintf(stderr, "Error opening connection to X server: xcb_connection_has_error() returned %d\n", error);
		exit(1);
	}

	// Get the first screen.
	screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

	// Change Window attributes.
	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->black_pixel;
	values[1] =
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE |
		XCB_EVENT_MASK_ENTER_WINDOW |
		XCB_EVENT_MASK_LEAVE_WINDOW;

	// Create the window.
	win = xcb_generate_id(c);
	xcb_create_window (
			c,                             /* connection    */
			XCB_COPY_FROM_PARENT,          /* depth         */
			win,                           /* window Id     */
			screen->root,                  /* parent window */
			0, 0,                          /* x, y          */
			width, height,                 /* width, height */
			0,                             /* border_width  */
			XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class         */
			screen->root_visual,           /* visual        */
			mask, values);                 /* masks         */

	// Setting the window title.
	xcb_change_property (
			c,                     /* connection */
			XCB_PROP_MODE_REPLACE, /* mode       */
			win,                   /* window     */
			XCB_ATOM_WM_NAME,      /* property   */
			XCB_ATOM_STRING,       /* type       */
			8,                     /* format     */
			title_len,             /* data_len   */
			title);                /* *data      */

	// Setting WM_SIZE_HINTS.
	size_hints.flags = 0;
	xcb_icccm_size_hints_set_min_size(&size_hints, width, height);
	xcb_icccm_size_hints_set_max_size(&size_hints, width, height);
	xcb_icccm_set_wm_size_hints(
			c,                        /* connection */
			win,                      /* window     */
			XCB_ATOM_WM_NORMAL_HINTS, /* property   */
			&size_hints);             /* hints      */
	// I don't know why I need to use XCB_ATOM_WM_NORMAL_HINTS if I am setting WM_SIZE_HINTS.

	// Initializing XCB-EWMH.
	atom_cookie = xcb_ewmh_init_atoms(c, &ewmh_c);
	xcb_ewmh_init_atoms_replies(&ewmh_c, atom_cookie, NULL);
	// Discarding the return code of the previous call.
	// xcb_ewmh_init_atoms_replies returns 1 on success, 0 otherwise.

	// Setting _NET_WM_WINDOW_TYPE.
	atoms[0] = ewmh_c._NET_WM_WINDOW_TYPE_DOCK;
	xcb_ewmh_set_wm_window_type(
			&ewmh_c, /* connection */
			win,     /* window     */
			1,       /* list_len   */
			atoms);  /* list       */

	// Setting _NET_WM_STATE.
	atoms[0] = ewmh_c._NET_WM_STATE_STICKY; // Does not work on Openbox.
	atoms[1] = ewmh_c._NET_WM_STATE_SKIP_TASKBAR;
	atoms[2] = ewmh_c._NET_WM_STATE_SKIP_PAGER;
	atoms[3] = ewmh_c._NET_WM_STATE_ABOVE;
	xcb_ewmh_set_wm_state(
			&ewmh_c, /* connection */
			win,     /* window     */
			4,       /* list_len   */
			atoms);  /* list       */

	// Setting _NET_WM_ALLOWED_ACTIONS.
	// _NET_WM_ALLOWED_ACTIONS should be _NET_WM_ACTION_ABOVE (or maybe just empty)
	xcb_ewmh_set_wm_allowed_actions(
			&ewmh_c, /* connection */
			win,     /* window     */
			0,       /* list_len   */
			atoms);  /* list       */

	// Later improvement:
	// * _NET_WM_PID property and _NET_WM_PING protocol

	// Map the window on the screen.
	xcb_map_window (c, win);

	xcb_flush (c);

	// Trying set the desktop to -1.
	// Inspired by: https://github.com/onli/simdock/blob/master/src/xstuff.cc
	// And also by looking at "xprop" output on sticky windows on Openbox.
	//
	// Only works after the window is mapped.
	// Makes the window sticky on Openbox.
	xcb_ewmh_request_change_wm_desktop(
			&ewmh_c,    /* connection  */
			0,          /* screen_nbr  */ // Why zero?
			win,        /* window      */
			0xFFFFFFFF, /* new_desktop */
			XCB_EWMH_CLIENT_SOURCE_TYPE_NORMAL);  /* source_indication */

	// Moving the window.
	mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
	// TODO: Position it at a good location.
	values[0] = 0;
	values[1] = 0;
	xcb_configure_window(
			c,       /* connection */
			win,     /* window     */
			mask,    /* mask       */
			values); /* value_list */

	xcb_flush (c);

	// // STILL NOT WORKING!
	// // Making the window sticky, by sending a message to the root window.
	// values[0] = XCB_EWMH_WM_STATE_ADD;
	// values[1] = ewmh_c._NET_WM_STATE_STICKY;
	// xcb_ewmh_send_client_message(
	// 		c,                    /* connection */
	// 		win,                  /* window     */
	// 		screen->root,         /* window     */
	// 		ewmh_c._NET_WM_STATE, /* xcb_atom_t */
	// 		2,                    /* data_len   */
	// 		values);              /* data       */
	// xcb_flush (c);

	while(1) {
		xcb_generic_event_t* e = xcb_wait_for_event(c);
		if (e) {
			switch (e->response_type & ~0x80) {
				case XCB_EXPOSE:
					// Does not happen.
					printf("XCB_EXPOSE\n");
					break;
				case XCB_KEY_PRESS:
					printf("XCB_KEY_PRESS\n");
					break;
				case XCB_BUTTON_PRESS:
					printf("XCB_BUTTON_PRESS\n");
					break;
				case XCB_BUTTON_RELEASE:
					printf("XCB_BUTTON_RELEASE\n");
					break;
				case XCB_ENTER_NOTIFY:
					printf("XCB_ENTER_NOTIFY\n");
					break;
				case XCB_LEAVE_NOTIFY:
					printf("XCB_LEAVE_NOTIFY\n");
					break;
				// TODO: Detect RandR and/or Xinerama events.
				// In other words, reposition the window if the display geometry has changed.
				// (resolution change, orientation change, external display connected or disconnected)
				default:
					printf("Ignoring event response_type %d\n", e->response_type);
			}
			free (e);
		} else {
			printf("Some error, somewhere, somehow... Aborting!\n");
			break;
		}
	}

	xcb_disconnect(c);

	return 0;
}
