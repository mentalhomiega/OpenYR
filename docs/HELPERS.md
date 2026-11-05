# Shared helpers

The engine has about 500,000 lines of code, and the same small utility keeps
being written a second time by someone who did not know it existed. Scan this
page before you write one. Names are as typed in the code, and every path is
under `code/`.

When you add a helper that a second file could use, add a line here in the same
change. A helper that only one file uses stays in that file; the section at the
end lists a few that are worth moving to a header the next time a second file
needs them.

## Hashing and checksums

| Helper | Header | Use |
|---|---|---|
| `FNV_BASIS`, `FNV_PRIME` | `fnv.h` | Start and multiplier of the 32-bit FNV-1a hash. Start every hash from `FNV_BASIS`. |
| `Fnv_Add_Bytes(hash, data, length)` | `fnv.h` | Adds bytes in memory order. The result depends on padding and byte order, so hash fields, not whole structs. |
| `Fnv_Add_U32(hash, value)` | `fnv.h` | Adds a 32-bit value least significant byte first on any machine. Use it for hashes that two machines compare. |
| `FNV64_BASIS`, `FNV64_PRIME`, `Fnv64_Add_Bytes` | `fnv.h` | The 64-bit FNV-1a forms of the above. |
| `CRC::Memory(data, length, crc)`, `CRC::String(string, crc)` | `crc.h` | CRC-32 (polynomial 0x04C11DB7) of a block or a null-terminated string. Pass the previous result as `crc` to continue. |
| `CRCEngine` | `crc.h` | Streaming CRC. Call it with a value to add it, and with no argument to read the result. |
| `SHAEngine` | `sha.h` | The SHA-1 the game files use. `Hash(data, length)` adds data, and `Result(buffer)` writes the 20-byte digest. |
| `CnCNetSHA1Class`, `CnCNet_SHA1_Hex(text)`, `CnCNet_Hex_Digest(digest)` | `cncnet/cncnetsha1.h` | A second SHA-1 for room keys and file hashes, and the 40-digit lowercase hex form of a digest. Not for anything that needs collision resistance. |
| `Compute_Sim_State_Hash()` | `statehash.h` | Two hashes (`State`, `Targets`) of the state that decides play, which replay checks and autotests compare. Selection and other view state are not in them. |
| `Base64_Encode`, `Base64_Decode` | `base64.h` | Base 64 of a buffer; both take `(source, slen, dest, dlen)`. |

## File and path handling

