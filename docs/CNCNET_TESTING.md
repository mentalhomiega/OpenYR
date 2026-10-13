# Testing OpenYR with the CnCNet client

This page sets up a CnCNet client for testing an OpenYR build. It covers only our side of the setup; the client itself is documented by the CnCNet project.

## Set up a separate client folder

Keep a second copy of the Yuri's Revenge client for OpenYR, in its own folder, so your normal install stays as it is.

Unpack the OpenYR build for the test into that folder. The build contains `Game.exe`, `Language.dll` and the `ui` folder.

Everyone in a match must run the same OpenYR build. A build that differs from the others can stop a join or cause a desync.

## Point the client at our executable

The client starts the file named by the first entry of `GameExecutableNames`, in the `[Settings]` section of `ClientDefinitions.ini`. The default is `Game.exe`, which is our executable's name, so the entry only needs changing if you rename the file.

Pass extra launch options with `ExtraCommandLineParams`, in the same section. The client adds them to the command line after `-SPAWN`.

## Which games to play

- Start with a LAN lobby, with everyone on the same network. Move to an online lobby once a LAN match runs.
- Play only in custom lobbies that have a password.
- Do not play ranked games or Quick Match. We cannot choose the players in those, so we cannot make sure everyone runs this build.

## Turn on the sync log

Add `-SYNCLOG` to `ExtraCommandLineParams` on every machine in the match:

```ini title="ClientDefinitions.ini"
[Settings]
ExtraCommandLineParams=-SYNCLOG
```

Each game then writes one line per frame to its debug log, for example `SYNC frame 400 crc 6134918a`. The two machines' lines match for as long as they stay in sync. The log grows quickly, so turn it on only for test matches.

If you start the game yourself with `-USERDIR`, you can set `SyncLog=yes` under `[Options]` in that folder's `RA2MD.INI` instead.

## Where the logs go

The game writes its logs to a `Debug` folder beside `Game.exe`:

- `DEBUG_<date>_<time>.LOG` is the debug log of one game.
- `SYNC_H<heap>_<date>_<time>_F<frame>.LOG` is the out-of-sync report, written when a game goes out of sync.

The game deletes older `DEBUG_` and `SYNC_` files when a new game starts, so send the files soon after a test.

## After a desync

Send us, from every machine in the match:

- the `DEBUG_` and `SYNC_` files from the `Debug` folder for that session;
- the time the desync happened, the map name and the number of computer players.

The logs contain player names. Remove any file you do not want to share before you send it.
