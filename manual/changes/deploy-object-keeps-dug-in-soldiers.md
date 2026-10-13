---
title: Leave dug-in soldiers alone in a mixed deploy
category: fix
release: 0.2.0
targets:
- type: command
  id: DeployObject
  effect: changed
credit: [MentalHomiega]
---

With a selection of deployer soldiers that is partly dug in, DeployObject now deploys the soldiers that are not dug in and leaves the dug-in ones where they are. Before, every selected soldier toggled, so the dug-in ones packed up. A selection in which every soldier is dug in still packs them all up, and one in which none is dug in still deploys them all.
