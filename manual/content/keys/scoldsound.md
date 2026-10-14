---
key: ScoldSound
summary: The sound played when the interface refuses a request.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "none"
---

Three cases play this sound:

- An order for anything other than a structure is placed while its production slot is busy, and the queue is full or the type has reached its [build limit](/systems/production/#the-queue). Only a house the player controls hears it.
- A sidebar scroll arrow is clicked while its strip is already at the end it moves toward: the top for the right arrow, the bottom for the left arrow. An arrow is disabled, and plays nothing, when the whole list fits on the screen. A keyboard command that scrolls or pages both columns plays it only when neither column can move; a command for one column never plays it.
- Repair is ordered on a structure already at maximum strength. The sound plays at the structure, and only for a house the player controls.

A movement order that an object cannot carry out does not play it.
