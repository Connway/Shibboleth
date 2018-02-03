/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#ifdef __linux__

#include "Gleam_Window_Linux.h"
#include "Gleam_RawInputRegisterFunction.h"
#include <X11/extensions/XInput2.h>
#include <X11/cursorfont.h>
#include <cmath>

#define _NET_WM_STATE_REMOVE    0
#define _NET_WM_STATE_ADD       1
#define _NET_WM_STATE_TOGGLE    2

NS_GLEAM

typedef struct
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long inputMode;
	unsigned long status;
} Hints;

static GleamArray<MessageHandler> g_global_message_handlers;
static GleamArray<Display*> gDisplays;
static GleamArray<Window*> gWindows;
static XEvent gEvent;

static bool RemoveMessageHandler(GleamArray<MessageHandler>& handlers, const MessageHandler& callback)
{
	auto it = handlers.linearSearch(callback);

	if (it != handlers.end()) {
		handlers.fastErase(it);
		return true;
	}

	return false;
}

void Window::AddGlobalMessageHandler(const MessageHandler& callback)
{
	g_global_message_handlers.emplacePush(callback);
}

bool Window::RemoveGlobalMessageHandler(const MessageHandler& callback)
{
	return RemoveMessageHandler(g_global_message_handlers, callback);
}

void Window::HandleWindowMessages(void)
{
	for (auto it = gDisplays.begin(); it != gDisplays.end(); ++it) {
		while (XPending(*it)) {
			XNextEvent(*it, &gEvent);
			WindowProc(gEvent);
		}
	}
}

