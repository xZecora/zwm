#include "zwm.h" /* header files babbyyyy */

static int xerror() { return 0; }

/* Change the window focus */
void wfocus(client *c) {
  if (!c) /* now we can handle 0 as an input */
    return;

  cur = c;

  int wws = windowws(c->w);
  if(wws != ws)
    ssel(wws);

  XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
  XRaiseWindow(d, cur->w);
}

/* Notify the WM that a window has been unmapped */
/* keeping in case I need it later */
void umnot(XEvent *e) {
  //wdel(e->xunmap.window);
  //wfocus(list);
}

/* Notify the WM that a window should be deleted, delete it, the focus the last window */
void ndes(XEvent *e) {
  wdel(e->xdestroywindow.window, KILL_WINDOW_FALSE); /* call wdel when ndes happens */
  if (list) /* if list isn't empty, focus prev element */
    wfocus(list->prev);
}

/* Check when a key was pressed and determine if its one of the
 * defined hotkeys, then run its function if it was */
void kpress(XEvent *e) {
  KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);

  for (unsigned int i = 0; i < sizeof(keys) / sizeof(*keys); ++i)
    if (keys[i].keysym == keysym &&
        mod_clean(keys[i].mod) == mod_clean(e->xkey.state))
      keys[i].function(keys[i].arg);
}

/* inform the WM that a button has been pressed */
void bpress(XEvent *e) {
  if (!e->xbutton.subwindow || e->xbutton.subwindow != cur->w)
    return;

  /* save new window info after resize/move with cursor */
  wsize(cur->w, &cur->wx, &cur->wy, &cur->ww, &cur->wh);
  XRaiseWindow(d, cur->w);
  mouse = e->xbutton;
}

/* inform the WM that a button has been released */
void brelease(XEvent *e) {
  mouse.subwindow = 0;
}

void drag(XEvent *e) {
  /* exit if there is no clicked window, the window is floating,
   * or its not the current window */
  if (!mouse.subwindow || !cur->floating || mouse.subwindow != cur->w)
    return;
  while (XCheckTypedEvent(d, MotionNotify, e));

  /* get difference between previous and current mouse */
  int xd = e->xbutton.x_root - mouse.x_root;
  int yd = e->xbutton.y_root - mouse.y_root;

  /* operate on the cur windows info */
  XMoveResizeWindow(d, mouse.subwindow,\
      cur->wx + (mouse.button == 1 ? xd : 0),\
      cur->wy + (mouse.button == 1 ? yd : 0),\
      MAX(1, cur->ww + (mouse.button == 3 ? xd : 0)),\
      MAX(1, cur->wh + (mouse.button == 3 ? yd : 0)));
}

/* returns the index of the workspace the provided window is part of */
int windowws(Window w) {
  int wws = 0;
  int con = 1;
  for (; wws < 10 && con; wws++){
    client *clist = slist[wws];
    client *curc = clist;

    for (client *t = 0; curc && t != clist->prev && con; t = curc, curc = curc->next)
      if(curc->w == w)
        con = False;
  }

  //if (con)
  //  running = 0;

  return wws - 1;
}

