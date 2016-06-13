# Stage9
A level editor and renderer for Genesis/MegaDrive. 
The editor interface aims to be cross platform using SDL2 and GTK2.
A sample game is included that loads, renders and scrolls a map.

#### User Manual
[Still working on this.](doc/MANUAL.md)

#### Prebuilt Binaries
These will be in [Releases](https://github.com/andwn/stage9/releases) once I get around to it.

#### Building (GNU/Linux)
- Editor:

1. Make sure the development packages for `SDL2` `SDL2_image` and `GTK2` are installed
2. From the `editor` directory type `make`

- Game:

1. Install [Gendev](https://github.com/kubilus1/gendev.git)
2. From the `game` directory type `make`

#### Building (Windows)
- I have not tested this
- Editor:

1. Install either MinGW or MSYS2
2. The dependencies are `SDL2` `SDL2_image` and `GTK2`
3. From the `editor` directory type `make`

- Game:

1. Install [SGDK](https://github.com/Stephane-D/SGDK.git)
2. From the `game` directory type `make`

#### Building (OSX)
- I know nothing about Macs so somebody is going to have to figure this out for me

#### License
- Editor: GPLv3 or later
- Sample Game: MIT
- Art Assets: See [resources.res](game/res/resources.res)
