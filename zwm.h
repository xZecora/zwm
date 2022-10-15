/* TODO: add more thorough commenting */
/* TODO: this is getting sloppy, make it more readable, maybe seperate into multiple files */
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */

/* shorthand larger/smaller of two items */
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define MIN(A, B) ((A) < (B) ? (A) : (B))

/* shorthand to get size of window, often used with & to change them */
#define wsize(W, gx, gy, gw, gh) \
  XGetGeometry(d, W, &(Window){0}, gx, gy, gw, gh,\
      &(unsigned int){0}, &(unsigned int){0})

/* for loop logic for iterating through list */
#define win (client *t = 0, *c = list; c && t != list->prev; t = c, c = c->next)
/* save the current list as the list for passed workspace */
#define ssave(W) slist[W] = list
/* select a new list to work with */
#define ssel(W) list = slist[ws = W]

#define mod_clean(mask) \
  (mask & ~(numlock | LockMask) & \
   (ShiftMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask))

/* Shorthand for moving and resizing a window to new coordinates */
#define change(c) XMoveResizeWindow(d, c->w, c->wx, c->wy, c->ww, c->wh)

#define INTERSECT(x,y,w,h,m) (MAX(0, MIN((x)+(w),(m)->mx+(m)->mw) - MAX((x),(m)->mx)) * MAX(0, MIN((y)+(h),(m)->my+(m)->mh) - MAX((y),(m)->my)))

enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType,
       NetWMWindowTypeDialog, NetClientList, NetLast };
enum {  WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast };

/* Struct definitions */
/* defines properties of generic args passed to functions */
typedef struct {
  const char **com;
  const int i;
  const Window w;
} Arg;

/* defines properties of key items */
struct key {
  unsigned int mod;
  KeySym keysym;
  void (*function)(const Arg arg);
  const Arg arg;
};

/* define monitor so it can be used in client */
typedef struct monitor monitor;

/* defines properties of the window clients */
typedef struct client {
  struct client *next, *prev;
  int wx, wy;
  unsigned int ww, wh;
  int floating;
  Window w;
  monitor *mon;
} client;

/* defines monitors */
struct monitor {
  int mx, my, mw, mh; /* monitor dimensions */
  int workspace;
  client *clients;
  monitor *next, *prev;
};

/* XEvents */
void bpress(XEvent *e);
void brelease(XEvent *e);
void kpress(XEvent *e);
void ndes(XEvent *e);
void mreq(XEvent *e);
void mnot(XEvent *e);
void umnot(XEvent *e);

/* Window functions */
void wadd(Window w, int floats);
void wkill(const Arg arg);
void wdel(Window w);
void wfocus(client *c);
void wprev(const Arg arg);
void wnext(const Arg arg);
void wmoveup(const Arg arg); /* move window up in workspace */
void wmovedown(const Arg arg); /* move window down in workspace Notify functions */
void wtype(client *c);
void wswap(client *initial, client *swapto);
void wfloatt(const Arg arg);
int windowws(Window w);

/* Workspace functions */
void wtos(const Arg arg);
void sgo(const Arg arg);
void mongo(const Arg arg);
monitor *wactivemon(int space);

/* WM functions */
void input_grab(Window root);
void quit(const Arg arg);
void run (const Arg arg);
void organize(client *current, client *previous, int n);
void setup(void);
void retile(void);
void drag(XEvent *e);
void center(client *c);
Atom agetprop(client *c, Atom prop);
void cleanup(void);
/* Monitor functions */
void monsetup(void);
monitor *checkmonfocus(int x, int y, int w, int h);

static Atom netatom[NetLast];

/* initialize initial client list, the workspace list, and cur client
 * pointer */
static client *list = {0}, *slist[10] = {0}, *cur;
/* create the ws (workspace) variable, screen width, screen height,
 * window x, and window y values
 * start numlock off as 0 */
static int ws=1, sw, sh, wx, wy, numlock = 0;
/* create the window width and window height variables, unsigned
 * because they shouldn't be negative */
static unsigned int ww, wh;

/* Display pointer */
static Display *d;
static XButtonEvent mouse;
/* Root window declaration */
static Window root;
/* Initial variable set to make sure the WM should keep running */
static int running = 1;

static monitor *monlist = {0}, *selmon;

/* List of valid XEvents */
static void (*events[LASTEvent])(XEvent *e) = {
  [DestroyNotify]     = ndes,
  [ButtonPress]       = bpress,
  [ButtonRelease]     = brelease,
  [KeyPress]          = kpress,
  [MotionNotify]      = drag,
  [UnmapNotify]       = umnot,
  [MapRequest]        = mreq,
  [MappingNotify]     = mnot};

#ifndef CONFIG_H
/* Load config.h for custom user configs */
#include "config.h"
#endif
