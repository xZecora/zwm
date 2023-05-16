# zwm: my attempt at a tiling WM
### Should I move to rust?

- tiles
- floating windows
- workspaces
- patches
- Multi-Monitor (more or less)

If you want something more, add it! If you want some simple functionality, you can probably make it work. I also have patches for it over at [zwm patches](https://git.vitrial.xyz/vitrial/zwm-patches) (currently on hold for the time being), so maybe what you want is there.

# TODO
:heavy_check_mark: add multi-monitor functionality \
:heavy_check_mark: make things more patch oriented \
add more comments to make it readable as a resource \
:heavy_check_mark: thinking about changing hotkeys to function more like ratpoison (patch) \
add more tiling layouts \
considering making this a multi file project for readability sake

# Bugs
- errors when spamming hotkey presses, at least as far as I can tell thats the issue.
- sometimes windows can become disconnected from list and are not unmapped when switching workspaces.

# sources
A lot of inspiration and code taken from dwm and qpwm
