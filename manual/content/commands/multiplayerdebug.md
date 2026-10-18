---
command_id: MultiplayerDebug
---

Shows or hides the multiplayer debug display. The display draws only in an IPX or Internet game, along the bottom of the screen. The default key is Ctrl+I when the keyboard file does not use that key for another command.

The display starts hidden, unless the game was started with [`-MPDEBUG`](/using/command-line/multiplayer-debug/), which starts it shown. Each press of the key switches it between shown and hidden.

The display shows this machine's frame, frame rate, `MaxAhead`, response time, requested frame rate and processing time, along with the retry delay, retry timeout and latency fudge. For each other player it shows a column with average and maximum response times, resends, packets lost, percentage lost, processing time, frame offset, queue sizes and missed packets.

A press changes only what is drawn. It does not change the game's timing, the packets sent to other players or the stall counts. In a single-player or skirmish game the command draws nothing.

A keyboard file that the keyboard dialog has saved is read as saved, so it has no key for this command unless it binds one. Bind the command on the keyboard dialog, or reset the Debug category there.
