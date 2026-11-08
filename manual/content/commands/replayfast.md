---
command_id: ReplayFast
---

Plays a replay without the frame limit that the game speed sets, and returns to the game speed on the next press. Each frame does the same work as at normal speed, so the replay still ends on the recorded hashes. The label at the top of the view reads `fast` while it is on. The command does nothing in any other game.

The command is on the numeric keypad's plus key in both control schemes unless [`KEYBOARDMD.INI`](/formats/keyboard-ini/) binds it to another key. A keyboard file saved before this command existed gains that key when it names no other command on it.
