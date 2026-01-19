#include <X11/Xlib.h>
#include <X11/keysym.h>

static KeySym terminalKey = XK_Return; // Mod+Enter
static KeySym closeKey = XK_q;         // Mod+q
static KeySym killKey = XK_k;          // Mod+k;

KeySym getTerminalBind()
{
    return terminalKey;
}

KeySym getCloseBind()
{
    return closeKey;
}

KeySym getKillBind()
{
    return killKey;
}