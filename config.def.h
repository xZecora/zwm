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
    {MOD, XK_q, window_kill, {0}},

    {MOD, XK_n, window_prev, {0}},
    {MOD, XK_o, window_next, {0}},
    {MOD, XK_e, window_move_up, {0}},
    {MOD, XK_i, window_move_down, {0}},

    /* May want to switch these to be more comfortable with your monitor setup */
    /* 0/1 changes the direction you swap to */
    {MOD | ShiftMask, XK_n, go_to_monitor, {.i = 1}},
    {MOD | ShiftMask, XK_o, go_to_monitor, {.i = 0}},

    {MOD, XK_f, window_float_toggle, {0}},

    {MOD, XK_d, run, {.com = menu}},
    {MOD, XK_Return, run, {.com = term}},
    {MOD | ShiftMask, XK_q, quit, {0}},

    /* workspaces bindings */
    {MOD, XK_1, go_to_workspace, {.i = 1}},
    {MOD, XK_2, go_to_workspace, {.i = 2}},
    {MOD, XK_3, go_to_workspace, {.i = 3}},
    {MOD, XK_4, go_to_workspace, {.i = 4}},
    {MOD, XK_5, go_to_workspace, {.i = 5}},
    {MOD, XK_6, go_to_workspace, {.i = 6}},
    {MOD, XK_7, go_to_workspace, {.i = 7}},
    {MOD, XK_8, go_to_workspace, {.i = 8}},
    {MOD, XK_9, go_to_workspace, {.i = 9}},
    {MOD, XK_0, go_to_workspace, {.i = 0}},
    {MOD | ShiftMask, XK_1, window_to_workspace, {.i = 1}},
    {MOD | ShiftMask, XK_2, window_to_workspace, {.i = 2}},
    {MOD | ShiftMask, XK_3, window_to_workspace, {.i = 3}},
    {MOD | ShiftMask, XK_4, window_to_workspace, {.i = 4}},
    {MOD | ShiftMask, XK_5, window_to_workspace, {.i = 5}},
    {MOD | ShiftMask, XK_6, window_to_workspace, {.i = 6}},
    {MOD | ShiftMask, XK_7, window_to_workspace, {.i = 7}},
    {MOD | ShiftMask, XK_8, window_to_workspace, {.i = 8}},
    {MOD | ShiftMask, XK_9, window_to_workspace, {.i = 9}},
    {MOD | ShiftMask, XK_0, window_to_workspace, {.i = 0}},
};

#endif