/* add a window and organize it into the tiling scheme */
void wadd(Window w, int floats) {
  int n = 0; /* variable used count number of clients */

  if (list) { /* do any windows even exist */
    n = 1; /* if so there is at least one */
    client *cur= list; /* make pointer to initial element */
    while (cur->next != list) { /* stop when you get to the end */
      cur= cur->next; /* go to next element */
      n++; /* increment the counter variable */
    }
  }

  client *c; /* create the variable to put it into */
  client *lastTiled = {0}; /* variable to store last tiled window in */
  int s = 0, fltCnt = 0; /* only needed if if statement executes but outside to stop errors */

  /* this bit returns the last tiled window, might not be a bad idea to make it a function, may come up later */
  if (list) { /* only execute this is there are windows */
    client *prev = list;
    bool allFloat = False; /* stops infinite loop if all windows are tiled */
    while ((prev!= list || s == 0) && !allFloat) {
      if (prev == list && fltCnt > 0) /* stops infinite loop */
        allFloat = True;
      if (!prev->floating) {
        lastTiled = prev;
        s++;
      } else {
        fltCnt++; /* iterate fltCnt if window is floating */
      }
      prev = prev->next;
    }
  }

  if (!(c = (client *)calloc(1, sizeof(client))))
    exit(1);

  c->mon = selmon;
  c->w = w; /* set window of new client to new window */
  if (!floats)
    wtype(c);
  else
    c->floating = floats;

  /* set default window size/position in client */
  wsize(c->w, &c->wx, &c->wy, &c->ww, &c->wh);

  if (lastTiled && list) {
    list->prev->next = c; /* linked list adding to end logic */
    c->prev = list->prev;
    list->prev = c;
    c->next = list;
    if (!c->floating){
      organize(c, lastTiled, s); /* organize it into the titling scheme */
      change(c);
      change(lastTiled);
    } else {
      center(c);
    }
    XRaiseWindow(d, cur->w);
  } else if (list) { /* if windows exist but are all floating */
    list->prev->next = c; /* linked list adding to end logic */
    c->prev = list->prev;
    list->prev = c;
    c->next = list;
    if (!c->floating) {
      organize(c, c, s); /* organize it into the titling scheme */
      change(c);
    } else {
      center(c);
    }
    XRaiseWindow(d, cur->w);
  } else {
    list = c; /* making c the first window in the list */
    list->prev = list->next = list; /* making the list cyclic */
    if (!c->floating) {
      organize(c, c, 0); /* organize it into a new tiling schema */
      change(c);
    } else {
      center(c);
    }
  }

  ssave(ws); /* finalize and save the ws list */
  wfocus(c);
}

/* takes a client and centers it in the middle of the screen */
void center(client *c) {
  c->wx = c->mon->mw / 2 - c->ww / 2;
  c->wy = c->mon->mh / 2 - c->wh / 2;
  XMoveWindow(d, c->w, c->wx, c->wy);
}

/* retiles entire screen to account for removing windows from schema */
void retile(void) {
  if (!list)
    return;
  int n = 0, fltCnt = 0;
  client *current = list;
  client *previous = current;
  bool allFloat = False; /* stops infinite loop if all windows are tiled */
  while ((current != list || n == 0) && !allFloat) {
    if (current == list && fltCnt > 0) /* stops infinite loop */
      allFloat = True;
    if (!current->floating) {
      organize(current, previous, n);
      previous = current;
      n++;
    } else {
      fltCnt++; /* iterate fltCnt if window is floating */
    }
    current = current->next;
  }

  /* this ensures only run the change
   * if the window was actually changed
   * excessive but it made me feel better
   * that this wasn't being run on everything */
  client *c = list;
  unsigned int tww, twh;
  int twx, twy;
  int s = 0;
  while (c != list || s == 0)  {
    wsize(c->w, &twx, &twy, &tww, &twh);
    if (c->ww != tww || c->wh != twh || c->wx != twx || c->wy != twy)
      change(c);
    c = c->next;
    s++;
  }
  if (cur)
    XRaiseWindow(d, cur->w);
}

/* Delete a client and reformat tiling scheme to account it for */
void wdel(Window w, int kill){
  client *x = 0; /* initialize holder for deleted client */
  int temp = ws;
  int wws = windowws(w);

  ssel(wws);

  for win {
    if (c->w == w) /* iterate through windows until you */
      x = c;       /* find the one to be deleted */
  }

  if (!list || !x) /* do nothing if list is empty or window not found */
    return;

  if (x->prev == x) /* check if x is the only element */
    list = 0;
  if (x->next) /* if x->next is defined, redefine its prev pointer */
    x->next->prev = x->prev;
  if (x->prev) /* if x->prev is defined, redefine its next pointer */
    x->prev->next = x->next;
  if (x == list)    /* if x was the head of the list, point */
    list = x->next; /* list to the new first element */

  XUnmapWindow(d, x->w); /* cleanup window */

  if (kill)
    XKillClient(d, x->w);

  free(x);

  if (list)
    retile();

  ssave(wws); /* finalize and save current list to ws */
  ssel(temp);
}

