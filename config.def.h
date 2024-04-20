#ifndef CONFIG_H
#define CONFIG_H

/* mod key */
/* Mod4Mask - win */
/* Mod1Mask - alt */
#define MOD Mod4Mask

/* set app launcher */
const char *menu[] = {"dmenu_run", 0};

/* set terminal command */
const char *term[] = {"st", 0};

/* set hotkeys */
static struct key keys[] = {
    {MOD, XK_q, wkill, {0}},

    {MOD, XK_n, wprev, {0}},
    {MOD, XK_o, wnext, {0}},
    {MOD, XK_e, wmoveup, {0}},
    {MOD, XK_i, wmovedown, {0}},

    /* May want to switch these to be more comfortable with your monitor setup */
    /* 0/1 changes the direction you swap to */
    {MOD | ShiftMask, XK_n, mongo, {.i = 1}},
    {MOD | ShiftMask, XK_o, mongo, {.i = 0}},

    {MOD, XK_f, wfloatt, {0}},

    {MOD, XK_d, run, {.com = menu}},
    {MOD, XK_Return, run, {.com = term}},
    {MOD | ShiftMask, XK_q, quit, {0}},

    /* workspaces bindings */
    {MOD, XK_1, sgo, {.i = 1}},
    {MOD, XK_2, sgo, {.i = 2}},
    {MOD, XK_3, sgo, {.i = 3}},
    {MOD, XK_4, sgo, {.i = 4}},
    {MOD, XK_5, sgo, {.i = 5}},
    {MOD, XK_6, sgo, {.i = 6}},
    {MOD, XK_7, sgo, {.i = 7}},
    {MOD, XK_8, sgo, {.i = 8}},
    {MOD, XK_9, sgo, {.i = 9}},
    {MOD, XK_0, sgo, {.i = 0}},
    {MOD | ShiftMask, XK_1, wtos, {.i = 1}},
    {MOD | ShiftMask, XK_2, wtos, {.i = 2}},
    {MOD | ShiftMask, XK_3, wtos, {.i = 3}},
    {MOD | ShiftMask, XK_4, wtos, {.i = 4}},
    {MOD | ShiftMask, XK_5, wtos, {.i = 5}},
    {MOD | ShiftMask, XK_6, wtos, {.i = 6}},
    {MOD | ShiftMask, XK_7, wtos, {.i = 7}},
    {MOD | ShiftMask, XK_8, wtos, {.i = 8}},
    {MOD | ShiftMask, XK_9, wtos, {.i = 9}},
    {MOD | ShiftMask, XK_0, wtos, {.i = 0}},
};

#endif
