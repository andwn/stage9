### S9A (Tile Attributes) Format Spec
 - Like S9M all multibyte values are big endian

#### Header
 - `2` bytes: Format Version
 - `2` bytes: Unused flags
 - `2` bytes: Number of tiles

#### Contents
 - `N*2` bytes: N is number of tiles
   - High Byte:
     - High 4 bits are "entrance" passability. Whether objects can "enter" the tile from each side
     - Low 4 bits are "exit" passability. Whether objects can leave the tile from each side
     - For both the order is `0x1` = left, `0x2` = up, `0x4` = right, `0x8` = down
     - Disabled flag (0) means solid and enabled (1) means passable
   - Low Byte:
     - High 2 bits are priority for `0x40` = top half and `0x80` = bottom half of the tile
     - Disabled (0) means below sprites, enabled (1) means above sprites
     - Low 6 bits are the terrain index, possible values 0-63
