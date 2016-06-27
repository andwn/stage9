### S9A (Tile Attributes) Format Spec
 - Like S9M all multibyte values are big endian

#### Header
 - `2` bytes: Format Version
 - `2` bytes: Number of tiles

#### Contents
 - `N*2` bytes: N is number of tiles. High byte is the flags below. Low byte is the terrain index
   - `0x01`, `0x02`, `0x04`, `0x08`: Passability left, up, right, down. 1 = Passable, 0 = Solid
   - `0x10`, `0x20`: Priority. 0 = Below player, 1 = Above. 0x10 Top half, 0x20 bottom half
   - `0x40`, `0x80`: Unused / Reserved
