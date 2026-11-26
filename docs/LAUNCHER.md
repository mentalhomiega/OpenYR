# The OpenYR launcher and beta updates

`OpenYR.exe` is the launcher the setup installs beside `Game.exe`. The Start Menu and desktop shortcuts start it. It starts `Game.exe` with the same arguments. Before that it lets the player pick a mod. It never starts a second game while one from the install is running.

The game checks our GitHub releases for a newer beta and offers it on its title menus. We never update without the player pressing Update, and the check never delays the menus.

## What the game does

1. On its first title menu, the game reads the installed beta from `openyr-install.ini`, the record the setup writes. Without that record, for example in an unpacked zip, it checks nothing and shows no panel. It also checks nothing when `[Update] Check=` is off or OPENTS.INI names no Yuri's Revenge folder.
2. A worker thread fetches the release feed, and the menus stay responsive. A feed that cannot be reached, or does not parse, is logged in the game's debug log as `UPDATE feed failed: <reason>` and no panel appears.
3. The launcher's own code decides the offer: a newer beta with both release assets, never a draft or prerelease. The game logs `UPDATE offered: ...` or `UPDATE not offered: ...`.
4. An offer shows a panel at the top right of the title menus, under the profile panel in the modern style and in the top right corner in the classic style. It reads "Beta 34 is available" over "You have Beta 33", with the buttons Update and Later. The panel never shows in a match.
5. Later hides the panel until the game is started again.
6. Update starts `OpenYR.exe` from the install folder with `/UPDATENOW=<game process id>` and closes the game. If the launcher cannot be started, the panel says why and the game keeps running.

## What the launcher does with /UPDATENOW

1. It waits for the game's process to close, up to 60 seconds.
2. It reads the feed again, as the check above does, and stops with a message if nothing newer is offered. It does not ask a question.
3. It copies itself to a temporary folder and starts the copy, which downloads the setup and its checksum, checks the SHA-256 digest, waits for the launcher to close, runs the setup silently over the install, and then starts `Game.exe`.
4. A failed download, a checksum that does not match, or a setup that does not finish stops the update. The copy tells the player and starts the version already installed.

The copy runs from the temporary folder because the setup replaces `OpenYR.exe` in the install. The launcher exits as soon as the copy starts, so nothing in the install folder is in use while the setup runs.

## Asking in the launcher

With `[Update] AskInLauncher=yes`, the launcher checks the feed before it starts the game, as earlier betas did, and asks "Beta 23 is available (you have Beta 22). Update now?" with the buttons Yes and Not now. Not now, or closing the question, starts the installed version. A Yes continues as in step 3 above. The game still shows its own panel.

## The mod picker

The picker is a window that lists "Yuri's Revenge (no mods)" and every folder in the `Mods` folder of the data directory, by the mod's name from its `mod.ini` or, without one, the folder's name. Selecting a row shows the mod's description. Play saves the choice and goes on; Cancel, the close button and Escape end the run.

The picker opens on the mods now in force, which are the `Mods=` list under `[Options]` in the player's settings or, when the settings have none, `[Paths] Mods=` in `OPENTS.INI`. A list that is not exactly no mods or one mod from the `Mods` folder, such as several mods, a folder that does not exist or a mod outside the `Mods` folder, adds a first row, "Keep the current mods", which is selected. Playing with that row leaves the list as it is. Picking any other row replaces the whole list with that one choice, so the picker never reorders or drops mods unless the player picks another row.

The picker is not shown, and the log says why, when:

- `/QUIET`, `/CHECKONLY` or `/NOPICKER` is given.
- The arguments for the game contain `-MOD=`, `-DATADIR=` or `-USERDIR=`, which decide the mods or the folders the picker would read.
- The player ticked "Do not ask again". `/PICKER` shows it anyway.
- There is no mod folder and the list in force is empty, so there is nothing to pick.

### Where the choice is stored

The launcher writes the choice as `Mods=` under `[Options]` in the settings file the game writes: `RA2MD.INI` (or the file `[Files] Settings=` names) in the user directory. It changes that line alone and keeps every other line and its line ends. When the user directory has no copy, the new file starts from the copy the game would read, in the install folder or the data directory. Choosing no mods writes `Mods=,`, which overrides a list in `OPENTS.INI`, as the in-game Mods page does.

The in-game Mods page and the picker read and write the same key with the same rules, so each shows what the other chose. A mod chosen in the launcher is in force the next time `Game.exe` starts, and a list saved in the game is the picker's starting point. The launcher compiles `code/modchoice.cpp`, the code behind the in-game page, for the list syntax and the rules for which folder names a list can hold. A folder whose name holds a comma or semicolon, or starts or ends with a space, is not offered.

"Do not ask again" writes `ModPicker=no` under `[Launcher]` in the same file. The launcher and the game read the value as a boolean: a value starting with `N`, `F` or `0` is no, and anything else, or no key, is yes. The in-game Mods page has a check box, "Ask which mod to use when OpenYR starts", for the same setting.

## Settings in OPENTS.INI

