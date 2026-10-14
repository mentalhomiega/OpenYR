# The OpenYR launcher and beta updates

`OpenYR.exe` is the launcher the setup installs beside `Game.exe`. The Start Menu and desktop shortcuts start it. It starts `Game.exe` with the same arguments, and before that it checks our GitHub releases for a newer beta. We ask the player before we update. We never update silently, and we never start a second game while one from the install is running.

## What it does

1. If `Game.exe` from this install is already running, the launcher says so and stops. It starts nothing and checks nothing.
2. It reads the installed beta from `openyr-install.ini`, the record the setup writes. Without that record, for example in an unpacked zip, it starts the game and checks nothing.
3. It fetches the release feed. A feed that cannot be reached, or does not parse, is logged and the game starts as usual.
4. It offers the update only when the feed names a newer beta that has both release assets. Drafts and prereleases are never offered.
5. If the player answers Yes, the launcher copies itself to a temporary folder and starts the copy. The copy downloads the setup and its checksum, checks the SHA-256 digest, waits for the launcher to close, runs the setup silently over the install, and then starts `Game.exe`.
6. A failed download, a checksum that does not match, or a setup that does not finish stops the update. The copy tells the player and starts the version already installed.

The copy runs from the temporary folder because the setup replaces `OpenYR.exe` in the install. The launcher exits as soon as the copy starts, so nothing in the install folder is in use while the setup runs.

The question reads "Beta 23 is available (you have Beta 22). Update now?" with the buttons Yes and Not now. Not now, or closing the question, starts the installed version.

## Settings in OPENTS.INI

| Key | Default | Effect |
| --- | --- | --- |
| `[Update] FeedURL=` | the GitHub latest release | The release feed. An `https://` address, or an absolute local path for tests. A plain `http://` address is refused. |
| `[Update] Check=` | `yes` | `no`, `0`, `off` or `false` turns update checks off. The game still starts. |
| `[Paths] DataDirectory=` | set by the setup | The Yuri's Revenge folder. The update passes it to the setup. |
| `[Paths] UserDirectory=` | `user` | The folder for settings, saves and the launcher log. A relative path is taken from the install folder. |

The setup keeps every other line of `OPENTS.INI` when it installs or updates, so these keys survive an update.

## Switches

The launcher passes any argument it does not know to `Game.exe`, and so does anything after `--`. These switches are the launcher's own:

| Switch | Effect |
| --- | --- |
| `/FEED=<url or path>` | The feed for this run, instead of `[Update] FeedURL=`. |
| `/LOG=<file>` | The log file. The default is `user\OpenYR-Launcher.log`. |
| `/CHECKONLY` | Reports what it would do and exits. It asks nothing, downloads nothing, changes nothing and starts nothing. |
| `/NOGAME` | Does not start `Game.exe`. |
| `/QUIET` | Shows no message box and asks no question. The log keeps the text. |
| `/NOSHORTCUTS` | Passed to the setup on an update. The setup writes no shortcuts or registry entry. |

`/QUIET`, `/NOGAME` and `/NOSHORTCUTS` are for the tests. `/CHECKONLY` is for checking a feed.

The update copy also takes switches the launcher writes itself: `/UPDATE=`, `/SETUPURL=`, `/SUMURL=`, `/FOLDER=`, `/YR=` and `/WAITFOR=`. Do not pass them by hand.

## Release assets

A release is named by its beta, for example `beta23`. It has two assets:

| Asset | Contents |
| --- | --- |
| `OpenYR-beta23-Setup.exe` | The setup with its package, as the packager writes it. |
| `OpenYR-beta23-Setup.exe.sha256` | The SHA-256 digest of the setup in lower-case hex. A file name after the digest, as `sha256sum` writes, is allowed. |

The feed is the latest release. A tag that is not a beta name is never offered. Beta numbers compare as numbers, so `beta9` is older than `beta22`.

The checksum comes from the same release as the setup. It catches a damaged or cut-short download. It does not protect against a release that was changed on GitHub; we do not sign releases yet.

## Logs

Each run appends to `user\OpenYR-Launcher.log`: the feed it read, the decision, what it started, and the setup's own lines from an update. The setup's `/LOG=` writes to the same file.

## Tests

- `LauncherTests` (ctest `launcher`) covers the beta names, the JSON feed, the asset choice, the checksum file, the update decision, the INI keys and argument quoting.
- `launcher_e2e.ps1` (ctest `launcher_e2e`) installs a fake beta22, reads a local feed, runs a check-only pass, runs the update copy against a bad checksum, a setup that refuses the Yuri's Revenge folder, and a good download, then checks the install. It starts no game and shows no dialog. It writes only under a temporary folder.

The question dialog, a running game, and the progress window are not covered by these tests. Check them by hand when the launcher changes.
