---
command_id: AttackMove
---

Arms an attack-move for the selected units. The next left click on the ground orders each selected unit to travel to that cell and to engage an enemy it reaches on the way. After the enemy is gone, the unit resumes travel to the cell. The arm ends with that click, so the next order is a normal move.

The key also works as a held modifier. While it is held, every left click on the ground is an attack-move. The cursor shows the attack cursor over ground the selection can move to whenever an order is armed or the key is held. Releasing the key ends the held state; an order that is already armed still waits for its click.

Holding Control and Shift with a left click on the ground gives the same order without arming first. The [Command bar](/commands/) attack-move button arms the order in the same way as the key. Holding Control and Alt with a left click on the ground gives the original game's guard-area order, not an attack-move.

A click on an enemy object still attacks it. Only ground orders are attack-moves; a click on a friendly object gives its usual order. An aircraft, a type with no primary weapon, and a type with [`PreventAttackMove=yes`](/keys/preventattackmove/) each keep their plain move or attack order.

The default key is A in the Modern control scheme, bound as a press. The Classic scheme leaves the command unbound, because Classic uses A for Toggle Alliance. The keyboard dialog refuses Ctrl or Shift on its own as this key, because holding both is already the chord above.