/* toggles a windows floating status and resorts all
 * the other windows tiling to account */
void wfloatt(const Arg arg) {
  if (!cur)
    return;
  if (cur->floating)
    cur->floating = 0;
  else
    cur->floating = 1;

  retile();
}

/* Organizes windows passed to it based on
 * their location in the list
 * Called when one is either added or destroyed
 * Only alters the values inside the client, call change afterwards
 * to set the window to be this size
 * parameter 2 is the new window
 * parameter 3 is the one before it in the tiling */
void organize(client *current, client *previous, int n) {
  if (n == 0) {  /* if this is the first window, fill entire screen */
    current->wx = current->mon->mx;
    current->wy = current->mon->my;
    current->wh = current->mon->mh;
    current->ww = current->mon->mw;
  } else if (n % 2) {  /* odd windows are split horizontally */
    previous->ww = previous->ww / 2;
    current->ww = previous->ww;
    current->wh = previous->wh;
    current->wx = previous->wx + previous->ww;
    current->wy = previous->wy;
  } else {  /* even windows are split vertically */
    previous->wh = previous->wh / 2;
    current->ww = previous->ww;
    current->wh = previous->wh;
    current->wx = previous->wx;
    current->wy = previous->wy + previous->wh;
  }
}


/* Kills windows */
/* The whole idea here right now is that I don't want to rely on wdel to delete the current window because it causes issues sometimes and overcomplicates it, but I think I found a decent solution. This like almost works and i hate that it doesn't quite. After deleting one window, its fine, but then deleting a second seems to unfocus me from the stack */
void wkill(const Arg arg) {
  if (!cur) /* only kill if a window is focused, otherwise exit */
    return;

  client *temp;
  if (list && cur == list->prev){
    temp = cur->prev;
  } else {
    temp = cur->next;
  }

  client *x = 0; /* initialize holder for deleted client */

  x = cur;

  if (x->prev == x) /* check if x is the only element */
    list = 0;
  else{
    if (x->next) /* if x->next is defined, redefine its prev pointer */
      x->next->prev = x->prev;
    if (x->prev) /* if x->prev is defined, redefine its next pointer */
      x->prev->next = x->next;
    if (x == list)    /* if x was the head of the list, point */
      list = x->next; /* list to the new first element */
  }

  XUnmapWindow(d, x->w); /* cleanup window */

  free(x);

  if (list)
    retile();

  ssave(ws); /* finalize and save current list to ws */

  if(list) {
    wfocus(temp);
  } else {
    selmon->clients = 0;
    cur = 0;
  }
}

/* returns the active monitor for a given workspace number */
monitor *wactivemon(int space) {
  monitor *activemon = 0;
  int n = 0;
  monitor *curmon = monlist;
  while (curmon != monlist || n == 0) {
    if (curmon->workspace == space)
      activemon = curmon;
    n++;
    curmon = curmon->next;
  }
  return activemon;
}

/* Moves windows between workspaces */
void wtos(const Arg arg) {
  if (!list || arg.i == selmon->workspace || !cur) /* if this isn't here, will send empty */
    return;  /* clients */

  int tmp = ws;
  monitor *tempmon = selmon;
  Window window = cur->w; /* might be able to make this more concise somehow */
  int f = cur->floating; /* store information for later */
  client *prev = 0;
  if (cur->prev != cur)
    prev = cur->prev;

  wdel(cur->w, KILL_WINDOW_FALSE); /* delete window just moved */
  XUnmapWindow(d, cur->w); /* unmap old window */
  ssave(tmp); /* save initial workspace */

  ssel(arg.i); /* focus new workspace */

  if (!(selmon = wactivemon(arg.i))) /* try and set selmon to mon the workspace is in */
    selmon = tempmon; /* if the workspace is inactive, default to original mon */
  wadd(window, f); /* add window to new workspace */

  if (selmon->workspace == arg.i) /* if selmon has the desired workspace, */
    XMapWindow(d, window);                /* map the window */

  ssave(arg.i);

  ssel(tmp);
  selmon = tempmon; /* reset the active monitor */
  wfocus(prev);
}

