### S9M (Tile Map) Format Spec
 - All multibyte values are stored big endian
 - All values are unsigned

#### Header
 - `2` bytes: Format Version
 - `1` byte: Flags
   - `0x01`: Enable upper layer
   - `0x02`: Draw lower layer to PLAN_A (assumes upper layer is NOT enabled)
   - `0x04`: Single byte tiles (reduces file size)
   - `0x08`: Wrap Horizontal
   - `0x10`: Wrap Vertical
   - `0x20` / `0x40` / `0x80`: Unused / Reserved
 - `1` byte: Length of the map's name. 0 is valid and means no name
 - `N` bytes: Map's name in ASCII. N is the length above. This is NOT null terminated
 - `2` bytes: Stage width in tiles / blocks
 - `2` bytes: Stage height in tiles / blocks
 - `W*H` bytes, or `W*H*2` bytes: Tileset index to draw for each tile in the map (Lower Layer)
   - Will be 1 byte per tile or 2 depending on the flag 0x04
 - `0` bytes, `W*H` bytes, or `W*H*2` bytes: Upper layer
   - Will be 1 byte per tile or 2 depending on the flag 0x04
   - Will not exist if flag 0x01 is disabled
