#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/cursorfont.h>
#include <cstdio>
#include <cstdlib>
#include "Bindings.hpp"

/**
 * @brief Emergency exit handler.
 * @param msg Error message to display before exiting.
 */
[[noreturn]] void panic(const char *msg) {
    fprintf(stderr, "FATAL: %s\n", msg);
    exit(EXIT_FAILURE);
}

class WindowManager {
private:
    Display *dpy;
    Window root;
    bool running;

    void setupEventMasks() {
        XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
    }

    void setupMouseBindings() {
        XGrabButton(dpy, Button1, 0, root, False, ButtonPressMask, 
                    GrabModeSync, GrabModeAsync, None, None);
    }

    void setupCursor() {
        Cursor cursor = XCreateFontCursor(dpy, XC_left_ptr);
        XDefineCursor(dpy, root, cursor);
    }

    void setupKeyBindings() {
        // Grab WIN (Super) key
        KeySym mod_sym = XK_Super_L;
        KeyCode mod_code = XKeysymToKeycode(dpy, mod_sym);

        // Grab K key for WIN+RCTRL+K binding
        KeySym k_sym = XK_k;
        KeyCode k_code = XKeysymToKeycode(dpy, k_sym);

        if (mod_code != 0 && k_code != 0) {
            // Grab WIN+RCTRL+K (Mod4Mask is Super/WIN key, ControlMask for CTRL)
            // We need both Control key states since we want specifically right control
            XGrabKey(dpy, k_code, Mod4Mask | ControlMask, root, False, 
                     GrabModeAsync, GrabModeAsync);
        }
    }

    void handleButtonPress(const XButtonEvent &event) {
        XAllowEvents(dpy, ReplayPointer, CurrentTime);
        printf("Left button pressed at (%d, %d)\n", event.x_root, event.y_root);
    }

    void handleKeyPress(const XKeyEvent &event) {
        XAllowEvents(dpy, ReplayKeyboard, CurrentTime);
        KeySym keysym = XLookupKeysym(const_cast<XKeyEvent*>(&event), 0);
        
        // Check for WIN+RCTRL+K (kill WM)
        if (keysym == XK_k && (event.state & Mod4Mask) && (event.state & ControlMask)) {
            printf("Kill binding triggered (WIN+RCTRL+K). Shutting down...\n");
            running = false;
            return;
        }
        
        printf("Key pressed: 0x%lx (state: 0x%x)\n", keysym, event.state);
    }

    void handleEvent(XEvent &event) {
        switch (event.type) {
            case ButtonPress:
                handleButtonPress(event.xbutton);
                break;
            case KeyPress:
                handleKeyPress(event.xkey);
                break;
            case MapNotify:
                printf("Window mapped: 0x%lx\n", event.xmap.window);
                break;
            case Expose:
                printf("Expose event\n");
                break;
            case ClientMessage:
                printf("Client message received\n");
                break;
            default:
                // Silently ignore other events (ConfigureRequest, etc.)
                break;
        }
    }

public:
    WindowManager() : dpy(nullptr), root(0), running(true) {}

    ~WindowManager() {
        if (dpy) {
            XCloseDisplay(dpy);
        }
    }

    void initialize() {
        dpy = XOpenDisplay(nullptr);
        if (!dpy) {
            panic("Unable to open X display");
        }

        root = DefaultRootWindow(dpy);

        // Check if another WM is running
        XSetErrorHandler([](Display*, XErrorEvent*) -> int {
            panic("Another window manager is already running");
            return 0;
        });

        setupEventMasks();
        XSync(dpy, False);

        // Reset error handler
        XSetErrorHandler(nullptr);

        setupMouseBindings();
        setupCursor();
        setupKeyBindings();
        
        XSync(dpy, False);
    }

    void run() {
        XEvent event;
        printf("Window manager started. Waiting for events...\n");
        printf("Press WIN+RCTRL+K to exit.\n");

        while (running) {
            XNextEvent(dpy, &event);
            handleEvent(event);
            XSync(dpy, False);
        }
        
        printf("Window manager shutting down cleanly.\n");
    }
};

int main() {
    WindowManager wm;
    
    try {
        wm.initialize();
        wm.run();
    } catch (...) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}