| Helper | Header | Use |
|---|---|---|
| `Read_Disk_File(path, text)` | `diskfile.h` | Reads a whole disk file as bytes into a `std::string`. Returns false and leaves `text` alone when the file cannot be opened. |
| `Write_Disk_File(path, bytes)` | `diskfile.h` | Writes bytes over a disk file in place. Returns false when it cannot open or write. It does not write to a temporary file first. |
| `Read_File_Bytes(file, bytes)` | `data.h` | Reads a whole `FileClass` (a mix member, a search-path file or a `RawFileClass`). Returns false, with `bytes` empty, for a missing, empty or short read. |
| `Terminate_Path(path)` | `gamedirs.h` | Appends the platform separator unless the path is empty or already ends in `\`, `/` or `:`. |
| `Data_Directory()`, `User_Directory()` | `gamedirs.h` | The directories as applied. Each is empty or ends in a separator. Write nothing to the data directory. |
| `User_File_Write_Name`, `Saved_Game_Name`, `Screenshot_Name`, `Replay_Name`, `Music_Folder` | `gamedirs.h` | Where a file the game writes belongs. Do not build these paths by hand. |
| `Executable_Path(&truncated)` | `gamedirs.h` | Full path of the running executable. A path longer than `MAX_PATH` is cut and `*truncated` is set. |
| `Extension_Start(name)` | `strutil.h` | Position of the dot that starts the extension, or `npos`. A dot in a directory name does not count. |
| `Base_Name(path)`, `Directory_Prefix(path)` | `strutil.h` | The part of a path after its last `/` or `\`, and the part up to and including it. Each is a view into `path`. |
| `LCW_Uncomp_Checked(source, source_length, dest, dest_length)` | `lcw.h` | LCW decompression that never reads or writes outside the two buffers. Use it on data you did not build. `LCW_Uncomp` does no bounds check. |
| `Decode_PCX_RLE_Line(get, out, length, store, allow_empty_run)` | `pcxrle.h` | Decodes one PCX run-length line from a byte callback. Returns false when the data ends before the line does. |

## Strings and text

| Helper | Header | Use |
|---|---|---|
| `Lower(text)`, `Upper(text)` | `strutil.h` | Copies with each byte passed through `tolower` or `toupper`, so the letters changed follow the C locale. |
| `Trim_Chars(text, blanks)` | `strutil.h` | A view of `text` without the characters in `blanks` at either end. It points into `text`. |
| `Trimmed(text, blanks)`, `TRIM_SPACE_TAB`, `TRIM_SPACE_TAB_EOL`, `TRIM_ISSPACE` | `strutil.h` | The same trim as an owned string, and the three blank sets callers pass: space and tab, those plus CR and LF, and the six bytes `isspace` accepts in the C locale. Pick the set the file format needs. |
| `Trim_End_Chars(text, blanks)` | `strutil.h` | A view of `text` without the characters in `blanks` at its end only. |
| `Equals_No_Case(left, right)` | `strutil.h` | Whether two texts are equal when only ASCII letters ignore case. High-bit bytes never fold. |
| `Split_List(text, separator, blanks, skip_empty)` | `strutil.h` | Splits a list into trimmed strings. An empty `blanks` keeps entries as written. `skip_empty` drops the entries that are empty after trimming. |
| `Starts_With_No_Case(text, prefix)` | `strutil.h` | Whether `text` starts with `prefix`, ignoring ASCII case. For an exact match use `std::string::starts_with`. |
| `NameEntry`, `Enum_From_Name(table, text, fallback)`, `Enum_Name(table, value, fallback)` | `nametable.h` | Name table for an enum saved as text. The lookup ignores case with `stricmp`, takes the first match and gives `fallback` when none matches. Include `always.h` first. |
| `Split_Parts(text, separator)` | `strutil.h` | Splits into strings and keeps empty parts, so `"a,,b"` gives three. |
| `Split_Views(text, separator)` | `strutil.h` | The same split, but returns views into `text`, so `text` must outlive them. |
| `Split_Host_Port(text)` | `strutil.h` | Splits `host:port` at the last colon into two views of `text` and checks neither side. Returns nothing without a colon. Each caller keeps its own host and port rules. |
| `Parse_Decimal_Int(text)` | `strutil.h` | Reads the whole text as a decimal int. Returns nothing for anything but an optional minus sign and digits that fit. Blanks and a plus sign fail. |
| `CnCNet_Equals_No_Case`, `CnCNet_Less_No_Case` | `cncnet/cncnettext.h` | Case folding with `tolower`; only the C locale's letters fold. |
| `CnCNet_Is_Control_Byte(ch)`, `CnCNet_Is_Join_Unsafe(ch)` | `cncnet/cncnettext.h` | Tests for 0x00 to 0x1F and 0x7F, and for those plus space and comma. |
| `Hex_Text(value, digits, upper_case)` | `hexformat.h` | Hex text of a number, zero-padded to at least `digits` digits. A wider value is written in full. |
| `Hex_Digit_Value(ch)` | `hexdigit.h` | Value 0 to 15 of a hex digit in either case, or -1. |
| `ahtoi(str)` | `ahtoi.h` | Hex string to int, built on `Hex_Digit_Value`; stops at the first character that is not a hex digit. |
| `stricmp`, `strcmpi` | `always.h` | Case-insensitive C string compare. Off Windows both map to `strcasecmp`. |
| `Fetch_String(label)` | `csf.h` | String-table text for a label. A missing label gives `MISSING:'<label>'` and a log line. `Fetch_String(int)` in `data.h` is the old numeric form. |
| `Fetch_String_UTF8(label)`, `Wide_To_UTF8(wide)` | `csf.h` | The same text, or any table string, as UTF-8. |
| `UTF8::Decode`, `Encode`, `Is_Valid`, `Copy`, `Boundary_Before`, `From_Windows_1252`, `To_Windows_1252` | `utf8.h` | UTF-8 and code page conversion. `Copy` and `Boundary_Before` never cut a sequence in half. |
| `strtrim(buffer)` | `trim.h` | Trims white space from both ends of a C string in place. |

## Byte and endian writers

| Helper | Header | Use |
|---|---|---|
| `Put_U16_LE(out, value)`, `Put_U32_LE(out, value)` | `littleendian.h` | Append a value least significant byte first to any `push_back` container (`std::vector<std::uint8_t>`, `std::string`). |
| `Rgb565_Widen(pixel, r, g, b)` | `rgb565.h` | Widens a 16-bit RGB565 pixel to 8 bits per channel, with the top level giving 255. |
| `Widen_Six_Bit_Palette(palette)` | `palette.h` | Widens 256 six-bit guns (0 to 63) to a `PaletteClass`. |

## Time and clocks

| Helper | Header | Use |
|---|---|---|
| `Monotonic_Milliseconds()` | `monotonic.h` | Steady clock in milliseconds. Only differences mean anything. Not sync-safe. |
| `Monotonic_Elapsed_Milliseconds(begin)` | `monotonic.h` | Milliseconds since a `steady_clock` reading. |
| `Local_Time(stamp, parts)`, `Utc_Time(stamp, parts)` | `wallclock.h` | Portable calendar fields of a `time_t`. Returns false when the conversion fails; read `parts` only after true. |
| `Frame` | `ftimer.h` | The game frame counter. Sync-safe, and the same on every machine. |
| `FrameCountdown` | `ftimer.h` | `Begin(frames)` and `Is_Expired()` on the game frame. Use it for a timer that only needs a start and a length. |
| `CDTimerClass<FrameTimerClass>` | `timer.h` | Countdown that runs on game frames, for simulation. |
| `CDTimerClass<SystemTimerClass>` | `timer.h` | Countdown that runs on the Windows clock. For UI only; it differs between machines. |

## Random numbers

Simulation code must draw from the scenario generator (`Scen->RandomNumber`,
a `Random2Class`), and every machine must make the same draws in the same
order. A draw that only one machine makes, such as a visual effect, uses the
non-critical generator or it desyncs the game.

| Helper | Header | Use |
|---|---|---|
| `Random_Pick(a, b)` | `ccrand.h` | Integer from `a` to `b` inclusive. Sync generator. |
| `Percent_Chance(percent)` | `ccrand.h` | True `percent` times in 100. Sync generator. |
| `Probability_Of(p)` | `ccrand.h` | True with fractional probability `p`, quantised to 1/10000. Sync generator. |
| `Random_Double(a, b)` | `ccrand.h` | Real number between two doubles. Sync generator. The `int, int` overload is a different function (it weighs a percentage), so spell both arguments as doubles. |
| `Sim_Random_Pick(a, b)`, `Sim_Percent_Chance(percent)` | `ccrand.h` | The same draws from `NonCriticalRandomNumber`. Not sync-safe by design: use them only for results that cannot change the game. |
| `Random_Dir(min, max)` | `inline.h` | A random full-circle `DirType` from the sync generator. `min` and `max` are ignored. |
| `Coord_Scatter(coord, distance, lock)` | `coord.h` | A random coordinate around an anchor, from the sync generator. |
| `TypeList<T>::Pick(index)`, `Random_Pick(rand)` | `typelist.h` | An element by index (wrapped) or at random. An empty list gives `T()`. |

## Maths and geometry

Positions are integer leptons, 256 to a cell (`CELL_LEPTON`). There is no
fixed-point class.

| Helper | Header | Use |
|---|---|---|
| `TPoint2D<T>`, `TPoint3D<T>` `Length()`, `Distance_To()`, `Normalize()`, `Dot_Product()` | `point.h` | Vector maths with double-precision square roots. Fine for UI and approximate work. |
| `Gamemd_Length()`, `Gamemd_Distance_To()`, `Gamemd_Distance(a, b)` | `point.h` | Lengths as gamemd computes them. Simulation code that must match gamemd uses these in place of `Length`. |
| `Gamemd_Sqrt`, `Gamemd_Sine`, `Gamemd_Cosine`, `Gamemd_Acos`, `Gamemd_Atan2`, `Gamemd_Length_2D`, `Gamemd_Length_3D`, `Gamemd_Magnitude_2D`, `Gamemd_Magnitude_3D` | `gmmath.h` | gamemd's table-based results bit for bit, identical on every machine. |
| `Gamemd_Lepton_To_Pixel`, `Gamemd_Reveal_Shift_Cells` | `gmmath.h` | gamemd's height-to-pixel lift and the sight shift it implies. |
| `Truncate_To_Single(value)` | `always.h` | Rounds a double toward zero to single precision, as gamemd's float stores do. |
| `Parse_Nearest_Double`, `Parse_Nearest_Single`, `Snprintf_Nearest` | `numparse.h` | Number text read and written under round-to-nearest, whatever the FPU mode. |
| `Distance(cell1, cell2)`, `Distance(coord1, coord2)` | `inline.h` | Distance in cells or leptons. Uses `Length`, so it is not gamemd-exact. |
| `Cell_To_Lepton`, `Lepton_To_Cell` | `inline.h` | Cell counts and leptons. |
| `Coord_Snap`, `Coord_Fraction`, `Coord_Whole`, `Coord_Mid` | `inline.h` | Cell centre, the offset inside the cell, the cell's corner, and the midpoint of two coordinates. |
| `Move_Coord(start, dir, distance)`, `Move_Cell(start, dir, distance)` | `inline.h` | Moves a point along a direction. `Move_Coord` uses the gamemd sine and cosine. |
| `Lerp(a, b, t)` | `wave.h` | Interpolates two `Coord` or two `Point2D`. `RGBClass::Lerp` does the same for colours. |
| `Union(r1, r2)`, `Intersect(r1, r2)`, `Bias_To(point, rect)` | `rect.h` | Rectangle merge and clip. `Intersect` returns an invalid rectangle when they do not overlap; test it with `Is_Valid()`. |
| `Clip_Line_To_Rect(p1, p2, rect)`, `Blit_Clip(...)` | `xsurface.h` | Clips a line or a blit to a rectangle. |
| `Binary_Search(array, n, target)` | `bsearch.h` | Pointer to the element equal to `target` in a sorted array, or `NULL`. |
| `ARRAY_SIZE(x)` | `always.h` | Element count of an array, as an `int`. |

### Directions and facings

| Helper | Header | Use |
|---|---|---|
| `DirType` | `face.h` | A 16-bit direction. `As_Facing()`, `As_Dir8/32/256()`, `As_Radian()` and `Round_To_8/16/32/64/256()` convert it. |
| `Direction(from, to)` | `inline.h` | The `DirType` from one `Coord`, `Cell` or `Point2D` to another. |
| `Dir_Facing(dir)`, `Facing_Dir(facing)` | `inline.h` | Between `DirType` or `Dir256` and the eight `FacingType` values; precision is lost going to a facing. |
| `Facing_Add(a, b)`, `Facing_Sub(a, b)` | `facing.h` | Wrap-around arithmetic on the eight facings. |
| `Shape_Facing_Index(dir, count)`, `Eight_Facing_Entry(dir, count)` | `face.h` | Frame index for a shape with 8, 16, 32 or 64 facings, and the entry of an 8-way list. Index 0 is northwest for the first and north for the second. |
| `Facing_Between_Points(p1, p2)` | `wave.h` | The nearest facing from one screen point to another, in screen space. |

## Cells and the map

| Helper | Header | Use |
|---|---|---|
| `Adjacent_Cell(cell_or_coord, facing)` | `inline.h` | The neighbouring cell or coordinate. Takes `FacingType` or `Dir256`. |
| `Cell_Spread_Count(radius)`, `Cell_Spread_Offset(index)` | `combat.h` | The cells within a radius, nearest first, in gamemd's order. Walk them for area effects. |
| `Map[cell]`, `Map.Is_Valid(cell)`, `Map.In_Radar(cell)` | `map.h` | Cell lookup, bounds test and the radar-area test. |
| `Cell::As_Coord(z)`, `Coord::As_Cell()` | `coord.h` | Between a cell and its centre coordinate. |
| `Map_Cell_Index(cell)`, `Map_Cell_Count()` | `astar.h` | Flat index of a cell and the cell total. |
| `Follow_Path(cell, count, path)` | `astar.h` | The cell reached by following a move list. Tunnel moves are followed to their far mouth. |
| `Spawn_House_Waypoint(name_or_house)` | `spawnhouse.h` | Start waypoint 0 to 7 of a `Spawn1`..`Spawn8` house, or -1. |
| `Screen_To_View_Point`, `View_To_Screen_Point`, `Screen_To_View_Offset`, `View_To_Screen_Offset` | `viewzoom.h` | Between screen pixels and the zoomed tactical view. Do not scale by hand. |

## Iteration over game objects

| Helper | Header | Use |
|---|---|---|
| `Technos`, `Objects` | `globals.h` | The global lists of every techno and every object. The count can change while you loop, so re-read it each pass. |
| `Beacons.For_Each_Visible(fn)` | `beacon.h` | Calls `fn(house, beacon)` for each beacon the local player can see, in house and slot order. |
| `CargoClass::For_Each_Attached(body)` | `cargo.h` | Calls `body(object)` for each passenger. The next link is read after `body` returns, so it may change that object. |
| `RadarTrackingClass::For_Each_Pixel(visit)` | `radar.h` | Visits each occupied radar pixel once, with the object drawn on it. |
| `Detach_This_From_All(target, all)` | `tracker.h` | Removes every reference to `target` before it dies. Pass `all` false for a cloaking object that stays in the game. |
| `DynamicVectorClass<T>` | `vector.h` | The growable array. `Add`, `Delete(object)`, `Delete_Index`, `Count()`, `ID(object)`, which returns -1 when absent. |
| `TypeList<T>::Is_In_List(object)` | `typelist.h` | Membership test on a `DynamicVectorClass`. |
| `TFind_Or_Make(name, vector)` | `findmake.h` | The type with that INI name, or a new one. `none` and `<none>` give `NULL`. |

## Scope guards

Each of these is not copyable. Declare it as a local and let the scope end.

| Helper | Header | Use |
|---|---|---|
| `SurfaceLock` | `surface.h` | Locks a surface until the end of scope. `As<T>()` is the pixel pointer, `Is_Valid()` is false when the lock failed, and a failed lock is not unlocked. |
| `ScenarioInitScope` | `globals.h` | Raises `ScenarioInit` until the scope ends, on every path out. |
| `PerfScope(phase)` | `perfstat.h` | Times a phase. Costs one flag test when `PerfLog` is off, and a phase already running in an outer scope adds nothing. |
| `HangWatchDialogScope`, `HangWatchLoadScope`, `HangWatchCounterScope<Counter>` | `hangwatch.h` | Hold the hang watchdog back during a native dialog or a scenario load. |
| `EmissiveDrawScope(strength)` | `emissive.h` | Sets the emissive draw strength and restores the previous one. |
| `PlanningRecordScope(recording)` | `planning.h` | While alive with `recording` true, the player's orders become planned orders. |
| `SaveStreamClass::BoundScope(stream, end)` | `savestream.h` | Limits a load to one record, so a member that reads too much is refused. Nested scopes restore the outer limit. |
| `SelectionVoiceScope(speaks)` | `techno.h` | Selects several objects so that only the first one speaks. Select through its `Select(object)`. |
| `WinsockGuard` | `cncnet/cncnetnative.h` | Holds a Winsock reference; `Started()` is false when startup failed. Always true off Windows. |
| `BenchScopeClass(which)` | `bench.h` | Debug builds only; ends a benchmark on every return path. |

## Logging and diagnostics

| Helper | Header | Use |
|---|---|---|
| `DebugString(format, ...)` | `dbgprint.h` | printf-style line in the debug log, the debugger and the console. A message that starts a line is time-stamped. |
| `DebugStringNoPrefix(format, ...)` | `dbgprint.h` | The same without the stamp, for continuing a line. |
| `Debug_Log_Block(text, wait_ms)` | `dbgprint.h` | Writes whole lines under one lock and flushes. Windows only; returns false when the lock stays busy. |
| `Last_Error_Text(error)` | `dbgprint.h` | Text of a Windows error code, in a per-thread buffer that the next call overwrites. |
| `AUTOTEST_LOG(...)` | `autotest.h` | `DebugString` with an `AUTOTEST ` prefix, only in an unattended run. Its arguments are not evaluated otherwise, so they may follow pointers that exist only in a test. |
| `AutoTest_Active()` | `autotest.h` | True in an `-AUTOTEST` run. |
| `Perf_Count(counter, amount)` | `perfstat.h` | Adds to a performance counter while recording is on. |
| `Hang_Watchdog_Beat()` | `hangwatch.h` | Call from a message pump; only beats from the main thread count. |

## INI reading

Methods on `INIClass` (`ini.h`) and `CCINIClass` (`ccini.h`). Every getter takes
a default and returns it when the section or entry is missing.

| Helper | Use |
|---|---|
| `Get_Bool` | True for a first letter `Y`, `T` or `1`, false for `N`, `F` or `0`, the default for anything else. |
| `Get_Int`, `Get_Hex`, `Get_Double` | Integer, hex (`%x`) and double. |
| `Get_Float` | Reads at single precision as gamemd does, and a trailing `%` divides by 100. Apply `Truncate_To_Single` where gamemd stores a float. |
| `Get_String(section, entry, default)` | Returns a `std::string`; the buffer overloads and the `TStringID<SIZE>` overloads in `ccini.h` also exist. |
| `Read_Numbers` | Reads a fixed count of ints or floats and reports `Absent`, `Malformed` or `Parsed`. |
| `Get_Lepton`, `Get_Angle`, `Get_Cell`, `Get_RGBClass`, `Get_IntList`, `Get_Vector`, `Get_Offset` | Typed reads. `Get_Lepton` converts cells to leptons and `Get_Angle` converts degrees to 0 to 255. |
| `Is_Present(section, entry)`, `Section_Present` | Test for a key or section without reading it. |
| `TGet_TypeList(ini, section, entry, default, size)` | `findmake.h`. Reads a list of type names into a `TypeList`. |

## Colour

| Helper | Header | Use |
|---|---|---|
| `RGBClass`, `HSVClass` | `rgb.h`, `hsv.h` | Colours that convert to each other with a cast. `RGBClass::Lerp(a, b, t)` interpolates and clamps. |
| `Fetch_Scheme_By_Name(name)` | `scheme.h` | A `ColorScheme` by name. `Fetch_Scheme_Index_By_Name` gives its index. |
| `House_Display_Scheme(house)`, `House_Display_RGB(house)` | `playerdisplay.h` | The scheme and radar colour to draw a house with in the current view, including the colour-blind-safe view. Do not read the house colour directly for display. |
| `Player_Color_Pack`, `Player_Color_Unpack`, `Player_Color_Parse`, `Player_Color_Preview` | `playercolor.h` | Custom player colours as a packed value. |
| `Player_Color_Distance`, `Player_Color_Too_Close` | `playercolor.h` | How far apart two colours look, taking the smallest over normal, protanopic and deuteranopic vision. |
| `ColorBlind_Matrix(mode, strength, matrix)` | `colourblind.h` | The nine-value correction matrix for a mode; the identity for Off. |
| `ColorBlind_Simulation_Matrix`, `COLORBLIND_PROTANOPIA_SIMULATION_D`, `COLORBLIND_DEUTERANOPIA_SIMULATION_D` | `colourblind.h` | The simulation matrices, as float and double. |
| `HDR_COLOUR_ADJUST_IDENTITY` | `hdrcolour.h` | The identity colour adjustment. |
| `Weapon_Beam_Colors(weapon, house, inner, outer)` | `laser.h` | The inner and outer beam colours of a weapon. |
| `Shape_Opacity(flags)` | `draw.h` | Opacity 0 to 255 of a translucency flag; 255 without one. |

## Surfaces and drawing

| Helper | Header | Use |
|---|---|---|
| `Allocate_Tactical_Surfaces()` | `init.h` | Allocates the hidden, composite, tile and sidebar surfaces for the current interface, and returns the tactical view rectangle. Call it after a display mode change or a save load, then set the map's view size. |
| `Allocate_Surfaces(hidden, composite, tile, sidebar, hidden_first)` | `init.h` | The same with explicit rectangles. |
| `Allocate_Map_Surfaces()` | `viewzoom.h` | Rebuilds the map's own surfaces to match the view and zoom. |
| `Draw_Shape(...)`, `Blit_Block(...)` | `draw.h` | Draw a shape or surface block with a convert table and clip window. |
| `Video_Mark_Dirty()` | `video.h` | Records that the visible surface was drawn to since the last present. |
| `Release_Handle(handle)` | `bgfxviews.hh` | Destroys a valid bgfx handle and resets it to invalid; an invalid handle is left alone. |

## Save and load

| Helper | Header | Use |
|---|---|---|
| `SaveVersionInfo::REVISION` | `savever.h` | The layout of the saved game state. Raise it by one with every change to what a save holds. A build refuses any other revision. |
| `Is_Current_Revision()`, `Is_Expected_Version(v)`, `Can_Load(v)` | `savever.h` | Revision test, internal version test, and both. Ask each one alone when the message must say which failed. |
| `Get_Savefile_Info(name, info)` | `saveload.h` | Reads a save's listing fields without loading it. |
| `Save_Object`, `Load_Object`, `Load_Object_As<T>` | `saveload.h` | Object records. `Load_Object_As` destroys a record of the wrong class and fails the stream. |

## UI screens

| Helper | Header | Use |
|---|---|---|
| `Clamp_Low_Wins(value, low, high)` | `ui/uiclamp.h` | Like `std::clamp`, but safe when rules leave the range empty (low above high): then a value below low gives low and any other value gives high. |
| `Fill_Count_Choices(list, values, limit)` | `ui/uiscreen.h` | Adds a choice labelled with its number for each value; returns the index of the last value not above the limit, or -1. Used by the sidebar row counts. |
| `UI_Name_At(names, index)` | `ui/uiscreen.h` | The name at an index, or an empty string outside the list. |
| `UI_Difficulty_Names()` | `ui/uienginehost.h` | The display names of the difficulty levels, easiest first. |
| `UI_Fill_Stock_Colors(colors, limit)`, `UI_Stock_Color_Count()`, `UI_Stock_Color_Name(i)` | `ui/uienginehost.h` | The eight stock player colours (name, index, CSS colour) for a lobby's colour list. |

## Lobby and network

| Helper | Header | Use |
|---|---|---|
| `UI_Net_Apply_Switch(which, on)`, `UI_Net_Apply_Slider(which, value)` | `ui/screens/netlobby/uinetlobby.h` | Write one lobby option into `Session.Options`. Return false for a control with no option. The caller sends the change. |
| `UI_Net_Fill_Options(state, ai_max, credits_min)` | `ui/screens/netlobby/uinetlobby.h` | Fills a lobby state from `Session.Options`. Used by LAN and CnCNet. |
| `CNCNET_OPTION_*` | `cncnet/cncnetspawn.h` | Names of the CnCNet room options. |
| `SPAWN_ALLY_KEYS` | `cncnet/spawnkeys.h` | SPAWN.INI alliance keys in slot order. Readers and writers must share this order. |
| `NativeSocket`, `NO_SOCKET`, `Close_Native(handle)` | `cncnet/cncnetnative.h` | Socket handle type for Winsock and POSIX. |
| `Show_Random_Map_Preview()` | `netshare.h` | Shows a random map's picture as the dialog preview. |

## Sound, messages and speech

| Helper | Header | Use |
|---|---|---|
| `Sound_Effect(voc, volume, pan, handle, group)` | `voc.h` | A sound with no place in the world. With a handle, a live sound of the same type is re-aimed. |
| `Sound_Effect(voc, coord, handle, group)` | `voc.h` | A sound at a map position, attenuated and panned by the view. |
| `Unit_Voice_Effect(voc)` | `voc.h` | A unit voice response; it follows the unit-voices volume option. |
| `Play_If_In_Range`, `Follow_Sound` | `voc.h` | Keep a placed sound going while it is in range. |
| `Speak(voice)`, `Speak_Eva(name)` | `vox.h` | EVA speech. |
| `Post_Player_Message(text, ticks)` | `playerdisplay.h` | Message in the local player's scheme. |
| `Show_Super_Message(label, style)` | `super.h` | A string-table message for ten seconds. |
| `Planning_Refusal(label)`, `Planning_Message(label)` | `planning.h` | Planning-mode refusal (message and scold sound, at most once a frame) and message. |

## Game actions

| Helper | Header | Use |
|---|---|---|
| `Queue_Produce(type, id, count)` | `sidebar.h` | Announces production and sends the player's PRODUCE event `count` times. |
| `Do_Reinforcements(team, waypoint)` | `reinf.h` | Brings in a team. |
| `Spawn_Named_Anim(name, coord, ...)`, `Spawn_Wake(where)` | `anim.h` | Create an animation by name, or a wake. |
| `Attach_Control_Ring(techno, type)` | `capture.h` | Attaches the mind control ring animation. |
| `Cloak_Cell_Occupiers(cell)` | `building.h` | Asks the mobile occupants of a cell to reconsider cloaking. |
| `Linked_Building_Underfoot(unit)` | `building.h` | The building a unit stands on while radio-linked to it, or `NULL`. |
| `Team_Has_Members(team)` | `init.h` | True when a player-controlled, non-limbo object belongs to the team. |
| `Strike_Cell_Damage(...)`, `Wide_Area_Damage(...)`, `Explosion_Damage(...)` | `combat.h` | Cell, area and blast damage. |
| `Aim_Coord(target)`, `Beam_Coord(target)` | `object.h` | The point a projectile homes on, and the point a beam is drawn to. They differ for a cell on a bridge. |
| `Find_Armor_Type(name, armor)` | `armortypes.h` | Case-insensitive armor lookup; leaves `armor` alone and returns false for an unknown name. |
| `Paradrop_Plane_Type()` | `house.h` | The PDPLANE type, or `NULL` when the rules have none. |
| `Opposite_Difficulty(d)`, `Computer_Difficulty_Name(d)` | `diff.hh` | The computer difficulty paired with a human one, and its lobby name. |
| `Key_Bits(shift, ctrl, alt)`, `Key_Bits_From_Window(mods)` | `keyboard.h` | The `WWKEY_*_BIT` modifier bits. |

## Mode and state queries

| Helper | Header | Use |
|---|---|---|
| `AutoTest_Active()` | `autotest.h` | An unattended test run. |
| `Spawner_Is_Active()`, `Spawner_Is_Requested()` | `spawner.h` | Whether the game was started by the spawner. |
| `Replay_Is_Playing()` | `replay.h` | A replay is playing back. |
| `Shellmap_Running()`, `Shellmap_In_Scope()`, `Shellmap_Silent()` | `shellmap.h` | The menu battle is loaded, its session state is swapped in, and its sounds are off. |
| `Modern_Interface()` | `hud.h` | The modern panel interface is on. |
| `MPMode_Is_Cooperative()` | `mpmode.h` | The running match is Cooperative. |

## Local to one file

These are written once, in a `.cpp`, and nothing else can call them. Move one
to a header when a second file needs it.

| Helper | File | Use |
|---|---|---|
| `For_Technos(filter, action)` | `autotest.cpp` | Runs an action on every techno a filter accepts, re-reading the count each pass. |
| `Run_Start_Cell(cell, step, part_of_run)` | `map.cpp` | First cell of a bridge run. |
| `For_Each_Ring_Offset(ring, visit)` | `slaveman.cpp` | Visits the offsets of one square ring. |
| `Enum_Name(table, value, fallback)` | `options.cpp` | Name of an enum value from a table. |
| `Find_Type_In(types, name)`, `Type_Name_At(types, id, none)`, `Named_House(name)` | `autotest.cpp` | Look up a techno type by name or ID, or a house by name (including "player"). |
| `Map.Cross_Bridge_Deck(old, new, bridge_height)` | `map.cpp` | Whether one step dropped onto or rose into a bridge deck. |
| `Mount_If_Available(name)`, `Add_CC_Expansion_Mix(name)`, `Add_Disk_Expansion_Mix(name)` | `init.cpp` | Mount a mixfile only when the game can find it. |
