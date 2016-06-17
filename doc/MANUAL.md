## Manual - Editor

#### Menus
These should work like menus in any other program.
- Map: Actions for the Map, Properties allows changing things like name and size
- Tileset: Load a tileset image
- Tile Attributes: Controls for an attribute file (stuff like passability and draw order)

#### Toolbar
There will be buttons next to the menu that do things someday

#### Map Layout
This will show the tiles on the map is a WYSIWYG fashion. 
If no tileset is loaded it will show numbers instead.
Left click will replace the tile under the cursor with the one currently selected in the tileset.
Arrow keys will scroll the map around if the map is large enough to scroll.
Eventually I will add scrollbars but right now I'm lazy.

#### Tileset
This is pretty self explanatory you left click a tile to select it.

## Manual - Sample Game
The sample game is an example of how the maps can be used in a game.
It controls like a top down RPG, use the d-pad to move, B to run.
The code and algorithm are meant to be reused for a full game.

#### Change the game to load your own map
Firstly, you want to drop your map and tileset in the game/res directory.
The tileset image must be indexed 16 colors. The first color is transparent.
Add a PALETTE and TILESET line for your tileset, and BIN for your map.
Modify game/src/main.c, at the start of main() to load your tileset and map.
Note that tilesets must be exactly 128 pixels wide to display correctly.

#### TODO List
- [ ] Actually make tile attributes do something
- [ ] Placement modes: Pencil, Box, Flood fill
- [ ] Selection tool: Cut, Copy, Paste
- [ ] Undo