void Window::WindowProc(XEvent& event)
{
	char buffer[sizeof(AnyMessage)];
	char secondary_buffer[sizeof(AnyMessage)];

	if (event.xcookie.type == GenericEvent && event.xcookie.extension == gOpCode) {
		for (unsigned int i = 0; i < gWindows.size(); ++i) {
			if (gWindows[i]->getDisplay() == event.xcookie.display) {
				if (!XGetEventData(gWindows[i]->getDisplay(), &event.xcookie)) {
					break;
				}

				AnyMessage* message = nullptr;
				Window* window = gWindows[i];

				switch (event.xcookie.evtype) {
					case XI_ButtonPress: {
						XIDeviceEvent* de = (XIDeviceEvent*)event.xcookie.data;

						if (de->detail == 4 || de->detail == 5) {
							message = (AnyMessage*)buffer;
							message->base.type = IN_MOUSEWHEEL;
							message->mouse_state.wheel = (de->detail == 4) ? 1 : -1;
						} else if (de->detail == 8 || de->detail == 9) {
							message = (AnyMessage*)buffer;
							message->base.type = IN_MOUSEDOWN;
							message->mouse_state.button = (MouseCode)(de->detail - 5);
						} else if (de->detail < 4) {
							message = (AnyMessage*)buffer;
							message->base.type = IN_MOUSEDOWN;
							message->mouse_state.button = (MouseCode)(de->detail - 1);
						}
					} break;

					case XI_ButtonRelease: {
						XIDeviceEvent* de = (XIDeviceEvent*)event.xcookie.data;

						if (de->detail == 8 || de->detail == 9) {
							message = (AnyMessage*)buffer;
							message->base.type = IN_MOUSEUP;
							message->mouse_state.button = (MouseCode)(de->detail - 5);
						} else if (de->detail < 4) {
							message = (AnyMessage*)buffer;
							message->base.type = IN_MOUSEUP;
							message->mouse_state.button = (MouseCode)(de->detail - 1);
						}
					} break;

					case XI_RawMotion: {
						XIRawEvent* re = (XIRawEvent*)event.xcookie.data;

						// Ignore mouse wheel
						if (!XIMaskIsSet(re->valuators.mask, 0) && !XIMaskIsSet(re->valuators.mask, 1)) {
							break;
						}

						message = (AnyMessage*)buffer;
						message->base.type = IN_MOUSEMOVE;

						// We have DX and DY changes together
						if (XIMaskIsSet(re->valuators.mask, 0) && XIMaskIsSet(re->valuators.mask, 1)) {
							message->mouse_move.dx = (int)re->raw_values[0];
							message->mouse_move.dy = (int)re->raw_values[1];

						// If only sending DX/DY changes, need to figure out which one it is and grab the first raw_value
						} else if (XIMaskIsSet(re->valuators.mask, 0)) {
							message->mouse_move.dx = (int)re->raw_values[0];
							message->mouse_move.dy = 0;
						} else if (XIMaskIsSet(re->valuators.mask, 1)) {
							message->mouse_move.dx = 0;
							message->mouse_move.dy = (int)re->raw_values[0];
						}

						// Grab the mouse X/Y position relative to the window's top-left corner
						double root_x, root_y, win_x, win_y;
						::Window root, child;
						XIButtonState button_state;
						XIModifierState mods;
						XIGroupState group;

						XIQueryPointer(window->getDisplay(), re->deviceid, window->getWindow(), &root, &child, &root_x, &root_y, &win_x, &win_y, &button_state, &mods, &group);

						// Make sure the cursor is contained within the window.
						// If it is not, move it inside the window.
						if (window->_contain) {
							bool moved = false;

							if (win_x < 0.0) {
								win_x = 0.0;
								moved = true;
							} else if (win_x > (double)window->_width) {
								win_x = (double)window->_width;
								moved = true;
							}

							if (win_y < 0.0) {
								win_y = 0.0;
								moved = true;
							} else if (win_y > (double)window->_height) {
								win_y = (double)window->_height;
								moved = true;
							}

							// This is not working 100%.
							// There's a frame of two where the mouse is actually outside the window.
							if (moved) {
								XWarpPointer(
									window->_display, None, window->_window,
									0, 0, 0, 0, (int)win_x, (int)win_y
								);

								XFlush(window->_display);
							}
						}

						message->mouse_move.rel_x = (int)win_x;
						message->mouse_move.rel_y = (int)win_y;

						//message->mouse_move.abs_x = (int)win_x;
						//message->mouse_move.abs_y = (int)win_y;

					} break;

					case XI_KeyPress: {
						XIDeviceEvent* de = (XIDeviceEvent*)event.xcookie.data;
						message = (AnyMessage*)buffer;
						message->base.type = IN_KEYDOWN;
						message->key_char.key = (KeyCode)de->detail;
					} break;

					case XI_KeyRelease: {
						XIDeviceEvent* de = (XIDeviceEvent*)event.xcookie.data;
						message = (AnyMessage*)buffer;
						message->base.type = IN_KEYUP;
						message->key_char.key = (KeyCode)de->detail;
					} break;
				}

				XFreeEventData(window->getDisplay(), &event.xcookie);

				if (message) {
					message->base.window = window;
					window->handleMessage(message);
				}

				break;
			}
		}
	}

	// if we make more than one window in our application
	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i]->getWindow() == event.xany.window) {
			AnyMessage* message = nullptr;
			AnyMessage* secondary_message = nullptr;
			Window* window = gWindows[i];

			switch (event.type) {
				case ClientMessage:
					if (event.xclient.message_type == window->_protocols &&
						event.xclient.data.l[0] == (long)window->_delete_window) {

						message = (AnyMessage*)buffer;
						message->base.type = WND_CLOSED;
					}
					break;

				case DestroyNotify:
					message = (AnyMessage*)buffer;
					message->base.type = WND_DESTROYED;
					break;

				case ConfigureNotify:
					if (event.xconfigure.width != (int)window->_width ||
						event.xconfigure.height != (int)window->_height) {

						message = (AnyMessage*)buffer;
						message->base.type = WND_RESIZED;

						window->_width = event.xconfigure.width;
						window->_height = event.xconfigure.height;
					}

					if (event.xconfigure.x != window->_pos_x ||
						event.xconfigure.y != window->_pos_x) {

						AnyMessage* msg = nullptr;

						window->_pos_x = event.xconfigure.x;
						window->_pos_y = event.xconfigure.y;

						if (message) {
							secondary_message = (AnyMessage*)secondary_buffer;
							msg = secondary_message;
						} else {
							message = (AnyMessage*)buffer;
							msg = message;
						}

						msg->base.type = WND_MOVED;
					}
					break;
			}

			if (message) {
				message->base.window = window;
				window->handleMessage(message);
			}

			if (secondary_message) {
				secondary_message->base.window = window;
				window->handleMessage(secondary_message);
			}

			break;
		}
	}
}


