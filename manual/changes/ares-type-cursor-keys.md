---
title: Choose the mouse cursors of a type
category: feature
release: 0.2.0
targets:
- type: key
  id: Cursor.Deploy
  effect: added
- type: key
  id: Cursor.NoDeploy
  effect: added
- type: key
  id: Cursor.Move
  effect: added
- type: key
  id: Cursor.NoMove
  effect: added
- type: key
  id: Cursor.Enter
  effect: added
- type: key
  id: Cursor.NoEnter
  effect: added
- type: key
  id: Cursor.Spy
  effect: added
credit:
- MentalHomiega
---

An object type can name the cursors that show for its deploy, move and enter actions (`Cursor.Deploy`, `Cursor.NoDeploy`, `Cursor.Move`, `Cursor.NoMove`, `Cursor.Enter`, `Cursor.NoEnter`), and a structure the cursor that shows over it for a spy (`Cursor.Spy`). A name can be a stock cursor or an entry of `[MouseCursors]`. The keys follow the Ares documentation, and a type that sets none of them shows the stock cursors.