/* swaps position of two windows passed to it */
void wswap(client *initial, client *swapto) {
  /* if they're the same, do nothing */
  if (initial == swapto)
    return;

  /* if either is floating, dont swap positions */
  if (initial->floating || swapto->floating)
    return;

  /* check if either is head and then swap them if so */
  if (initial == list)
    list = swapto;
  else if (swapto == list)
    list = initial;

  /* store values so the swapto can be assigned them later */
  int tww = initial->ww;
  int twh = initial->wh;
  int twx = initial->wx;
  int twy = initial->wy;

  /* assign initial the same size and position as swapto */
  initial->ww = swapto->ww;
  initial->wh = swapto->wh;
  initial->wx = swapto->wx;
  initial->wy = swapto->wy;

  /* assign swap to the initial position of initial */
  swapto->ww = tww;
  swapto->wh = twh;
  swapto->wx = twx;
  swapto->wy = twy;
}

/* swaps the current window with the one above it in the list
 * arg is a filler parameter */
void wmoveup(const Arg arg) {
  /* if list is empty or only one element, exit */
  if (!list || list->next == list)
    return;

  wswap(cur, cur->next); /* call wswap to swap their physical positions */

  /* this swaps the list positions of the two nodes youre swapping */
  client* temp = cur->next;
  cur->next = cur->prev;
  temp->prev = cur->prev;
  cur->next->next = temp;

  temp = cur->prev;
  cur->prev = cur->prev->prev;
  cur->prev->next = cur;
  temp->prev = cur;

  retile();
}

/* will be very similar to w move up, probably will refactor code into a helper function for the two */
void wmovedown(const Arg arg) {
  /* if list is empty or only one element, exit */
  if (!list || list->next == list)
    return;

  wswap(cur, cur->prev); /* call wswap to swap their physical positions */

  /* this swaps the list positions of the two nodes youre swapping */
  client* temp = cur->prev;
  cur->prev = cur->next;
  temp->next = cur->next;
  cur->prev->prev = temp;

  temp = cur->next;
  cur->next = cur->next->next;
  cur->next->prev = cur;
  temp->next = cur;

  retile();
}

/* Focus the previous window */
void wprev(const Arg arg) {
  if (!cur) /* check if a window is currently focused */
    return;

  wfocus(cur->prev); /* if a window is focused, focus its prev element */
}

/* Focus the next window */
void wnext(const Arg arg) {
  if (!cur) /* check if a window is currently focused */
    return;

  wfocus(cur->next); /* if a window is focused, focus its next element */
}

/* checks if it should be floating and sets it be floating if it is */
void wtype(client *c) {
  Atom wtype = agetprop(c, netatom[NetWMWindowType]);

  if (wtype == netatom[NetWMWindowTypeDialog]) {
    c->floating = 1;
  }
}

/* Focus passed workspace number */
void sgo(const Arg arg) {
  if (arg.i == ws) /* return if moving to current workspace */
    return;

  int tmp = selmon->workspace;  /* initialize a temp store for current ws */

  selmon->workspace = arg.i; /* define current mons new workspace */

  ssave(ws); /* save current workspace */
  ssel(arg.i); /* select workspace being moved it */

  selmon->clients = list;

  /* checks to see that the new list for the monitor has properly defined monitors */
  if (list && list->mon != selmon) {
    client *current = list;
    current->mon = selmon;
    current = current->next;
    while (current != list) {
      current->mon = selmon;
      current = current->next;
    }
    retile(); /* retile to fit new monitor */
  }

  if (list)
    for win  /* map all windows in the new ws to the screen */
      XMapWindow(d, c->w);

  ssel(tmp); /* select the initial ws */

  if (list)
    for win /* unmap all windows in initial ws */
      XUnmapWindow(d, c->w);

  monitor *curmon = monlist;
  int n = 0;
  while (curmon != monlist || n == 0) { /* checking all monitors */
    if (curmon->workspace == selmon->workspace && curmon != selmon){ /* if clients is defined and it isn't the selected monitor */
      if (list){                           /* if list is defined we want to all of its clients monitors to be the selected one */
        curmon->clients = list;
        client *curlist = curmon->clients;
        curlist->mon = curmon;
        curlist = curlist->next;
        while(curlist != curmon->clients){
          curlist->mon = curmon;
          curlist = curlist->next;
        }
      } else { /* otherwise we want to set the monitors clients to undefined */
        curmon->clients = 0;
      }
    }
    curmon = curmon->next;
    n++;
  }

  /* loop to coorect all of the workspace values */
  curmon = monlist;
  n = 0;
  while (curmon != monlist || n == 0) { /* checking all monitors */
    if (curmon->workspace == arg.i && curmon != selmon) /* if clients is defined and it isn't the selected monitor */
      curmon->workspace = tmp;
    curmon = curmon->next;
    n++;
  }

  /* if list is defined and its monitor has changed, retile it to fit new mon */
  if (list && list->mon != selmon) {
    retile();
    for win  /* map all windows in the new ws to the screen */
      XMapWindow(d, c->w);
  }

  ssave(tmp);

  ssel(arg.i); /* reselect new ws */
  /* if new workspaces list doesn't exist, clear current selected window */
  if(list) {
    wfocus(list);
  } else {
    cur = 0;
    wfocus(0);
  }
}

