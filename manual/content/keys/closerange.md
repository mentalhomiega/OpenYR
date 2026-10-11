---
key: CloseRange
summary: Makes an object that closes on a target accept a spot within 1.2 cells of it, and makes an infantryman step to the free spot of a cell nearest the target.
see_also: [CanApproachTarget, "system:target-selection"]
when_omitted:
  kind: value
  value: "no"
---

A `CloseRange=yes` object that has to move to reach its target picks its approach spot with these differences.

- It accepts a spot within 1.2 cells of the target even when its weapon cannot reach the target from there.
- An infantryman searches at least 332 leptons from the target, and aims for the free spot of each candidate cell that is nearest the target, not the cell center.
- An object that is already within 1.5 cells of the target, measured between cell centers, steps to the free spot of its own cell that lies nearest the target. It then heads for the target's cell. It makes this step only when the spot is closer to the target than the object's own position.
- The object is lifted off the map while each candidate cell is tested, so it does not block a candidate cell it already occupies.

Stock rules set this flag on the infantry types `BRUTE`, `DNOA`, and `MUMY`, whose attacks work at melee distance.

```ini title="rules.ini"
[MYBRUTE] ; an InfantryType registered in [InfantryTypes]
CloseRange=yes ; closes to melee distance before attacking
```