Window::Window(void):
	_window_mode(FULLSCREEN), _cursor_visible(true),
	_contain(false), _display(nullptr),
	_visual_info(nullptr), _window(0),
	_delete_window(None), _protocols(None)
{
}

Window::~Window(void)
{
	destroy();
}

bool Window::init(const char* app_name, MODE window_mode,
					unsigned int width, unsigned int height,
					short refresh_rate, int pos_x, int pos_y,
					const char* display_name)
{
	_display = XOpenDisplay(display_name);

	if (!_display) {
		return false;
	}

	::Window root = DefaultRootWindow(_display);

	if (!root) {
		return false;
	}

	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	_visual_info = glXChooseVisual(_display, DefaultScreen(_display), att);

	if (!_visual_info) {
		return false;
	}

	XSetWindowAttributes swa;
	swa.border_pixel = 0;
	swa.colormap = XCreateColormap(_display, root, _visual_info->visual, AllocNone);
	swa.event_mask = ExposureMask | EnterWindowMask |
					LeaveWindowMask | FocusChangeMask |
					StructureNotifyMask | SubstructureNotifyMask;

	int final_size = 0, final_rate = 0, num_sizes = 0, num_rates = 0;
	short* rates = nullptr;
	XRRScreenConfiguration* config = XRRGetScreenInfo(_display, root);
	XRRScreenSize* sizes = XRRConfigSizes(config, &num_sizes);

	if (!config || !sizes || num_sizes < 1) {
		if (config) {
			XRRFreeScreenConfigInfo(config);
		}

		return false;
	}

	unsigned long mask = CWBorderPixel | CWColormap | CWEventMask;

	switch (window_mode) {
		case FULLSCREEN_WINDOWED:
			pos_x = pos_y = 0;

			XWindowAttributes attributes;
			XGetWindowAttributes(_display, root, &attributes);

			width = attributes.width;
			height = attributes.height;
			break;

		case FULLSCREEN: {
			pos_x = pos_y = 0;

			if (!width || !height) {
				XWindowAttributes attributes;
				XGetWindowAttributes(_display, root, &attributes);

				width = attributes.width;
				height = attributes.height;
			}

			final_size = chooseClosestResolution(sizes, num_sizes, width, height);
			if (final_size < 0) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			width = sizes[final_size].width;
			height = sizes[final_size].height;

			rates = XRRConfigRates(config, final_size, &num_rates);

			if (!rates || num_rates < 1) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			final_rate = chooseClosestRate(rates, num_rates, refresh_rate);

			if (final_rate < 0) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			refresh_rate = rates[final_rate];
		} break;

		case WINDOWED:
			if (!width || !height) {
				width = 800;
				height = 600;
			}
			break;
	}

	_pos_x = pos_x;
	_pos_y = pos_y;
	_width = width;
	_height = height;
	_window_mode = window_mode;
	_refresh_rate = refresh_rate;

	_original_rate = XRRConfigCurrentRate(config);
	_original_size = sizes[XRRConfigCurrentConfiguration(config, &_original_rotation)];

	_window = XCreateWindow(_display, root, _pos_x, _pos_y, _width, _height, 0,
							_visual_info->depth, InputOutput, _visual_info->visual,
							mask, &swa);

	if (!_window) {
		XRRFreeScreenConfigInfo(config);
		return false;
	}

	_protocols = XInternAtom(_display, "WM_PROTOCOLS", False);
	_delete_window = XInternAtom(_display, "WM_DELETE_WINDOW", False);

	if (_protocols == None || _delete_window == None) {
		XRRFreeScreenConfigInfo(config);
		return false;
	}

	XSetWMProtocols(_display, _window, &_delete_window, 1);
	XSetStandardProperties(_display, _window, app_name, app_name, None, nullptr, 0, nullptr);

	switch (_window_mode) {
		// On Linux, FULLSCREEN and FULLSCREEN_WINDOWED are basically the same thing
		case FULLSCREEN:
			XRRSetScreenConfigAndRate(_display, config, root, final_size, _original_rotation, rates[final_rate], CurrentTime);

		case FULLSCREEN_WINDOWED: {
			Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
			Atom state = XInternAtom(_display, "_NET_WM_STATE", False);
			Atom fscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);
			Atom max_vert = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
			Atom max_horz = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);

			if (motif == None || state == None ||
				max_vert == None || max_horz == None) {

				XRRFreeScreenConfigInfo(config);
				return false;
			}

			Hints hints = { 0 };
			hints.flags = 2;

			XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, (unsigned char*)&hints, 5);

			XEvent xev;
			memset(&xev, 0, sizeof(xev));
			xev.type = ClientMessage;
			xev.xclient.window = _window;
			xev.xclient.message_type = state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = _NET_WM_STATE_ADD;
			xev.xclient.data.l[1] = fscreen;
			xev.xclient.data.l[2] = max_vert;
			xev.xclient.data.l[3] = max_horz;
			xev.xclient.data.l[4] = 0;

			XSendEvent(_display, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
		} break;

		case WINDOWED:
			// Nothing more to do
			break;
	}

	XMapRaised(_display, _window);

	XRRFreeScreenConfigInfo(config);
	gWindows.push(this);

	if (gDisplays.linearSearch(_display) == gDisplays.end()) {
		gDisplays.push(_display);
	}

	_application_name = app_name;

	return true;
}