/* move focus to next monitor */
void mongo(const Arg arg) {
  if (selmon == selmon->next) /* if moving to current monitor, return */
    return;

  ssave(ws);
  if (arg.i) /* if arg is 1 go forward */
    selmon = selmon->next; /* focus next monitor */
  else       /* if arg is 0 go backwards */
    selmon = selmon->prev;

  ssel(selmon->workspace); /* select the next monitors clients */
  wfocus(list);
}

/* run command given, typically a char array */
void run(const Arg arg) {
  if (fork())
    return;

  if (d)
    close(ConnectionNumber(d));

  setsid();
  execvp((char *)arg.com[0], (char **)arg.com);
}

void mreq(XEvent *e) {
  Window w = e->xmaprequest.window;

  wsize(w, &wx, &wy, &ww, &wh); /* get size to draw windows, useful for dialogues */
  wadd(w, 0); /* add new window to current ws list */

  XMapWindow(d, w); /* map the new window the screen */
  /* Must do this here, otherwise it focuses things incorrectly */
  wfocus(list->prev); /* focus the new window */
  /* this bit makes sure that windows who close themselves are properly handled */
  XSetWMProtocols(d, w, &netatom[WMDelete], 1);
  XSelectInput(d, w, StructureNotifyMask);
}

void mnot(XEvent *e) {
  XMappingEvent *ev = &e->xmapping;

  if (ev->request == MappingKeyboard || ev->request == MappingModifier) {
    XRefreshKeyboardMapping(ev);
    input_grab(root);
  }
}

/* used to grab inputs from the root window to treat as hotkeys */
void input_grab(Window root) {
  unsigned int i, j, modifiers[] = {0, LockMask, numlock, numlock | LockMask};
  XModifierKeymap *modmap = XGetModifierMapping(d);
  KeyCode code;

  for (i = 0; i < 8; i++)
    for (int k = 0; k < modmap->max_keypermod; k++)
      if (modmap->modifiermap[i*modmap->max_keypermod + k] == XKeysymToKeycode(d, 0xff7f))
        numlock = (1 << i);

  XUngrabKey(d, AnyKey, AnyModifier, root);

  for( i = 0; i < sizeof(keys) / sizeof(*keys); i++)
    if ((code = XKeysymToKeycode(d, keys[i].keysym)))
      for (j = 0; j < sizeof(modifiers) / sizeof(*modifiers); j++)
        XGrabKey(d, code, keys[i].mod | modifiers[j], root, True, GrabModeAsync, GrabModeAsync);

  XUngrabButton(d, AnyButton, AnyModifier, root); /* ungrab button so it can be used by other processes */

  for (i = 1; i < 4; i +=2)
    for (j = 0; j < sizeof(modifiers) / sizeof(*modifiers); j++)
      XGrabButton(d, i, MOD | modifiers[j], root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, 0, 0);
  XFreeModifiermap(modmap);
}

