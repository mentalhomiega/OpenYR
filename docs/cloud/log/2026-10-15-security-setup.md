# Security review log: setup program and launcher, 2026-10-15

Branch `cloud/security-setup-2026-10-15`, begun from `origin/modern` (`4d36e67d`). No earlier session worked on this job, so there was nothing to carry forward.

Scope: `tools/setup` (OpenYR-Setup.exe) and `tools/launcher` (OpenYR.exe), read as code that handles untrusted input. Each fix has a test in `tests/setup` or `tests/launcher`.

## Commits

| Commit | What it does |
| --- | --- |
| Refuse package names that Windows reads as devices or streams | `tools/setup/setup_names.h` holds the package name rule. The old rule refused `..`, a drive colon and a leading separator only. It now also refuses wildcards, control characters including NUL, steps ending in a dot or a space, and device names (`CON`, `NUL`, `COM1`...). A `.` step is dropped. |
| Check a zip entry's data offset without wrapping on Win32 | `local + 30 > size` wrapped for an offset near 4 GB in the 32-bit build, then read outside the archive. |
| Refuse a package trailer whose sizes wrap around | `offset + length + 24 == size` could hold by 64-bit wrap-around, and the huge length then threw out of `std::string::assign`. |
| Ignore install record lines that leave the install or name player data | `Read_Installed` keeps a `[Files]` line only if it passes the package name rule unchanged and is not under `user\`, `Debug\`, or `OPENTS.INI` or the record. An update and an uninstall deleted every listed line before, including `..\x`. Ignored lines are logged. |
| Do not write or delete through a junction inside the install folder | New `Crosses_Link`. An install stops with an error when a folder on the way to a file is a link. An uninstall and the stale-file sweep leave such a file. `Remove_Folder_If_Empty` no longer removes a link. |
| Keep the uninstaller's self-removal away from paths cmd.exe would expand | The `cmd.exe` line that deletes the uninstaller is skipped when a path holds `%` or a quote, and runs with `/v:off`. |
| Accept only a single digest line in the checksum file | `Parse_Checksum` read the first token and ignored the rest. It now refuses control characters and any text after the first line. |
| Cap the feed parser's value count and replace lone surrogates | At most 100000 values (the 1 MB feed limit allowed about 500000, around 75 MB in a 32-bit process). `bad_alloc` becomes a parse error. A lone low surrogate produced invalid UTF-8; it is now U+FFFD. `Json_Parse` clears its result first. |
| Make the update folder private to the user with a random name | `%TEMP%\OpenYR-Update-<pid>-<16 hex>` is created with a protected DACL for the user and SYSTEM. The old name was pid and tick count, with the temp folder's inherited access. The update copy and the downloaded setup run from it. |
| Remove only OpenYR-Update folders in the temp folder after an update | `Schedule_Folder_Removal` ran `rmdir /s /q` on the folder of whichever `OpenYR.exe` ran the update switches. Started by hand from the install, it deleted the install, saves included. |
| Refuse local-file assets from a feed read over https | A web feed could name `C:\...` as the setup. Only a local feed (the tests) may now. |
| Cap the update download and set the TLS and redirect policy | 512 MB cap, also against the declared length, with the length read as 64 bits. TLS 1.2 and 1.3 only (1.2 alone where 1.3 is unknown). Redirects from https to http refused explicitly. |
| Limit DLL loading in the setup and launcher to System32 | `tools/common/dll_search.h` calls `SetDllDirectoryW(L"")` and `SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_SYSTEM32)` first in both `wWinMain`. The link line adds `/DEPENDENTLOADFLAG:0x800` for DLLs loaded before `wWinMain`. |
| Document the input checks of the setup and the launcher | `docs/BUILDING.md` (setup) and `docs/LAUNCHER.md`. |
| Stop a feed or a download that takes too long | 20 s in total for the feed, 15 minutes for the download. A failed `WinHttpQueryDataAvailable` in the feed loop was treated as the end of the data. |

No `manual/` page or `manual/changes/` record: nothing in the game's behaviour or the modding surface changed. `manage.py check` was not run.

## Reviewed and found sound

- Zip names: `..` steps, `C:` and `\\server` prefixes and leading separators were already refused; entries are stored, CRC-checked and bounded by the archive. The zip is only ever written as plain files, so a link entry in a zip cannot make a link.
- `Remove_Tree` removes a junction or symbolic link as a link and never enters it. `Uninstall_Package` removes `user\` and `Debug\` through it.
- Registry: writes go to one fixed `HKCU` key and values built from the install path and the build text; the build text cannot hold a line end. `RegDeleteTree` runs on the same fixed key.
- Command lines: `Quote_Argument` round-trips through `CommandLineToArgvW` (existing test); every `CreateProcessW` call passes a quoted program path in the command line and `cmd.exe` by its System32 path.
- WinHTTP: certificate checks are not relaxed anywhere, the scheme gate refuses `http://` and UNC paths, and the status must be 200.
- JSON: depth limit 64, control characters in strings refused, `\u` escapes and surrogate pairs checked, numbers read only from a closed character set, missing or mistyped fields skipped or refused.
- SHA-256: both digests are lower-case 64-character hex before the comparison.

## Open findings