void Window::destroy(void)
{
	if (_display) {
		setToOriginalResolutionRate();

		if (_window) {
			XDestroyWindow(_display, _window);
			_window = 0;
		}

		XCloseDisplay(_display);
		_display = nullptr;
	}

	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i] == this) {
			gWindows.fastErase(i);
			break;
		}
	}
}

void Window::addWindowMessageHandler(const MessageHandler& callback)
{
	_window_callbacks.emplacePush(callback);
}

bool Window::removeWindowMessageHandler(const MessageHandler& callback)
{
	return RemoveMessageHandler(_window_callbacks, callback);
}

void Window::showCursor(bool show)
{
	_cursor_visible = show;

	if (show) {
		XUndefineCursor(_display, _window);

	} else {
		GAFF_ASSERT(_display);

		char bm_no_data[] = { 0 };
		XColor black, dummy;

		Colormap cmap = DefaultColormap(_display, DefaultScreen(_display));
		XAllocNamedColor(_display, cmap, "black", &black, &dummy);
		Pixmap bm_no = XCreateBitmapFromData(_display, _window, bm_no_data, 1, 1);
		Cursor no_ptr = XCreatePixmapCursor(_display, bm_no, bm_no, &black, &black, 0, 0);

		XDefineCursor(_display, _window, no_ptr);
		XFreeCursor(_display, no_ptr);

		if (bm_no != None) {
			XFreePixmap(_display, bm_no);
		}

		XFreeColors(_display, cmap, &black.pixel, 1, 0);
	}
}

void Window::containCursor(bool contain)
{
	_contain = contain;
}

bool Window::isCursorVisible(void) const
{
	return _cursor_visible;
}

bool Window::isCursorContained(void) const
{
	return _contain;
}

