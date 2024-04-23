# zwm: my attempt at a tiling WM

- tiles
- floating windows
- workspaces
- patches
- Multi-Monitor (more or less)

If you want something more, add it! If you want some simple functionality, you can probably make it work. I also have patches for it over at [zwm patches](https://git.vitrial.xyz/vitrial/zwm-patches) (currently on hold for the time being), so maybe what you want is there.

# TODO
- [x] add multi-monitor functionality
- [x] make things more patch oriented
- [ ] add more comments to make it readable as a resource
- [x] thinking about changing hotkeys to function more like ratpoison (patch)
~~- [ ] add more tiling layouts~~
- [ ] forget tiling layouts, I love frames so much, let's implement them
- [ ] <span style="font-size:0.5em;">...okay maybe that'll just be a patch, lets add more tiling patterns</span>
- [ ] considering making this a multi file project for readability sake

# Bugs
- errors when spamming hotkey presses, at least as far as I can tell thats the issue.
- sometimes windows can become disconnected from list and are not unmapped when switching workspaces.

# sources
A lot of inspiration and code taken from dwm and qpwm