1. **No code signature.** `docs/LAUNCHER.md` already says so. The `.sha256` comes from the same release as the setup, so a replaced release asset passes the check. Suggested fix: sign `OpenYR-<tag>-Setup.exe` and have the update copy call `WinVerifyTrust` before it runs the file, or ship the expected digest in the previous build.
2. **`Clear_Read_Only` follows links.** `tools/setup/setup_system.cpp`, `Clear_Read_Only`: `SetFileAttributesW` on a symbolic link changes the target's attributes, while `GetFileAttributesW` read the link's. Failing case: a symlinked `Game.exe` whose target is read-only gets the flag cleared. Suggested fix: skip the call when the attributes carry `FILE_ATTRIBUTE_REPARSE_POINT`.
3. **`/PAYLOAD=` follows file links.** `Load_Folder_Payload` (`setup_payload.cpp`) uses `is_regular_file`, which follows a symbolic link, so a link in a package folder copies the file it points to into the install. A developer switch; suggested fix: `symlink_status`.
4. **A path with `%` keeps its temporary folder.** `Schedule_Folder_Removal` now skips such a path, so an account named like `50%off` leaves the downloaded setup (tens of MB) in `%TEMP%` after each update. Suggested fix: delete from the update copy with `RemoveDirectory` calls after the setup exits, using no `cmd.exe`, and let the next launch sweep stale `OpenYR-Update-*` folders.
5. **The update copy runs from any folder.** `/UPDATE=` on a hand-started `OpenYR.exe` still runs the update; the removal is guarded now, but `launcher_e2e.ps1` relies on running the copy from a test folder, so the mode was not restricted. Suggested fix: pass a token in the temp folder and require it.
6. **Residual race.** The setup is hashed and then started by path. The folder is private now, so only code running as the same user can swap it. Suggested fix: keep a read handle without `FILE_SHARE_WRITE` open from the hash to the process start.
7. **Long paths.** `GetModuleFileNameW` buffers are `MAX_PATH * 4` in `launcher_system.cpp`, `setup_main.cpp`; a longer module path is cut off silently.
8. **Windows 7.** `SetDefaultDllDirectories` needs KB2533623 there, and TLS 1.2 needs its own update; the code falls back without them (and the launcher then cannot reach GitHub).

## What I ran

No Visual Studio, so nothing was built with MSVC. Instead:

- Installed `g++-mingw-w64-x86-64-posix` and `wine64` (Wine 9) in the sandbox and built with `x86_64-w64-mingw32-g++ -std=c++20 -Wall -Wextra`: `setup_core.cpp`, `setup_find.cpp`, `setup_payload.cpp`, `setup_system.cpp` with `tests/setup/setup_tests.cpp`; `launcher_core.cpp` with `tests/launcher/launcher_tests.cpp`; the whole launcher (`launcher_main.cpp`, `launcher_system.cpp`); and `setup_main.cpp` and `setup_wizard.cpp` with `-fsyntax-only`. No warnings.
- Ran both test programs under Wine: all setup checks pass (one junction check skipped, see below) and all launcher checks pass, including the new DLL search, private folder and update folder checks.
- Built the new setup tests against the original `setup_payload.cpp`: the hostile-name checks fail there and the trailer-wrap check aborts the run, as expected.
- Ran the mingw-built `OpenYR.exe` under Wine: a `/CHECKONLY` run against a local feed; the update copy against a fake setup with a good checksum (exit 0, "Updated to Beta 23") and with a two-line checksum (refused); `Make_Update_Folder` and `Schedule_Folder_Removal` (the random private folder is removed, `C:\keepme` is not).
- Made a unix symlink where a junction should be and ran `Install_Package` and `Uninstall_Package` over it: the install is refused, and the uninstall leaves the target's file and the link.

## Not run, and what only Windows can check

- The MSVC build in the four supported configurations, and the `setup`, `launcher` and `launcher_e2e` ctests. `launcher_e2e.ps1` needs PowerShell; it was only read against the changes (the update copy's helper folders under the test root are no longer deleted after a run, and nothing in the script depends on that).
- `Test_Junction`: Wine cannot make a junction or a symbolic link, so the test skipped there. On Windows it uses `mklink /J`, and the link logic itself was exercised with the unix symlink above.
- `/DEPENDENTLOADFLAG:0x800`: no test reads it. Check with `dumpbin /loadconfig OpenYR.exe` and `OpenYR-Setup.exe`; "Dependent Load Flag" should read 800.
- The DACL: Wine returned the two entries (user, SYSTEM) and no inherited entry; the protected flag and the access of another account need a real NTFS volume, e.g. `icacls %TEMP%\OpenYR-Update-*` during an update.
- WinHTTP behaviour: TLS protocol flags, the redirect policy, the 512 MB cap and the two deadlines need a TLS server. No network test exists, because `Fetch_Text` and `Download_File` refuse `http://`. A local HTTPS test server with a self-signed certificate would need a test-only trust switch, which I did not add.
- The wizard, task dialog and progress window.
- The 32-bit wrap in the zip offset: the test passes on `x64` with or without the fix; it fails before the fix only in a `Win32` build.

## Ideas for the next session

- Add a `dumpbin`/PE-header check to ctest for the dependent load flag.
- Fuzz `Load_Zip_Payload` and `Json_Parse` (job C covers parsers; these two sit in `tools/`).
- Decide on signing the setup (finding 1).

## Questions for MentalHomiega

- Is `scripts/package_modern.ps1` meant to be in the repository? `docs/BUILDING.md` names it, but it is not on `modern`, so I could not check how the `.sha256` file is written. The parser accepts the `sha256sum` form and a bare digest.
- Should the update copy refuse to run outside `%TEMP%` (finding 5)? It would need a change to `launcher_e2e.ps1`.
