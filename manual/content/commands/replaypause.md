---
command_id: ReplayPause
---

Pauses a replay on the frame it has reached, and resumes it on the next press. While paused, the view keeps scrolling and selecting, the menu and the screen capture work, and the label at the top of the view reads `paused`. The command does nothing in any other game.

The command is on Pause in both control schemes unless [`KEYBOARDMD.INI`](/formats/keyboard-ini/) binds it to another key. A keyboard file saved before this command existed gains the Pause key when it names no other command on that key.
