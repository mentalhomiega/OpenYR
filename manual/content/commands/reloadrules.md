---
command_id: ReloadRules
---

Reads `rulesmd.ini`, `art.ini`, the rules and art overlays of the active [mods](/formats/mod-ini/) and the current map's rule overrides again without leaving the game, so an edit to a loose `rulesmd.ini` or `art.ini` in the game folder, or to a mod's overlay, takes effect at once. The default key is Ctrl+Shift+R when the keyboard file does not use that key for another command.

The command works in single-player missions and skirmish games. In a multiplayer game it only shows a message, because every player must run the same rules.

What changes after a reload:

- Every type takes the values now in the files, and the map's own overrides are applied again on top.
- Each house's firepower, speed, rate of fire, cost and build-time factors are worked out again from the difficulty settings, and its armor factor from its country.
- Objects already on the map keep the values they copied from their type when they were made, such as their current strength.
- A key removed from a file keeps the value it had until the game is restarted.
- New types added to a type list need a restart; a reload is meant for changing existing types.