/* returns the atom property that you request */
Atom agetprop(client *c, Atom prop) {
  /* some filler variables to pass for return arguments */
  int di;
  unsigned long dl;
  unsigned char *p = NULL;
  Atom da, atom = None; /* atom is the final return */

  if (XGetWindowProperty(d, c->w, prop, 0L, sizeof atom , False, XA_ATOM, &da, &di, &dl, &dl, &p) == Success && p) {
    atom = *(Atom *)p;
    XFree(p);
  }
  return atom;
}

/* setup monitors to have independent windowing */
void monsetup(void) {
#ifdef XINERAMA /* XINERAMA */
  if (XineramaIsActive(d)) {
    int scount = 0;
    XineramaScreenInfo *screens = XineramaQueryScreens(d, &scount); /* returns an array of screeninfo structs */
    for (int i = 0; i < scount; i++){ /* iterates through those screeninfo structs and pulls out what we need */
      monitor *temp;
      if (!(temp = (monitor *)calloc(1, sizeof(monitor)))) /* memory allocation */
        exit(1);

      temp->mx = screens[i].x_org; /* stores XY coordinates for the new monitor */
      temp->my = screens[i].y_org;

      temp->mw = screens[i].width; /* stores dimensions for the new monitor */
      temp->mh = screens[i].height;

      if (monlist){ /* if monlist exists, put it inside there */
        temp->next = monlist;
        temp->prev = monlist->prev;  /* linked list logic */
        monlist->prev->next = temp;
        monlist->prev = temp;
      } else { /* if it doesn't, start the list */
        monlist = temp;
        monlist->prev = temp; /* linked list logic */
        monlist->next = temp;
      }

      temp->workspace = i+1; /* start with workspace 1 */
      temp->clients = slist[temp->workspace]; /* start with workspace */

      if (i == 0)
        selmon = temp; /* make default monitor the first one made */
    }
  } else
#endif /* XINERAMA */
  { /* run this if Xinerama is not being used, makes one large screen */
    monitor *temp;
    if (!(temp = (monitor *)calloc(1, sizeof(monitor))))
      exit(1);

    temp->mx = 0;
    temp->my = 0;
    temp->mw = sw;
    temp->mh = sh;
    temp->next = temp;
    temp->prev = temp;
    temp->workspace = 1;
    temp->clients = slist[1];
    selmon = temp;
    monlist = temp;
  }
}

/* Switches the running variable to 0 to quit on command */
void quit(const Arg arg) {
  running = 0;
}

/* sets up some important things we need for properties right now */
void setup(void) {
  /* only setting up the ones currently used, more may be added as neccessary */
  netatom[NetWMWindowType] = XInternAtom(d,\
      "_NET_WM_WINDOW_TYPE", False);
  netatom[NetWMWindowTypeDialog] = XInternAtom(d,\
      "_NET_WM_WINDOW_TYPE_DIALOG", False);
  netatom[WMDelete] = XInternAtom(d,\
      "WM_DELETE_WINDOW", False);
}

int main(void) {
  XEvent ev; /* make a holder for XEvents */
  if (!(d = XOpenDisplay(0))) /* if the display isn't open, exit */
    exit(1);

  signal(SIGCHLD, SIG_IGN); /* sig handling */
  XSetErrorHandler(xerror); /* xerror handling */
  setup();

  int s = DefaultScreen(d);  /* set default screen to use */
  root = RootWindow(d, s);   /* setup root window for inputs */
  sw = XDisplayWidth(d, s);  /* screen width for doing tiling */
  sh = XDisplayHeight(d, s); /* screen height for doing tiling */

  monsetup();

  XSelectInput(d, root, SubstructureRedirectMask); /* select input from root window */
  XDefineCursor(d, root, XCreateFontCursor(d, 68)); /* define the cursor style so it looks normal */
  input_grab(root); /* grab inputs needed */

  while (running && !XNextEvent(d, &ev)) /* exit when running = 0 */
    if (events[ev.type])
      events[ev.type](&ev);

  XCloseDisplay(d);
  return EXIT_SUCCESS;
}
