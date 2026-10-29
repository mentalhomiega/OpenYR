---
command_id: AttackMove
---

Arms an attack-move for the selected units. The next left click on the ground orders each selected unit to travel to that cell and to engage an enemy it reaches on the way. After the enemy is gone, the unit resumes travel to the cell. The arm ends with that click, so the next order is a normal move.

The key also works as a held modifier. While it is held, every left click on the ground is an attack-move. The cursor shows the attack cursor wherever a click would give a move whenever an order is armed or the key is held. Releasing the key ends the held state; an order that is already armed still waits for its click.

Holding Control and Shift with a left click on the ground gives the same order without arming first. The [Command bar](/commands/) attack-move button arms the order in the same way as the key. Holding Control and Alt with a left click on the ground gives the original game's guard-area order, not an attack-move.

A move or an attack order is an attack-move whatever it names. A click on an enemy unit or structure gives each unit an attack-move with that enemy as its target: the unit goes for it, and while it is out of reach it takes any enemy it can already hit, then goes back to the target. A click on a friendly vehicle travels to it as an attack-move, and a ground attack with Control held goes for that cell the same way. A click that selects, enters, captures or sabotages keeps its usual order. An aircraft, a structure, a type with no primary weapon, and a type with [`PreventAttackMove=yes`](/keys/preventattackmove/) each keep their plain move or attack order.

Control with Shift gives an attack-move only when every selected object can attack-move, and the command arms only then. With an aircraft or a structure in the selection, Control with Shift gives the usual orders, and the command shows a notice and arms nothing. Control with Shift over an object counts as neither key, so it neither force-fires nor toggles the selection.

The default key is A in the Modern control scheme, bound as a press. The Classic scheme binds Control with Shift held, the chord above, because Classic uses A for Toggle Alliance. That binding has no key, so the game reads it as held while both modifiers are down; it never fires as a press. The keyboard dialog can also bind a key with modifiers, such as Control, Shift and X. That key arms an attack-move on its press like A does, and it is held while its modifiers and key are down.