bool Window::setWindowMode(MODE window_mode, int width, int height, short refresh_rate)
{
	if (window_mode == window_mode && width == _width &&
		height == _height && refresh_rate == _refresh_rate) {
		return true;
	}

	setToOriginalResolutionRate();

	switch (window_mode) {
		case FULLSCREEN: {
			// If not defined, just use the desktop resolution
			if (!width || !height) {
				width = _width;
				height = _height;
			}

			::Window root = DefaultRootWindow(_display);
			int num_sizes;

			XRRScreenConfiguration* config = XRRGetScreenInfo(_display, root);
			XRRScreenSize* sizes = XRRConfigSizes(config, &num_sizes);

			if (!config || !sizes || num_sizes < 1) {
				if (config) {
					XRRFreeScreenConfigInfo(config);
				}

				return false;
			}

			int index = chooseClosestResolution(sizes, num_sizes, width, height);

			if (index < 0) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			int num_rates;
			short* rates = XRRConfigRates(config, index, &num_rates);

			if (!rates || num_rates < 1) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			int final_rate = chooseClosestRate(rates, num_rates, refresh_rate);

			if (final_rate < 0) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			_width = sizes[index].width;
			_width = sizes[index].height;
			_refresh_rate = rates[final_rate];

			XRRSetScreenConfigAndRate(
				_display, config, root, index,
				_original_rotation, _refresh_rate, CurrentTime
			);
			XRRFreeScreenConfigInfo(config);
		}

		// Fullscreen will drop down to here and run this code as well
		case FULLSCREEN_WINDOWED: {
			Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
			Atom state = XInternAtom(_display, "_NET_WM_STATE", False);
			Atom fscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);
			Atom max_vert = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
			Atom max_horz = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);

			if (motif == None || state == None || fscreen == None || max_vert == None || max_horz == None) {
				return false;
			}

			// Remove title bar
			Hints hints = { 0 };
			hints.flags = 2;

			XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, (unsigned char*)&hints, 5);

			XEvent xev;
			memset(&xev, 0, sizeof(xev));
			xev.type = ClientMessage;
			xev.xclient.window = _window;
			xev.xclient.message_type = state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = _NET_WM_STATE_ADD;
			xev.xclient.data.l[1] = fscreen;
			xev.xclient.data.l[2] = max_vert;
			xev.xclient.data.l[3] = max_horz;
			xev.xclient.data.l[4] = 0;

			XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

			_pos_x = _pos_y = 0;

			XMoveResizeWindow(_display, _window, 0, 0, _width, _height);
		} break;

		case WINDOWED: {
			if (!width || !height) {
				width = 800;
				height = 600;
			}

			_width = width;
			_height = height;

			Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
			Atom state = XInternAtom(_display, "_NET_WM_STATE", False);
			Atom fscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);
			Atom max_vert = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", False);
			Atom max_horz = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", False);

			if (motif == None || state == None || fscreen == None || max_vert == None || max_horz == None) {
				return false;
			}

			// Add title bar back
			Hints hints = { 0 };
			hints.flags = 2;
			hints.decorations = 1;

			XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, (unsigned char*)&hints, 5);

			XEvent xev;
			memset(&xev, 0, sizeof(xev));
			xev.type = ClientMessage;
			xev.xclient.window = _window;
			xev.xclient.message_type = state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = _NET_WM_STATE_REMOVE;
			xev.xclient.data.l[1] = fscreen;
			xev.xclient.data.l[2] = max_vert;
			xev.xclient.data.l[3] = max_horz;
			xev.xclient.data.l[4] = 0;

			XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

			XMoveResizeWindow(_display, _window, _pos_x, _pos_y, _width, _height);
		} break;
	}

	XMapRaised(_display, _window);
	_window_mode = window_mode;

	return true;
}

Window::MODE Window::getWindowMode(void) const
{
	return _window_mode;
}

void Window::getPos(int& x, int& y) const
{
	x = _pos_x;
	y = _pos_y;
}

void Window::getDimensions(unsigned int& width, unsigned int& height) const
{
	width = _width;
	height = _height;
}

int Window::getPosX(void) const
{
	return _pos_x;
}

int Window::getPosY(void) const
{
	return _pos_y;
}

unsigned int Window::getWidth(void) const
{
	return _width;
}

unsigned int Window::getHeight(void) const
{
	return _height;
}

bool Window::isFullScreen(void) const
{
	return _window_mode == FULLSCREEN;
}

bool Window::setIcon(const char*)
{
	return true;
}

XVisualInfo* Window::getVisualInfo(void) const
{
	return _visual_info;
}

Display* Window::getDisplay(void) const
{
	return _display;
}

::Window Window::getWindow(void) const
{
	return _window;
}

