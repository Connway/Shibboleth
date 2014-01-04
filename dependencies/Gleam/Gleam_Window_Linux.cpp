/************************************************************************************
Copyright (C) 2013 by Nicholas LaCroix

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

#include "Gleam_Window_Linux.h"
#include "Gaff_IncludeAssert.h"
#include <cmath>
#include <iostream>

NS_GLEAM

typedef struct
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long inputMode;
	unsigned long status;
} Hints;

GleamArray(Window*) Window::gWindows;

void Window::WindowProc(const XEvent& event)
{
	char buffer[sizeof(AnyMessage)];
	char secondary_buffer[sizeof(AnyMessage)];

	// if we make more than one window in our application
	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i]->getWindow() == event.xany.window) {
			AnyMessage* message = NULLPTR;
			AnyMessage* secondary_message = NULLPTR;
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

				case ResizeRequest:
					window->_width = event.xresizerequest.width;
					window->_height = event.xresizerequest.height;

					message = (AnyMessage*)buffer;
					message->base.type = WND_RESIZED;

					// if (event.xconfigure.x != window->_pos_x ||
					// 	event.xconfigure.y != window->_pos_x) {
						
					// 	window->_pos_x = event.xconfigure.x;
					// 	window->_pos_y = event.xconfigure.y;

					// 	AnyMessage* msg;

					// 	if (message) {
					// 		secondary_message = (AnyMessage*)secondary_buffer;
					// 		msg = secondary_message;
					// 	} else {
					// 	}

					// 	msg->base.type = WND_MOVED;
					// }
					break;

				case KeyPress:
					// Only send one event for each KeyPress. No repeating events.
					if (window->_no_repeats && window->_prev_keycode == event.xkey.keycode) {
						break;
					}

					message = (AnyMessage*)buffer;
					message->base.type = IN_KEYDOWN;
					message->key_char.key = (KeyCode)XLookupKeysym((XKeyEvent*)&event.xkey, 0);

					window->_prev_keycode = event.xkey.keycode;

					// convert keysym to character code
					// secondary_message = (AnyMessage*)secondary_buffer;
					// secondary_message->base.type = IN_CHARACTER;
					// secondary_message->key_char.character = event.xkey.keycode;
					break;

				case KeyRelease: {
					// emulate how Windows does it's key repeat
					if (XEventsQueued(window->getDisplay(), QueuedAfterReading)) {
						XEvent next_event;
						XPeekEvent(window->getDisplay(), &next_event);

						// If the next event is a KeyPress with the same keycode as us, we didn't actually
						// release the key
						if (next_event.type == KeyPress && next_event.xkey.time == event.xkey.time &&
							next_event.xkey.keycode == event.xkey.keycode) {

							break;
						}
					}

					message = (AnyMessage*)buffer;
					message->base.type = IN_KEYUP;
					message->key_char.key = (KeyCode)XLookupKeysym((XKeyEvent*)&event.xkey, 0);
				} break;

				case MotionNotify:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEMOVE;
					message->mouse_move.x = event.xmotion.x;
					message->mouse_move.y = event.xmotion.y;

					if (window->_first_mouse) {
						window->_first_mouse = false;
						message->mouse_move.dx = 0;
						message->mouse_move.dy = 0;
					} else {
						message->mouse_move.dx = message->mouse_move.x - window->_mouse_prev_x;
						message->mouse_move.dy = message->mouse_move.y - window->_mouse_prev_y;
					}

					window->_mouse_prev_x = message->mouse_move.x;
					window->_mouse_prev_y = message->mouse_move.y;
					break;

				case ButtonPress:
					message = (AnyMessage*)buffer;

					if (event.xbutton.button == Button4 || event.xbutton.button == Button5) {
						message->base.type = IN_MOUSEWHEEL;
						message->mouse_state.wheel = (event.xbutton.button == Button4) ? 1 : -1;

					} else {
						message->base.type = IN_MOUSEDOWN;
						message->mouse_state.button = (MouseCode)(event.xbutton.button - 1);
					}
					break;

				case ButtonRelease:
					message = (AnyMessage*)buffer;
					message->base.type = IN_MOUSEUP;
					message->mouse_state.button = (MouseCode)(event.xbutton.button - 1);
					break;
			}

			if (secondary_message) {
				secondary_message->base.window = window;
			}

			if (message) {
				message->base.window = window;

				for (unsigned int j = 0; j < window->_window_callbacks.size(); ++j) {
					window->_window_callbacks[j](*message);

					if (secondary_message) {
						window->_window_callbacks[j](*secondary_message);
					}
				}

				for (unsigned int j = 0; j < window->_window_message_handlers.size(); ++j) {
					window->_window_message_handlers[j]->handleMessage(*message);

					if (secondary_message) {
						window->_window_message_handlers[j]->handleMessage(*secondary_message);
					}
				}
			}
			break;
		}
	}
}


Window::Window(void):
	_application_name(NULLPTR), _window_mode(FULLSCREEN),
	_no_repeats(false), _contain(false), _display(NULLPTR),
	_visual_info(NULLPTR), _window(0), _delete_window(None),
	_protocols(None), _prev_keycode(0), _mouse_prev_x(0),
	_mouse_prev_y(0), _first_mouse(true)
{
}

Window::~Window(void)
{
	destroy();
}

bool Window::init(const GChar* app_name, MODE window_mode,
						unsigned int width, unsigned int height,
						short refresh_rate, int pos_x, int pos_y)
{
	_display = XOpenDisplay(NULL);

	if (!_display) {
		return false;
	}

	::Window root = DefaultRootWindow(_display);

	if (!root) {
		return false;
	}

	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	_visual_info = glXChooseVisual(_display, 0, att);

	if (!_visual_info) {
		return false;
	}

	XSetWindowAttributes swa;
	swa.border_pixel = 0;
	swa.colormap = XCreateColormap(_display, root, _visual_info->visual, AllocNone);
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
					ButtonPressMask | ButtonReleaseMask |
					EnterWindowMask | LeaveWindowMask |
					PointerMotionMask | ResizeRedirectMask |
					FocusChangeMask | SubstructureNotifyMask;

	int final_size, final_rate, num_sizes, num_rates;
	short* rates = NULLPTR;
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

			if (!width || !height) {
				XWindowAttributes attributes;
				XGetWindowAttributes(_display, root, &attributes);

				width = attributes.width;
				height = attributes.height;
			}
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

			if (pos_x < 0 || pos_y < 0) {
				XWindowAttributes attributes;
				XGetWindowAttributes(_display, root, &attributes);

				pos_x = (attributes.width - width)  / 2;
				pos_y = (attributes.height - height) / 2;
			}
			break;
	}

	_pos_x = pos_x;
	_pos_y = pos_y;
	_width = width;
	_height = height;
	_window_mode = window_mode;
	_refresh_rate = refresh_rate;

	Rotation rotation;

	_original_rate = XRRConfigCurrentRate(config);
	_original_size = sizes[XRRConfigCurrentConfiguration(config, &rotation)];

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
	XSetStandardProperties(_display, _window, app_name, app_name, None, NULLPTR, 0, NULLPTR);

	switch (_window_mode) {
		case FULLSCREEN_WINDOWED: {
			Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

			if (motif == None) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			Hints hints = { 0 };
			hints.flags = 2;

			XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, (unsigned char*)&hints, 5);

		} break;

		case FULLSCREEN: {
			XRRSetScreenConfigAndRate(_display, config, root, final_size, rotation, rates[final_rate], CurrentTime);

			XGrabKeyboard(_display, _window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
			XGrabPointer(_display, _window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
						GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
		} break;


		case WINDOWED:
			break;
	}

	XMapRaised(_display, _window);

	XRRFreeScreenConfigInfo(config);
	gWindows.push(this);

	return true;
}

void Window::destroy(void)
{
	setToOriginalResolutionRate();

	if (_display) {
		if (_window) {
			XDestroyWindow(_display, _window);
			_window = 0;
		}

		XCloseDisplay(_display);
		_display = NULLPTR;
	}

	for (unsigned int i = 0; i < gWindows.size(); ++i) {
		if (gWindows[i] == this) {
			gWindows.erase(i);
			break;
		}
	}

	_first_mouse = true;
	_prev_keycode = 0;
}

void Window::handleWindowMessages(void)
{
	while (XPending(_display)) {
		XNextEvent(_display, &_event);
		WindowProc(_event);
	}
}

void Window::addWindowMessageHandler(WindowCallback callback)
{
	_window_callbacks.push(callback);
}

bool Window::removeWindowMessageHandler(WindowCallback callback)
{
	for (unsigned int i = 0; i < _window_callbacks.size(); ++i) {
		if (_window_callbacks[i] == callback) {
			_window_callbacks.erase(i);
			return true;
		}
	}

	return false;
}

void Window::addWindowMessageHandler(IWindowMessageHandler* handler)
{
	_window_message_handlers.push(handler);
}

bool Window::removeWindowMessageHandler(IWindowMessageHandler* handler)
{
	for (unsigned int i = 0; i < _window_message_handlers.size(); ++i) {
		if (_window_message_handlers[i] == handler) {
			_window_message_handlers.erase(i);
			return true;
		}
	}

	return false;
}

void Window::showCursor(bool show)
{
	if (show) {
		XUndefineCursor(_display, _window);

	} else {
		assert(_display);

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

void Window::allowRepeats(bool allow)
{
	_no_repeats = !allow;
}

void Window::containCursor(bool contain)
{
	_contain = contain;
	XUngrabPointer(_display, CurrentTime);
	XGrabPointer(_display, _window, (_window_mode == FULLSCREEN) ? False : True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
				GrabModeAsync, GrabModeAsync, (contain) ? _window : None, None, CurrentTime);
}

bool Window::setWindowMode(MODE window_mode, int width, int height)
{
	if (_window_mode == FULLSCREEN || _window_mode == FULLSCREEN_WINDOWED) {
		setToOriginalResolutionRate();
		XUngrabKeyboard(_display, CurrentTime);
		XUngrabPointer(_display, CurrentTime);
	}

	switch (window_mode) {
		case FULLSCREEN: {
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

			int index = chooseClosestResolution(sizes, num_sizes, _width, _height);

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

			int final_rate = chooseClosestRate(rates, num_rates, _refresh_rate);
			
			if (final_rate < 0) {
				XRRFreeScreenConfigInfo(config);
				return false;
			}

			_refresh_rate = rates[final_rate];

			printf("%d %d %d %d\n", sizes[index].width, sizes[index].height, _width, _height);

			Rotation rotation;
			XRRSetScreenConfigAndRate(_display, config, root, index,
									XRRConfigRotations(config, &rotation),
									_refresh_rate, CurrentTime);
			XRRFreeScreenConfigInfo(config);

			Atom state = XInternAtom(_display, "_NET_WM_STATE", False);
			Atom fscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);

			if (state == None || fscreen == None) {
				return false;
			}

			XEvent xev;
			memset(&xev, 0, sizeof(xev));
			xev.type = ClientMessage;
			xev.xclient.window = _window;
			xev.xclient.message_type = state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = 1;
			xev.xclient.data.l[1] = fscreen;
			xev.xclient.data.l[2] = 0;

			XSendEvent(_display, root, False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

			XGrabKeyboard(_display, _window, False, GrabModeAsync, GrabModeAsync, CurrentTime);
			XGrabPointer(_display, _window, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
						GrabModeAsync, GrabModeAsync, (_window_mode == FULLSCREEN) ? _window : None, None, CurrentTime);
		} break;

		case FULLSCREEN_WINDOWED: {
			Atom state = XInternAtom(_display, "_NET_WM_STATE", False);
			Atom fscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);

			if (state == None || fscreen == None) {
				return false;
			}

			XEvent xev;
			memset(&xev, 0, sizeof(xev));
			xev.type = ClientMessage;
			xev.xclient.window = _window;
			xev.xclient.message_type = state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = 0;
			xev.xclient.data.l[1] = fscreen;
			xev.xclient.data.l[2] = 0;

			XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

			// Remove title bar
			Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

			if (motif == None) {
				return false;
			}

			Hints hints = { 0 };
			hints.flags = 2;

			XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, (unsigned char*)&hints, 5);

			_pos_x = _pos_y = 0;

			XWindowAttributes attributes;
			XGetWindowAttributes(_display, DefaultRootWindow(_display), &attributes);

			_width = attributes.width;
			_height = attributes.height;

			XMoveResizeWindow(_display, _window, 0, 0, _width, _height);
		} break;

		case WINDOWED: {
			Atom state = XInternAtom(_display, "_NET_WM_STATE", False);
			Atom fscreen = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", False);

			if (state == None || fscreen == None) {
				return false;
			}

			XEvent xev;
			memset(&xev, 0, sizeof(xev));
			xev.type = ClientMessage;
			xev.xclient.window = _window;
			xev.xclient.message_type = state;
			xev.xclient.format = 32;
			xev.xclient.data.l[0] = 0;
			xev.xclient.data.l[1] = fscreen;
			xev.xclient.data.l[2] = 0;

			XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);

			// Add title bar back
			Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);

			if (motif == None) {
				return false;
			}

			Hints hints = { 0 };
			hints.flags = 2;
			hints.decorations = 1;

			XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, (unsigned char*)&hints, 5);
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
		return NULLPTR;
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
		return NULLPTR;
	}
	
	short* rates = XRRRates(_display, screen, 0, &num_rates);
	
	if (!rates || num_rates < 0) {
		return NULLPTR;
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
		return NULLPTR;
	}
	
	int i = 0;
	for (; i < ns; ++i) {
		if (sizes[i].width == resolution.width &&
			sizes[i].height == resolution.height) {
				
			break;
		}
	}
	
	if (i == ns) {
		return NULLPTR;
	}
	
	short* rates = XRRRates(_display, screen, i, &num_rates);
	
	if (!rates || num_rates < 0) {
		return NULLPTR;
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

	Rotation rotation;
	XRRSetScreenConfigAndRate(_display, config, root, size_index,
							XRRConfigRotations(config, &rotation),
							_original_rate, CurrentTime);

	XRRFreeScreenConfigInfo(config);
}

NS_END