| Key | Default | Effect |
| --- | --- | --- |
| `[Update] FeedURL=` | the GitHub latest release | The release feed. An `https://` address, or an absolute local path for tests. A plain `http://` address is refused. |
| `[Update] Check=` | `yes` | `no`, `0`, `off` or `false` turns update checks off, in the game and in the launcher. The game still starts. |
| `[Update] AskInLauncher=` | `no` | `yes`, `1`, `on` or `true` makes the launcher check the feed and ask before it starts the game. |
| `[Paths] DataDirectory=` | set by the setup | The Yuri's Revenge folder. The update passes it to the setup. |
| `[Paths] UserDirectory=` | `user` | The folder for settings, saves and the launcher log. A relative path is taken from the install folder. The mod picker writes its choice to the settings file in this folder, and beside `Game.exe` when the key is absent, as the game does. |
| `[Paths] Mods=` | empty | The mods used while the player's settings hold no list. The picker reads it and never writes it. |
| `[Files] Settings=` | `RA2MD.INI` | The name of the player's settings file. |

The launcher reads comments as the game does: a `;` ends the value, so `Check=no ; skip` reads as `no`. When a key appears twice, the launcher takes the first. The setup keeps every other line of `OPENTS.INI` when it installs or updates, so these keys survive an update.

## Switches

The launcher passes any argument it does not know to `Game.exe`, and so does anything after `--`. These switches are the launcher's own:

| Switch | Effect |
| --- | --- |
| `/FEED=<url or path>` | The feed for this run, instead of `[Update] FeedURL=`. |
| `/LOG=<file>` | The log file. The default is `user\OpenYR-Launcher.log`. |
| `/CHECKONLY` | Reports what it would do, as if `AskInLauncher=yes`, and exits. It asks nothing, downloads nothing, changes nothing and starts nothing. |
| `/NOGAME` | Does not start `Game.exe`. |
| `/QUIET` | Shows no message box and asks no question. The log keeps the text. |
| `/NOSHORTCUTS` | Passed to the setup on an update. The setup writes no shortcuts or registry entry. |
| `/UPDATENOW=<pid>` | The game with that process id asked for the update. The launcher waits for it to close, then updates without a question. The game passes it; do not pass it by hand. |

`/QUIET`, `/NOGAME`, `/NOSHORTCUTS` and `/SETMOD=` are for the tests; `/NOGAME` alone shows the picker, which makes it the way to look at the picker without starting the game. `/CHECKONLY` is for checking a feed.

The update copy also takes switches the launcher writes itself: `/UPDATE=`, `/SETUPURL=`, `/SUMURL=`, `/FOLDER=`, `/YR=` and `/WAITFOR=`. Do not pass them by hand.

## Release assets

A release is named by its beta, for example `beta23`. A hotfix to a beta adds a dot and a number of 1 or more, for example `beta34.1`; its assets are `OpenYR-beta34.1-Setup.exe` and `.sha256`. It has two assets:

| Asset | Contents |
| --- | --- |
| `OpenYR-beta23-Setup.exe` | The setup with its package, as the packager writes it. |
| `OpenYR-beta23-Setup.exe.sha256` | The SHA-256 digest of the setup as 64 hex digits. A file name after the digest, as `sha256sum` writes, is allowed. The file is refused if a later line holds text or any control character other than tab and line ends. |

The feed is the latest release. A tag that is not a beta name is never offered. Beta numbers compare as numbers, so `beta9` is older than `beta22`. Within a beta the hotfix number decides: `beta34` is older than `beta34.1`, which is older than `beta35`. `beta34.0`, `beta34.` and `beta34.1.2` are not beta names. The label shows `Beta 34` or `Beta 34.1`.

The checksum comes from the same release as the setup. It catches a damaged or cut-short download. It does not protect against a release that was changed on GitHub; we do not sign releases yet.

## What the update checks

- A feed read over `https://` may name only `https://` assets. A local feed, which the tests use, may name local files.
- A download stops with an error at 512 MB, or when it passes the length the server declared. The launcher offers TLS 1.2 and 1.3 and refuses a redirect from `https://` to `http://`.
- The update copy runs from `%TEMP%\OpenYR-Update-<process id>-<random digits>`. Only the current user and SYSTEM can open that folder, and the launcher stops the update if it cannot make the folder that way.
- When the update ends, the copy removes its own folder only if the folder is an `OpenYR-Update-*` folder directly in `%TEMP%`, and its path has no `%` or quote. A copy started by hand elsewhere leaves its folder alone.
- The launcher and the setup load DLLs from System32 only.
- The setup is not signed, and Windows does not check a signature before the launcher runs it.

## Logs

Each run appends to `user\OpenYR-Launcher.log`: the feed it read, the decision, what it started, and the setup's own lines from an update. The setup's `/LOG=` writes to the same file.

## Tests

- `LauncherTests` (ctest `launcher`) covers the beta names, the JSON feed, the asset choice, the checksum file, the update decision, the INI keys, the process id and the update-now command, and argument quoting.
- `LauncherModsTests` (ctest `launchermods`) covers the INI text the launcher reads and edits, the settings and mod folders it finds, the choices the picker offers and their agreement with the game's `ModChoiceClass`.
- `launcher_e2e.ps1` (ctest `launcher_e2e`) installs a fake beta22, reads a local feed, runs a check-only pass, runs the update asked for in the game (`/UPDATENOW`) against a feed with nothing newer and against beta23, runs the update copy against a bad checksum, a setup that refuses the Yuri's Revenge folder, and a good download, then checks the install and `/SETMOD`. It starts no game and shows no dialog. It writes only under a temporary folder.

- `tests/autotest/update-panel-menu.txt` gives the title menu inline release text through the `ui updatecheck <build> <feed>` step, and checks the log lines for an offer, no offer and a missing feed, with a screenshot of the panel and the Later button. It does not press Update.

The picker window, the question dialog, a running game, the Update button and the progress window are not covered by these tests. Check them by hand when the launcher or the panel changes.