XRRScreenSize* Window::getResolutions(int& num_sizes, int screen) const
{
	if (screen < 0) {
		screen = DefaultScreen(_display);
	}

	XRRScreenSize* sizes = XRRSizes(_display, screen, &num_sizes);
	if (!sizes || num_sizes < 0) {
		return nullptr;
	}

	return sizes;
}

short* Window::getRefreshRates(int& num_rates, int screen) const
{
	if (screen < 0) {
		screen = DefaultScreen(_display);
	}

	int ns;
	XRRScreenSize* sizes = getResolutions(ns, screen);

	if (!sizes || ns < 0) {
		return nullptr;
	}

	short* rates = XRRRates(_display, screen, 0, &num_rates);

	if (!rates || num_rates < 0) {
		return nullptr;
	}

	return rates;
}

short* Window::getRefreshRates(const XRRScreenSize& resolution, int& num_rates, int screen) const
{
	if (screen < 0) {
		screen = DefaultScreen(_display);
	}

	int ns;
	XRRScreenSize* sizes = getResolutions(ns, screen);

	if (!sizes || ns < 0) {
		return nullptr;
	}

	int i = 0;
	for (; i < ns; ++i) {
		if (sizes[i].width == resolution.width &&
			sizes[i].height == resolution.height) {

			break;
		}
	}

	if (i == ns) {
		return nullptr;
	}

	short* rates = XRRRates(_display, screen, i, &num_rates);

	if (!rates || num_rates < 0) {
		return nullptr;
	}

	return rates;
}

int Window::chooseClosestResolution(XRRScreenSize* sizes, int num_sizes,
									unsigned int width, unsigned int height)
{
	int offset_x = 0x7FFFFFFF, offset_y = 0x7FFFFFFF;
	int closest = -1;
	for (int i = 0; i < num_sizes; ++i) {
		if (sizes[i].width == (int)width && sizes[i].height == (int)height) {
			return i;
		}

		int x = fabsf(sizes[i].width - width);
		int y = fabsf(sizes[i].height - height);

		if (x*x + y*y < offset_x*offset_x + offset_y*offset_y) {
			offset_x = x;
			offset_y = y;
			closest = i;
		}
	}

	return closest;
}

int Window::chooseClosestRate(short* rates, int num_rates, short rate)
{
	short offset = 0x7FFF;
	int closest = -1;
	for (int i = 0; i < num_rates; ++i) {
		if (rates[i] == rate) {
			return i;
		}

		short diff = fabsf(rates[i] - rate);

		if (diff < offset) {
			offset = diff;
			closest = i;
		}
	}

	return closest;
}

void Window::setToOriginalResolutionRate(void)
{
	if (_window_mode != FULLSCREEN || ((int)_width == _original_size.width &&
		(int)_height == _original_size.height && _refresh_rate == _original_rate)) {

		// Setting _width and _height for calls to setWindowMode()
		_width = _original_size.width;
		_height = _original_size.height;
		return;
	}

	int num_sizes, size_index = -1;
	::Window root = DefaultRootWindow(_display);
	XRRScreenConfiguration* config = XRRGetScreenInfo(_display, root);
	XRRScreenSize* sizes = XRRConfigSizes(config, &num_sizes);

	if (!sizes || num_sizes < 1) {
		return;
	}

	for (int i = 0; i < num_sizes; ++i) {
		if (sizes[i].width == _original_size.width &&
			sizes[i].height == _original_size.height) {

			size_index = i;
			break;
		}
	}

	if (size_index < 0) {
		return;
	}

	_width = sizes[size_index].width;
	_height = sizes[size_index].height;

	XRRSetScreenConfigAndRate(
		_display, config, root, size_index,
		_original_rotation, _original_rate, CurrentTime
	);

	XRRFreeScreenConfigInfo(config);
}

void Window::handleMessage(AnyMessage* message)
{
	for (unsigned int i = 0; i < _window_callbacks.size(); ++i) {
		_window_callbacks[i](*message);
	}

	for (auto it = g_global_message_handlers.begin(); it != g_global_message_handlers.end(); ++it) {
		(*it)(*message);
	}
}

NS_END

#endif