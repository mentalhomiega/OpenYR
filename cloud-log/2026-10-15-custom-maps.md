# Custom map readers log, 2026-10-15

Branch `cloud/custom-maps-2026-10-15`, begun from `origin/modern` (`4d36e67d`). The worktree I was handed sat on an older commit, so I switched to `origin/modern` before the first change.

The sources were public pages only (ModEnc's pages on IsoMapPack5, OverlayPack, PreviewPack, Triggers, Tags, Events, Map and Basic, and the CnCNet resource docs). None of them compares Westwood's maps with FinalAlert 2 or World-Altering Editor output directly, so the comparison below is what those pages say about each section, set against our readers. Nothing here was checked against gamemd or a real editor-saved map.

## Commits

| Commit | What it does |
| --- | --- |
| Accept zero padding and empty blocks in LZO terrain packs | `LZOStraw` no longer calls a stream damaged for zero bytes after the last block, for a block header of two zero counts, or for bytes after a block's end marker when the block expands to its stated size. Test in `tests/lzoblock`. |
| Accept zero padding after the last terrain record | `Read_Cell_Records` reads one to three zero bytes where the next record would start as the end of the list. Test in `tests/cellrecords`. |
| Place no overlay for a type index the rules do not define | `OverlayClass::Read_INI` indexed `OverlayTypes` with any byte up to 254. `Overlay_Index_From_Byte` in `code/overlaylayer.h` returns no overlay for 255 and for an index past the type count. Test in the new `tests/custommaps`. |
| Read the map preview from wherever its sections stand | `MapPreviewClass::Read_INI_Preview` parsed only the text before the first `[Map]`. `Preview_Sections_Text` in `code/previewtext.h` now picks `[Preview]` and `[PreviewPack]` out of the whole file. Test in `tests/custommaps`. |

Each behaviour change has a record in `manual/changes/` and an edit to `manual/content/formats/scenario-terrain.md`. The page gains a short "Preview pack" section.

All four are intentional changes that make a reader accept more than it did. I could not check any of them against gamemd. The first two follow from the earlier terminator fix (`3ad785e1`), which already reads a stream that ends between records as whole; padding that carries no data is treated the same way. The third turns an out-of-range read into no overlay. The fourth is the only one that changes what the multiplayer map list shows.

## How editor maps differ from Westwood's, per section

| Section | What the public pages say | Our reader |
| --- | --- | --- |
| `[IsoMapPack5]` | 11-byte records and a 4-byte zero terminator, LZO blocks with an output size of 8192 except the last. Tile index `0xFFFF` means no tile. Cells without a record become height 0 clear ground. | The terminator is optional since `3ad785e1`, and now so is zero padding. Tile indexes past the tile list become no tile in `CellClass::Recalc_Attributes` (`code/cell.cpp:1300`). |
| `[OverlayPack]`, `[OverlayDataPack]` | 262,144 bytes after LCW, one per cell at `X + 512 * Y`; `255` is no overlay. FinalAlert 2 limits frames to 60. | Overlay indexes past the rules' last type no longer index past the type list. Both packs are read only when `[Basic] NewINIFormat` is above 1 (`code/overlay.cpp:397`). |
| `[PreviewPack]` | LZO blocks of pixels, 3 bytes each, no row padding. `[Preview] Size` gives the picture size. FinalAlert 2 can make a lower quality preview from tiles; XCC Mixer and CNCMaps can inject a better one. Proportions that stray from the documented ratio leave black pixels. | The picture is read where the sections stand in the file. The byte order of a pixel is an open question below. |
| `[Triggers]` | Eight fields per row. The name should hold no comma. FinalAlert 2 writes persistence on the tag, not the trigger. | `TriggerTypeClass::Read_INI` takes missing difficulty and inherit fields as the manual says. A row with fewer than two fields still crashes. |
| `[Tags]` | `ID=persistence,name,trigger`. IDs should start at `01000000` or higher. | `TagTypeClass::Read_INI` copes with a missing name or trigger. IDs compare ignoring case, so lowercase hex IDs match. |
| `[Events]` | Three values per event, plus a string third parameter for events 60 and 61. | `TEventClass::Read_INI` reads the extra field when the parameter code is 2. A row with fewer fields than its count crashes, which `scenario-triggers.md` already states. |
| `[Actions]` | Eight values per action. | Same as events. |
| `[Map]` | `Size=0,0,W,H`, `LocalSize=X,Y,W,H`. The engine wants `W + H` at most 512. FinalAlert 2 1.02 allows 16 to 200 per side, patched editors up to 492 with the same sum limit, World-Altering Editor at least 50 per side. Official FinalAlert 2 writes `Theater` in capitals, FA2sp and World-Altering Editor may write lowercase. | `Theater` ignores case. `Size` is not checked against the 512 limit (finding 4). `LocalSize` is clipped to the play area in `MapClass::Set_Local_Dimensions`. |
| `[Basic]` | Editors add keys such as `Author`, `GameMode`, `MinPlayer` and `MaxPlayer`. | Unknown keys are not read, so they do no harm. `NewINIFormat` is read with a default of 0 (`code/scenario.cpp:4051`). |
| INI text | Pack lines are 70 characters. | Section and key names are case sensitive, a repeated key keeps the later value, and a pack line over 127 characters is cut. Maps from the editors above should not hit any of this, but I have not seen one. |

## Findings that still need a fix or an answer

1. `code/preview.cpp:312`: the reader takes the first byte of a pixel as red. ModEnc says the pixels are BGR888. If it is right, previews of Westwood and FinalAlert 2 maps show red and blue swapped. Failing case: load a map with a known red object in its preview. Suggested fix: after checking one real map on Windows, swap the byte order in `Read_INI` and `Write_INI` together.
2. `code/sendfile.cpp:591` and `code/session.cpp:1089`: the host's map list gives a map without `[Digest]` the digest `No Digest`, while `Find_Local_Scenario` reads the same file as `No digest here mate. Nope.`. Failing case: a LAN or Westwood Online guest looks up a map with no `[Digest]` section and does not find it, though the file name and size match. Suggested fix: use one default in both places.
3. `code/taction.cpp:550`, `562`, `573`: a team, trigger or tag parameter shorter than three characters indexes `TeamTypes`, `TriggerTypes` or `TagTypes` with the number, without a range check. Failing case: an action with team parameter `7` in a map with three team types. Suggested fix: treat an index outside the list as no team, trigger or tag.
4. `code/display.cpp:3136`: `[Map] Size` is not checked against the engine's 512 limit on `W + H`, and `Set_Map_Dimensions` walks `MapRect.Width + 2` columns of a 512-wide cell array. Failing case: `Size=0,0,300,300`. Suggested fix: refuse the scenario with a message, as damaged terrain is refused.
5. `code/trigtype.cpp:505-526`, `code/tevent.cpp:640`, `code/taction.cpp:521`: `strtok` results go straight to `atoi` or `stricmp`. A trigger row with one field, or an event or action row shorter than its count, crashes. The manual documents the crash for events and actions, so I left it. Suggested fix, if the owner wants it: read missing fields as 0 and cap the stated count at the number of fields on the row. I wrote and then dropped that change.
6. `code/overlay.cpp:397` and `code/scenario.cpp:4051`: a map without `NewINIFormat=4` loads with no overlays, and building and unit rows read their cell as one number instead of X and Y. I expect the editors named above to write 4, but no page I read says so, and I did not change it. The owner can say whether gamemd treats a missing key the same way.
7. `code/lzostraw.cpp`: a block whose header claims more than 16 KiB of output is still damage. I found nothing that says an editor writes such blocks.

## Ran

- `g++ -std=c++20` builds of `tests/lzoblock`, `tests/cellrecords` and `tests/custommaps`, with the vendored LZO compiled by `gcc`: all pass. I checked that the new padding test fails against the old `lzostraw.cpp`.
- `python3 manual/tools/manage.py update` and `check` from the repository root after each manual change: all checks verified; the only message was the expected "site dependencies are missing". I discarded the regenerated `manual/data` churn each time.

## Not run

- The Windows build. `overlay.cpp` and `preview.cpp` need Windows headers, so I re-read their diffs instead.
- A CMake configure of the new `tests/custommaps` target. Its `CMakeLists.txt` copies the pattern of `tests/missionnames`, and `tests/CMakeLists.txt` lists it.
- The game, and any map saved by an editor.

## For the next session

- Take a real FinalAlert 2 map and a World-Altering Editor map, run the unit-sized readers over their packs, and compare block sizes, padding and section order with the cases above.
- Check `Read_INI_Preview` on the Windows build with a map that has `[PreviewPack]` after `[Map]`.

## Questions for MentalHomiega

- Is a map with no `NewINIFormat` meant to load without overlays, as gamemd does it?
- Should short trigger, event and action rows load as zeros instead of crashing (finding 5)?
