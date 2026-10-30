/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

/*
**	A test script is a text file of lines "<frame> <command> [arguments]". Blank lines and
**	lines starting with ';' are ignored. Commands run once the game frame reaches their frame:
**
**	command <Name>			runs a registered command, such as CenterBase or ScreenCapture
**	options					opens the options menu, as Escape does during play
**	select <TypeID>			selects every object of that type the player owns
**	produce <TypeID>		starts building that type
**	produceai <TypeID> x y	starts building that type in the structure on that cell, as a computer house's factory does
**	place <TypeID>			places a finished structure at the first legal cell near the
**							player's construction yard
**	move <TypeID> <x> <y>	orders the player's objects of that type to a cell
**	order <TypeID> <x> <y>	gives the player's objects of that type the move order a click gives, through the mission event,
**							which drops their target as a player's order does
**	cmove <TypeID> <x> <y>	gives the computer's foot objects of that type a move to a cell, keeping any target they hold
**	attack <TypeID>			orders the player's objects of that type to attack the nearest
**							structure of an enemy that has a base
**	enemies					writes the structures of other houses and the count of their units
**	scatter <TypeID> x y [forced] [nokidding]	calls Scatter on each live soldier of that type, whoever owns it, with the threat at the
**							centre of the cell (x -1 for no threat), then writes its cell and sub-cell position, the destination it was given
**							and how many numbers the game's random generator handed out
**	strikeobj <TypeID> x y [radius] [land|naval|all] [AttackerTypeID]	orders the player's armed units and infantry to attack the nearest
**							live object of that type within radius cells (default 3) of the cell, looked up when the line runs;
**							land or naval limits the orders to units that are not, or are, ships (default all); an attacker type
**							limits them to objects of that type, PlayerControl houses' included
**	owners <TypeID>			writes the type's owner bits and each house's country bit
**	canbuild <TypeID>		writes each house's own and acted country, the type's RequiredHouses and ForbiddenHouses, and whether the house can build it now
**	cameos					writes each sidebar strip's cameos in order, with the type and whether the player can build it now
**	clickcameo <TypeID>		starts production of the type, as a click on its cameo does, when a strip still offers the cameo
**	cameopress <TypeID> <left|right>	presses the type's visible sidebar cameo through the cameo's own action, as a click does, so Shift held with hold applies its Shift behaviour
**	queuestate <TypeID>		writes how many of the type the player's factory holds, in production and queued, how many are queued, and the player's credits
**	prereqs <TypeID>		writes the prerequisites the type lists, then each type whose Prerequisite list names it
**	view x y				centres the view on a cell
**	cell x y				writes a cell's shroud state, tile, height and occupier
**	bridge x y				writes a cell's bridge tile, overlay, deck and under-bridge flags
**	bridgecensus			writes each bridge deck overlay on the map with its flags, then how many high and railway decks are flagged, and how many low-bridge and concrete-bridge overlays there are, and the cell of each concrete-bridge overlay
**	bridgearea x y			writes the bridge tile, overlay and flags of each cell within four cells of x,y
**	wallcensus				writes each overlay type the rules mark as a wall, with the wall overlays the scenario's OverlayPack names (outside the radar counted apart) and the ones the loaded map holds, and whether the type has an image and a cell animation
**	repairbridge x y		writes what MapClass::Can_Repair_Bridge says about the cell
**	bridgehit x y			damages the bridge on the cell as a weapon impact does (MapClass::Damage_Bridge) and writes whether a span came down
**	bridgehut x y			brings the bridge near the cell down as a bridge repair hut's C4 does (MapClass::Destroy_Bridge_At)
**	bridgedecks x0 x1 y		writes how many high bridge deck overlays are left on row y from x0 to x1
**	blast <Warhead|ion> x y	explodes the warhead on the cell as a weapon impact does (Explosion_Damage); a wood low bridge there takes one hit
**	concretehit x y			damages the concrete low bridge on the cell as a weapon impact does (MapClass::Damage_Concrete_Bridge) and writes whether a span came down
**	passable x y			writes whether a foot or tracked unit can move onto the cell, and the overlay of the cell
**	targetaction <AttackerTypeID> <TargetTypeID>	writes the action the player's first object of the attacker type gets over the first live enemy object of the target type,
**							with that target's height, whether it is in the air, and whether the attacker's chosen weapon can fire at it
**	enemystrike <TypeID> x y	orders the first computer-owned object of that type to attack what stands on that cell
**	hutaction <TypeID> x y	writes the action the player's object of that type gets over the building on that cell, the object path a mouse-over takes
**	hutclick <TypeID> x y	clicks the building on that cell with the player's object of that type, through the object path a real click takes
**	hutc4 x y				writes the C4 state of the building on that cell: whether a charge is on it, its countdown and strength
**	c4watch <frames> x y	follows the building on that cell for that many frames: writes its charge's placement, countdown and strength
**							changes, and its destruction with the saboteur's experience and the player's money and points
**	click x y				queues a left click at that screen position
**	windowmove x y			moves the pointer to that window pixel, which the hover and placement code reads
**	key <Key> [ctrl|shift|alt]	presses and releases that key on the keyboard queue, skipping the interface; the key
**							is a letter or digit, or a virtual-key code in decimal (one character is read as a letter or
**							digit, so Tab is written 09); the modifier words hold that key with the modifier
**	hold <Key> [ctrl|shift|alt]	presses that key on the keyboard queue and leaves it down, as a player holding it does, until a release
**	release <Key> [ctrl|shift|alt]	lets go of a key a hold step left down; Keyboard->Down stays false in autotests, so the held state is the queue's
**	winkey <Key> [ctrl|shift|alt]	presses and releases that key through the game window, so the interface sees it first as it does
**							a player's key; a one-character name is read as a letter or digit, so Backspace is written 08
**	binding <Key> [Double|Hold|Held]	writes the command the key's binding of that trigger runs, with the modifier words
**	scheme <Classic|Modern>	replaces the bindings with that scheme's defaults, as the keyboard screen does when the scheme changes
**	camera					writes the tactical position the view is centred on, in leptons
**	wintype <text>			types the text through the game window, one character at a time, each as a press, its text event and a release
**	alliance <house>		queues the alliance toggle for the house with that index, so every copy of a match makes it on one frame
**	tab				writes the sidebar tab shown, how many items each tab lists, and the object pending placement
**	modes				writes which special mode is on (repair, sell, power, waypoint, beacon, and targeting) and the object pending placement
**	placement				writes the object pending placement, the cell its cursor is on, how many cells of its footprint are marked, and whether the pointer is over the tactical view and over the map
**	windowpress x y			moves the pointer to that window pixel and presses the left button there, as the start of a drag does
**	windowrelease x y		moves the pointer to that window pixel and releases the left button there, as the end of a drag does
**	windowclick x y			moves the pointer to that window pixel and clicks there through the window's event path,
**							which converts it to the frame
**	follow <TypeID>			keeps the view centred on one of the player's objects of that type
**	record <frames>			saves a screenshot every that many frames; 0 stops
**	spawn <TypeID> x y		puts an object owned by the first computer house with a
**							construction yard on that cell
**	own <TypeID> x y		puts an object owned by the player on that cell
**	face <dir256> x y		turns the body and turret of the player's objects on that cell to the 256-step facing (0 is north)
**	voxelshade <type>		writes the shade each normal of that voxel normal table gets under the live light, diffuse alone and with a specular highlight
**	team <TeamTypeID> [plane]	makes a team of that type for that computer house, holding all its free units, active at once. With plane, it holds the house's first free aircraft instead, added with Team::Add so the aircraft keeps its mission and target
**	runtrigger <TriggerTypeID>	carries out the actions of that trigger type for its house, as if its events had all happened;
**							actions that look at attached objects see the map's trigger of that type
**	typecounts				writes how many types of each kind there are, and any whose ID is not a plain name
**	triggers [all]			writes the trigger types with their owner, events and whether a live trigger of each is enabled (only
**							the enabled ones, unless "all"), then every tag with the objects and cells it rides on, and the
**							local and global variables that are set
**	triggerlog <0|1>		writes a line each time a trigger springs
**	triggercoverage			writes each trigger type with how many times it has sprung, then those that never did
**	waitcell <TypeID> x y [radius] [timeout]	pauses the script until one of the player's objects of that type is within radius cells of the cell
**	waittrigger <TriggerID|name> [timeout]	pauses the script until that trigger has sprung once
**	waitowns <House> <TypeID> <count> [timeout]	pauses the script until the house owns at least count of the type
**	waitlocal <index> <0|1> [timeout]	pauses the script until that local variable has that value
**	waitgone <TypeID> x y [radius] [timeout]	pauses the script until no object of that type that strikeobj could pick is within radius cells of the cell
**	hurt <TypeID> <percent>	sets the strength of the player's objects of that type
**	warpout <TypeID> <0|1>	sets or clears the warping-out flag of the player's objects of that type, without a warp effect
**	cover x y			writes how many buildings screen that cell and whether it counts as covered
**	hiddenmarker <mode>		0 hides the hidden-object marker, 1 shows it, 2 shows brackets in place of
**							the Behind animation
**	beacon place x y	queues a beacon for the player on that cell, as a click in beacon mode does
**	beacon select x y	selects the beacon near that cell, as a click on it does
**	beacon delete		deletes the selected beacon when the player owns it, as the Delete command does
**	beacon list		writes each beacon with its house, slot, cell, visibility and text
**	beacontext <text>	sets the text of the player's selected beacon through the beacon event
**	clickcell <TypeID> x y	clicks the player's object of that type on that cell, as the player would
**							with it selected
**	typesounds <TypeID>		writes the sound numbers the type's create and transport sounds resolved to
**	maps					writes every XMP multiplayer map the file system finds
**	cratesounds				writes the sound numbers the crate pickup sounds resolved to
**	elite <TypeID>			makes the player's objects of that type elite
**	canrepair x y			writes whether the structure on that cell can be repaired with the repair cursor
**	overpower x y			writes how many objects charge the structure on that cell and whether it is
**							overpowered
**	ruleanims				writes the animations some rules settings resolved to
**	canfire <TypeID> x y	writes whether the player's object of that type could fire its primary
**							weapon at the object on that cell now, and why not, then the
**							weapon it would choose and whether that one could fire
**	weapons <TypeID>		writes each weapon slot of the type with its bullet's AA and AG flags and range
**	waypoint <any> x y	places a waypoint on the cell on the selected path, as a click in waypoint mode does
**	wpcell <n>		writes the cell of the map's waypoint n and the structure on it
**	wpsend <any> x y	gives the selected objects the follow-path order a click on the waypoint at the cell gives
**	wpstate <TypeID>	writes each aircraft of the type, whoever owns it, with its cell, height above ground, mission and status,
**					ammo, the waypoint path and next waypoint it follows, whether it moves, and its radio contact
**	airborne <TypeID>		writes each object of the type with its height above ground, whether it is in the air,
**							its render layer, and whether the ground or bridge occupier chain of its cell holds it
**	rocket <TypeID>			writes each foot object of the type with its coordinate in leptons, its height above ground, its flat and
**							vertical distance to the destination its locomotor flies to, and whether that locomotor is moving
**	shake <Warhead>			starts the screen shake that warhead's detonation would
**	screen					writes the current screen shake offset
**	setlocal <index> <0|1>	sets or clears that local variable, as a trigger action would; setglobal does the same to a global
**	dumpfile <name>	copies a file the game can read to dump-<name> in the working directory
**	mixof <name>	writes each attached archive that holds that file, in lookup order, with its size; the first is the copy Retrieve returns
**	stockcheck		reads every VXL, HVA, SHP and PCX that the art and rules files name, through the loaders, and logs each one refused
**	previewtime		reads the preview of each map on the multiplayer list three times and logs how long each pass took
**	trigaction <number>[@<House>] <value> <waypoint>	carries out that trigger action for the player's house, or the named one, with the value in its number, house or time field
**	killhouse <House> [1|2|3]	destroys the buildings (1), the units, infantry and aircraft (2) or everything (0, the default) the house owns,
**							with a player's object as the attacker; 3 destroys only the units, infantry and aircraft on cells the player can see
**	killtag <Tag>			destroys every object that carries a tag of that type, with a player's object as the attacker
**	counts <House>			writes the house's owned unit, building, infantry and aircraft counts, the ones the map events read
**	listhouse <House>		writes every object the house has, limbo'd ones too, with its limbo and transport state, cargo, team, occupants and visibility, then the counts the land-units event reads
**	transfer <TypeID>		gives the first computer object of that type to the player through Set_Owning_House
**	primary <TypeID>		makes the player's first building of that type its house's primary factory, as the sidebar's primary command does
**	factories				writes each factory structure, whoever owns it, with what it builds, its naval, primary and limbo flags
**	kill <TypeID>			destroys the objects of that type other houses own
**	crate <Powerup> x y		puts a crate holding that powerup (money, unit, heal, cloak, explosion, napalm, squad, darkness, reveal, armor, speed, firepower, icbm, invuln, veteran, ion, gas, tiberium or pod) on the nearest free cell to the cell
**	hit <TypeID>:<Warhead>[@<FirerTypeID>] <amount>	hits every object of that type, whoever owns it, with that
**							warhead, fired by one of the player's objects of another type, or of the named type
**	infiltrate barracks|warfactory	marks the player's house as having spied on that building, so its new trainable infantry or units start as veterans
**	occupy <TypeID> x y	puts the player's infantry of that type inside the structure on that cell, without walking there
**	rank <TypeID> <0|1|2>	makes every object of that type, whoever owns it, rookie, veteran or elite
**	veterancy <TypeID>	writes the experience, rank and cost of every object of that type, with the veterancy rules and the player's score
**	action <TypeID> x y		writes what the player's object of that type would do when clicked
**							on the object standing on that cell
**	rule <Key> <0|1>		overrides a rules setting the tests need: CanDetonateTimeBomb and AircraftWaypointReload
**	clickon <TypeID> x y	clicks the player's object of that type on the object standing on that
**							cell, as the player would with it selected
**	enter <TypeID> x y		sends the player's soldiers or vehicles of that type into the structure on
**							that cell
**	unload x y				orders the structure on that cell to unload
**	capture <TypeID> x y	sends the player's idle objects of that type to capture or infiltrate
**							the structure on that cell
**	tabflash				writes which tabs of the modern sidebar panel flash, one bit per tab
**	ambient <TypeID>		writes the ambient sound each object of that type has attached
**	trails					writes how many line trails exist and how many of their places still show
**	ammo <TypeID>			writes the ammunition and reload countdown of each vehicle of that type
**	enable <SuperWeaponTypeID>	gives the player that super weapon to charge from empty, as a structure does
**	houses					writes each house's money, power, spy effects, resource gatherers and powered unit centres
**	defeat <house>			runs the defeat a house gets once it has nothing left, on that house: its objects are destroyed
**							(HouseClass::Destroy_All), then MPlayer_Defeated runs. The house is named as the waits name one; "player" is the local player
**	houseobjects <house>	writes the units, infantry, aircraft and buildings the house owns outside limbo, its objects in limbo,
**							the objects it originally owns, and the living vehicles it owns
**	shortgame <0|1>			sets the Short Game option on this copy, as the setup screen does
**	observer				writes the local player's defeat, watching, whole-map and Session.ObiWan state, its owned objects,
**							the houses still playing, its selection and the beacon and planning modes
**	statics					writes how many sounds that Play Sound Effect At started are still going
**	supers					writes each house's aimed cell and base center, and each present super weapon: owner, charge
**							left, charge time and whether it is ready
**	garrisons				writes every structure that can be garrisoned
**	radar <TypeID>			writes each player object of that type with its limbo, radar tracking, discovery and whether Is_Radar_Visible plots it
**	radarall <TypeID>		writes each object of that type whoever owns it, with its owner and whether Is_Radar_Visible plots it
**	stripfill <column> <count>	offers building types 0 to count-1 to that sidebar strip and writes how many it took
**	aiscan					writes each foot object no player owns with its mission, team, target, range and area scan picks,
**							and the nearest player object with whether it is in range and can be fired at
**	lockscan				writes each object outside limbo whose IsLocked flag is clear, whoever owns it, with whether its cell is in
**							the local radar now, then the count of such objects and how many of them are in the local radar
**	gattling <TypeID>		writes each object of that type, whoever owns it, with its primary facing and the facing it turns to, a building's
**							aim point, the fire error of its primary weapon (a building only), barrel pitch, spin, gattling stage, turret
**							animation frame, recoil state, target cell, and a building's turret and barrel frame counts
**	where <TypeID>			writes each object of that type, whoever owns it, with its cell, mission, destination and target,
**							then its cloak state, whether it is firing, and the fire error its target gets from its primary weapon,
**							then how the local player is shown it (visual: the Visual_Character values, its sensed cell, whether it is
**							ready to cloak and its CloakDelay), whether the local player's radar plots it (radar: plotted, and how it
**							counts as detected), then the checks of Is_Ready_To_Cloak (recloak: its rearm, immobilizing
**							state, radio contact, and whether its target is close enough to fire at)
**	animlog <TypeID>		writes each object of that type, whoever owns it, with its walk counter and the body voxel frame it draws,
**							its turret frame and facings, whether its locomotor is moving, its firing state, its target, and whether a DiskLaser ring is active
**	forceattack <TypeID> x y	orders the player's objects of that type to force-attack the object on that cell, or the ground there, as a Ctrl+click does
**	turretcoord <TypeID>	writes each object of that type, whoever owns it, with its body and turret facings in 256 directions, its
**							TurretOffset, its weapon 0 mounting, and the point Turret_Coord returns relative to its render point,
**							with the body's draw matrix rows
**	recoil <TypeID>			writes the recoil phase, frames left and travel of the turret body and barrel of each building of that type
**	rock <TypeID>			writes the sideways and forwards tilt of each object of that type, the tilt per frame still to come, its facing and strength, and whether a parasite is inside it
**	speed <TypeID> [interval]	writes the current and target speed of the player's flying or jump-jet objects of that type,
**							now and every interval frames after when one is given
**	stop <TypeID>			sends the Stop command's idle event to the player's objects of that type that can move or fire
**	quantity <TypeID>		writes how many of that type each house is counted as having, owned and active
**	count <TypeID>			writes how many live objects of that type each house has
**	load <TypeID>			fills the player's objects of that type, a harvester for one, with ore to capacity
**	partsys					writes each live particle system with its type, the frames it has left and the object it rides on
**	bullets					writes each live projectile with its type, the house whose color scheme it uses and its type's trail delays
**	radbeams				writes how many radiation beams of each kind are live and how many eruption beams are visible
**	radlevel x y			writes the radiation level of a cell on a line of its own
**	effects <TypeID>		writes the AttachEffect count and multipliers, speed, strength and
**							reload countdown of every live object of that type
**	price <TypeID>			writes what the player pays for the type
**	types <prefix>			writes every structure type whose ID starts with the prefix
**	schemes					writes the color schemes and the scheme each house draws with
**	seq <TypeID>			writes an infantry type's art sequences
**	plan					writes each computer house's base plan
**	buildtrace <0|1>		logs the structures a computer house starts and places
**	truecolour <NAME.SHP>	writes whether a PNG replaces that shape, with its sheet size and one pixel
**	truecolourdir <path>	adds a directory the game searches for files, such as PNG sprites
**	dump					writes the player's credits, objects and missions to the log
**	fps					writes the game frames counted in the last whole second, which the game's FPS readout shows
**	log <text>				writes the text to the log
**	uploadverify <0|1>		1 checks each present that the copy kept of every uploaded texture matches the picture it came from; 0 writes the counts of checks and mismatches
**	presentdigest <0|1>		1 logs a PRESENT_DIGEST line at each present: the game frame, the present index, and a hash of the frame and the map layer; 0 stops the lines
**	drawdigest <0|1>		1 logs a DRAW_DIGEST line at each draw of the map view: the game frame, the draw index, and a hash of the map view's pixels; 0 stops the lines
**	drawprobe <0|1>		1 counts the call stacks that lock the map view during its foreground pass; 0 logs a DRAW_PROBE line per stack with its count
**	gpureplay <0|1>		1 records each object pass of the map view and logs a GPU_REPLAY line comparing a replay of the record with it; 0 stops
**	gpucheck <0|1>		1 also draws each recorded pass on the GPU and logs a GPU_DIFF line comparing it with the map view; 0 stops
**	searchdir <path>		adds a directory the game searches for files; it is added as the
**							script is read, before the game reads any file, whatever the frame
**	recordreplay <name>	records the skirmish to Replays/<name> in the user directory; it applies from the start,
**							whatever the frame
**	playreplay <name>		plays the recorded events of Replays/<name> in place of the orders given; it applies from
**							the start, whatever the frame
**	loadshot <name>			saves what the window shows each time a loading screen is
**							complete, as <name>-<count>.tga in the screenshots folder; it
**							applies from the start, whatever the frame
**	continuewin				a won mission goes on to the next one instead of ending the run; the game over
**							line still says won. The score screen then ends at once. The next mission is
**							chosen without a map selection screen, so no step is needed there. It applies
**							from the start, whatever the frame
**	continuelose			a lost game goes on to its score screen instead of ending the run; the game over
**							line still says lost. The score screen then ends at once. It applies from the start,
**							whatever the frame
**	quit					ends the process
*/

#include "always.h"

#include "autotest.h"
#include "mainloop.h"
#include "combat.h"
#include "perfstat.h"
#include "session.h"
#include "replay.h"
#include "spawner.h"
#include "statehash.h"
#include "rulesreload.h"

#include "_command.h"
#include "_keyboar.h"
#include "_map.h"
#include "beacon.h"
#include "queue.h"
#include "rocket.h"
#include "_rules.h"
#include "_tactica.h"
#include "audio/audioengine.h"
#include "aircraft.h"
#include "airctype.h"
#include "animtype.h"
#include "building.h"
#include "factory.h"
#include "ccfile.h"
#include "ccini.h"
#include "lcwstraw.h"
#include "xstraw.h"
#include "builtype.h"
#include "bullet.h"
#include "bullettype.h"
#include "cell.h"
#include "command.h"
#include "conquer.h"
#include "crate.hh"
#include "dbgprint.h"
#include "event.h"
#include "globals.h"
#include "goptions.h"
#include "gpudraw.h"
#include "hangwatch.h"
#include "house.h"
#include "houstype.h"
#include "infantry.h"
#include "infatype.h"
#include "init.h"
#include "keybindings.h"
#include "gamewindow.h"
#include "keyboard.h"
#include "loco.h"
#include "movie.h"
#include "logic.h"
#include "fly.h"
#include "jumpjet.h"
#include "teleport.h"
#include "tactical.h"
#include "tiberium.h"
#include "scheme.h"
#include "side.h"
#include "rules.h"
#include "saveload.h"
#include "syncrechook.h"
#include "scenario.h"
#include "shapeset.h"
#include "script.h"
#include "super.h"
#include "sidebar.h"
#include "hud.h"
#include "linetrail.h"
#include "ambient.h"
#include "suprtype.h"
#include "taskforc.h"
#include "teamtype.h"
#include "terrain.h"
#include "taction.h"
#include "lstorm.h"
#include "lightcon.h"
#include "_convert.h"
#include "trigger.h"
#include "trigtype.h"
#include "tag.h"
#include "tagtype.h"
#include "tevent.h"
#include "need.hh"
#include "reinf.h"
#include "team.h"
#include "overlay.h"
#include "overtype.h"
#include "vein.h"
#include "anim.h"
#include "voc.h"
#include "unit.h"
#include "viewzoom.h"
#include "modsync.h"
#include "mods.h"
#include "vidscale.h"
#include "video.h"
#include "bgfxbackend.h"
#include "gamedirs.h"
#include "theme.h"
#include "_rect.h"
#include "vox.h"
#include "_voxel.h"
#include "_vanim.h"
#include "vanim.h"
#include "vanimtype.h"
#include "voxlib.h"
#include "motlib.h"
#include "mixfile.h"
#include "mouse.h"
#include "shapeset.h"
#include "_pk.h"
#include "particle.h"
#include "ptype.h"
#include "partsys.h"
#include "psystype.h"
#include "pcx.h"
#include "preview.h"
#include "ui/rml/rmlimage.h"

extern int VoxelNormalTableEntryCount[];
#include "unittype.h"
#include "light.h"
#include "warhead.h"
#include "_warhead.h"
#include "radbeam.h"
#include "rawfile.h"
#include "armortypes.h"
#include "cameopcx.h"
#include "empulse.h"
#include "surface.h"
#include "truecolour.h"
#include "weapon.h"
#include "waypoint.h"
#include "ui/uiscript.h"
#include "windowevent.hh"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <sstream>
#include <string>
#include <vector>

// Where the pointer rests, in window pixels.
static int _CursorX = 200;
static int _CursorY = 200;

// What each house has finished producing: units, infantry, aircraft and structures.
struct ProducedTallyStruct {
	int Count[4];
};
static std::map<HouseClass const *, ProducedTallyStruct> _ProducedBy;

// The folder of the script being read, absolute and ending in a separator. Empty before a script is read.
static std::string AutoTest_Script_Folder;

// The folder of a script file as an absolute path with a trailing separator; empty when that cannot be worked out.
static std::string AutoTest_Folder_Of(char const * filename)
{
	std::error_code error;
	std::string folder = std::filesystem::absolute(filename, error).parent_path().string();
	if (error) {
		return std::string();
	}
	if (!folder.empty() && folder.back() != '\\' && folder.back() != '/') {
		folder += '\\';
	}
	return folder;
}

// A path that a step reads from disk: a relative one is taken from the script's folder, a rooted one is kept as written.
static std::string AutoTest_Input_Path(std::string const & path)
{
	std::filesystem::path const given(path);
	if (AutoTest_Script_Folder.empty() || given.has_root_name() || given.has_root_directory()) {
		return path;
	}
	return AutoTest_Script_Folder + path;
}

void AutoTest_Note_Built(HouseClass const * house, int rtti)
{
	int slot = -1;
	if (rtti == RTTI_UNIT) slot = 0;
	else if (rtti == RTTI_INFANTRY) slot = 1;
	else if (rtti == RTTI_AIRCRAFT) slot = 2;
	else if (rtti == RTTI_BUILDING) slot = 3;
	if (slot >= 0) {
		_ProducedBy[house].Count[slot]++;
	}
}


namespace {

struct StepType
{
	int Frame;
	std::string Command;
	std::string Argument;
	int X;
	int Y;
	// The line as written, without trailing white space, for the wait messages.
	std::string Line;
};

bool Active = false;

// The keys a script holds with hold and lets go with release. An unattended run reads the held keys from here,
// so Keyboard->Down sees the keys the script holds.
bool ScriptHeldKeys[256];
std::vector<StepType> Steps;
std::size_t Next = 0;

// Frames between the screenshots of a recording, or 0 when not recording.
int RecordInterval = 0;
int HashInterval = 0;
int StateCRCInterval = 0;

// The type the speed probe logs, empty when it is off, and the frames between its logs (0 logs once).
std::string SpeedProbeType;
int SpeedProbeInterval = 0;

// The type the view keeps centred on, or empty when it stays put.
std::string FollowType;

// The name loading screen captures are saved under, or empty when none are taken.
std::string LoadShotName;
int LoadShotCount = 0;

// Set by a continuewin step; a won mission then carries on instead of ending the run.
bool ContinueAfterWin = false;
bool PostWinContinue = false;
// Set by a continuelose line; a lost game then carries on to its score screen instead of ending the run.
bool ContinueAfterLose = false;
bool PostLoseContinue = false;
// Set by a requirera2 line; the run then ends if the mission was read without Red Alert 2 house numbering.
bool RequireRA2 = false;

// Frames a wait step waits for its condition when its line gives no timeout.
int const Wait_Default_Timeout = 3000;

// A wait's steps run this many frames after their authored frames. It grows each time a wait ends,
// so a line keeps its spacing from the wait line.
int WaitShift = 0;

// The index of the step whose arguments were last checked, so a wait's problems are logged once.
std::size_t WaitChecked = static_cast<std::size_t>(-1);

// Whether each trigger spring is logged, and how many times each trigger type has sprung this run.
bool TriggerLog = false;
std::map<TriggerTypeClass const *, int> TriggerSprings;


TechnoTypeClass const * Find_Type(std::string const & name)
{
	for (int index = 0; index < BuildingTypes.Count(); index++) {
		if (stricmp(BuildingTypes[index]->Name(), name.c_str()) == 0) return(BuildingTypes[index]);
	}
	for (int index = 0; index < UnitTypes.Count(); index++) {
		if (stricmp(UnitTypes[index]->Name(), name.c_str()) == 0) return(UnitTypes[index]);
	}
	for (int index = 0; index < InfantryTypes.Count(); index++) {
		if (stricmp(InfantryTypes[index]->Name(), name.c_str()) == 0) return(InfantryTypes[index]);
	}
	for (int index = 0; index < AircraftTypes.Count(); index++) {
		if (stricmp(AircraftTypes[index]->Name(), name.c_str()) == 0) return(AircraftTypes[index]);
	}
	return(NULL);
}


template<class T>
void For_Player_Objects(DynamicVectorClass<T *> & list, std::string const & name, void (*action)(T *))
{
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		// A house the map marks PlayerControl=yes takes orders as the player's own do, so its objects count too.
		if (object != NULL && (object->House == PlayerPtr || object->House->Is_Player_Control()) && !object->IsInLimbo && stricmp(object->TClass->Name(), name.c_str()) == 0) {
			action(object);
		}
	}
}


// Whether one of the player's objects of that type is on the map within radius cells of the cell,
// measured as a straight line between the two cells.
template<class T>
bool Player_Object_Within(DynamicVectorClass<T *> & list, std::string const & name, Cell const & cell, int radius)
{
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		if (object == NULL || object->IsInLimbo || (object->House != PlayerPtr && !object->House->Is_Player_Control()) || stricmp(object->TClass->Name(), name.c_str()) != 0) {
			continue;
		}
		Cell const at = object->Get_Cell();
		int const dx = at.X - cell.X;
		int const dy = at.Y - cell.Y;
		if (dx * dx + dy * dy <= radius * radius) {
			return(true);
		}
	}
	return(false);
}


// Whether other people's copies of the game take part. Their orders then go through the event queue, as a
// player's orders do, so every copy applies them. Direct state changes would leave the other copies behind.
bool Network_Game(void)
{
	return(Session.Type == GAME_IPX || Session.Type == GAME_INTERNET);
}


// Gives the player's objects of that type in one list a mission as a queued event, as a click's order is given.
template<class T>
void Queue_Player_Missions(DynamicVectorClass<T *> & list, std::string const & name, MissionType mission, AbstractClass * target, AbstractClass * destination)
{
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		if (object != NULL && object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->TClass->Name(), name.c_str()) == 0) {
			object->Player_Assign_Mission(mission, target, destination);
		}
	}
}


void Select_Type(std::string const & name)
{
	Unselect_All();
	For_Player_Objects<UnitClass>(Units, name, [](UnitClass * object) {object->Select();});
	For_Player_Objects<InfantryClass>(Infantry, name, [](InfantryClass * object) {object->Select();});
	For_Player_Objects<BuildingClass>(Buildings, name, [](BuildingClass * object) {object->Select();});
	For_Player_Objects<AircraftClass>(Aircraft, name, [](AircraftClass * object) {object->Select();});
}


void Place(std::string const & name)
{
	TechnoTypeClass const * type = Find_Type(name);
	if (type != NULL && type->Fetch_RTTI() != RTTI_BUILDINGTYPE) {
		// A finished vehicle, aircraft or infantry leaves its factory on the sidebar's no-cell placement.
		RTTIType const kind = type->Fetch_RTTI() == RTTI_UNITTYPE ? RTTI_UNIT : type->Fetch_RTTI() == RTTI_AIRCRAFTTYPE ? RTTI_AIRCRAFT : RTTI_INFANTRY;
		DebugString("AUTOTEST place %s: leaves its factory\n", name.c_str());
		OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PLACE, kind, type->Fetch_Heap_ID(), CELL_NONE));
		return;
	}
	if (type == NULL || type->Fetch_RTTI() != RTTI_BUILDINGTYPE || PlayerPtr->ConYards.Count() == 0) {
		DebugString("AUTOTEST place %s: no structure type or no construction yard\n", name.c_str());
		return;
	}

	BuildingTypeClass const * building = (BuildingTypeClass const *)type;
	Cell const center = PlayerPtr->ConYards[0]->Get_Cell();
	for (int radius = 1; radius < 24; radius++) {
		for (int dy = -radius; dy <= radius; dy++) {
			for (int dx = -radius; dx <= radius; dx++) {
				if (std::abs(dx) != radius && std::abs(dy) != radius) continue;
				Cell const cell = center + Cell(dx, dy);
				if (!Map.In_Local_Radar(cell)) continue;
				if (!building->Legal_Placement(cell, PlayerPtr)) continue;
				if (!Map.Passes_Proximity_Check(building, PlayerPtr->Class->House, building->Occupy_List(true), cell)) continue;
				DebugString("AUTOTEST place %s at %d,%d\n", name.c_str(), cell.X, cell.Y);
				OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PLACE, RTTI_BUILDINGTYPE, building->Fetch_Heap_ID(), cell));
				return;
			}
		}
	}
	DebugString("AUTOTEST place %s: no legal cell found\n", name.c_str());
}


// Queues the placement event for a finished structure on the given cell, as Place does for the cell it finds.
void PlaceAt(std::string const & name, int x, int y)
{
	TechnoTypeClass const * type = Find_Type(name);
	if (type == NULL || type->Fetch_RTTI() != RTTI_BUILDINGTYPE || PlayerPtr->ConYards.Count() == 0) {
		DebugString("AUTOTEST placeat %s: no structure type or no construction yard\n", name.c_str());
		return;
	}
	BuildingTypeClass const * building = (BuildingTypeClass const *)type;
	Cell const cell(x, y);
	bool const legal = Map.In_Local_Radar(cell) && building->Legal_Placement(cell, PlayerPtr)
		&& Map.Passes_Proximity_Check(building, PlayerPtr->Class->House, building->Occupy_List(true), cell);
	DebugString("AUTOTEST placeat %s at %d,%d: %s\n", name.c_str(), x, y, legal ? "queued" : "not legal");
	if (legal) {
		OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PLACE, RTTI_BUILDINGTYPE, building->Fetch_Heap_ID(), cell));
	}
}


// Returns false, after logging the refusal, when the game's placement check fails for the foundation that a building
// type would cover at cell and a structure stands on one of its cells. A unit or terrain alone does not refuse.
bool Footprint_Free(char const * command, TechnoTypeClass const * type, Cell const & cell, HouseClass * house)
{
	if (type->Fetch_RTTI() != RTTI_BUILDINGTYPE) return(true);
	BuildingTypeClass const * building = (BuildingTypeClass const *)type;
	if (building->Legal_Placement(cell, house)) return(true);
	for (Cell const * offset = building->Occupy_List(true); *offset != REFRESH_EOL; offset++) {
		Cell const covered = cell + *offset;
		BuildingClass const * other = Map[covered].Cell_Building();
		if (other != NULL) {
			DebugString("AUTOTEST %s %s at %d,%d: refused: footprint overlaps %s at %d,%d\n", command, type->Name(), cell.X, cell.Y, other->Class->Name(), covered.X, covered.Y);
			return(false);
		}
	}
	return(true);
}


void Move(std::string const & name, int x, int y)
{
	Select_Type(name);
	Cell const cell(x, y);
	if (Network_Game()) {
		PlanningRecordScope const recording(Map.IsPlanningMode);
		Queue_Player_Missions(Units, name, MISSION_MOVE, NULL, &Map[cell]);
		Queue_Player_Missions(Infantry, name, MISSION_MOVE, NULL, &Map[cell]);
		Queue_Player_Missions(Aircraft, name, MISSION_MOVE, NULL, &Map[cell]);
		return;
	}
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * unit = Units[index];
		if (unit->IsSelected) unit->Assign_Destination(&Map[cell]), unit->Assign_Mission(MISSION_MOVE);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (infantry->IsSelected) infantry->Assign_Destination(&Map[cell]), infantry->Assign_Mission(MISSION_MOVE);
	}
	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * aircraft = Aircraft[index];
		if (aircraft->IsSelected) aircraft->Assign_Destination(&Map[cell]), aircraft->Assign_Mission(MISSION_MOVE);
	}
}


void Order(std::string const & name, int x, int y)
{
	Cell const cell(x, y);
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->Is_Foot() && techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), name.c_str()) == 0) {
			techno->Player_Assign_Mission(MISSION_MOVE, NULL, &Map[cell]);
		}
	}
}


void Computer_Move(std::string const & name, int x, int y)
{
	Cell const cell(x, y);
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->Is_Foot() && techno->House != PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), name.c_str()) == 0) {
			FootClass * foot = static_cast<FootClass *>(techno);
			foot->Assign_Destination(&Map[cell]);
			foot->Assign_Mission(MISSION_MOVE);
		}
	}
}


/// <summary>
/// Puts the computer players' foot objects of that type on the hunt mission, with no target or destination.
/// </summary>
void Computer_Hunt(std::string const & name)
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->Is_Foot() && techno->House != PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), name.c_str()) == 0) {
			FootClass * foot = static_cast<FootClass *>(techno);
			foot->Assign_Target(NULL);
			foot->Assign_Destination(NULL);
			foot->Assign_Mission(MISSION_HUNT);
			if (foot->Ready_To_Commence()) {
				foot->Commence();
			}
		}
	}
}


/// <summary>
/// Puts the player's foot objects of that type on the repair mission, as a harvester with nothing to do is put on it,
/// so UnitClass::Do_MISSION_REPAIR runs its depot search.
/// </summary>
void Player_Repair(std::string const & name)
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->Is_Foot() && techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), name.c_str()) == 0) {
			FootClass * foot = static_cast<FootClass *>(techno);
			foot->Assign_Mission(MISSION_REPAIR);
		}
	}
}


/// <summary>
/// Queues the Stop command's idle event for the player's objects of that type that can move or fire.
/// </summary>
void Stop_Objects(std::string const & name)
{
	int sent = 0;
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == PlayerPtr && !techno->IsInLimbo && (techno->Can_Player_Move() || techno->Can_Player_Fire()) && stricmp(techno->TClass->Name(), name.c_str()) == 0) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::IDLE, TargetClass(techno)));
			sent++;
		}
	}
	DebugString("AUTOTEST stop %s: sent to %d\n", name.c_str(), sent);
}


BuildingClass * Nearest_Enemy_Building(void)
{
	if (PlayerPtr->ConYards.Count() == 0) return(NULL);
	Coord const home = PlayerPtr->ConYards[0]->Center_Coord();

	BuildingClass * best = NULL;
	int bestdistance = 0;
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * building = Buildings[index];
		// Only a house that has a base counts; this leaves out the map's civilian and neutral structures.
		if (building->IsInLimbo || building->House == NULL || building->House->Is_Ally(PlayerPtr) || building->House->ConYards.Count() == 0) continue;
		int const distance = ::Distance(home, building->Center_Coord());
		if (best == NULL || distance < bestdistance) {
			best = building;
			bestdistance = distance;
		}
	}
	return(best);
}


void Attack(std::string const & name)
{
	BuildingClass * target = Nearest_Enemy_Building();
	if (target == NULL) {
		DebugString("AUTOTEST attack: no enemy structure\n");
		return;
	}
	DebugString("AUTOTEST attack %s -> %s at %d,%d\n", name.c_str(), target->Class->Name(), target->Get_Cell().X, target->Get_Cell().Y);

	Select_Type(name);
	if (Network_Game()) {
		PlanningRecordScope const recording(Map.IsPlanningMode);
		Queue_Player_Missions(Units, name, MISSION_ATTACK, target, NULL);
		Queue_Player_Missions(Infantry, name, MISSION_ATTACK, target, NULL);
		Queue_Player_Missions(Aircraft, name, MISSION_ATTACK, target, NULL);
		return;
	}
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * unit = Units[index];
		if (unit->IsSelected) unit->Assign_Target(target), unit->Assign_Mission(MISSION_ATTACK);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (infantry->IsSelected) infantry->Assign_Target(target), infantry->Assign_Mission(MISSION_ATTACK);
	}
	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * aircraft = Aircraft[index];
		if (aircraft->IsSelected) aircraft->Assign_Target(target), aircraft->Assign_Mission(MISSION_ATTACK);
	}
}


/*
**	strikeobj: the nearest live object of the type within radius cells of the cell is the target,
**	whoever owns it except the player and its allies. Every armed unit and infantry the player owns
**	is ordered to attack it. A target that moves stays the target, and an object that dies is
**	replaced by the next strikeobj line.
*/
bool Strike_Side_Allows(bool naval, std::string const & side)
{
	if (side == "land") return !naval;
	if (side == "naval") return naval;
	return true;
}


// Without an attacker type every armed unit of the player's own house attacks; with one, only objects of that type from the player's
// house or a PlayerControl house do.
bool Strike_Attacker_Allows(TechnoClass const * techno, std::string const & attacker)
{
	if (attacker.empty()) return(techno->House == PlayerPtr);
	return((techno->House == PlayerPtr || techno->House->Is_Player_Control()) && stricmp(techno->TClass->Name(), attacker.c_str()) == 0);
}


// The nearest live object of the type within radius cells of the cell, whoever owns it except the player and its allies.
// A type of * stands for any type that is neither Insignificant nor DontScore.
TechnoClass * Strike_Target(std::string const & name, int x, int y, int radius)
{
	TechnoClass * target = NULL;
	int bestdistance = 0;
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->IsInLimbo || !techno->IsActive || techno->House == PlayerPtr || techno->House->Is_Ally(PlayerPtr)) continue;
		if (name == "*") {
			// Any type the house counts as owned, which is what the destroyed-house events read.
			if (techno->TClass->IsInsignificant || techno->TClass->IsDontScore) continue;
		} else if (stricmp(techno->TClass->Name(), name.c_str()) != 0) {
			continue;
		}
		Cell const cell = techno->Get_Cell();
		int const dx = cell.X - x;
		int const dy = cell.Y - y;
		int const distance = dx * dx + dy * dy;
		if (distance > radius * radius) continue;
		if (target == NULL || distance < bestdistance) {
			target = techno;
			bestdistance = distance;
		}
	}
	return(target);
}


void Strike_Object(std::string const & name, int x, int y, int radius, std::string const & side, std::string const & attacker)
{
	if (side != "" && side != "all" && side != "land" && side != "naval") {
		DebugString("AUTOTEST strikeobj %s: side must be land, naval or all, not %s\n", name.c_str(), side.c_str());
		return;
	}
	TechnoClass * target = Strike_Target(name, x, y, radius);
	if (target == NULL) {
		DebugString("AUTOTEST strikeobj %s near %d,%d: no live target within %d cells\n", name.c_str(), x, y, radius);
		return;
	}

	int orders = 0;
	bool const network = Network_Game();
	PlanningRecordScope const recording(Map.IsPlanningMode);
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * unit = Units[index];
		if (Strike_Attacker_Allows(unit, attacker) && !unit->IsInLimbo && unit->Is_Weapon_Equipped() && Strike_Side_Allows(unit->TClass->IsNaval, side)) {
			if (network) {
				unit->Player_Assign_Mission(MISSION_ATTACK, target);
			} else {
				unit->Assign_Target(target);
				unit->Assign_Mission(MISSION_ATTACK);
			}
			orders++;
		}
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * infantry = Infantry[index];
		if (Strike_Attacker_Allows(infantry, attacker) && !infantry->IsInLimbo && infantry->Is_Weapon_Equipped() && Strike_Side_Allows(infantry->TClass->IsNaval, side)) {
			if (network) {
				infantry->Player_Assign_Mission(MISSION_ATTACK, target);
			} else {
				infantry->Assign_Target(target);
				infantry->Assign_Mission(MISSION_ATTACK);
			}
			orders++;
		}
	}
	DebugString("AUTOTEST strikeobj %s -> %s at %d,%d: %d orders\n", name.c_str(), target->TClass->Name(), target->Get_Cell().X, target->Get_Cell().Y, orders);
}


/*
**	scatter: Scatter is called on each live soldier of the type with the given threat, and the result is written.
**	Frames before and after the call show where it sent the soldier.
*/
void Scatter_Objects(std::string const & name, int x, int y, bool forced, bool nokidding)
{
	Coord const threat = x == -1 ? COORD_NONE : Cell(x, y).As_Coord();
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * soldier = Infantry[index];
		if (soldier->IsInLimbo || !soldier->IsActive || soldier->Strength <= 0 || stricmp(soldier->Class->Name(), name.c_str()) != 0) continue;
		Coord const from = soldier->PositionCoord;
		int const before = Scen->RandomNumber.Index_1();
		soldier->Scatter(threat, forced, nokidding);
		int const draws = (Scen->RandomNumber.Index_1() - before + Random2Class::TABLE_SIZE) % Random2Class::TABLE_SIZE;
		Cell const nav = soldier->NavCom != NULL ? soldier->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   scatter %s of %s at %d,%d (%d,%d) do %d threat %d,%d forced %d nokidding %d: mission %s nav %d,%d draws %d\n", soldier->Class->Name(), soldier->House->Class->Name(), soldier->Get_Cell().X, soldier->Get_Cell().Y,
			from.X & 255, from.Y & 255, (int)soldier->Doing, x, y, (int)forced, (int)nokidding, MissionClass::Mission_Name(soldier->Get_Mission()), nav.X, nav.Y, draws);
	}
}


void Enemies(void)
{
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * object = Buildings[index];
		if (object->House == PlayerPtr || object->IsInLimbo) continue;
		DebugString("AUTOTEST   enemy building %s house %s cell %d,%d strength %d\n", object->Class->Name(), object->House->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->Strength);
	}
	int units = 0;
	int infantry = 0;
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass const * unit = Units[index];
		if (unit->House == PlayerPtr) continue;
		units++;
		Cell const tar = unit->TarCom != NULL ? unit->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   enemy unit %s cell %d,%d height %d layer %d strength %d berzerk %d tar %d,%d\n", unit->Class->Name(), unit->Get_Cell().X, unit->Get_Cell().Y, unit->HeightAGL, (int)unit->In_Which_Layer(), unit->Strength, (int)unit->IsBerzerk, tar.X, tar.Y);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass const * soldier = Infantry[index];
		if (soldier->House == PlayerPtr) continue;
		infantry++;
		Cell const nav = soldier->NavCom != NULL ? soldier->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   enemy infantry %s cell %d,%d strength %d mission %s nav %d,%d limbo %d\n", soldier->Class->Name(), soldier->Get_Cell().X, soldier->Get_Cell().Y, soldier->Strength, MissionClass::Mission_Name(soldier->Get_Mission()), nav.X, nav.Y, (int)soldier->IsInLimbo);
	}
	DebugString("AUTOTEST   enemy units %d infantry %d\n", units, infantry);
}


// Writes each house's base IQ (Control.IQ, the map's IQ= value) and effective IQ, and whether each [IQ] gate is open.
void Iq(void)
{
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass const * house = Houses[index];
		DebugString("AUTOTEST iq house %d %s human %d control %d effective %d paranoid %d basebuild %d difficulty %d"
			" sw %d repairsell %d sellback %d production %d harvester %d guardarea %d crush %d scatter %d contentscan %d\n",
			index, house->Class->Name(), (int)house->Is_Human_Player(), house->Control.IQ, house->IQ, (int)house->IsParanoid,
			(int)house->IsBaseBuilding, (int)house->Difficulty,
			(int)(Session.Type != GAME_NORMAL || house->IQ >= Rule->IQSuperWeapons),
			(int)(house->IQ >= Rule->IQRepairSell), (int)(house->Control.IQ >= Rule->IQSellBack),
			(int)(house->IQ >= Rule->IQProduction), (int)(house->IQ >= Rule->IQHarvester),
			(int)(house->IQ >= Rule->IQGuardArea), (int)(house->IQ >= Rule->IQCrush),
			(int)(house->IQ >= Rule->IQScatter), (int)(house->IQ >= Rule->IQContentScan));
	}
}


void Dump(void)
{
	DebugString("AUTOTEST dump frame %d credits %d power %d drain %d\n", Frame, PlayerPtr->Available_Money(), PlayerPtr->Power, PlayerPtr->Drain);
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass * object = Buildings[index];
		if (object->House != PlayerPtr) continue;
		DebugString("AUTOTEST   building %s cell %d,%d strength %d curtain %d mission %s silo %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->Strength, (int)object->IronCurtainTimer, MissionClass::Mission_Name(object->Get_Mission()), (int)object->Class->IsNukeSilo);
		if (object->Class->IsGattling) {
			DebugString("AUTOTEST     gattling stage %d value %d turret frame %d\n", object->CurrentGattlingStage, object->GattlingValue, object->TurretAnimFrame);
		}
	}
	for (int index = 0; index < Units.Count(); index++) {
		UnitClass * object = Units[index];
		if (object->House != PlayerPtr) continue;
		Cell const nav = object->NavCom != NULL ? object->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		Cell const tar = object->TarCom != NULL ? object->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		ClassID const loco = Locomotion_Class_ID(object->Locomotion.get());
		DebugString("AUTOTEST   unit %s cell %d,%d mission %s status %d nav %d,%d tar %d,%d strength %d moving %d limbo %d loco %08X typeloco %08X speed %d load %d%% ore %d curtain %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), object->Status, nav.X, nav.Y, tar.X, tar.Y, object->Strength, (int)object->Locomotion->Is_Moving(), (int)object->IsInLimbo, (unsigned)loco.Data1, (unsigned)object->Class->Locomotor.Data1, object->Class->MaxSpeed, (int)(object->Tiberium_Load() * 100), (int)object->Get_Cell_Ptr()->Tiberium_Value(), (int)object->IronCurtainTimer);
		if (object->Is_Iron_Curtained()) {
			DebugString("AUTOTEST     tint stage %d light %d\n", object->IronTintStage, object->Apparent_Brightness(1000));
		}
		if (object->Class->IsGattling) {
			DebugString("AUTOTEST     gattling stage %d value %d turret frame %d tar %s\n", object->CurrentGattlingStage, object->GattlingValue, object->TurretAnimFrame, object->TarCom != NULL ? "yes" : "no");
		}
		if (object->ParasiteEatingMe != NULL) {
			DebugString("AUTOTEST     parasite %s inside\n", object->ParasiteEatingMe->TClass->Name());
		}
		if (object->Class->IsGunner) {
			WeaponTypeClass const * weapon = object->Get_Class_Weapon_Data(object->CurrentWeaponNumber)->Weapon;
			DebugString("AUTOTEST     gunner weapon %d (%s) turret %d passengers %d\n", object->CurrentWeaponNumber, weapon != NULL ? weapon->Name() : "none", object->CurrentTurretNumber, object->Cargo.How_Many());
		}
	}
	for (int index = 0; index < Aircraft.Count(); index++) {
		AircraftClass * object = Aircraft[index];
		if (object->House != PlayerPtr) continue;
		TechnoClass const * contact = object->Contact_With_Whom();
		DebugString("AUTOTEST   aircraft %s cell %d,%d height %d mission %s ammo %d limbo %d radio %s slot %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, object->HeightAGL, MissionClass::Mission_Name(object->Get_Mission()), object->Ammo, (int)object->IsInLimbo, contact != NULL ? contact->TClass->Name() : "-", contact != NULL ? contact->Find_Link_Index(object) : -1);
	}
	for (int index = 0; index < Infantry.Count(); index++) {
		InfantryClass * object = Infantry[index];
		if (object->House != PlayerPtr) continue;
		Cell const tar = object->TarCom != NULL ? object->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   infantry %s cell %d,%d mission %s do %d deployed %d strength %d rank %d tar %d,%d opentopped %d lastfire %d height %d speed %d nav %d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, MissionClass::Mission_Name(object->Get_Mission()), (int)object->Doing, (int)object->Is_Deployed(), (int)object->Strength, object->Veterancy.Is_Elite() ? 2 : (object->Veterancy.Is_Veteran() ? 1 : 0), tar.X, tar.Y, (int)object->IsInOpenToppedTransport, object->LastFireFrame, object->HeightAGL, (int)(object->Speed * 100), object->NavCom != NULL ? 1 : 0);
		if (object->DisguiseType != NULL) {
			DebugString("AUTOTEST     disguised as %s of %s\n", object->DisguiseType->Name(), object->DisguiseHouse != NULL ? object->DisguiseHouse->Class->Name() : "-");
		}
	}
}


template<class T>
void Log_Technos_Parts(char const * kind, DynamicVectorClass<T *> & list)
{
	for (int index = 0; index < list.Count(); index++) {
		T * object = list[index];
		DebugString("AUTOTEST   %s %d %s at %d,%d,%d facing %d strength %d mission %s limbo %d house %d\n",
			kind, index, object->Class->Name(), object->PositionCoord.X, object->PositionCoord.Y, object->PositionCoord.Z,
			object->PrimaryFacing.Current().As_Int(), object->Strength, MissionClass::Mission_Name(object->Get_Mission()),
			(int)object->IsInLimbo, Houses.ID(object->House));
	}
}


/// <summary>
/// Logs each value the state hash covers, one object per line, so two runs whose hashes differ
/// can be compared line by line.
/// </summary>
void Log_State_Parts(void)
{
	DebugString("AUTOTEST hashparts frame %d\n", Frame);
	Log_Technos_Parts("building", Buildings);
	Log_Technos_Parts("unit", Units);
	Log_Technos_Parts("aircraft", Aircraft);
	Log_Technos_Parts("infantry", Infantry);
	for (int index = 0; index < Houses.Count(); index++) {
		DebugString("AUTOTEST   house %d credits %d\n", index, Houses[index]->Credits);
	}
	unsigned char const * random = reinterpret_cast<unsigned char const *>(&Scen->RandomNumber);
	std::string bytes;
	for (size_t index = 0; index < sizeof(Scen->RandomNumber); index++) {
		char text[4];
		std::snprintf(text, sizeof(text), "%02X", random[index]);
		bytes += text;
	}
	DebugString("AUTOTEST   random %s\n", bytes.c_str());
}


/// <summary>
/// Logs a hash of the game state that decides play: every building, vehicle, aircraft and soldier
/// (position, facing, strength, mission, limbo, owner), each house's credits and the scenario's
/// random-number state. Two builds given the same script and seed should log the same hashes.
/// Reading the state does not change it.
/// </summary>
void Log_State_Hash(void)
{
	unsigned int const hash = Compute_Sim_State_Hash().State;
	DebugString("AUTOTEST hash frame %d %08X buildings %d units %d aircraft %d infantry %d\n", Frame, hash, Buildings.Count(), Units.Count(), Aircraft.Count(), Infantry.Count());
}


/// <summary>
/// Logs two checksums of the game state: Object_CRCs, the per-object checksum that the multiplayer
/// check folds in and that leaves out each object's target, and a second checksum over every
/// object's target. Runs with the same script and seed should log the same values. Reading the
/// state does not change it.
/// </summary>
void Log_State_CRC(void)
{
	unsigned int const targets = Compute_Sim_State_Hash().Targets;
	CRCEngine objects = Object_CRCs();
	DebugString("AUTOTEST statecrc frame %d objects %08X targets %08X\n", Frame, (unsigned int)objects(), targets);
}


/// <summary>
/// Logs the speed of each player object of the probed type. A flying object reports its speed
/// as a fraction of its full speed, a jump-jet object in leptons per frame. Reading the state
/// does not change it.
/// </summary>
void Log_Speed_Probe(void)
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass const * techno = Technos[index];
		if (techno->House != PlayerPtr || techno->IsInLimbo || !techno->Is_Foot() || stricmp(techno->TClass->Name(), SpeedProbeType.c_str()) != 0) {
			continue;
		}
		FootClass const * foot = static_cast<FootClass const *>(techno);
		Cell const nav = foot->NavCom != NULL ? foot->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
		FlyLocomotionClass const * fly = dynamic_cast<FlyLocomotionClass const *>(foot->Locomotion.get());
		JumpjetLocomotionClass const * jump = dynamic_cast<JumpjetLocomotionClass const *>(foot->Locomotion.get());
		DebugString("AUTOTEST facing %s frame %d primary %d desired %d loco %d\n", techno->TClass->Name(), Frame, (int)techno->PrimaryFacing.Current().As_Int(), (int)techno->PrimaryFacing.Desired().As_Int(), jump != NULL ? (int)jump->Facing_Current().As_Int() : -1);
		if (fly != NULL) {
			DebugString("AUTOTEST speed %s frame %d kind fly cell %d,%d height %d mission %s nav %d,%d moving %d current %.4f target %.4f\n", techno->TClass->Name(), Frame, techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->HeightAGL, MissionClass::Mission_Name(techno->Get_Mission()), nav.X, nav.Y, (int)foot->Locomotion->Is_Moving(), fly->Current_Speed(), fly->Target_Speed());
		} else if (jump != NULL) {
			Cell const head = foot->Locomotion->Head_To_Coord().As_Cell();
			DebugString("AUTOTEST speed %s frame %d kind jump state %s cell %d,%d height %d mission %s nav %d,%d head %d,%d moving %d current %.4f target %.4f\n", techno->TClass->Name(), Frame, jump->State_Name(), techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->HeightAGL, MissionClass::Mission_Name(techno->Get_Mission()), nav.X, nav.Y, head.X, head.Y, (int)foot->Locomotion->Is_Moving(), jump->Current_Speed(), jump->Target_Speed());
		} else {
			DebugString("AUTOTEST speed %s frame %d kind other cell %d,%d height %d mission %s\n", techno->TClass->Name(), Frame, techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->HeightAGL, MissionClass::Mission_Name(techno->Get_Mission()));
		}
	}
}


// speed <TypeID> [interval]: logs the speed of the player's objects of that type now, and every interval frames
// after when one is given. A line with interval 0 logs once and stops the repeats.
static void Speed_Probe(StepType const & step)
{
	SpeedProbeType = step.Argument;
	SpeedProbeInterval = std::max(0, step.X);
	Log_Speed_Probe();
}


/// <summary>
/// Writes, for each tiberium type, its cells with their total density and credit value, how many
/// of them can still grow or spread, and the lengths of its growth and spread queues, for watching
/// ore fields regrow and spread over a long game.
/// </summary>
void Log_Ore(void)
{
	DebugString("AUTOTEST   ore frame %d\n", Frame);
	for (int type = 0; type < Tiberiums.Count(); type++) {
		int cells = 0, density = 0, value = 0, growable = 0, spreadable = 0;
		Map.Reset_Iterator();
		for (CellClass * cellptr = Map.Iterate(); cellptr != NULL; cellptr = Map.Iterate()) {
			if (cellptr->Tiberium_Type_Here() == Tiberiums[type]->HeapID) {
				cells++;
				density += cellptr->OverlayData + 1;
				value += cellptr->Tiberium_Value();
				growable += cellptr->Can_Tiberium_Grow() ? 1 : 0;
				spreadable += cellptr->Can_Tiberium_Spread() ? 1 : 0;
			}
		}
		DebugString("AUTOTEST   ore %s cells %d density %d value %d growable %d spreadable %d queued growth %d spread %d\n",
			Tiberiums[type]->Name(), cells, density, value, growable, spreadable, Tiberiums[type]->GrowthQueue.Count(), Tiberiums[type]->SpreadQueue.Count());
	}
}


/// <summary>
/// Writes the tiberium cells nearest the player's first building, nearest first.
/// </summary>
void Log_Ore_Cells(int limit)
{
	if (limit > 32) limit = 32;
	Cell origin(-1, -1);
	for (int index = 0; index < Buildings.Count(); index++) {
		BuildingClass const * building = Buildings[index];
		if (building->House == PlayerPtr && !building->IsInLimbo) {
			origin = building->Get_Cell();
			break;
		}
	}
	Cell found[32];
	int distances[32];
	int count = 0;
	Map.Reset_Iterator();
	for (CellClass * cellptr = Map.Iterate(); cellptr != NULL; cellptr = Map.Iterate()) {
		if (cellptr->Tiberium_Value() <= 0) continue;
		Cell const cell = cellptr->As_Coord().As_Cell();
		int const dx = cell.X - origin.X;
		int const dy = cell.Y - origin.Y;
		int const distance = dx * dx + dy * dy;
		if (count == limit && distance >= distances[count - 1]) continue;
		int slot = count < limit ? count++ : count - 1;
		while (slot > 0 && distances[slot - 1] > distance) {
			found[slot] = found[slot - 1];
			distances[slot] = distances[slot - 1];
			slot--;
		}
		found[slot] = cell;
		distances[slot] = distance;
	}
	DebugString("AUTOTEST   orecells from %d,%d: %d listed\n", origin.X, origin.Y, count);
	for (int index = 0; index < count; index++) {
		DebugString("AUTOTEST   orecell %d,%d distance %d\n", found[index].X, found[index].Y, distances[index]);
	}
}


/*
**	treehit <Warhead> x y: hits the terrain object on that cell with 100 points of damage from the
**	warhead, with no firer. trees: writes every terrain object on the map with its cell, strength,
**	armor and whether it is immune.
*/
static void Terrain_Step(StepType const & step)
{
	if (step.Command == "treehit") {
		WarheadTypeClass const * warhead = WarheadTypeClass::From_Name(step.Argument.c_str());
		TerrainClass * terrain = Map[Cell(step.X, step.Y)].Cell_Terrain();
		if (terrain == NULL || warhead == NULL) {
			DebugString("AUTOTEST treehit %s %d,%d: %s\n", step.Argument.c_str(), step.X, step.Y, terrain == NULL ? "no terrain object" : "no such warhead");
			return;
		}
		int const before = terrain->Strength;
		int damage = 100;
		ResultType const result = terrain->Take_Damage(damage, 0, warhead, NULL, false);
		DebugString("AUTOTEST treehit %s at %d,%d: strength %d -> %d damage %d result %d removed %d\n", terrain->Class->Name(), step.X, step.Y, before, (int)terrain->Strength, damage, (int)result, (int)(Map[Cell(step.X, step.Y)].Cell_Terrain() == NULL));
	} else if (step.Command == "terrainonore") {
		TerrainType const type = TerrainTypeClass::From_Name(step.Argument.c_str());
		if (type == TERRAIN_NONE) {
			DebugString("AUTOTEST terrainonore %s: no such terrain type\n", step.Argument.c_str());
			return;
		}
		Map.Reset_Iterator();
		for (CellClass * cell = Map.Iterate(); cell != NULL; cell = Map.Iterate()) {
			if (cell->Tiberium_Value() > 0 && cell->Cell_Terrain() == NULL && Map.In_Radar(cell->CellID)) {
				int const before = cell->Overlay;
				// Placed as the scenario reader does, which skips the runtime placement tests.
				ScenarioInit++;
				new TerrainClass(TerrainTypes[type], cell->CellID);
				ScenarioInit--;
				DebugString("AUTOTEST terrainonore %s at %d,%d: overlay %d -> %d, terrain %d\n", step.Argument.c_str(), cell->CellID.X, cell->CellID.Y, before, (int)cell->Overlay, cell->Cell_Terrain() != NULL ? 1 : 0);
				return;
			}
		}
		DebugString("AUTOTEST terrainonore %s: no ore cell\n", step.Argument.c_str());
	} else {
		for (int index = 0; index < Terrains.Count(); index++) {
			TerrainClass * terrain = Terrains[index];
			DebugString("AUTOTEST tree %s at %d,%d strength %d armor %d immune %d overlay %d\n", terrain->Class->Name(), terrain->Get_Cell().X, terrain->Get_Cell().Y, terrain->Strength, (int)terrain->Class->Armor, (int)terrain->Class->IsImmune, (int)Map[terrain->Get_Cell()].Overlay);
		}
	}
}


/*
**	trigaction <number> <value> <waypoint>: carries out that trigger action for the player's house, as a
**	trigger would. The value fills the action's number, house or time field.
*/
static void Trigger_Action_Step(StepType const & step)
{
	TActionClass * action = new TActionClass;
	action->Action = (TActionType)std::atoi(step.Argument.c_str());
	action->Data.Value = step.X;
	action->EffectLocation = step.Y;

	// "<number>@<House>" runs the action for that house, with underscores for spaces, instead of the player's.
	HouseClass * house = PlayerPtr;
	std::size_t const at = step.Argument.find('@');
	if (at != std::string::npos) {
		std::string name = step.Argument.substr(at + 1);
		std::replace(name.begin(), name.end(), '_', ' ');
		house = House_From_Name(name.c_str());
	}
	bool const done = house != NULL && (*action)(house, NULL, NULL, Cell(0, 0));
	Point2D const view = TacticalMap->Get_Tactical_Position();
	DebugString("AUTOTEST   trigaction %d value %d waypoint %d: %s, view %d,%d, scenario tint %d,%d,%d, first tile tint %d,%d,%d, waypoint cell %d,%d, lightning storm active %d pending %d\n", (int)action->Action, step.X, step.Y, done ? "done" : "failed", view.X, view.Y,
		Scen->RedTint, Scen->GreenTint, Scen->BlueTint,
		TileDrawers.Count() > 0 ? TileDrawers[0]->NormalRedTint : -1, TileDrawers.Count() > 0 ? TileDrawers[0]->NormalGreenTint : -1, TileDrawers.Count() > 0 ? TileDrawers[0]->NormalBlueTint : -1,
		Scen->Is_Valid_Waypoint(step.Y) ? Scen->Get_Waypoint_Cell(step.Y).X : -1, Scen->Is_Valid_Waypoint(step.Y) ? Scen->Get_Waypoint_Cell(step.Y).Y : -1,
		(int)LightningStormClass::Is_Active(), (int)(LightningStormClass::Is_Active_Or_Pending() && !LightningStormClass::Is_Active()));
	delete action;
}


// How many times the trigger type has sprung this run, or 0 for NULL or a type that has not sprung.
int Trigger_Springs(TriggerTypeClass const * type)
{
	std::map<TriggerTypeClass const *, int>::const_iterator const found = TriggerSprings.find(type);
	return(found != TriggerSprings.end() ? found->second : 0);
}


/*
**	triggerlog <0|1>: writes a line each time a trigger springs. triggercoverage: writes each trigger
**	type with how many times it has sprung this run, then the types that have not sprung at all.
*/
static void Trigger_Step(StepType const & step)
{
	if (step.Command == "triggerlog") {
		TriggerLog = std::atoi(step.Argument.c_str()) != 0;
		return;
	}
	std::vector<std::string> never;
	for (int index = 0; index < TriggerTypes.Count(); index++) {
		TriggerTypeClass const * type = TriggerTypes[index];
		int const fired = Trigger_Springs(type);
		DebugString("AUTOTEST trigger coverage %s '%s' house %s fired %d\n", (char const *)type->IniName, (char const *)type->GivenName, type->House != NULL ? type->House->Class->Name() : "NOHOUSE", fired);
		if (fired == 0) {
			never.push_back(std::string((char const *)type->IniName) + " '" + (char const *)type->GivenName + "'");
		}
	}

	// A message holds 4096 bytes at most, so a long list goes out as several lines of about 3000 bytes.
	std::string chunk;
	bool first = true;
	auto flush = [&]() {
		if (first) {
			DebugString("AUTOTEST trigger never fired (%d of %d): %s\n", (int)never.size(), TriggerTypes.Count(), chunk.c_str());
		} else {
			DebugString("AUTOTEST trigger never fired (continued): %s\n", chunk.c_str());
		}
		first = false;
		chunk.clear();
	};
	for (std::string const & entry : never) {
		if (!chunk.empty() && chunk.size() + entry.size() + 2 > 3000) {
			flush();
		}
		if (!chunk.empty()) {
			chunk += ", ";
		}
		chunk += entry;
	}
	if (never.empty()) {
		chunk = "none";
	}
	flush();
}


/*
**	Wait steps: waitcell, waittrigger, waitowns, waitlocal and waitgone pause the script until their condition
**	holds, or until their timeout passes. Their words are the line's frame and command, then the
**	arguments: see the wait functions below for each one's word positions.
*/
bool Is_Wait_Command(std::string const & command)
{
	return(command == "waitcell" || command == "waittrigger" || command == "waitowns" || command == "waitlocal" || command == "waitgone");
}


// The whitespace-separated words of a script line: the frame, the command, then the arguments.
std::vector<std::string> Line_Words(std::string const & line)
{
	std::istringstream stream(line);
	std::vector<std::string> words;
	std::string word;
	while (stream >> word) {
		words.push_back(word);
	}
	return(words);
}


// Word <index> of a script line, or an empty string when the line has no such word.
std::string Word(std::vector<std::string> const & words, std::size_t index)
{
	return(index < words.size() ? words[index] : std::string());
}


// Word <index> as a number, or fallback when the line has no such word.
int Number(std::vector<std::string> const & words, std::size_t index, int fallback)
{
	return(index < words.size() ? std::atoi(words[index].c_str()) : fallback);
}


// The fewest words a wait line needs. waitcell and waitgone are: frame, command, TypeID, x, y, then radius and timeout.
// waittrigger is: frame, command, trigger, timeout. waitowns is: frame, command, house, TypeID, count,
// timeout. waitlocal is: frame, command, index, value, timeout.
std::size_t Wait_Min_Words(std::string const & command)
{
	if (command == "waitcell" || command == "waitgone" || command == "waitowns") {
		return(5);
	}
	if (command == "waittrigger") {
		return(3);
	}
	return(4);
}


// The word that holds a wait's timeout, in the layout Wait_Min_Words describes.
std::size_t Wait_Timeout_Word(std::string const & command)
{
	if (command == "waitcell" || command == "waitgone") {
		return(6);
	}
	if (command == "waittrigger") {
		return(3);
	}
	if (command == "waitowns") {
		return(5);
	}
	return(4);
}


// The house a wait names: "player" for the player, otherwise a house name with an underscore for each space.
HouseClass * Wait_House(std::string name)
{
	if (stricmp(name.c_str(), "player") == 0) {
		return(PlayerPtr);
	}
	std::replace(name.begin(), name.end(), '_', ' ');
	return(House_From_Name(name.c_str()));
}


// How many objects of the type the house has on the map, not counting those in limbo.
template<typename T>
int Placed_Count_In(DynamicVectorClass<T *> & list, HouseClass const * house, TechnoTypeClass const * type)
{
	int count = 0;
	for (int index = 0; index < list.Count(); index++) {
		T const * object = list[index];
		if (object != NULL && !object->IsInLimbo && object->House == house && object->TClass == type) {
			count++;
		}
	}
	return(count);
}


// How many of the type the house owns and has placed. A structure still in production is in the
// house's quantity but not on the map, so it is not counted.
int Placed_Count(HouseClass const * house, TechnoTypeClass const * type)
{
	return(Placed_Count_In(Buildings, house, type) + Placed_Count_In(Units, house, type)
		+ Placed_Count_In(Infantry, house, type) + Placed_Count_In(Aircraft, house, type));
}


// Why a wait line cannot run as written, or an empty string when it can.
std::string Wait_Problem(std::vector<std::string> const & words)
{
	std::string const command = Word(words, 1);
	if (words.size() < Wait_Min_Words(command)) {
		return("too few arguments");
	}
	if ((command == "waitcell" || command == "waitgone") && Find_Type(Word(words, 2)) == NULL) {
		return("no such type " + Word(words, 2));
	}
	if (command == "waittrigger" && TriggerTypeClass::From_Name(Word(words, 2).c_str()) == NULL) {
		return("no such trigger " + Word(words, 2));
	}
	if (command == "waitowns") {
		if (Wait_House(Word(words, 2)) == NULL) {
			return("no such house " + Word(words, 2));
		}
		if (Find_Type(Word(words, 3)) == NULL) {
			return("no such type " + Word(words, 3));
		}
	}
	if (command == "waitlocal") {
		bool value = false;
		if (!Scen->Fetch_Local_Value(Number(words, 2, -1), value)) {
			return("no such local variable " + Word(words, 2));
		}
	}
	return("");
}


// Whether a wait's condition holds now. A line that has a problem never holds.
bool Wait_Met(std::vector<std::string> const & words)
{
	std::string const command = Word(words, 1);
	if (words.size() < Wait_Min_Words(command)) {
		return(false);
	}
	if (command == "waitcell") {
		Cell const cell(Number(words, 3, 0), Number(words, 4, 0));
		int const radius = Number(words, 5, 2);
		std::string const name = Word(words, 2);
		return(Player_Object_Within(Units, name, cell, radius) || Player_Object_Within(Infantry, name, cell, radius)
			|| Player_Object_Within(Buildings, name, cell, radius) || Player_Object_Within(Aircraft, name, cell, radius));
	}
	if (command == "waitgone") {
		return(Strike_Target(Word(words, 2), Number(words, 3, 0), Number(words, 4, 0), Number(words, 5, 3)) == NULL);
	}
	if (command == "waittrigger") {
		return(Trigger_Springs(TriggerTypeClass::From_Name(Word(words, 2).c_str())) > 0);
	}
	if (command == "waitowns") {
		HouseClass * house = Wait_House(Word(words, 2));
		TechnoTypeClass const * type = Find_Type(Word(words, 3));
		return(house != NULL && type != NULL && Placed_Count(house, type) >= Number(words, 4, 0));
	}
	if (command == "waitlocal") {
		bool value = false;
		bool const legal = Scen->Fetch_Local_Value(Number(words, 2, -1), value);
		return(legal && value == (Number(words, 3, 0) != 0));
	}
	return(false);
}


/*
**	Ends the wait at the front of the script once its condition holds or its timeout passes. The
**	steps after it move by the frames the wait took, so they keep their spacing from the wait line.
**	Returns false while the wait goes on.
*/
bool Wait_Over(StepType const & step)
{
	std::vector<std::string> const words = Line_Words(step.Line);
	int const timeout = Number(words, Wait_Timeout_Word(step.Command), Wait_Default_Timeout);
	int const start = step.Frame + WaitShift;
	bool const met = Wait_Met(words);
	if (!met && Frame < start + timeout) {
		return(false);
	}
	int const end = met ? Frame : start + timeout;
	WaitShift = end - step.Frame;
	if (met) {
		DebugString("AUTOTEST wait done at frame %d: %s\n", end, step.Line.c_str());
	} else {
		DebugString("AUTOTEST wait timed out: %s\n", step.Line.c_str());
	}
	return(true);
}


// Adds each word of the text that holds a letter, in upper case, as a file name the check may read.
static void Add_Stock_Names(std::set<std::string> & names, char const * text)
{
	std::string word;
	bool letter = false;
	for (char const * at = text; ; at++) {
		unsigned char const c = static_cast<unsigned char>(*at);
		if (std::isalnum(c) || c == '_') {
			word += static_cast<char>(std::toupper(c));
			letter = letter || std::isalpha(c);
		} else {
			if (letter && word.size() <= 16) {
				names.insert(word);
			}
			word.clear();
			letter = false;
		}
		if (c == '\0') {
			break;
		}
	}
}


/*
**	stockcheck: reads every voxel, animation, shape and picture that the INI files name, through the loaders
**	the game uses, and logs each one a loader refuses with the loader's reason. A MIX keeps only the CRC of
**	each name, so the names are the words of the INI files' sections and values, which is how the art and
**	rules name their images. The summary counts the files those names reach and the ones refused, and lists
**	the MIX archives the game attached with their entry counts.
*/
static void Stock_Check_Step(void)
{
	static char const * const sources[] = {
		"ART.INI", "ARTMD.INI", "RULES.INI", "RULESMD.INI", "UI.INI", "UIMD.INI", "MISSION.INI", "MISSIONMD.INI",
		"THEME.INI", "THEMEMD.INI", "BATTLE.INI", "BATTLEMD.INI", "AI.INI", "AIMD.INI", "SOUND.INI", "SOUNDMD.INI",
		"MULTI.INI", "MULTIMD.INI", "EVA.INI", "EVAMD.INI", "SCORE.INI", "SCOREMD.INI", "CAMEO.INI", "CAMEOMD.INI"};

	std::set<std::string> names;
	// Picture names the engine's own code opens by name, such as the loading screens and the side pictures.
	static char const * const coded[] = {"ARBI", "DJBI", "FRAI", "GBRI", "GERI", "JAPI", "LATI", "OBSI", "RUSI", "USAI", "YRII",
		"LOADING", "TITLE", "LOAD400A", "LOAD400B", "LOAD400C", "LOAD400D", "LOAD480A", "LOAD480B", "LOAD480C", "LOAD480D",
		"LOAD600A", "LOAD600B", "LOAD600C", "LOAD600D", "SBACK", "SBACKH", "02D", "NODI", "GDII"};
	for (char const * text : coded) {
		Add_Stock_Names(names, text);
	}
	int sections = 0;
	int sources_read = 0;
	for (char const * source : sources) {
		CCFileClass file(source);
		CCINIClass ini;
		if (!file.Is_Available() || !ini.Load(file, false)) {
			continue;
		}
		sources_read++;

		for (int index = 0; index < ini.Section_Count(); index++) {
			char const * section = ini.Section_Name(index);
			if (section == NULL) {
				continue;
			}
			sections++;
			Add_Stock_Names(names, section);

			for (int entry = 0; entry < ini.Entry_Count(section); entry++) {
				char value[512] = "";
				ini.Get_String(section, ini.Get_Entry(section, entry), "", value, sizeof(value));
				Add_Stock_Names(names, value);
			}
		}
	}

	int found_voxels = 0;
	int found_anims = 0;
	int found_shapes = 0;
	int found_pictures = 0;
	int pairs = 0;
	int refused_voxels = 0;
	int refused_anims = 0;
	int refused_shapes = 0;
	int refused_pictures = 0;
	int frames = 0;
	int empty_shapes = 0;

	for (std::string const & name : names) {
		std::string const vxl = name + ".VXL";
		std::string const hva = name + ".HVA";
		std::string const shp = name + ".SHP";
		std::string const pcx = name + ".PCX";

		bool const has_vxl = CCFileClass(vxl.c_str()).Is_Available();
		bool const has_hva = CCFileClass(hva.c_str()).Is_Available();
		if (has_vxl) {
			CCFileClass file(vxl.c_str());
			VoxelLibrary library;
			found_voxels++;
			if (!library.Read_File(file, false)) {
				refused_voxels++;
				DebugString("AUTOTEST   stockcheck refused %s\n", vxl.c_str());
			}
		}
		if (has_hva) {
			CCFileClass file(hva.c_str());
			MotionLibrary motion;
			found_anims++;
			if (!motion.Read_File(file)) {
				refused_anims++;
				DebugString("AUTOTEST   stockcheck refused %s\n", hva.c_str());
			} else if (motion.Get_Frame_Count() > 1) {
				DebugString("AUTOTEST   stockcheck multiframe %s frames %d\n", hva.c_str(), motion.Get_Frame_Count());
			}
		}
		if (has_vxl && has_hva) {
			pairs++;
		}

		if (CCFileClass(shp.c_str()).Is_Available()) {
			std::vector<std::uint8_t> bytes;
			CCFileClass file(shp.c_str());
			if (file.Open(FileClass::READ)) {
				int const size = file.Size();
				if (size > 0) {
					bytes.resize(static_cast<std::size_t>(size));
					if (file.Read(bytes.data(), size) != size) {
						bytes.clear();
					}
				}
				file.Close();
			}

			found_shapes++;
			int const count = bytes.size() >= 8 ? (bytes[6] | (bytes[7] << 8)) : 0;
			bool loaded = bytes.size() >= 8;
			if (loaded && count == 0) {
				empty_shapes++;
			}
			int failed = -1;
			for (int frame = 0; loaded && frame < count; frame++) {
				UIImageIndexed image;
				loaded = UI_Decode_SHP(std::span<std::uint8_t const>(bytes.data(), bytes.size()), frame, image);
				frames += loaded ? 1 : 0;
				if (!loaded) {
					failed = frame;
				}
			}
			if (!loaded) {
				refused_shapes++;
				if (bytes.size() >= 8 && failed >= 0) {
					std::size_t const at = 8 + 24 * static_cast<std::size_t>(failed);
					if (at + 24 <= bytes.size()) {
						auto const word = [&bytes](std::size_t where) {return(bytes[where] | (bytes[where + 1] << 8));};
						DebugString("AUTOTEST   stockcheck refused %s: frame %d of %d, canvas %dx%d, frame box %d,%d %dx%d, kind %d\n",
							shp.c_str(), failed, count, word(2), word(4), word(at), word(at + 2), word(at + 4), word(at + 6), bytes[at + 8]);
					} else {
						DebugString("AUTOTEST   stockcheck refused %s: %d bytes, too short for its %d frames\n", shp.c_str(), (int)bytes.size(), count);
					}
				} else {
					DebugString("AUTOTEST   stockcheck refused %s: %d bytes, too short for a header\n", shp.c_str(), (int)bytes.size());
				}
			}
		}

		if (CCFileClass(pcx.c_str()).Is_Available()) {
			CCFileClass file(pcx.c_str());
			std::unique_ptr<Surface> picture(Read_PCX_File(file));
			found_pictures++;
			if (picture == NULL) {
				refused_pictures++;
				DebugString("AUTOTEST   stockcheck refused %s\n", pcx.c_str());
			}
		}
	}

	int mixes = 0;
	int entries = 0;
	for (MixFileClass * mix = MixFileClass::First_Attached(); mix != NULL; mix = mix->Next_Attached()) {
		mixes++;
		entries += mix->Member_Count();
		DebugString("AUTOTEST   stockcheck mix %s: %d entries\n", mix->Filename, mix->Member_Count());
	}

	// WDT.MIX is mounted only by the world domination setup, when it is on hand. The check mounts it the same way.
	if (CCFileClass("WDT.MIX").Is_Available()) {
		MixFileClass * wdt = new MixFileClass("WDT.MIX", &FastKey);
		mixes++;
		entries += wdt->Member_Count();
		DebugString("AUTOTEST   stockcheck mix WDT.MIX (mounted by the world domination setup): %d entries\n", wdt->Member_Count());
		delete wdt;
	}

	DebugString("AUTOTEST stockcheck: %d INI files, %d sections, %d names; found VXL %d, HVA %d, SHP %d, PCX %d; %d VXL and HVA pairs; refused VXL %d, HVA %d, SHP %d, PCX %d; %d SHP frames decoded, %d SHP empty; %d mixes attached with %d entries\n",
		sources_read, sections, (int)names.size(), found_voxels, found_anims, found_shapes, found_pictures, pairs, refused_voxels, refused_anims, refused_shapes, refused_pictures, frames, empty_shapes, mixes, entries);
}


/*
**	previewtime: loads the multiplayer list the game builds from its packet files, then reads the preview of
**	each map on it that is on hand, three times, and logs how long each step took.
*/
static void Preview_Time_Step(void)
{
	std::chrono::steady_clock::time_point const listed = std::chrono::steady_clock::now();
	Session.Read_Scenario_Descriptions();
	std::chrono::duration<double, std::milli> const list_time = std::chrono::steady_clock::now() - listed;
	DebugString("AUTOTEST   previewtime list: %d scenarios in %.1f ms\n", Session.Scenarios.Count(), list_time.count());

	for (int pass = 1; pass <= 3; pass++) {
		std::chrono::steady_clock::time_point const start = std::chrono::steady_clock::now();
		int on_hand = 0;
		int previews = 0;
		for (int index = 0; index < Session.Scenarios.Count(); index++) {
			char const * filename = Session.Scenarios[index]->Get_Filename();
			if (!CCFileClass(filename).Is_Available()) {
				continue;
			}
			on_hand++;
			MapPreviewClass preview;
			if (preview.Read_INI_Preview(filename)) {
				previews++;
			}
		}
		std::chrono::duration<double, std::milli> const elapsed = std::chrono::steady_clock::now() - start;
		DebugString("AUTOTEST   previewtime pass %d: %d maps on hand, %d with a preview, %.1f ms\n", pass, on_hand, previews, elapsed.count());
	}
}


// modsig [text]: the mods in force as a join request carries them. With text, which is another machine's
// signature, also says whether a host with these mods would take that machine in.
static void Mod_Signature_Step(StepType const & step)
{
	ModSync::Signature const mine = ModSync::Describe(Active_Mods());
	DebugString("AUTOTEST   modsig [%s] names %s\n", ModSync::Encode(mine).c_str(), ModSync::List_Names(mine).c_str());
	if (!step.Argument.empty()) {
		ModSync::Signature theirs;
		bool const valid = ModSync::Decode(step.Argument, theirs);
		ModSync::Difference const difference = ModSync::Compare(mine, theirs);
		DebugString("AUTOTEST   modsig join %s valid %d: %s\n", difference.Same() ? "accepted" : "refused", (int)valid,
			difference.Same() ? "same mods" : ModSync::Describe_Difference(difference, mine, theirs, "The joiner").c_str());
	}
}


// key <Key>: the virtual-key code of a single letter or digit, or the decimal code given.
// The first word of a key step's argument: a letter, or a virtual-key code in decimal.
static int Key_Code(std::string const & name)
{
	std::string const word = name.substr(0, name.find_first_of(" \t"));
	if (word.size() == 1) {
		return(std::toupper((unsigned char)word[0]));
	}
	return(std::atoi(word.c_str()));
}


// The modifier words after the key of a key step, such as "ctrl" in "49 ctrl", as the window's modifier bits.
static int Key_Modifiers(std::string const & argument)
{
	int modifiers = 0;
	std::size_t const first = argument.find_first_of(" \t");
	std::string const rest = (first == std::string::npos) ? std::string() : argument.substr(first);
	if (rest.find("ctrl") != std::string::npos) modifiers |= WINDOW_MOD_CTRL;
	if (rest.find("shift") != std::string::npos) modifiers |= WINDOW_MOD_SHIFT;
	if (rest.find("alt") != std::string::npos) modifiers |= WINDOW_MOD_ALT;
	return(modifiers);
}


// The trigger word after the key of a binding step, such as "Double"; Press when none is given.
static HotkeyTrigger Binding_Trigger(std::string const & argument)
{
	HotkeyTrigger trigger = HOTKEY_PRESS;
	std::size_t const first = argument.find_first_of(" \t");
	std::string const rest = (first == std::string::npos) ? std::string() : argument.substr(first);
	char const * const names[] = { "Double", "Hold", "Held" };
	HotkeyTrigger const found[] = { HOTKEY_DOUBLE, HOTKEY_HOLD, HOTKEY_HELD };
	for (int index = 0; index < 3; index++) {
		if (rest.find(names[index]) != std::string::npos) {
			trigger = found[index];
		}
	}
	return(trigger);
}


// tab: writes the sidebar tab shown, the items each tab lists, the completed buildings the structure and defense
// tabs hold, and the object waiting to be placed.
static void Tab_Step(void)
{
	BuildingClass * structure = Map.Completed_Building(0);
	BuildingClass * defense = Map.Completed_Building(1);
	DebugString("AUTOTEST tab shown %d items %d %d %d %d completed %s %s pending %s\n", Map.ActiveTab,
		Map.Column[0].BuildableCount, Map.Column[1].BuildableCount, Map.Column[2].BuildableCount, Map.Column[3].BuildableCount,
		structure != NULL ? structure->Class->Name() : "none", defense != NULL ? defense->Class->Name() : "none",
		Map.PendingObject != NULL ? Map.PendingObject->Name() : "none");
}


// Writes the type when its Prerequisite list names the structure with heap ID id, for the prereqs step.
static void Note_If_Gated(TechnoTypeClass const * other, int id, char const * target)
{
	for (int pre = 0; pre < other->Prerequisite.Count(); pre++) {
		if (other->Prerequisite[pre] == id) {
			DebugString("AUTOTEST   gated %s by %s\n", other->Name(), target);
		}
	}
}


// Writes which special cursor mode the player is in, so a script can check that one mode ended when another began.
static void Modes_Step(void)
{
	DebugString("AUTOTEST modes repair %d sell %d power %d waypoint %d beacon %d targeting %d pending %s\n",
		(int)Map.IsRepairMode, (int)Map.IsSellMode, (int)Map.IsPowerMode, (int)Map.IsWaypointMode,
		(int)Map.IsPlaceBeaconMode, Map.IsTargettingMode != SUPER_NONE ? 1 : 0,
		Map.PendingObject != NULL ? Map.PendingObject->Name() : "none");
}


// Writes the cell the placement cursor sits on, how many cells of its footprint are marked, and where the pointer is.
static void Placement_Step(void)
{
	Point2D const mouse = MouseCursor->Get_Mouse_Point();
	int marked = 0;
	if (Map.CursorSize != NULL && Map.ZoneCell != CELL_NONE) {
		for (Cell const * ptr = Map.CursorSize; *ptr != REFRESH_EOL; ptr++) {
			Cell const cell = Cell(Map.ZoneCell + Map.ZoneOffset + *ptr);
			if (Map.In_Radar(cell) && Map[cell].IsCursorHere) {
				marked++;
			}
		}
	}
	DebugString("AUTOTEST placement pending %s zone %d,%d marked %d mouse %d,%d in_tactical %d over_map %d\n",
		Map.PendingObject != NULL ? Map.PendingObject->Name() : "none", Map.ZoneCell.X, Map.ZoneCell.Y, marked,
		mouse.X, mouse.Y, (int)TacticalRect.Is_Point_Within(mouse), (int)Point_Over_Map(mouse));
}


// The techno on a cell for the probes: the ground occupier if there is one, else a flying object over the cell.
static TechnoClass * Techno_Over_Cell(Cell const & cell)
{
	TechnoClass * target = Map[cell].Cell_Techno();
	for (int index = 0; target == NULL && index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->IsDown && !techno->IsInLimbo && techno->Get_Cell().X == cell.X && techno->Get_Cell().Y == cell.Y) {
			target = techno;
		}
	}
	return(target);
}

// spawns <TypeID>: each object of the type with its cell, body and desired facing in 256 steps, whether its body is rotating and its
// locomotor moving, its locomotor destination and its TarCom, then its spawn cycle's current and queued targets, status and next spawn
// frame, and the status of each spawn.
static void Spawn_Probe_Step(StepType const & step)
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass const * techno = Technos[index];
		if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
		Cell const cell = techno->Get_Cell();
		bool const foot = techno->Is_Foot();
		FootClass const * vehicle = foot ? static_cast<FootClass const *>(techno) : NULL;
		Coord const dest = vehicle != NULL ? vehicle->Locomotion->Destination() : Coord(-1, -1, -1);
		Cell const tar = techno->TarCom != NULL ? techno->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   spawns %s of %s frame %d cell %d,%d body %d desired %d rotating %d moving %d moving_now %d dest %d,%d tarcom %d,%d mission %s\n",
			techno->TClass->Name(), techno->House->Class->Name(), Frame, cell.X, cell.Y, (int)techno->PrimaryFacing.Current().As_Dir256(), (int)techno->PrimaryFacing.Desired().As_Dir256(),
			(int)techno->PrimaryFacing.Is_Rotating(), vehicle != NULL ? (int)vehicle->Locomotion->Is_Moving() : -1, vehicle != NULL ? (int)vehicle->Locomotion->Is_Moving_Now() : -1,
			dest.X, dest.Y, tar.X, tar.Y, MissionClass::Mission_Name(techno->Get_Mission()));
		if (!techno->SpawnManager) continue;
		SpawnManagerClass const & spawns = *techno->SpawnManager;
		AbstractClass const * target = spawns.Current_Target();
		AbstractClass const * queued = spawns.Queued_Target();
		Cell const aim = target != NULL ? target->Center_Coord().As_Cell() : Cell(-1, -1);
		Cell const next = queued != NULL ? queued->Center_Coord().As_Cell() : Cell(-1, -1);
		DebugString("AUTOTEST   spawncycle %s frame %d target %d,%d queued %d,%d status %d next_spawn %d nodes", techno->TClass->Name(), Frame, aim.X, aim.Y, next.X, next.Y, spawns.Cycle_Status(), spawns.Next_Spawn_Frame());
		for (int node = 0; node < spawns.Node_Count(); node++) DebugString(" %d", spawns.Node_Status(node));
		DebugStringNoPrefix("\n");
	}
}


// typedump [Name]: the main fields of every weapon, projectile and warhead type, or only those with that name, one line each.
static void Type_Dump_Step(StepType const & step)
{
	auto wanted = [&](char const * name) { return step.Argument.empty() || stricmp(name, step.Argument.c_str()) == 0; };
	for (int index = 0; index < Weapons.Count(); index++) {
		WeaponTypeClass const * weapon = Weapons[index];
		if (!wanted(weapon->Name())) continue;
		DebugString("AUTOTEST   typedump weapon %s damage %d rof %d range %d speed %d burst %d projectile %s warhead %s\n", weapon->Name(), (int)weapon->Attack, (int)weapon->ROF,
			(int)weapon->Range, (int)weapon->MaxSpeed, weapon->Burst, weapon->Bullet != NULL ? weapon->Bullet->Name() : "(none)", weapon->WarheadPtr != NULL ? weapon->WarheadPtr->Name() : "(none)");
	}
	for (int index = 0; index < BulletTypes.Count(); index++) {
		BulletTypeClass const * bullet = BulletTypes[index];
		if (!wanted(bullet->Name())) continue;
		DebugString("AUTOTEST   typedump projectile %s rot %d high %d inaccurate %d floater %d aa %d ag %d\n", bullet->Name(), (int)bullet->ROT, (int)bullet->IsHigh,
			(int)bullet->IsInaccurate, (int)bullet->IsFloater, (int)bullet->IsAntiAircraft, (int)bullet->IsAntiGround);
	}
	for (int index = 0; index < ::Warheads.Count(); index++) {
		WarheadTypeClass const * warhead = ::Warheads[index];
		if (!wanted(warhead->Name())) continue;
		DebugString("AUTOTEST   typedump warhead %s spread %.3f spreadfactor %d walls %d rad %d verses", warhead->Name(), (double)warhead->CellSpread, (int)warhead->SpreadFactor,
			(int)warhead->IsWallDestroyer, (int)warhead->IsRadiation);
		for (int armor = 0; armor < (int)ARMOR_COUNT; armor++) DebugStringNoPrefix(" %.3f", warhead->Versus((ArmorType)armor));
		DebugStringNoPrefix("\n");
	}
}


static void Air_Probe_Step(StepType const & step)
{
	if (step.Command == "weapons") {
		// weapons <TypeID>: each weapon slot of the type with its bullet's AA and AG flags and its range in leptons.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		for (int slot = 0; type != NULL && slot < TechnoTypeClass::WEAPON_SLOT_COUNT; slot++) {
			WeaponTypeClass const * weapon = type->Weapons[slot].Weapon;
			if (weapon != NULL) {
				DebugString("AUTOTEST   weapon %s slot %d %s AA %d AG %d range %d\n", type->Name(), slot, weapon->Name(), (int)weapon->Bullet->IsAntiAircraft, (int)weapon->Bullet->IsAntiGround, (int)weapon->Range);
			}
		}
	} else if (step.Command == "airborne") {
		// airborne <TypeID>: each object of the type with its height above ground, whether it is in the air, its render layer,
		// and whether the ground or bridge occupier chain of its cell holds it.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			Cell const cell = techno->Get_Cell();
			bool ground_chain = false;
			for (ObjectClass const * link = Map[cell].Cell_Occupier(false); link != NULL && !ground_chain; link = link->Next) ground_chain = link == techno;
			bool bridge_chain = false;
			for (ObjectClass const * link = Map[cell].Cell_Occupier(true); link != NULL && !bridge_chain; link = link->Next) bridge_chain = link == techno;
			// phase is the flight phase of a rocket locomotor (0 none, 1 pause, 2 tilt, 3 climb, 4 cruise, 5 dive), or -1 for another locomotor.
			FootClass const * foot = techno->Is_Foot() ? static_cast<FootClass const *>(techno) : NULL;
			RocketLocomotionClass const * rocket = foot != NULL ? dynamic_cast<RocketLocomotionClass const *>(foot->Locomotion.get()) : NULL;
			int const phase = rocket != NULL ? rocket->Flight_Phase() : -1;
			DebugString("AUTOTEST   airborne %s of %s cell %d,%d agl %d in_air %d layer %d obj_layer %d on_bridge %d down %d ground_chain %d bridge_chain %d phase %d\n",
				techno->TClass->Name(), techno->House->Class->Name(), cell.X, cell.Y, (int)techno->HeightAGL, (int)techno->In_Air(), (int)techno->In_Which_Layer(), (int)techno->Layer,
				(int)techno->IsOnBridge, (int)techno->IsDown, (int)ground_chain, (int)bridge_chain, phase);
		}
	} else if (step.Command == "rocket") {
		// rocket <TypeID>: each foot object of the type with its coordinate in leptons, its height above ground, its flat and
		// vertical distance to the destination its locomotor flies to, and whether that locomotor is moving.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0 || !techno->Is_Foot()) continue;
			FootClass const * foot = static_cast<FootClass const *>(techno);
			Coord const at = foot->Get_Coord();
			Coord const to = foot->Locomotion->Destination();
			DebugString("AUTOTEST   rocket %s of %s id %p frame %d at %d,%d,%d agl %d to %d,%d,%d flat %d above %d moving %d\n",
				foot->TClass->Name(), foot->House->Class->Name(), (void const *)foot, Frame, at.X, at.Y, at.Z, (int)foot->HeightAGL, to.X, to.Y, to.Z,
				(int)std::hypot(double(to.X - at.X), double(to.Y - at.Y)), (int)(at.Z - to.Z), (int)foot->Locomotion->Is_Moving());
		}
	}
}


// cursor <TypeID> x y: writes the action and the mouse shape (its MouseCursorType number) that hovering
// over the object on that cell shows for the player's first object of that type.
static void Cursor_Probe(std::string const & type_name, int x, int y)
{
	Cell const cell(x, y);
	ObjectClass * target = Map[cell].Cell_Occupier();
	for (int index = 0; target != NULL && index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), type_name.c_str()) == 0) {
			// The probed object is the only selection, so the action is the one its cursor shows.
			Unselect_All();
			techno->Select();
			ActionType const action = techno->What_Action(target, false);
			Map.Mouse_Left_Up(cell, false, target, action, false);
			DebugString("AUTOTEST   cursor %s on %s: action %s mouse %d\n", techno->TClass->Name(), target->Class_Of()->Name(), ActionName[action], (int)Map.Get_Mouse_Shape());
			Unselect_All();
			return;
		}
	}
}


// swaim <SuperSection> x y: aims that super weapon as its cameo click does, then writes the action the cell and the
// object under it give, and the mouse shape that action shows (Mouse_Left_Up). A cell under shroud is probed as shroud.
static void SW_Aim_Probe(std::string const & section, int x, int y)
{
	SuperWeaponType const id = SuperWeaponTypeClass::From_Name(section.c_str());
	if (id == SUPER_NONE) {
		DebugString("AUTOTEST   swaim %s: no such super weapon\n", section.c_str());
		return;
	}

	Cell const cell(x, y);
	ObjectClass * target = Map[cell].Cell_Occupier();
	bool const shadow = Map.Is_Shrouded(Map[cell].Center_Coord());
	Unselect_All();
	Map.IsTargettingMode = id;
	ActionType const action = SuperWeaponTypes[id]->What_Action(cell, target);
	Map.Mouse_Left_Up(cell, shadow, target, action, false);
	DebugString("AUTOTEST   swaim %s cell %d,%d shroud %d object %s action %s mouse %d range %d line %d\n", section.c_str(), x, y, (int)shadow, target != NULL ? target->Class_Of()->Name() : "none", ActionName[action], (int)Map.Get_Mouse_Shape(), (int)SuperWeaponTypes[id]->Range, SuperWeaponTypes[id]->LineMultiplier);
}


// canbuild <TypeID>: writes each house's own and acted country, the type's RequiredHouses and ForbiddenHouses,
// whether the country lists pass with the own country (gate own) and with the acted one (gate acted), and what
// Can_Build says for that house now.
static void Can_Build_Step(StepType const & step)
{
	TechnoTypeClass const * type = Find_Type(step.Argument);
	if (type == NULL) {
		return;
	}
	for (int index = 0; index < Houses.Count(); index++) {
		HouseClass * house = Houses[index];
		int const required = type->RequiredHouses;
		int const forbidden = type->ForbiddenHouses;
		int const own = house->Own_Mask();
		int const acted = house->Acted_Mask();
		bool const own_gate = (required == -1 || (required & own) != 0) && (forbidden == -1 || (forbidden & own) == 0);
		bool const acted_gate = (required == -1 || (required & acted) != 0) && (forbidden == -1 || (forbidden & acted) == 0);
		DebugString("AUTOTEST   canbuild %s house %s own %s acted %s required %08X forbidden %08X gate own %d acted %d level %d tech %d can %d\n",
			type->Name(), house->Class->Name(),
			house->OwnCountry != HOUSE_NONE ? HouseTypes[house->OwnCountry]->Name() : "<none>",
			house->ActLike != HOUSE_NONE ? HouseTypes[house->ActLike]->Name() : "<none>",
			required, forbidden, (int)own_gate, (int)acted_gate, type->Level, house->Control.TechLevel, house->Can_Build(type, false, false));
	}
}


/// <summary>
/// Logs the bridge state of one cell, for a test that checks whether a span came down.
/// </summary>
/// <param name="step">A step whose argument is the cell's X and whose X field is its Y.</param>
static void Bridge_Cell_Step(StepType const & step)
{
	CellClass const & c = Map[Cell(std::atoi(step.Argument.c_str()), step.X)];
	DebugString("AUTOTEST   bridge %d,%d tile %d sub %d height %d overlay %d od %d deck %d under %d was %d ew %d rail %d road %d zone %d zonebridge %d land %d\n",
		(int)c.CellID.X, (int)c.CellID.Y, (int)c.ITType, (int)c.SubTile, (int)c.Height, (int)c.Overlay, (int)c.OverlayData,
		(int)c.IsBridgeDeck, (int)c.IsUnderBridge, (int)c.WasUnderBridge, (int)c.IsBridgeEastWest, (int)c.Is_Tile_Train_Bridge(), (int)c.Is_Tile_Bridge(),
		Map.Get_Cell_Zone(c.CellID, MZONE_NORMAL, false), Map.Get_Cell_Zone(c.CellID, MZONE_NORMAL, true), (int)c.Land_Type());
}


/// <summary>
/// Logs each bridge deck overlay and each bridge terrain tile on the map with its bridge flags, then the counts.
/// </summary>
/// <param name="step">The step, which takes no argument.</param>
static void Bridge_Census_Step(StepType const & step)
{
	(void)step;
	int high = 0;
	int high_flagged = 0;
	int railway = 0;
	int railway_flagged = 0;
	int lowbridge = 0;
	int concrete = 0;

	Map.Reset_Iterator();
	for (CellClass * cptr = Map.Iterate(); cptr != NULL; cptr = Map.Iterate()) {
		if (cptr->Overlay >= OVERLAY_LOWBRIDGE_FIRST && cptr->Overlay <= OVERLAY_LOWBRIDGE_LAST) {
			lowbridge++;
		}
		if (cptr->Overlay >= OVERLAY_CONCRETE_BRIDGE_FIRST && cptr->Overlay <= OVERLAY_CONCRETE_BRIDGE_LAST) {
			concrete++;
			DebugString("AUTOTEST   bridgeconcrete %d,%d overlay %d\n", (int)cptr->CellID.X, (int)cptr->CellID.Y, (int)cptr->Overlay);
		}
		if (cptr->Is_Tile_Bridge() || cptr->Is_Tile_Train_Bridge()) {
			DebugString("AUTOTEST   bridgetile %d,%d tile %d sub %d height %d overlay %d deck %d under %d was %d ew %d train %d high %d\n",
				(int)cptr->CellID.X, (int)cptr->CellID.Y, (int)cptr->ITType, (int)cptr->SubTile, (int)cptr->Height, (int)cptr->Overlay,
				(int)cptr->IsBridgeDeck, (int)cptr->IsUnderBridge, (int)cptr->WasUnderBridge, (int)cptr->IsBridgeEastWest,
				(int)cptr->Is_Tile_Train_Bridge(), (int)cptr->Is_Tile_Bridge());
		}
		bool const is_high = cptr->Overlay == OVERLAY_BRIDGE1 || cptr->Overlay == OVERLAY_BRIDGE2;
		bool const is_railway = cptr->Overlay == OVERLAY_RAILWAY_BRIDGE1 || cptr->Overlay == OVERLAY_RAILWAY_BRIDGE2;
		if (!is_high && !is_railway) {
			continue;
		}
		bool const flagged = cptr->IsBridgeDeck && cptr->IsUnderBridge;
		if (is_high) {
			high++;
			high_flagged += flagged ? 1 : 0;
		} else {
			railway++;
			railway_flagged += flagged ? 1 : 0;
		}
		DebugString("AUTOTEST   bridgedeck %d,%d overlay %d od %d deck %d under %d was %d\n",
			(int)cptr->CellID.X, (int)cptr->CellID.Y, (int)cptr->Overlay, (int)cptr->OverlayData,
			(int)cptr->IsBridgeDeck, (int)cptr->IsUnderBridge, (int)cptr->WasUnderBridge);
	}
	DebugString("AUTOTEST bridgecensus high %d flagged %d railway %d flagged %d lowbridge %d concrete %d\n", high, high_flagged, railway, railway_flagged, lowbridge, concrete);
}


/// <summary>
/// Logs each overlay type the rules mark as a wall, with the wall overlays the scenario's OverlayPack names
/// and the ones the loaded map holds, the first cell of each, and whether the type has an image or a cell animation.
/// </summary>
/// <param name="step">The step, which takes no argument.</param>
static void Wall_Census_Step(StepType const & step)
{
	(void)step;
	int const types = OverlayTypes.Count();
	std::vector<int> raw(types, 0);
	std::vector<int> raw_outside(types, 0);
	std::vector<int> raw_first_x(types, -1);
	std::vector<int> raw_first_y(types, -1);
	std::vector<int> loaded(types, 0);
	std::vector<int> first_x(types, -1);
	std::vector<int> first_y(types, -1);
	int rejected = 0;

	CCINIClass ini;
	CCFileClass file(Scen->ScenarioName);
	if (file.Is_Available()) {
		ini.Load(file, false);
	}
	std::vector<unsigned char> pack(640 * 400 * 2);
	int const len = ini.Get_UUBlock("OverlayPack", pack.data(), (int)pack.size());
	if (len > 0) {
		BufferStraw bpipe(pack.data(), len);
		LCWStraw uncomp(LCWStraw::DECOMPRESS);
		uncomp.Get_From(&bpipe);
		for (int y = 0; y < MAP_CELL_H; y++) {
			for (int x = 0; x < MAP_CELL_W; x++) {
				unsigned char value = 0xFF;
				uncomp.Get(&value, sizeof(value));
				if (value == 0xFF) {
					continue;
				}
				if (value >= types) {
					rejected++;
					continue;
				}
				if (!OverlayTypes[value]->IsWall) {
					continue;
				}
				if (raw[value] == 0) {
					raw_first_x[value] = x;
					raw_first_y[value] = y;
				}
				raw[value]++;
				if (!Map.In_Radar(Cell(x, y))) {
					raw_outside[value]++;
				}
			}
		}
	}

	Map.Reset_Iterator();
	for (CellClass * cptr = Map.Iterate(); cptr != NULL; cptr = Map.Iterate()) {
		if (cptr->Overlay != OVERLAY_NONE && OverlayTypes[cptr->Overlay]->IsWall) {
			if (loaded[cptr->Overlay] == 0) {
				first_x[cptr->Overlay] = cptr->CellID.X;
				first_y[cptr->Overlay] = cptr->CellID.Y;
			}
			loaded[cptr->Overlay]++;
		}
	}

	int total_raw = 0;
	int total_loaded = 0;
	for (int index = 0; index < types; index++) {
		if (!OverlayTypes[index]->IsWall) {
			continue;
		}
		total_raw += raw[index];
		total_loaded += loaded[index];
		if (raw[index] == 0 && loaded[index] == 0) {
			continue;
		}
		DebugString("AUTOTEST   wallcensus %s index %d raw %d outside %d loaded %d raw_first %d,%d loaded_first %d,%d image %d cellanim %d\n",
			OverlayTypes[index]->Name(), index, raw[index], raw_outside[index], loaded[index],
			raw_first_x[index], raw_first_y[index], first_x[index], first_y[index],
			OverlayTypes[index]->Get_Image_Data() != NULL ? 1 : 0, OverlayTypes[index]->CellAnim != NULL ? 1 : 0);
	}
	DebugString("AUTOTEST wallcensus %s raw %d loaded %d rejected %d\n", Scen->ScenarioName, total_raw, total_loaded, rejected);
}


/// <summary>
/// Logs, for each foot the player owns of the type, whether it is standing on a bridge and the bridge state of its cell.
/// </summary>
/// <param name="step">A step whose argument is the type name.</param>
static void Bridge_Unit_Step(StepType const & step)
{
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass const * techno = Technos[index];
		if (!techno->Is_Foot() || techno->House != PlayerPtr || techno->IsInLimbo || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) {
			continue;
		}
		FootClass const * foot = static_cast<FootClass const *>(techno);
		Cell const cell = foot->Get_Cell();
		DebugString("AUTOTEST   bridgeunit %s cell %d,%d onbridge %d under %d deck %d height %d\n", foot->TClass->Name(), cell.X, cell.Y,
			(int)foot->IsOnBridge, (int)Map[cell].IsUnderBridge, (int)Map[cell].IsBridgeDeck, Map.Get_Height_GL(foot->Center_Coord()));
	}
}


/// <summary>
/// Logs the bridge tile, overlay and flags of every on-map cell within four cells of a cell.
/// </summary>
/// <param name="step">A step whose argument is the centre's X and whose X field is its Y.</param>
static void Bridge_Area_Step(StepType const & step)
{
	int const centre_x = std::atoi(step.Argument.c_str());
	for (int y = step.X - 4; y <= step.X + 4; y++) {
		for (int x = centre_x - 4; x <= centre_x + 4; x++) {
			if (!Map.Is_Valid(Cell(x, y))) {
				continue;
			}
			CellClass const & c = Map[Cell(x, y)];
			DebugString("AUTOTEST   bridgearea %d,%d tile %d overlay %d sub %d height %d deck %d under %d was %d ew %d rail %d road %d\n",
				x, y, (int)c.ITType, (int)c.Overlay, (int)c.SubTile, (int)c.Height,
				(int)c.IsBridgeDeck, (int)c.IsUnderBridge, (int)c.WasUnderBridge, (int)c.IsBridgeEastWest, (int)c.Is_Tile_Train_Bridge(), (int)c.Is_Tile_Bridge());
		}
	}
}


/// <summary>
/// Logs what MapClass::Can_Repair_Bridge reports for a cell.
/// </summary>
/// <param name="step">A step whose argument is the cell's X and whose X field is its Y.</param>
static void Repair_Bridge_Step(StepType const & step)
{
	int const x = std::atoi(step.Argument.c_str());
	DebugString("AUTOTEST   repairbridge %d,%d %d\n", x, step.X, (int)Map.Can_Repair_Bridge(Cell(x, step.X)));
}


/// <summary>
/// Hits the bridge on a cell through MapClass::Damage_Bridge, the call a weapon impact makes, and logs whether a span came down.
/// </summary>
/// <param name="step">The x in the argument and the y in X.</param>
static void Bridge_Hit_Step(StepType const & step)
{
	int const x = std::atoi(step.Argument.c_str());
	bool const destroyed = Map.Damage_Bridge(Cell(x, step.X));
	DebugString("AUTOTEST   bridgehit %d,%d %d\n", x, step.X, (int)destroyed);
}


/// <summary>
/// Brings down the bridge near a cell as the C4 on a bridge repair hut does, through MapClass::Destroy_Bridge_At.
/// </summary>
/// <param name="step">The x in the argument and the y in X.</param>
static void Bridge_Hut_Step(StepType const & step)
{
	int const x = std::atoi(step.Argument.c_str());
	Map.Destroy_Bridge_At(Cell(x, step.X));
	DebugString("AUTOTEST   bridgehut %d,%d\n", x, step.X);
}


/// <summary>
/// Logs how many high bridge deck overlays stand on one row between two columns.
/// </summary>
/// <param name="step">The first column in the argument, the last in X, and the row in Y.</param>
static void Bridge_Decks_Step(StepType const & step)
{
	int const first = std::atoi(step.Argument.c_str());
	int left = 0;
	for (int x = first; x <= step.X; x++) {
		CellClass const & c = Map[Cell(x, step.Y)];
		if ((c.Overlay == OVERLAY_BRIDGE1 || c.Overlay == OVERLAY_BRIDGE2) && c.IsBridgeDeck) {
			left++;
		}
	}
	DebugString("AUTOTEST   bridgedecks %d..%d,%d left %d\n", first, step.X, step.Y, left);
}


/// <summary>
/// Hits the concrete low bridge on a cell through MapClass::Damage_Concrete_Bridge, the call a weapon impact
/// makes on a concrete piece, and logs whether a span came down.
/// </summary>
/// <param name="step">The x in the argument and the y in X.</param>
static void Concrete_Hit_Step(StepType const & step)
{
	int const x = std::atoi(step.Argument.c_str());
	bool const destroyed = Map.Damage_Concrete_Bridge(Cell(x, step.X));
	DebugString("AUTOTEST   concretehit %d,%d %d\n", x, step.X, (int)destroyed);
}


/// <summary>
/// Logs whether a foot unit and a tracked unit can move onto a cell, and the overlay on it.
/// </summary>
/// <param name="step">The x in the argument and the y in X.</param>
static void Passable_Step(StepType const & step)
{
	int const x = std::atoi(step.Argument.c_str());
	CellClass const & c = Map[Cell(x, step.X)];
	DebugString("AUTOTEST   passable %d,%d foot %d track %d overlay %d\n", x, step.X,
		(int)c.Is_Clear_To_Move(SPEED_FOOT, true, true), (int)c.Is_Clear_To_Move(SPEED_TRACK, true, true), (int)c.Overlay);
}


/// <summary>
/// Logs the action the first player's object of one type gets over the first live object of another type that the player's house does not
/// allow to be an ally, with the target's height, whether it is in the air, and whether the attacker's chosen weapon can fire at it now.
/// </summary>
/// <param name="step">The attacker's and target's types, the words after the command in the line.</param>
static void Target_Action_Step(StepType const & step)
{
	std::vector<std::string> const words = Line_Words(step.Line);
	if (words.size() < 4) {
		DebugString("AUTOTEST   targetaction needs an attacker type and a target type\n");
		return;
	}
	TechnoClass * attacker = NULL;
	TechnoClass * target = NULL;
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (attacker == NULL && techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), words[2].c_str()) == 0) {
			attacker = techno;
		}
		if (target == NULL && !techno->IsInLimbo && techno->House != PlayerPtr && stricmp(techno->TClass->Name(), words[3].c_str()) == 0) {
			target = techno;
		}
	}
	if (attacker == NULL || target == NULL) {
		DebugString("AUTOTEST   targetaction %s %s: no attacker or no target\n", words[2].c_str(), words[2].c_str());
		return;
	}
	int const which = attacker->What_Weapon_Should_I_Use(target);
	DebugString("AUTOTEST   targetaction %s on %s: action %s in_air %d agl %d fire %d weapon %d range %d\n", attacker->TClass->Name(), target->TClass->Name(),
		ActionName[attacker->What_Action((ObjectClass const *)target)], (int)target->In_Air(), (int)target->HeightAGL, (int)attacker->Can_Fire(target, which), which, (int)attacker->In_Range(target, which));
}


/// <summary>
/// Orders the first computer-owned object of a type to attack what stands on a cell, as a computer house's unit does with a target.
/// </summary>
/// <param name="step">The type in the argument, and the cell in X and Y.</param>
static void Enemy_Strike_Step(StepType const & step)
{
	TechnoClass * target = Map[Cell(step.X, step.Y)].Cell_Techno();
	DebugString("AUTOTEST enemystrike %s -> %s\n", step.Argument.c_str(), target != NULL ? target->TClass->Name() : "(none)");
	if (target == NULL) {
		return;
	}
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House != PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
			techno->Assign_Target(target);
			techno->Assign_Mission(MISSION_ATTACK);
			return;
		}
	}
}


/// <summary>
/// Logs the action the player's object of a type gets over a building, through the object path a real mouse-over takes.
/// </summary>
/// <param name="step">The type in the argument, and the building's cell in X and Y.</param>
static void Hut_Action_Step(StepType const & step)
{
	BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
	if (building == NULL) {
		DebugString("AUTOTEST   hutaction %d,%d no building\n", step.X, step.Y);
		return;
	}
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
			ActionType const action = techno->What_Action(building, false);
			DebugString("AUTOTEST   hutaction %s on %s %d,%d: %s\n", techno->TClass->Name(), building->Class->Name(), step.X, step.Y, ActionName[action]);
			return;
		}
	}
	DebugString("AUTOTEST   hutaction %s no object\n", step.Argument.c_str());
}


/// <summary>
/// Gives every object of a type a target and no other order: InfantryClass::Assign_Target on the object that stands on a cell.
/// </summary>
/// <param name="step">The type in the argument, and the target's cell in X and Y. A word after those, "computer", picks the objects of
/// that type that neither the player nor an ally owns; otherwise the player's objects are picked. A word "attack" also gives each object
/// the attack mission.</param>
/// <summary>
/// Finds the house a script names: a spawn name or IniName (an underscore stands for a space), or #n for the house at position n.
/// </summary>
/// <param name="name">The name as the script writes it.</param>
/// <returns>Returns the house, or NULL when no house has that name.</returns>
static HouseClass * House_Named_By_Step(std::string name)
{
	std::replace(name.begin(), name.end(), '_', ' ');
	if (!name.empty() && name[0] == '#') {
		int const position = std::atoi(name.c_str() + 1);
		return(position >= 0 && position < Houses.Count() ? Houses[position] : NULL);
	}
	HouseClass * owner = House_From_Name(name.c_str());
	for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
		if (stricmp(Houses[index]->IniName, name.c_str()) == 0) {
			owner = Houses[index];
		}
	}
	return(owner);
}


static void Set_Target_Step(StepType const & step)
{
	std::vector<std::string> const words = Line_Words(step.Line);
	bool const computer = Word(words, 5) == "computer" || Word(words, 6) == "computer";
	bool const attack = Word(words, 5) == "attack" || Word(words, 6) == "attack";
	bool const hunt = Word(words, 5) == "hunt" || Word(words, 6) == "hunt";
	// capturedest sets the structure as the destination of the Capture mission with no target, as a team's garrison mission does.
	bool const capturedest = Word(words, 5) == "capturedest" || Word(words, 6) == "capturedest";
	TechnoClass * target = Techno_Over_Cell(Cell(step.X, step.Y));
	DebugString("AUTOTEST settarget %s -> %s\n", step.Argument.c_str(), target != NULL ? target->TClass->Name() : "(none)");
	for (int index = 0; target != NULL && index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		bool const owned = computer ? (techno->House != PlayerPtr && !techno->House->Is_Ally(PlayerPtr)) : techno->House == PlayerPtr;
		if (owned && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
			if (capturedest) {
				techno->Assign_Destination(target);
				techno->Assign_Mission(MISSION_CAPTURE);
				continue;
			}
			techno->Assign_Target(target);
			if (attack) {
				techno->Assign_Mission(MISSION_ATTACK);
			} else if (hunt) {
				techno->Assign_Mission(MISSION_HUNT);
			}
		}
	}
}


/// <summary>
/// Writes the target that each object of a type would pick for itself: the map-wide scan of Greatest_Threat with no limits, the one a hunting
/// or an idle soldier makes.
/// </summary>
/// <param name="step">The type in the argument. A fourth word, "computer", picks the objects of that type that neither the player nor an
/// ally owns; otherwise the player's objects are picked.</param>
static void Threat_Probe_Step(StepType const & step)
{
	bool const computer = Word(Line_Words(step.Line), 3) == "computer";
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		bool const owned = computer ? (techno->House != PlayerPtr && !techno->House->Is_Ally(PlayerPtr)) : techno->House == PlayerPtr;
		if (owned && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
			AbstractClass * target = techno->Greatest_Threat(THREAT_NORMAL, techno->Center_Coord(), false);
			ObjectClass * object = target != NULL ? target->As_ObjectClass() : NULL;
			if (object != NULL && object->Is_Techno()) {
				TechnoClass * hit = static_cast<TechnoClass *>(object);
				DebugString("AUTOTEST   threat %s at %d,%d -> %s at %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, hit->TClass->Name(), hit->Get_Cell().X, hit->Get_Cell().Y);
			} else {
				DebugString("AUTOTEST   threat %s at %d,%d -> %s\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, target != NULL ? "(not a techno)" : "(none)");
			}
		}
	}
}


/// <summary>
/// Clicks the building on a cell with the player's object of a type, through the object path a real click takes.
/// </summary>
/// <param name="step">The type in the argument, and the building's cell in X and Y.</param>
static void Hut_Click_Step(StepType const & step)
{
	BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
	if (building == NULL) {
		DebugString("AUTOTEST   hutclick %d,%d no building\n", step.X, step.Y);
		return;
	}
	for (int index = 0; index < Technos.Count(); index++) {
		TechnoClass * techno = Technos[index];
		if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
			ActionType const action = techno->What_Action(building, false);
			DebugString("AUTOTEST   hutclick %s on %s %d,%d: %s\n", techno->TClass->Name(), building->Class->Name(), step.X, step.Y, ActionName[action]);
			PlanningRecordScope const recording(Map.IsPlanningMode);
			techno->Active_Click_With(action, building, false);
			return;
		}
	}
	DebugString("AUTOTEST   hutclick %s no object\n", step.Argument.c_str());
}


/// <summary>
/// Logs the C4 state of the building on a cell: whether a charge is on it, the countdown, and its strength.
/// </summary>
/// <param name="step">The building's cell in X and Y.</param>
static void Hut_C4_Step(StepType const & step)
{
	BuildingClass const * building = Map[Cell(step.X, step.Y)].Cell_Building();
	if (building == NULL) {
		DebugString("AUTOTEST   hutc4 %d,%d no building\n", step.X, step.Y);
		return;
	}
	DebugString("AUTOTEST   hutc4 %s %d,%d c4 %d going_to_blow %d countdown %d strength %d active %d\n", building->Class->Name(), step.X, step.Y, (int)building->IsC4Applied, (int)building->IsGoingToBlow, building->CountDown.Value(), (int)building->Strength, (int)building->IsActive);
}


// The building a c4watch step follows, the frame the watch ends at, and the charge state the log has seen.
static BuildingClass * C4WatchBuilding = NULL;
static int C4WatchEnd = 0;
static bool C4WatchPlaced = false;
static int C4WatchPlacedFrame = 0;
static int C4WatchCountdown = -1;
static int C4WatchStrength = -1;
static TechnoClass * C4WatchSaboteur = NULL;
static double C4WatchExperience = 0.0;
static int C4WatchMoney = 0;
static int C4WatchPoints = 0;


// The list is searched rather than the pointer read, since a destroyed building may already be freed.
static bool C4_Watch_Still_Built(BuildingClass const * building)
{
	for (int index = 0; index < Buildings.Count(); index++) {
		if (Buildings[index] == building) {
			return(true);
		}
	}
	return(false);
}


static bool C4_Watch_Still_Live(TechnoClass const * techno)
{
	for (int index = 0; index < Technos.Count(); index++) {
		if (Technos[index] == techno) {
			return(true);
		}
	}
	return(false);
}


// Returns -1 once the saboteur is no longer on the techno list, so its fields are not read.
static double C4_Watch_Experience(void)
{
	return(C4_Watch_Still_Live(C4WatchSaboteur) ? C4WatchSaboteur->Veterancy.Experience : -1.0);
}


/// <summary>
/// Starts following the building on a cell for a number of frames. The log writes a line when the charge is
/// placed, when the countdown or strength changes, and when the building is destroyed. The placement line gives
/// the saboteur's experience and the player's money; the destruction line gives both again with the player's points.
/// </summary>
/// <param name="step">The frame count in the argument, and the building's cell in X and Y.</param>
static void C4_Watch_Start(StepType const & step)
{
	BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
	if (building == NULL) {
		DebugString("AUTOTEST   c4watch %d,%d no building\n", step.X, step.Y);
		return;
	}
	C4WatchBuilding = building;
	C4WatchEnd = Frame + std::atoi(step.Argument.c_str());
	C4WatchPlaced = false;
	C4WatchPlacedFrame = 0;
	C4WatchCountdown = -1;
	C4WatchStrength = -1;
	C4WatchSaboteur = NULL;
	DebugString("AUTOTEST   c4watch %s %d,%d from frame %d to %d\n", building->Class->Name(), step.X, step.Y, Frame, C4WatchEnd);
}


// Advances the c4watch log by one frame; the building is read only while it is on the building list.
static void C4_Watch_Frame(void)
{
	if (C4WatchBuilding == NULL) {
		return;
	}
	if (!C4_Watch_Still_Built(C4WatchBuilding)) {
		double const after = C4_Watch_Experience();
		DebugString("AUTOTEST   c4 destroyed frame %d: %d frames after placement at %d; money %d -> %d; points %d -> %d; saboteur experience %.2f -> %.2f\n",
			Frame, C4WatchPlaced ? Frame - C4WatchPlacedFrame : -1, C4WatchPlacedFrame, C4WatchMoney, PlayerPtr->Available_Money(), C4WatchPoints, PlayerPtr->PointTotal, C4WatchExperience, after);
		C4WatchBuilding = NULL;
		return;
	}

	BuildingClass const * building = C4WatchBuilding;
	if (!C4WatchPlaced && building->IsC4Applied) {
		C4WatchPlaced = true;
		C4WatchPlacedFrame = Frame;
		C4WatchSaboteur = static_cast<TechnoClass *>(building->WhomToRepay);
		C4WatchExperience = C4_Watch_Experience();
		C4WatchMoney = PlayerPtr->Available_Money();
		C4WatchPoints = PlayerPtr->PointTotal;
		DebugString("AUTOTEST   c4 placed frame %d: %s countdown %d going_to_blow %d strength %d saboteur %s experience %.2f money %d points %d\n",
			Frame, building->Class->Name(), building->CountDown.Value(), (int)building->IsGoingToBlow, (int)building->Strength,
			C4WatchSaboteur != NULL && C4_Watch_Still_Live(C4WatchSaboteur) ? C4WatchSaboteur->TClass->Name() : "(none)", C4WatchExperience, C4WatchMoney, C4WatchPoints);
	}
	if (C4WatchPlaced && building->CountDown.Value() != C4WatchCountdown) {
		C4WatchCountdown = building->CountDown.Value();
		DebugString("AUTOTEST   c4 countdown frame %d: %d\n", Frame, C4WatchCountdown);
	}
	if ((int)building->Strength != C4WatchStrength) {
		C4WatchStrength = (int)building->Strength;
		DebugString("AUTOTEST   c4 strength frame %d: %d\n", Frame, C4WatchStrength);
	}
	if (Frame >= C4WatchEnd) {
		DebugString("AUTOTEST   c4watch ended frame %d; the building still stands\n", Frame);
		C4WatchBuilding = NULL;
	}
}


/*
**	Waypoint probes: the waypoint placement, path send and aircraft path state steps. They sit in their own
**	function, as the air probes do, because the Run chain is nested too deeply to take more branches.
*/
static void Waypoint_Probe_Step(StepType const & step)
{
	if (step.Command == "waypoint") {
		// waypoint <any> x y: places a waypoint on the cell on the selected path, as a click in waypoint mode does.
		Cell const cell(step.X, step.Y);
		Coord coord = cell.As_Coord();
		coord.Z = (Map[cell].IsUnderBridge ? BRIDGE_LEPTON_HEIGHT : 0) + Map.Get_Height_GL(coord);
		PlayerPtr->Place_Waypoint(coord);
		DebugString("AUTOTEST   waypoint %d,%d selected path %d waypoints on it %d\n", step.X, step.Y, (int)PlayerPtr->SelectedPath, PlayerPtr->SelectedPath == PATH_NONE ? 0 : PlayerPtr->Paths[PlayerPtr->SelectedPath]->Waypoint_Count());
	} else if (step.Command == "wpcell") {
		// wpcell <n>: the cell of the map's waypoint n, or that it is not set.
		int const number = std::atoi(step.Argument.c_str());
		if (Scen->Is_Valid_Waypoint(number)) {
			Cell const cell = Scen->Get_Waypoint_Cell(number);
			BuildingClass * building = Map[cell].Cell_Building();
			DebugString("AUTOTEST   wpcell %d at %d,%d building %s\n", number, cell.X, cell.Y, building != NULL ? building->Class->Name() : "-");
		} else {
			DebugString("AUTOTEST   wpcell %d not set\n", number);
		}
	} else if (step.Command == "wpsend") {
		// wpsend <move|attack> x y: a click on the waypoint at that cell by the selected objects. A move click is the
		// move a click on the ground makes; an attack click is an attack on the object standing on the cell. Either
		// goes through DisplayClass::Active_Click, which hands each object the path that holds the waypoint.
		bool const attack = step.Argument == "attack";
		TechnoClass * over = attack ? Techno_Over_Cell(Cell(step.X, step.Y)) : NULL;
		Map.Active_Click(over, Cell(step.X, step.Y), attack ? ACTION_ATTACK : ACTION_MOVE);
		DebugString("AUTOTEST   wpsend %s %d,%d over %s\n", attack ? "attack" : "move", step.X, step.Y, over != NULL ? over->TClass->Name() : "ground");
	} else if (step.Command == "padstate") {
		// padstate <TypeID>: each building of the type, whoever owns it, with the objects in each of its radio slots.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * building = Buildings[index];
			if (stricmp(building->Class->Name(), step.Argument.c_str()) != 0) continue;
			std::string slots;
			for (int slot = 0; slot < building->Link_Count(); slot++) {
				TechnoClass const * linked = building->Link(slot);
				slots += (slot > 0 ? "," : "") + std::string(linked != NULL ? linked->TClass->Name() : "-");
			}
			DebugString("AUTOTEST   padstate %s of %s cell %d,%d links %d slots %s\n", building->Class->Name(), building->House->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y, building->Link_Count(), slots.c_str());
		}
	} else if (step.Command == "wpstate") {
		// wpstate <TypeID>: each aircraft of the type, whoever owns it, with its cell, height above ground, mission and status, ammo,
		// the waypoint path it follows and the next waypoint, and its radio contact.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->RTTI != RTTI_AIRCRAFT || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			AircraftClass const * plane = static_cast<AircraftClass const *>(techno);
			FootClass const * foot = static_cast<FootClass const *>(techno);
			TechnoClass const * contact = plane->In_Radio_Contact() ? plane->Contact_With_Whom() : NULL;
			DebugString("AUTOTEST   wpstate #%04X %s of %s cell %d,%d agl %d in_air %d mission %s status %d ammo %d path %d next %d moving %d limbo %d contact %s\n",
				index & 0xFFFF, techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->HeightAGL, (int)techno->In_Air(),
				MissionClass::Mission_Name(techno->Get_Mission()), (int)techno->Status, (int)techno->Ammo, (int)foot->CurrentPath, (int)foot->NextWaypoint,
				(int)plane->Locomotion->Is_Moving(), (int)techno->IsInLimbo, contact != NULL ? contact->TClass->Name() : "-");
		}
	}
}


// presentdigest, drawdigest, gpureplay, gpucheck and drawprobe switch the drawing checks on and off. True when
// the step was one of them.
// cloudcheck <SoldierTypeID>: whether the type is NotHuman, how many InfantryVirus animations the map holds and how many particles of the InfantryVirus
// animation's particle type are live, so a virus death can be told from an animation to a bare particle.
static void Cloud_Check_Step(StepType const & step)
{
	InfantryTypeClass const * type = NULL;
	for (int index = 0; index < InfantryTypes.Count(); index++) {
		if (stricmp(InfantryTypes[index]->Name(), step.Argument.c_str()) == 0) type = InfantryTypes[index];
	}
	AnimTypeClass const * virus = Rule->InfantryVirus;
	int anims = 0;
	for (int index = 0; virus != NULL && index < Anims.Count(); index++) {
		anims += Anims[index]->Class == virus;
	}
	int particles = 0;
	for (int index = 0; virus != NULL && virus->SpawnsParticle != PARTICLE_NONE && index < Particles.Count(); index++) {
		particles += Particles[index]->Class == ParticleTypes[virus->SpawnsParticle];
	}
	DebugString("AUTOTEST   cloudcheck %s nothuman %d virusanims %d virusparticles %d\n", step.Argument.c_str(), type != NULL ? (int)type->IsNotHuman : -1, anims, particles);
}


// crushcheck <CrusherTypeID> <SoldierTypeID>: for each vehicle of the first type and each soldier of the second, whether the vehicle may crush the soldier
// (UnitClass::Can_Crush), with the soldier's sequence and its dug-in crush protection.
static void Crush_Check_Step(StepType const & step)
{
	std::string const & crusher = step.Argument;
	std::string const victim = step.Line.substr(step.Line.rfind(' ') + 1);
	for (int unit_index = 0; unit_index < Units.Count(); unit_index++) {
		UnitClass const * unit = Units[unit_index];
		if (unit->IsInLimbo || stricmp(unit->Class->Name(), crusher.c_str()) != 0) continue;
		for (int index = 0; index < Infantry.Count(); index++) {
			InfantryClass const * soldier = Infantry[index];
			if (soldier->IsInLimbo || stricmp(soldier->Class->Name(), victim.c_str()) != 0) continue;
			DebugString("AUTOTEST   crushcheck %s %s doing %d protected %d cancrush %d\n", unit->Class->Name(), soldier->Class->Name(), (int)soldier->Doing, (int)soldier->IsDugInSafe, (int)unit->Can_Crush(soldier));
		}
	}
}


static bool Draw_Check_Step(StepType const & step)
{
	if (step.Command == "presentdigest") {
		// presentdigest <0|1>: 1 logs a PRESENT_DIGEST line at each present that reaches the presenter; 0 stops the lines.
		bool const on = std::atoi(step.Argument.c_str()) != 0;
		Video_Set_Present_Digest(on);
		DebugString("AUTOTEST presentdigest %d\n", (int)on);
		return(true);
	}
	if (step.Command == "drawdigest") {
		// drawdigest <0|1>: 1 logs a DRAW_DIGEST line for each draw of the map view; 0 stops the lines.
		bool const on = std::atoi(step.Argument.c_str()) != 0;
		Video_Set_Draw_Digest(on);
		DebugString("AUTOTEST drawdigest %d\n", (int)on);
		return(true);
	}
	if (step.Command == "gpureplay") {
		// gpureplay <0|1>: 1 records each object pass of the map view and logs how far a replay of the record is from it.
		bool const on = std::atoi(step.Argument.c_str()) != 0;
		GpuDraw_Set_Replay_Check(on);
		DebugString("AUTOTEST gpureplay %d\n", (int)on);
		return(true);
	}
	if (step.Command == "gpucheck") {
		// gpucheck <0|1>: 1 draws each object pass of the map view on the GPU too and logs how far it is from the map view.
		bool const on = std::atoi(step.Argument.c_str()) != 0;
		GpuDraw_Set_Gpu_Check(on);
		DebugString("AUTOTEST gpucheck %d\n", (int)on);
		return(true);
	}
	if (step.Command == "drawprobe") {
		// drawprobe <0|1>: 1 counts the call stacks that lock the map view in its foreground pass; 0 logs them.
		bool const on = std::atoi(step.Argument.c_str()) != 0;
		Video_Set_Draw_Probe(on);
		DebugString("AUTOTEST drawprobe %d\n", (int)on);
		return(true);
	}
	return(false);
}


void Run(StepType const & step)
{
	DebugString("AUTOTEST frame %d: %s %s\n", Frame, step.Command.c_str(), step.Argument.c_str());

	// The drawing checks sit before the chain below, which is nested too deeply to take another branch.
	if (Draw_Check_Step(step)) {
		return;
	}

	// bridge x y and bridgecensus: see Bridge_Cell_Step and Bridge_Census_Step. They sit before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "bridge") {
		Bridge_Cell_Step(step);
		return;
	}
	if (step.Command == "bridgecensus") {
		Bridge_Census_Step(step);
		return;
	}
	if (step.Command == "wallcensus") {
		Wall_Census_Step(step);
		return;
	}
	if (step.Command == "bridgearea") {
		Bridge_Area_Step(step);
		return;
	}
	if (step.Command == "bridgeunit") {
		Bridge_Unit_Step(step);
		return;
	}
	if (step.Command == "repairbridge") {
		Repair_Bridge_Step(step);
		return;
	}
	if (step.Command == "bridgehit") {
		Bridge_Hit_Step(step);
		return;
	}
	if (step.Command == "bridgehut") {
		Bridge_Hut_Step(step);
		return;
	}
	if (step.Command == "bridgedecks") {
		Bridge_Decks_Step(step);
		return;
	}
	if (step.Command == "concretehit") {
		Concrete_Hit_Step(step);
		return;
	}
	if (step.Command == "passable") {
		Passable_Step(step);
		return;
	}
	if (step.Command == "targetaction") {
		Target_Action_Step(step);
		return;
	}
	if (step.Command == "enemystrike") {
		Enemy_Strike_Step(step);
		return;
	}
	if (step.Command == "hutaction") {
		Hut_Action_Step(step);
		return;
	}
	if (step.Command == "hutclick") {
		Hut_Click_Step(step);
		return;
	}
	if (step.Command == "cloudcheck") {
		Cloud_Check_Step(step);
		return;
	}
	if (step.Command == "crushcheck") {
		Crush_Check_Step(step);
		return;
	}
	if (step.Command == "settarget") {
		Set_Target_Step(step);
		return;
	}
	if (step.Command == "threat") {
		Threat_Probe_Step(step);
		return;
	}
	if (step.Command == "hutc4") {
		Hut_C4_Step(step);
		return;
	}
	if (step.Command == "c4watch") {
		C4_Watch_Start(step);
		return;
	}

	// weapons, airborne and rocket: see Air_Probe_Step. They sit before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "weapons" || step.Command == "airborne" || step.Command == "rocket") {
		Air_Probe_Step(step);
		return;
	}

	if (step.Command == "typedump") {
		Type_Dump_Step(step);
		return;
	}

	// spawns: see Spawn_Probe_Step, which sits before the chain below for the same reason.
	if (step.Command == "spawns") {
		Spawn_Probe_Step(step);
		return;
	}

	if (step.Command == "waypoint" || step.Command == "wpsend" || step.Command == "wpstate" || step.Command == "padstate" || step.Command == "wpcell") {
		Waypoint_Probe_Step(step);
		return;
	}

	// cursor <TypeID> x y: see Cursor_Probe. It sits before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "cursor") {
		Cursor_Probe(step.Argument, step.X, step.Y);
		return;
	}

	// shape: writes the mouse shape the map shows now, as the last hover set it (MouseType numbers, MOUSE_CAN_ATTACK is 21),
	// with the first frame of its cursor entry and the frames the loaded cursor sheet holds; the sheet has no art at a frame past its count.
	// shown is the shape on screen, which an override such as the panel's normal pointer can differ from.
	if (step.Command == "shape") {
		MouseType const shape = Map.Get_Mouse_Shape();
		DebugString("AUTOTEST   shape %d frame %d sheet frames %d shown %d\n", (int)shape, Map.Get_Mouse_Start_Frame(shape), MouseClass::MouseShapes != NULL ? MouseClass::MouseShapes->Get_Count() : -1, (int)Map.Get_Displayed_Mouse_Shape());
		return;
	}

	// swaim <SuperSection> x y: see SW_Aim_Probe, dispatched here for the same reason as cursor.
	if (step.Command == "swaim") {
		SW_Aim_Probe(step.Argument, step.X, step.Y);
		return;
	}

	// speed <TypeID> [interval]: see Speed_Probe. It sits before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "speed") {
		Speed_Probe(step);
		return;
	}

	// stop <TypeID>: see Stop_Objects. It sits before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "stop") {
		Stop_Objects(step.Argument);
		return;
	}

	// strikeobj <TypeID> x y [radius]: see Strike_Object. It sits before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "strikeobj") {
		std::vector<std::string> const words = Line_Words(step.Line);
		Strike_Object(step.Argument, step.X, step.Y, Number(words, 5, 3), Word(words, 6), Word(words, 7));
		return;
	}

	// scatter <TypeID> x y [forced] [nokidding]: see Scatter_Objects. It sits before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "scatter") {
		std::vector<std::string> const words = Line_Words(step.Line);
		Scatter_Objects(step.Argument, step.X, step.Y, Number(words, 5, 0) != 0, Number(words, 6, 0) != 0);
		return;
	}

	// canbuild <TypeID>: see Can_Build_Step. It sits before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "canbuild") {
		Can_Build_Step(step);
		return;
	}

	// chrono <frames>: starts the chronoshift screen effect, as the Chrono Screen Effect trigger action does.
	if (step.Command == "chrono") {
		Start_Chrono_Screen_Effect(std::atoi(step.Argument.c_str()));
		return;
	}

	// key, hotkey and tab sit before the chain below, which is nested too deeply to take another branch.
	if (step.Command == "hotkey") {
		// hotkey <key>: the command the key is bound to, given as a virtual-key code or a letter.
		int const key = Key_Code(step.Argument);
		CommandClass const * command = HotkeyCommands[key];
		DebugString("AUTOTEST   hotkey %d %s\n", key, command != NULL ? command->Get_Unique_Name() : "none");
		return;
	}

	if (step.Command == "key") {
		WindowEvent event;
		event.VirtualKey = Key_Code(step.Argument);
		event.Modifiers = Key_Modifiers(step.Argument);
		event.Type = WINDOW_EVENT_KEY_DOWN;
		Keyboard->Handle_Window_Event(event);
		event.Type = WINDOW_EVENT_KEY_UP;
		Keyboard->Handle_Window_Event(event);
		return;
	}

	// hold <Key> / release <Key>: the press or the release alone, so the key stays down between steps as a held key does.
	if (step.Command == "hold" || step.Command == "release") {
		WindowEvent event;
		event.VirtualKey = Key_Code(step.Argument);
		event.Modifiers = Key_Modifiers(step.Argument);
		event.Type = (step.Command == "hold") ? WINDOW_EVENT_KEY_DOWN : WINDOW_EVENT_KEY_UP;
		Keyboard->Handle_Window_Event(event);
		if (event.VirtualKey > 0 && event.VirtualKey < 256) {
			ScriptHeldKeys[event.VirtualKey] = (step.Command == "hold");
		}
		return;
	}

	// binding <key> [Press|Double|Hold|Held]: the command the key's binding of that trigger runs, with the key's modifier words.
	if (step.Command == "binding") {
		int const mods = Key_Modifiers(step.Argument);
		int const key = Key_Code(step.Argument) | ((mods & WINDOW_MOD_SHIFT) != 0 ? WWKEY_SHIFT_BIT : 0)
			| ((mods & WINDOW_MOD_CTRL) != 0 ? WWKEY_CTRL_BIT : 0) | ((mods & WINDOW_MOD_ALT) != 0 ? WWKEY_ALT_BIT : 0);
		HotkeyTrigger const trigger = Binding_Trigger(step.Argument);
		CommandClass const * command = (trigger == HOTKEY_PRESS) ? HotkeyCommands[key] : HotkeyTriggered[key * 4 + (int)trigger];
		DebugString("AUTOTEST   binding %d %s %s\n", key, Hotkey_Trigger_Name(trigger), command != NULL ? command->Get_Unique_Name() : "none");
		return;
	}

	// scheme <Classic|Modern>: replaces the bindings with that scheme's defaults, as the keyboard screen does when it changes the scheme.
	if (step.Command == "scheme") {
		KeyboardScheme scheme = KEYBOARD_SCHEME_CLASSIC;
		Keyboard_Scheme_From_Name(step.Argument.substr(0, step.Argument.find_first_of(" \t")), scheme);
		HotkeyScheme = scheme;
		HotkeyBindings = Scheme_Hotkeys(scheme);
		Apply_Hotkeys();
		DebugString("AUTOTEST   scheme %s bindings %d\n", Keyboard_Scheme_Name(scheme), (int)HotkeyBindings.size());
		return;
	}

	// bind <Command> <vk> [trigger]: gives the command the virtual-key code vk (decimal) with the trigger, a HotkeyTrigger
	// number (0 Press, 1 Double, 2 Hold, 3 Held), displacing a conflicting binding as the keyboard screen does.
	if (step.Command == "bind") {
		HotkeyTrigger const trigger = (step.Y >= 0 && step.Y <= 3) ? (HotkeyTrigger)step.Y : HOTKEY_PRESS;
		Hotkey_Assign(HotkeyBindings, step.Argument, 0, step.X, trigger, true);
		Apply_Hotkeys();
		DebugString("AUTOTEST   bind %s key %d %s\n", step.Argument.c_str(), step.X, Hotkey_Trigger_Name(trigger));
		return;
	}

	// camera: writes the tactical position the view is centred on, in leptons.
	if (step.Command == "camera") {
		Point2D const position = TacticalMap->Get_Tactical_Position();
		DebugString("AUTOTEST   camera %d,%d\n", position.X, position.Y);
		return;
	}

	// winkey <VK>: a key press and release sent to the game window, so the interface sees it
	// first as it does a player's key. The key step skips that routing.
	if (step.Command == "winkey") {
		WindowEvent event;
		event.VirtualKey = Key_Code(step.Argument);
		event.Modifiers = Key_Modifiers(step.Argument);
		event.Type = WINDOW_EVENT_KEY_DOWN;
		Game_Window_Handle_Event(event);
		event.Type = WINDOW_EVENT_KEY_UP;
		Game_Window_Handle_Event(event);
		return;
	}

	// wintype <text>: each character as a key press, the text event that carries the typed
	// character, and the release, all sent to the game window in the order a keyboard sends them.
	if (step.Command == "wintype") {
		for (char const character : step.Argument) {
			WindowEvent event;
			event.VirtualKey = std::toupper((unsigned char)character);
			event.Type = WINDOW_EVENT_KEY_DOWN;
			Game_Window_Handle_Event(event);
			event.Type = WINDOW_EVENT_TEXT;
			event.Text = (unsigned char)character;
			Game_Window_Handle_Event(event);
			event.Type = WINDOW_EVENT_KEY_UP;
			Game_Window_Handle_Event(event);
		}
		return;
	}

	// alliance <house>: queues the ALLY event the alliance toggle queues, so every copy of a match
	// makes the alliance on the same frame. A house that is already an ally is made an enemy.
	if (step.Command == "alliance") {
		int const index = std::atoi(step.Argument.c_str());
		if (index < 0 || index >= Houses.Count() || Houses[index] == NULL || Houses[index] == PlayerPtr) {
			DebugString("AUTOTEST alliance %s: no such house other than the player\n", step.Argument.c_str());
			return;
		}
		OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::ALLY, index));
		DebugString("AUTOTEST alliance %s %s queued\n", step.Argument.c_str(), Houses[index]->Class->Name());
		return;
	}

	if (step.Command == "tab") {
		Tab_Step();
		return;
	}

	if (step.Command == "modes") {
		Modes_Step();
		return;
	}

	if (step.Command == "placement") {
		Placement_Step();
		return;
	}

	if (step.Command == "modsig") {
		Mod_Signature_Step(step);
		return;
	}

	if (step.Command == "stockcheck") {
		Stock_Check_Step();
		return;
	}

	if (step.Command == "previewtime") {
		Preview_Time_Step();
		return;
	}

	if (step.Command == "liveanims") {
		// liveanims [TypeID]: writes each live animation, or only those of the named type, with its cell, loops left, delay, rate, stage, and whether it is attached to a cell or an object.
		int shown = 0;
		for (int index = 0; index < Anims.Count(); index++) {
			AnimClass const * anim = Anims[index];
			if (anim == NULL || anim->Class == NULL) continue;
			if (!step.Argument.empty() && stricmp(anim->Class->Name(), step.Argument.c_str()) != 0) continue;
			Cell const cell = anim->Center_Coord().As_Cell();
			DebugString("AUTOTEST   liveanim %s at %d,%d loops %d delay %d rate %d stage %d cellattached %d objectattached %d\n", anim->Class->Name(), cell.X, cell.Y, (int)anim->Loops, (int)anim->Delay, (int)anim->Fetch_Rate(), (int)anim->Fetch_Stage(), (int)anim->IsAttachedToCell, anim->xObject != NULL ? 1 : 0);
			shown++;
		}
		DebugString("AUTOTEST   liveanims: %d of %d\n", shown, Anims.Count());
		return;
	}

	if (step.Command == "psoffset") {
		// psoffset <TypeID>: writes the DamageSmokeOffset and DamSmkOffScrnRel of the type, and the offset its damage smoke and spark systems are placed at.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			Coord const offset = type->Particle_System_Offset();
			DebugString("AUTOTEST   psoffset %s smoke %d,%d,%d screen %d particle %d,%d,%d\n", type->Name(),
				type->DamageSmokeOffset.X, type->DamageSmokeOffset.Y, type->DamageSmokeOffset.Z, (int)type->DamSmkOffScrnRel, offset.X, offset.Y, offset.Z);
		}
		return;
	}

	if (step.Command == "vanims") {
		// vanims [TypeID]: writes each live voxel animation, or only those of the named type, with its position and cell.
		int shown = 0;
		for (int index = 0; index < VoxelAnims.Count(); index++) {
			VoxelAnimClass const * anim = VoxelAnims[index];
			if (anim == NULL || anim->Class == NULL) continue;
			if (!step.Argument.empty() && stricmp(anim->Class->Name(), step.Argument.c_str()) != 0) continue;
			Coord const where = anim->Get_Coord();
			DebugString("AUTOTEST   vanim frame %d %s at %d,%d,%d cell %d,%d\n", Frame, anim->Class->Name(), where.X, where.Y, where.Z,
				where.As_Cell().X, where.As_Cell().Y);
			shown++;
		}
		DebugString("AUTOTEST   vanims: %d of %d\n", shown, VoxelAnims.Count());
		return;
	}

	if (step.Command == "movesounds") {
		// movesounds [TypeID]: writes each foot object, or only those of the named type, with its cell, whether its move sound is playing, the frames left before it ends, and the sound, then how many are playing.
		int shown = 0;
		int playing = 0;
		for (int index = 0; index < Feet.Count(); index++) {
			FootClass const * foot = Feet[index];
			if (foot == NULL || foot->TClass == NULL) continue;
			if (!step.Argument.empty() && stricmp(foot->TClass->Name(), step.Argument.c_str()) != 0) continue;
			Cell const cell = foot->Get_Cell();
			DebugString("AUTOTEST   movesound frame %d %s at %d,%d playing %d delay %d sound %s\n", Frame, foot->TClass->Name(), cell.X, cell.Y,
				foot->IsMoveSoundPlaying ? 1 : 0, foot->MoveSoundDelay, foot->IsMoveSoundPlaying ? Voc_Name(foot->MoveSoundVoc) : "none");
			shown++;
			if (foot->IsMoveSoundPlaying) playing++;
		}
		DebugString("AUTOTEST   movesounds: %d playing of %d\n", playing, shown);
		return;
	}

	if (step.Command == "particles") {
		// particles [TypeID]: writes each live particle, or only those of the named type, with its animation state, speed, translucency, drift and position.
		int shown = 0;
		for (int index = 0; index < Particles.Count(); index++) {
			ParticleClass const * particle = Particles[index];
			if (particle == NULL || particle->Class == NULL) continue;
			if (!step.Argument.empty() && stricmp(particle->Class->Name(), step.Argument.c_str()) != 0) continue;
			Coord const where = particle->Get_Coord();
			DebugString("AUTOTEST   particle frame %d %s state %d advance %d translucency %d speed %.3f drift %d,%d,%d at %d,%d,%d from %d,%d,%d to %d,%d,%d\n", Frame, particle->Class->Name(), (int)particle->StateAI, (int)particle->StateAIAdvance,
				(int)particle->Translucency, (double)particle->Speed, particle->GasDrift.X, particle->GasDrift.Y, particle->GasDrift.Z, where.X, where.Y, where.Z,
				particle->FireOrigin.X, particle->FireOrigin.Y, particle->FireOrigin.Z, particle->FireTarget.X, particle->FireTarget.Y, particle->FireTarget.Z);
			shown++;
		}
		DebugString("AUTOTEST   particles: %d of %d\n", shown, Particles.Count());
		return;
	}

	if (step.Command == "veinholes") {
		// veinholes: writes each veinhole monster with its cell, state, drawn frame, growth timer and the veins it has grown.
		for (int index = 0; index < VeinholeMonsterClass::VeinholeMonsters.Count(); index++) {
			VeinholeMonsterClass const * monster = VeinholeMonsterClass::VeinholeMonsters[index];
			DebugString("AUTOTEST   veinhole at %d,%d state %d desired %d frame %d dead %d growth timer %d veins %d queued %d\n", monster->CellID.X, monster->CellID.Y, monster->CurrentState, monster->DesiredState, monster->ShapeFrame, (int)monster->IsDead, (int)monster->GrowthTimer, monster->VeinCount, monster->GrowthCount);
		}
		DebugString("AUTOTEST   veinholes: %d\n", VeinholeMonsterClass::VeinholeMonsters.Count());
		return;
	}

	if (step.Command == "treehit" || step.Command == "trees" || step.Command == "terrainonore") {
		Terrain_Step(step);
		return;
	}

	if (step.Command == "triggerlog" || step.Command == "triggercoverage") {
		Trigger_Step(step);
		return;
	}

	if (step.Command == "trigaction") {
		Trigger_Action_Step(step);
		return;
	}

	if (step.Command == "dumpfile") {
		// dumpfile <name>: copies a file the game can read, from a MIX or the game folder, to "dump-<name>" in the working directory.
		CCFileClass file(step.Argument.c_str());
		std::string const path = "dump-" + step.Argument;
		if (file.Is_Available() && file.Open(FileClass::READ)) {
			int const size = file.Size();
			std::vector<char> data(size);
			file.Read(data.data(), size);
			file.Close();
			FILE * f = std::fopen(path.c_str(), "wb");
			if (f != NULL) {
				std::fwrite(data.data(), 1, size, f);
				std::fclose(f);
			}
			DebugString("AUTOTEST   dumpfile %s: %d bytes to %s\n", step.Argument.c_str(), size, path.c_str());
		} else {
			DebugString("AUTOTEST   dumpfile %s: not found\n", step.Argument.c_str());
		}
		return;
	}

	if (step.Command == "mixof") {
		// mixof <name>: writes each attached archive that holds the name, in lookup order (the first one is the copy Retrieve returns), with its size.
		int order = 0;
		bool found = false;
		for (MixFileClass * mix = MixFileClass::First_Attached(); mix != NULL; mix = mix->Next_Attached(), order++) {
			int size = 0;
			if (mix->Member_Size(step.Argument.c_str(), &size)) {
				DebugString("AUTOTEST   mixof %s: archive %d %s size %d\n", step.Argument.c_str(), order, mix->Filename, size);
				found = true;
			}
		}
		if (!found) {
			DebugString("AUTOTEST   mixof %s: not in an archive\n", step.Argument.c_str());
		}
		return;
	}

	if (step.Command == "spyon") {
		// spyon <House> x y: the named house's spy has infiltrated the structure on that cell, which is the call a spy's entry
		// makes (BuildingClass::Spied_By). The spy's walk in is not part of this step.
		HouseClass * spy = House_Named_By_Step(step.Argument);
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		if (spy == NULL) {
			DebugString("AUTOTEST spyon %s: no such house\n", step.Argument.c_str());
		} else if (building == NULL) {
			DebugString("AUTOTEST spyon %s at %d,%d: no structure\n", step.Argument.c_str(), step.X, step.Y);
		} else {
			building->Spied_By(spy);
			DebugString("AUTOTEST spyon %s %s at %d,%d: spied by %s\n", spy->Class->Name(), building->Class->Name(), step.X, step.Y, spy->Class->Name());
		}
		return;
	}
	if (step.Command == "infiltrate") {
		// infiltrate barracks|warfactory: marks the player's house as having spied on that building, as a spy's infiltration does.
		if (step.Argument == "barracks") {
			PlayerPtr->IsBarracksInfiltrated = true;
		} else if (step.Argument == "warfactory") {
			PlayerPtr->IsWarFactoryInfiltrated = true;
		}
		DebugString("AUTOTEST   infiltrate %s\n", step.Argument.c_str());
		return;
	}
	if (step.Command == "iq") {
		// iq: writes each house's base and effective IQ and which [IQ] gates it opens.
		Iq();
		return;
	}
	if (step.Command == "crate") {
		// crate <Powerup> x y: puts a crate that holds that powerup on the nearest free cell to the cell.
		static struct {
			char const * Name;
			CrateType Powerup;
		} const powerups[] = {
			{"money", CRATE_MONEY}, {"unit", CRATE_UNIT}, {"heal", CRATE_HEAL_BASE}, {"cloak", CRATE_CLOAK},
			{"explosion", CRATE_EXPLOSION}, {"napalm", CRATE_NAPALM}, {"squad", CRATE_SQUAD}, {"darkness", CRATE_DARKNESS},
			{"reveal", CRATE_REVEAL}, {"armor", CRATE_ARMOR}, {"speed", CRATE_SPEED}, {"firepower", CRATE_FIREPOWER},
			{"icbm", CRATE_ICBM}, {"invuln", CRATE_INVULN}, {"veteran", CRATE_VETERAN}, {"ion", CRATE_ION_STORM},
			{"gas", CRATE_GAS}, {"tiberium", CRATE_TIBERIUM}, {"pod", CRATE_POD},
		};
		int powerup = -1;
		for (auto const & entry : powerups) {
			if (stricmp(entry.Name, step.Argument.c_str()) == 0) {
				powerup = entry.Powerup;
			}
		}
		if (powerup >= 0) {
			bool const placed = Map.Place_Crate(Cell(step.X, step.Y), powerup);
			DebugString("AUTOTEST crate %s near %d,%d: %s\n", step.Argument.c_str(), step.X, step.Y, placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST crate %s: no such powerup\n", step.Argument.c_str());
		}
		return;
	}
	if (step.Command == "occupy") {
		// occupy <TypeID> x y: puts the first object of that type (an infantry unit) inside the structure on that cell, without walking there.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		InfantryClass * soldier = NULL;
		for (int index = 0; soldier == NULL && index < Infantry.Count(); index++) {
			if (Infantry[index]->House == PlayerPtr && !Infantry[index]->IsInLimbo && stricmp(Infantry[index]->TClass->Name(), step.Argument.c_str()) == 0) {
				soldier = Infantry[index];
			}
		}
		bool const occupied = building != NULL && soldier != NULL && building->Class->IsCanBeOccupied;
		if (occupied) {
			building->Occupy(soldier);
		}
		DebugString("AUTOTEST   occupy %s at %d,%d: %s\n", step.Argument.c_str(), step.X, step.Y, occupied ? "occupied" : "not occupied");
		return;
	}
	if (step.Command == "rank") {
		// rank <TypeID> <0|1|2>: makes every object of that type, whoever owns it, rookie, veteran or elite.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				if (step.X >= 2) {
					techno->Veterancy.Set_Elite(true);
				} else if (step.X == 1) {
					techno->Veterancy.Set_Veteran(true);
				} else {
					techno->Veterancy.Set_Rookie(true);
				}
			}
		}
		DebugString("AUTOTEST   rank %s %d\n", step.Argument.c_str(), step.X);
		return;
	}
	if (step.Command == "veterancy") {
		// veterancy <TypeID>: writes the experience, rank and cost of every object of that type, whoever owns it, with the rules that scale them and the player's score.
		DebugString("AUTOTEST   veterancy rules ratio %.3f cap %.3f points %d\n", Rule->VeteranRatio, Rule->VeteranCap, PlayerPtr != NULL ? PlayerPtr->PointTotal : 0);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int const rank = techno->Veterancy.Is_Elite() ? 2 : (techno->Veterancy.Is_Veteran() ? 1 : 0);
				DebugString("AUTOTEST   veterancy %s house %s at %d,%d rank %d experience %.6f cost %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, rank, techno->Veterancy.Experience, techno->TClass->Cost_Of(techno->House));
			}
		}
		return;
	}
	// sell <x> <y>: starts selling the player's structure on that cell, as the sell cursor's click does.
	// Kept out of the chain below, which MSVC cannot nest any deeper.
	if (step.Command == "sell") {
		BuildingClass * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		DebugString("AUTOTEST sell %s at %d,%d\n", building != NULL ? building->Class->Name() : "(none)", std::atoi(step.Argument.c_str()), step.X);
		if (building != NULL && building->House == PlayerPtr) {
			if (Network_Game()) {
				OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::SELL, TargetClass(building)));
			} else {
				building->Sell_Back(-1);
			}
		}
		return;
	}

	// repairon <TypeID>: starts repairing the player's structures of that type, as a click with the repair cursor does.
	if (step.Command == "repairon") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				building->Repair(1);
				DebugString("AUTOTEST repairon %s repairing %d\n", building->Class->Name(), (int)building->IsRepairing);
			}
		}
		return;
	}

	// poweroff <TypeID>: turns off the player's structures of that type, as a click with the power cursor does.
	if (step.Command == "poweroff") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				building->Turn_Off();
				DebugString("AUTOTEST poweroff %s on %d\n", building->Class->Name(), (int)building->IsOn);
			}
		}
		return;
	}

	// rally <TypeID> x y: sets the rally point of the player's structures of that type to the cell, skipping the nearby-cell search an Alt-click makes.
	if (step.Command == "rally") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				if (Network_Game()) {
					OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::ARCHIVE, TargetClass(building), TargetClass(&Map[Cell(step.X, step.Y)])));
				} else {
					building->Assign_Archive_Target(&Map[Cell(step.X, step.Y)]);
				}
				DebugString("AUTOTEST rally %s to %d,%d\n", building->Class->Name(), step.X, step.Y);
			}
		}
		return;
	}

	// rallyclick <TypeID> x y: gives the player's structures of that type the rally click an Alt-click on the ground makes, with the nearby-cell search.
	if (step.Command == "rallyclick") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				building->Active_Click_With(ACTION_RALLY_TO_POINT, Cell(step.X, step.Y), false);
				DebugString("AUTOTEST rallyclick %s at %d,%d\n", building->Class->Name(), step.X, step.Y);
			}
		}
		return;
	}

	if (step.Command == "ally") {
		// ally <TypeID> x y: puts an object of the type, owned by the first house that is an ally of the player, on that cell.
		HouseClass * owner = NULL;
		for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
			if (Houses[index] != PlayerPtr && PlayerPtr->Is_Ally(Houses[index])) {
				owner = Houses[index];
			}
		}
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL && owner != NULL) {
			TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(owner));
			Cell cell(step.X, step.Y);
			ScenarioInit++;
			bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
			ScenarioInit--;
			DebugString("AUTOTEST %s %s at %d,%d owner %s: %s\n", step.Command.c_str(), type->Name(), step.X, step.Y, owner->Class->Name(), placed ? "placed" : "failed");
		} else {
			DebugString("AUTOTEST %s %s: %s\n", step.Command.c_str(), step.Argument.c_str(), type == NULL ? "no such type" : "no house allied with the player");
		}
		return;
	}

	if (step.Command == "transfer") {
		// transfer <TypeID>: gives the first live object of that type that a computer house owns to the
		// player through TechnoClass::Set_Owning_House, the path a mind control capture takes.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House != PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				HouseClass * from = techno->House;
				bool const given = techno->Set_Owning_House(PlayerPtr, true);
				DebugString("AUTOTEST transfer %s at %d,%d from %s: %s\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, from->Class->Name(), given ? "given" : "refused");
				break;
			}
		}
		return;
	}

	// planning <0|1>: turns planning mode on or off without the Z key. No hold key is set, so the mode stays on until planning 0.
	if (step.Command == "planning") {
		Map.Planning_Mode_Control(std::atoi(step.Argument.c_str()) != 0 ? 1 : 0, -1);
		return;
	}

	// beacon <place|select|delete|list> [x y]: the beacon steps. place queues the placement event.
	if (step.Command == "beacon") {
		if (step.Argument == "place") {
			Coord coord = Cell(step.X, step.Y).As_Coord();
			xCoord where;
			where = coord;
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::BEACON_PLACE, where));
			DebugString("AUTOTEST   beacon place %d,%d queued\n", step.X, step.Y);
		} else if (step.Argument == "select") {
			BeaconClass * beacon = Beacons.Find_Near(Cell(step.X, step.Y));
			Unselect_All();
			if (beacon != NULL) {
				Beacons.Select(beacon);
				DebugString("AUTOTEST   beacon select house %d slot %d\n", beacon->House, beacon->Slot);
			} else {
				DebugString("AUTOTEST   beacon select %d,%d: no beacon\n", step.X, step.Y);
			}
		} else if (step.Argument == "delete") {
			Beacon_Delete_Selected();
			DebugString("AUTOTEST   beacon delete queued\n");
		} else if (step.Argument == "list") {
			for (int house = 0; house < MAX_PLAYERS; house++) {
				for (int slot = 0; slot < BEACONS_PER_HOUSE; slot++) {
					BeaconClass const * beacon = Beacons.Get(house, slot);
					if (beacon != NULL) {
						DebugString("AUTOTEST   beacon house %d slot %d at %d,%d visible %d selected %d text %s\n", beacon->House, beacon->Slot,
							beacon->Where.As_Cell().X, beacon->Where.As_Cell().Y, Beacons.Visible_To_Player(beacon) ? 1 : 0, beacon->IsSelected ? 1 : 0, beacon->Text);
					}
				}
			}
		}
		return;
	}

	// beacontext <text>: sets the text of the player's selected beacon through the beacon event.
	if (step.Command == "beacontext") {
		Beacon_Text_Send(step.Argument.c_str());
		DebugString("AUTOTEST   beacontext queued: %s\n", step.Argument.c_str());
		return;
	}

	// planstate <TypeName>: for each of the player's units of that type, its cell, queued planned orders and whether its plan runs.
	if (step.Command == "planstate") {
		for (int index = 0; index < Feet.Count(); index++) {
			FootClass const * foot = Feet[index];
			if (foot->House == PlayerPtr && stricmp(foot->TClass->Name(), step.Argument.c_str()) == 0) {
				DebugString("AUTOTEST   planstate %s at %d,%d orders %d running %d mission %d leg %d target %d\n", foot->TClass->Name(), foot->Get_Cell().X, foot->Get_Cell().Y,
					foot->PlannedOrders.Count(), foot->IsPlanRunning ? 1 : 0, (int)foot->CurrentMission, foot->HasPlannedRunningOrder ? (int)foot->PlannedRunningOrder.Mission : -1, (int)foot->PlannedRunningOrder.Target.Is_Valid());
			}
		}
		return;
	}

	if (step.Command == "primary") {
		// primary <TypeID>: queues the sidebar's primary command for the player's first building of that type.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->House == PlayerPtr && !building->IsInLimbo && stricmp(building->Class->Name(), step.Argument.c_str()) == 0) {
				OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PRIMARY, TargetClass(building)));
				DebugString("AUTOTEST primary %s at %d,%d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y);
				break;
			}
		}
		return;
	}

	// factories: each structure that builds something, whoever owns it, with what it builds, whether it is naval,
	// whether it is its house's primary factory and whether it is in limbo.
	if (step.Command == "factories") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->Class->ToBuild == RTTI_NONE) continue;
			RTTIType const builds = building->Class->ToBuild;
			DebugString("AUTOTEST   factory %s of %s cell %d,%d builds %s naval %d primary %d limbo %d\n", building->Class->Name(), building->House->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
				builds == RTTI_UNITTYPE ? "vehicle" : builds == RTTI_INFANTRYTYPE ? "infantry" : builds == RTTI_AIRCRAFTTYPE ? "aircraft" : builds == RTTI_BUILDINGTYPE ? "structure" : "other",
				(int)building->Class->IsNaval, (int)building->IsLeader, (int)building->IsInLimbo);
		}
		// Each live production, with the object on its line and how many copies of that type the factory holds.
		for (int index = 0; index < Factories.Count(); index++) {
			FactoryClass * factory = Factories[index];
			TechnoClass const * object = factory->Get_Object();
			DebugString("AUTOTEST   production of %s object %s held %d\n", factory->Get_House()->Class->Name(),
				object != NULL ? object->TClass->Name() : "none", object != NULL ? factory->Total(object->TClass) : 0);
		}
		return;
	}

	// The commands are split into two chains; one chain of them all nests too deeply for MSVC.
	bool handled = true;
	if (step.Command == "command") {
		Execute_Command(step.Argument.c_str());
	} else if (step.Command == "select") {
		Select_Type(step.Argument);
	} else if (step.Command == "selectadd") {
		// selectadd <TypeID>: adds the player's objects of that type to the selection, as a shift-click would.
		For_Player_Objects<UnitClass>(Units, step.Argument, [](UnitClass * object) {object->Select();});
		For_Player_Objects<InfantryClass>(Infantry, step.Argument, [](InfantryClass * object) {object->Select();});
		For_Player_Objects<BuildingClass>(Buildings, step.Argument, [](BuildingClass * object) {object->Select();});
		For_Player_Objects<AircraftClass>(Aircraft, step.Argument, [](AircraftClass * object) {object->Select();});
	} else if (step.Command == "produce") {
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {

			// A cameo is offered only for a type the house may build, and so is a produce step.
			if (PlayerPtr->Can_Build(type, false, true) == 0) {
				DebugString("AUTOTEST   produce %s: no cameo, the house may not build it\n", type->Name());
			} else {
				OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PRODUCE, type->Fetch_RTTI(), type->Fetch_Heap_ID()));
			}
		}
	} else if (step.Command == "produceai") {
		// produceai <TypeID> x y: starts building that type in the structure on that cell, the way BuildingClass::Factory_AI
		// starts a computer house's production, so the structure's own factory holds the object. A structure that is
		// already producing is left alone. The queue runs only the player's events, so this changes this copy only.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		if (type != NULL && building != NULL && building->House != NULL && building->Factory == NULL) {
			building->Factory = new FactoryClass;
			if (building->Factory->Set(*type, *building->House, false)) {
				building->House->Production_Begun(building->Factory->Get_Object());
				building->Factory->Start(false);
				DebugString("AUTOTEST produceai %s for %s at %d,%d started\n", type->Name(), building->House->Class->Name(), step.X, step.Y);
			} else {
				delete building->Factory;
				building->Factory = NULL;
				DebugString("AUTOTEST produceai %s for %s at %d,%d refused\n", type->Name(), building->House->Class->Name(), step.X, step.Y);
			}
		} else {
			DebugString("AUTOTEST produceai %s at %d,%d: no structure, type or free factory\n", step.Argument.c_str(), step.X, step.Y);
		}
	} else if (step.Command == "place") {
		Place(step.Argument);
	} else if (step.Command == "placeat") {
		PlaceAt(step.Argument, step.X, step.Y);
	} else if (step.Command == "move" || step.Command == "order" || step.Command == "cmove") {
		if (step.Command == "move") {
			Move(step.Argument, step.X, step.Y);
		} else if (step.Command == "order") {
			Order(step.Argument, step.X, step.Y);
		} else {
			Computer_Move(step.Argument, step.X, step.Y);
		}
	} else if (step.Command == "hunt") {
		// hunt <TypeID>: puts the computer players' objects of that type on the hunt mission (Computer_Hunt).
		Computer_Hunt(step.Argument);
	} else if (step.Command == "repairmission") {
		// repairmission <TypeID>: puts the player's objects of that type on the repair mission (Player_Repair).
		Player_Repair(step.Argument);
	} else if (step.Command == "attack") {
		Attack(step.Argument);
	} else if (step.Command == "strike") {
		// strike <TypeID> x y: the player's objects of that type attack what stands on that cell.
		TechnoClass * target = Map[Cell(step.X, step.Y)].Cell_Techno();
		DebugString("AUTOTEST strike %s -> %s\n", step.Argument.c_str(), target != NULL ? target->TClass->Name() : "(none)");
		PlanningRecordScope const recording(Map.IsPlanningMode);
		for (int index = 0; target != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if ((techno->House == PlayerPtr || techno->House->Is_Player_Control()) && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				if (Network_Game()) {
					techno->Player_Assign_Mission(MISSION_ATTACK, target);
				} else {
					techno->Assign_Target(target);
					techno->Assign_Mission(MISSION_ATTACK);
				}
			}
		}
	} else if (step.Command == "planmove") {
		// planmove <TypeID> x y: the player's objects of that type are given a move to the cell, as a click on the ground
		// gives them. While planning is on, the move is recorded as a planned leg, so the next leg of the plan follows it.
		Cell const cell(step.X, step.Y);
		PlanningRecordScope const recording(Map.IsPlanningMode);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0 && techno->Is_Foot()) {
				DebugString("AUTOTEST   planmove %s from %d,%d to %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, step.X, step.Y);
				techno->Player_Assign_Mission(MISSION_MOVE, NULL, &Map[cell]);
			}
		}
	} else if (step.Command == "plantbomb") {
		// plantbomb <TypeID>: fixes an Ivan bomb to each of the player's objects of that type, planted by the player's first soldier.
		TechnoClass * planter = NULL;
		for (int index = 0; planter == NULL && index < Infantry.Count(); index++) {
			if (Infantry[index]->House == PlayerPtr && !Infantry[index]->IsInLimbo) {
				planter = Infantry[index];
			}
		}
		for (int index = 0; planter != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Plant_Bomb(planter);
				DebugString("AUTOTEST   plantbomb %s at %d,%d detonates at frame %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, techno->BombDetonateFrame);
			}
		}
	} else if (step.Command == "planguard") {
		// planguard <TypeID> x y: the player's objects of that type are given a guard area order on the cell, as the guard area
		// action gives them. While planning is on, the order is recorded as a planned leg.
		Cell const cell(step.X, step.Y);
		PlanningRecordScope const recording(Map.IsPlanningMode);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0 && techno->Is_Foot()) {
				DebugString("AUTOTEST   planguard %s from %d,%d to %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, step.X, step.Y);
				techno->Player_Assign_Mission(MISSION_GUARD_AREA, &Map[cell]);
			}
		}
	} else if (step.Command == "forceattack") {
		// forceattack <TypeID> x y: the player's objects of that type force-attack the object on that cell, or the ground
		// there when no object stands on it. Autotests report Ctrl as not held, so the order is given as the Ctrl+click
		// outcome: FootClass::Active_Click_With(ACTION_ATTACK) with the object or cell as the target.
		Cell const cell(step.X, step.Y);
		TechnoClass * over = Techno_Over_Cell(cell);
		AbstractClass * target = over != NULL ? static_cast<AbstractClass *>(over) : static_cast<AbstractClass *>(&Map[cell]);
		PlanningRecordScope const recording(Map.IsPlanningMode);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				DebugString("AUTOTEST   forceattack %s at %d,%d on %s (fire %d)\n", techno->TClass->Name(), step.X, step.Y, over != NULL ? over->TClass->Name() : "ground", (int)techno->Can_Player_Fire());
				if (techno->Can_Player_Fire()) {
					techno->Player_Assign_Mission(MISSION_ATTACK, target);
				}
			}
		}
	} else if (step.Command == "counts") {
		// counts <House>: the owned counts, then the active unit and soldier counts the Destroyed all events read
		// (an underscore stands for a space in the name).
		std::string house_name = step.Argument;
		std::replace(house_name.begin(), house_name.end(), '_', ' ');
		HouseClass * owner = House_From_Name(house_name.c_str());
		for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
			if (stricmp(Houses[index]->IniName, house_name.c_str()) == 0) owner = Houses[index];
		}
		if (owner != NULL) {
			DebugString("AUTOTEST   counts %s units %d buildings %d infantry %d aircraft %d active units %d infantry %d\n", step.Argument.c_str(), owner->CurUnits, owner->CurBuildings, owner->CurInfantry, owner->CurAircraft, owner->AUQuantity.Total(), owner->AIQuantity.Total());
		} else {
			DebugString("AUTOTEST   counts %s: no such house\n", step.Argument.c_str());
		}
	} else if (step.Command == "listhouse") {
		// listhouse <House>: writes every object the house has in the object list, limbo'd or not, with what holds it,
		// then the land, naval and infantry counts the land-units event reads and the ones the objects add up to.
		std::string house_name = step.Argument;
		std::replace(house_name.begin(), house_name.end(), '_', ' ');
		HouseClass * owner = House_From_Name(house_name.c_str());
		for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
			if (stricmp(Houses[index]->IniName, house_name.c_str()) == 0) owner = Houses[index];
		}
		if (owner == NULL) {
			DebugString("AUTOTEST   listhouse %s: no such house\n", step.Argument.c_str());
		} else {
			int land = 0, naval = 0, infantry = 0, live = 0, limbo = 0;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass * techno = Technos[index];
				if (techno->House != owner) continue;
				if (techno->IsInLimbo) limbo++; else live++;
				if (techno->What_Am_I() == RTTI_UNIT && techno->IsActive) {
					UnitClass const * unit = static_cast<UnitClass const *>(techno);
					if (unit->Class->IsNaval && unit->Class->MaxPassengers == 0) naval++; else land++;
				}
				if (techno->What_Am_I() == RTTI_INFANTRY && techno->IsActive) infantry++;
				FootClass * foot = dynamic_cast<FootClass *>(techno);
				BuildingClass * building = dynamic_cast<BuildingClass *>(techno);
				char cell[32] = "-";
				if (!techno->IsInLimbo) {
					Cell where = techno->Get_Cell();
					std::snprintf(cell, sizeof(cell), "%d,%d", (int)where.X, (int)where.Y);
				}
				int const visible = (!techno->IsInLimbo && Map[techno->Get_Cell()].IsVisible[PlayerPtr]) ? 1 : 0;
				DebugString("AUTOTEST   listhouse %s obj %d %s rtti %d limbo %d active %d strength %d cell %s insignificant %d dontscore %d counted %d transporter %s intransport %d cargo %d team %s occupants %d visible %d\n",
					owner->Class->Name(), index, techno->TClass->Name(), (int)techno->What_Am_I(), (int)techno->IsInLimbo, (int)techno->IsActive, techno->Strength, cell,
					(int)techno->TClass->IsInsignificant, (int)techno->TClass->IsDontScore, (int)techno->CountedAsOwnedInfantry,
					techno->Transporter != NULL ? techno->Transporter->TClass->Name() : "-", (int)techno->IsInTransport, techno->Cargo.How_Many(),
					(foot != NULL && foot->Team != NULL) ? foot->Team->Class->Name() : "-", building != NULL ? building->Occupants.Count() : 0, visible);
			}
			for (int index = 0; index < Teams.Count(); index++) {
				TeamClass * team = Teams[index];
				if (team->House == owner && !team->Is_Empty()) {
					DebugString("AUTOTEST   listhouse %s team %s with members\n", owner->Class->Name(), team->Class->Name());
				}
			}
			DebugString("AUTOTEST   listhouse %s summary: active land units %d naval units %d infantry %d; objects live %d limbo %d; owned counts units %d infantry %d\n",
				owner->Class->Name(), land, naval, infantry, live, limbo, owner->CurUnits, owner->CurInfantry);
		}
	} else if (step.Command == "enemies") {
		Enemies();
	} else if (step.Command == "threat") {
		// threat <TypeID>: the target the player's first object of the type would pick around itself, and how many it weighed.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				AbstractClass * target = techno->Greatest_Threat(ThreatType(THREAT_RANGE|THREAT_AREA), techno->Center_Coord(), false);
				ObjectClass const * object = dynamic_cast<ObjectClass const *>(target);
				DebugString("AUTOTEST   threat %s picks %s (%d candidates)\n", techno->TClass->Name(), object != NULL ? object->Class_Of()->Name() : "-", (int)techno->ThreatCandidates.size());
				break;
			}
		}
	} else if (step.Command == "threatscan") {
		// threatscan <TypeID> <ThreatType flags>: the target the first object of the type owned by a house that is no ally of the player picks anywhere on the map for those flags.
		unsigned const flags = (unsigned)step.X;
		std::string const type_name = step.Argument;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (!techno->House->Is_Ally(PlayerPtr) && !techno->IsInLimbo && stricmp(techno->TClass->Name(), type_name.c_str()) == 0) {
				AbstractClass * target = techno->Greatest_Threat(ThreatType(flags), techno->Center_Coord(), false);
				ObjectClass const * object = dynamic_cast<ObjectClass const *>(target);
				DebugString("AUTOTEST   threatscan %s of %s picks %s at %d,%d\n", type_name.c_str(), techno->House->Class->Name(), object != NULL ? object->Class_Of()->Name() : "-",
					object != NULL ? object->Get_Cell().X : -1, object != NULL ? object->Get_Cell().Y : -1);
				break;
			}
		}
	} else if (step.Command == "bunkers") {
		// bunkers: each Bunker=yes structure and the vehicle inside it.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * bunker = Buildings[index];
			if (bunker->Class->IsBunker && !bunker->IsInLimbo) {
				TechnoClass const * inside = bunker->BunkerLinkedItem;
				DebugString("AUTOTEST   bunker %s cell %d,%d holds %s\n", bunker->Class->Name(), bunker->Get_Cell().X, bunker->Get_Cell().Y, inside != NULL ? inside->TClass->Name() : "nothing");
			}
		}
	} else if (step.Command == "owners") {
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   %s Ownable %08X RequiredHouses %08X ForbiddenHouses %08X AIBasePlanningSide %d\n", type->Name(), type->Ownable, type->RequiredHouses, type->ForbiddenHouses, type->AIBasePlanningSide);
		}
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			DebugString("AUTOTEST   house %s ActLike %d mask %08X own %d ownmask %08X conyards %d\n", house->Class->Name(), (int)house->ActLike, house->Acted_Mask(), (int)house->OwnCountry, house->Own_Mask(), house->ConYards.Count());
		}
	} else if (step.Command == "view") {
		FollowType.clear();
		// A trigger scroll still under way, such as a mission's opening camera pan, would carry the view back next frame.
		TacticalMap->MoveFrom = Point2D(0, 0);
		TacticalMap->MoveTo = Point2D(0, 0);
		TacticalMap->MoveSpeed = 0;
		TacticalMap->MoveFactor = 0;
		Coord coord = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Center_Coord();
		TacticalMap->Set_Tactical_Position(coord);
		Point2D wanted = TacticalMap->Coord_To_Pixel_Absolute(coord);
		Point2D actual = TacticalMap->Get_Tactical_Position();
		DebugString("AUTOTEST   view wanted %d,%d actual %d,%d local %d,%d %dx%d play %dx%d\n", wanted.X, wanted.Y, actual.X, actual.Y,
			Map.LocalRect.X, Map.LocalRect.Y, Map.LocalRect.Width, Map.LocalRect.Height, Map.PlayRect.Width, Map.PlayRect.Height);
	} else if (step.Command == "click") {
		int x = std::atoi(step.Argument.c_str());
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.X = x;
		event.Y = step.X;
		Keyboard->Handle_Window_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Keyboard->Handle_Window_Event(event);
	} else if (step.Command == "windowmove") {
		// windowmove x y: moves the pointer to that window pixel, for the hover and placement code that reads it.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_MOVE;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		_CursorX = event.X;
		_CursorY = event.Y;
		Game_Window_Handle_Event(event);
	} else if (step.Command == "windowclick" || step.Command == "windowdouble") {
		// windowclick x y: a left click at that window pixel, taken from the window the way the mouse gives it,
		// so it is converted to the frame as a player's click is. windowdouble x y is the same click as the second
		// of a quick pair, whose press SDL reports with Clicks 2.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_MOVE;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		_CursorX = event.X;
		_CursorY = event.Y;
		Game_Window_Handle_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.Button = WINDOW_BUTTON_LEFT;
		event.Clicks = (step.Command == "windowdouble") ? 2 : 1;
		Game_Window_Handle_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Game_Window_Handle_Event(event);
	} else if (step.Command == "windowpress" || step.Command == "windowrelease") {
		// windowpress x y and windowrelease x y: move the pointer to that window pixel and press or release the left
		// button there, so a drag can start on the map and end over the command bar or the panel.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_MOVE;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		_CursorX = event.X;
		_CursorY = event.Y;
		Game_Window_Handle_Event(event);
		event.Type = (step.Command == "windowpress") ? WINDOW_EVENT_MOUSE_DOWN : WINDOW_EVENT_MOUSE_UP;
		event.Button = WINDOW_BUTTON_LEFT;
		event.Clicks = 1;
		Game_Window_Handle_Event(event);
	} else if (step.Command == "xbutton") {
		// xbutton <1|2>: a press and release of a side button at the pointer, given to the window the way the mouse gives it.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.Button = (step.Argument.find('2') != std::string::npos) ? WINDOW_BUTTON_X2 : WINDOW_BUTTON_X1;
		event.Clicks = 1;
		event.X = _CursorX;
		event.Y = _CursorY;
		Game_Window_Handle_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Game_Window_Handle_Event(event);
	} else if (step.Command == "mapclick") {
		// mapclick x y: a left click at that screen point, given straight to the map the way the window gives it.
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_DOWN;
		event.Button = WINDOW_BUTTON_LEFT;
		event.Clicks = 1;
		event.X = std::atoi(step.Argument.c_str());
		event.Y = step.X;
		Map.Handle_Window_Event(event);
		event.Type = WINDOW_EVENT_MOUSE_UP;
		Map.Handle_Window_Event(event);
	} else if (step.Command == "cell") {
		CellClass const & cell = Map[Cell(std::atoi(step.Argument.c_str()), step.X)];
		DebugString("AUTOTEST   cell building %s occupier %s\n", cell.Cell_Building() != NULL ? cell.Cell_Building()->Class->Name() : "-", cell.Cell_Occupier() != NULL ? cell.Cell_Occupier()->Class_Of()->Name() : "-");
		ObjectClass const * occupier = cell.Cell_Occupier();
		TechnoClass const * techno = dynamic_cast<TechnoClass const *>(occupier);
		DebugString("AUTOTEST   cell %d,%d mapped %d visible %d fogmapped %d tile %d height %d level %d overlay %d occupier %s rad %d ambient %d brightness %d lights %d gap %d land %d sensed %d cloak %d\n",
			std::atoi(step.Argument.c_str()), step.X, (int)cell.IsMapped[PlayerPtr], (int)cell.IsVisible[PlayerPtr], (int)cell.IsFogMapped[PlayerPtr],
			(int)cell.ITType, (int)cell.Height, (int)cell.Elevation, (int)cell.Overlay, occupier != NULL ? occupier->Class_Of()->Name() : "-", (int)cell.RadLevel, (int)cell.Ambient, (int)cell.Brightness, LightSources.Count(), cell.GapCount, (int)cell.Land_Type(),
			(int)cell.Is_Sensed(PlayerPtr), techno != NULL ? (int)techno->Cloak : -1);
	} else if (step.Command == "water") {
		// water x y: logs the open water cell nearest to that cell.
		Cell const from(std::atoi(step.Argument.c_str()), step.X);
		Cell best = CELL_NONE;
		int bestdist = INT_MAX;
		for (int y = 0; y < MAP_CELL_H; y++) {
			for (int x = 0; x < MAP_CELL_W; x++) {
				Cell const where(x, y);
				if (Map.In_Radar(where) && Map[where].Land_Type() == LAND_WATER && !Map[where].IsUnderBridge) {
					int const dist = (x - from.X) * (x - from.X) + (y - from.Y) * (y - from.Y);
					if (dist < bestdist) {
						bestdist = dist;
						best = where;
					}
				}
			}
		}
		DebugString("AUTOTEST   water nearest %d,%d: %d,%d\n", from.X, from.Y, best.X, best.Y);
	} else if (step.Command == "terrain") {
		// terrain x y: logs the land type (LandType, 2 is water, 6 is beach), whether the cell is a shore piece and whether it holds any water.
		CellClass const & cell = Map[Cell(std::atoi(step.Argument.c_str()), step.X)];
		DebugString("AUTOTEST   terrain %d,%d land %d shore %d water %d\n", std::atoi(step.Argument.c_str()), step.X, (int)cell.Land_Type(), (int)cell.Is_Tile_Shore(), (int)cell.Is_Tile_With_Water());
	} else if (step.Command == "follow") {
		FollowType = step.Argument;
	} else if (step.Command == "enter") {
		// enter <TypeID> x y [enemy]: the player's objects of that type enter the structure, or else the vehicle, on that cell.
		// With enemy, the objects of the type that the player does not own and is not allied with enter it instead.
		bool const enemy_enters = Word(Line_Words(step.Line), 5) == "enemy";
		TechnoClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		if (building == NULL) {
			building = Map[Cell(step.X, step.Y)].Cell_Unit();
		}
		DebugString("AUTOTEST enter %s -> %s\n", step.Argument.c_str(), building != NULL ? building->TClass->Name() : "(none)");
		for (int index = 0; building != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			bool const owned = enemy_enters ? (techno->House != PlayerPtr && !techno->House->Is_Ally(PlayerPtr)) : techno->House == PlayerPtr;
			if (techno->Is_Foot() && owned && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Mission(MISSION_ENTER);
				techno->Assign_Destination(building);
			}
		}
	} else if (step.Command == "spy") {
		// spy <TypeID> x y: the player's objects of that type get the capture order a click on an enemy structure gives
		// (FootClass::Active_Click_With ACTION_CAPTURE, which sets the structure as the destination). Infiltration happens on arrival.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		DebugString("AUTOTEST spy %s -> %s\n", step.Argument.c_str(), building != NULL ? building->TClass->Name() : "(none)");
		for (int index = 0; building != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Is_Foot() && (techno->House == PlayerPtr || techno->House->Is_Player_Control()) && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Player_Assign_Mission(MISSION_CAPTURE, NULL, building);
			}
		}
	} else if (step.Command == "teamspy") {
		// teamspy <TypeID> x y: the player's objects of that type get the capture mission and target a team's spy mission gives
		// (TeamClass, team.cpp), with no destination set. The spy's own target assignment must then set one.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		DebugString("AUTOTEST teamspy %s -> %s\n", step.Argument.c_str(), building != NULL ? building->TClass->Name() : "(none)");
		for (int index = 0; building != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Is_Foot() && (techno->House == PlayerPtr || techno->House->Is_Player_Control()) && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Mission(MISSION_CAPTURE);
				techno->Assign_Target(building);
			}
		}
	} else if (step.Command == "unload") {
		// unload <x> <y>: the structure on that cell unloads; otherwise the vehicle on it unloads its passengers.
		Cell const cell(std::atoi(step.Argument.c_str()), step.X);
		BuildingClass * building = Map[cell].Cell_Building();
		if (building != NULL) {
			building->Assign_Mission(MISSION_UNLOAD);
		} else {
			TechnoClass * techno = Map[cell].Cell_Techno();
			if (techno != NULL && techno->RTTI == RTTI_UNIT) {
				DebugString("AUTOTEST unload %s at %d,%d\n", techno->TClass->Name(), cell.X, cell.Y);
				techno->Assign_Mission(MISSION_UNLOAD);
			}
		}
	} else if (step.Command == "garrisons") {
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * building = Buildings[index];
			if (building->Class->IsCanBeOccupied && (building->Occupants.Count() > 0 || building->Class->MaxNumberOccupants > 0)) {
				DebugString("AUTOTEST   garrison %s cell %d,%d house %s occupants %d/%d strength %d fire %d tar %d arm %d mission %d queue %d ready %d firing %d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
					building->House->Class->Name(), building->Occupants.Count(), building->Class->MaxNumberOccupants, building->Strength, (int)building->Can_Occupy_Fire(), (int)(building->TarCom != NULL), (int)building->Arm, (int)building->Mission, (int)building->MissionQueue, (int)building->IsReadyToCommence, building->FiringOccupantIndex);
			}
		}
	} else if (step.Command == "radar") {
		// radar <TypeID>: each player object of that type, with whether it is in limbo, tracked on the radar and discovered,
		// and whether Is_Radar_Visible plots it.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				DetectedType detected = DETECTED_NONE;
				bool const visible = techno->Is_Radar_Visible(detected);
				DebugString("AUTOTEST   radar %s at %d,%d limbo %d tracked %d discovered %d pos %d,%d visible %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y,
					techno->IsInLimbo ? 1 : 0, techno->IsRadarTracked ? 1 : 0, techno->DiscoveredBy[PlayerPtr] ? 1 : 0, techno->RadarPos.X, techno->RadarPos.Y, visible ? 1 : 0);
			}
		}
	} else if (step.Command == "radarall") {
		// radarall <TypeID>: each object of that type whoever owns it, with its owner, its cell and whether Is_Radar_Visible plots it.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				DetectedType detected = DETECTED_NONE;
				bool const visible = techno->Is_Radar_Visible(detected);
				DebugString("AUTOTEST   radarall %s house %s at %d,%d limbo %d discovered %d visible %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y,
					techno->IsInLimbo ? 1 : 0, techno->DiscoveredBy[PlayerPtr] ? 1 : 0, visible ? 1 : 0);
			}
		}
	} else if (step.Command == "stripfill") {
		// stripfill <column> <count>: offers the sidebar strip of that column the building types 0 to count-1, as the sidebar
		// does for each type that comes to it, and writes how many were taken and how many the strip holds.
		int const column = std::atoi(step.Argument.c_str());
		bool const valid = column >= 0 && column < SidebarClass::COLUMNS;
		int const before = valid ? Map.Column[column].BuildableCount : -1;
		int taken = 0;
		int refused = 0;
		if (valid) {
			for (int id = 0; id < step.X && id < BuildingTypes.Count(); id++) {
				if (Map.Column[column].Add(RTTI_BUILDINGTYPE, id)) {
					taken++;
				} else {
					refused++;
				}
			}
		}
		DebugString("AUTOTEST   stripfill column %d before %d taken %d refused %d count %d\n", column, before, taken, refused, valid ? Map.Column[column].BuildableCount : -1);
	} else if (step.Command == "spawn" || step.Command == "grantunit") {
		// spawn <TypeID> x y: puts an object of the type, owned by the first computer house, on that cell.
		// grantunit <TypeID> x y: the same, owned by the player.
		bool const player_owned = step.Command == "grantunit";
		HouseClass * enemy = player_owned ? PlayerPtr : NULL;
		for (int index = 0; !player_owned && index < Houses.Count(); index++) {
			if (Houses[index] != PlayerPtr && Houses[index]->ConYards.Count() > 0) {
				enemy = Houses[index];
				break;
			}
		}
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (enemy != NULL && type != NULL) {
			Cell cell(step.X, step.Y);
			if (Footprint_Free(step.Command.c_str(), type, cell, enemy)) {
				TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(enemy));
				ScenarioInit++;
				bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
				ScenarioInit--;
				if (placed) {
					object->Assign_Mission(MISSION_GUARD);
				}
				DebugString("AUTOTEST %s %s at %d,%d: %s\n", step.Command.c_str(), type->Name(), step.X, step.Y, placed ? "placed" : "failed");
			}
		} else {
			DebugString("AUTOTEST %s %s: %s\n", step.Command.c_str(), step.Argument.c_str(), type == NULL ? "no such type" : "no computer house with a construction yard");
		}
	} else if (step.Command == "houseunit") {
		// houseunit <TypeID>@<House> x y: puts an object of the type, owned by the named house, on that cell, on Guard, as spawn does.
		// An underscore in the house name stands for a space.
		std::string type_name = step.Argument;
		std::string house_name;
		std::size_t const at = type_name.find('@');
		if (at != std::string::npos) {
			house_name = type_name.substr(at + 1);
			type_name.erase(at);
		}
		std::replace(house_name.begin(), house_name.end(), '_', ' ');
		HouseClass * owner = NULL;
		if (!house_name.empty() && house_name[0] == '#') {
			// #<n> names the house at position n in the house list, for houses with no spawn name.
			int const position = std::atoi(house_name.c_str() + 1);
			owner = position >= 0 && position < Houses.Count() ? Houses[position] : NULL;
		} else {
			owner = House_From_Name(house_name.c_str());
			for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
				if (stricmp(Houses[index]->IniName, house_name.c_str()) == 0) {
					owner = Houses[index];
				}
			}
		}
		TechnoTypeClass const * type = Find_Type(type_name);
		if (owner != NULL && type != NULL) {
			Cell cell(step.X, step.Y);
			if (Footprint_Free(step.Command.c_str(), type, cell, owner)) {
				TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(owner));
				ScenarioInit++;
				bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
				ScenarioInit--;
				if (placed) {
					object->Assign_Mission(MISSION_GUARD);
				}
				DebugString("AUTOTEST houseunit %s %s at %d,%d: %s\n", type->Name(), owner->Class->Name(), step.X, step.Y, placed ? "placed" : "failed");
			}
		} else {
			DebugString("AUTOTEST houseunit %s: %s\n", step.Argument.c_str(), type == NULL ? "no such type" : "no such house");
		}
	} else if (step.Command == "own" || step.Command == "neutral" || step.Command == "enemy") {
		// own <TypeID> x y: puts an object of the type, owned by the player, on that cell. neutral does the
		// same for the house of the Civilian side. enemy does it for the first computer house with a
		// construction yard that the player is not allied with.
		HouseClass * owner = PlayerPtr;
		if (step.Command == "neutral") {
			owner = NULL;
			SideType const civilian = SideClass::From_Name("Civilian");
			for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
				if (Houses[index]->Class->Side == civilian) {
					owner = Houses[index];
				}
			}
		} else if (step.Command == "enemy") {
			owner = NULL;
			for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
				if (Houses[index] != PlayerPtr && Houses[index]->ConYards.Count() > 0 && !PlayerPtr->Is_Ally(Houses[index])) {
					owner = Houses[index];
				}
			}
		}
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL && owner != NULL) {
			Cell cell(step.X, step.Y);
			if (Footprint_Free(step.Command.c_str(), type, cell, owner)) {
				TechnoClass * object = static_cast<TechnoClass *>(type->Create_One_Of(owner));
				ScenarioInit++;
				bool placed = object != NULL && object->Unlimbo(Map[cell].Center_Coord(), DIR_N);
				ScenarioInit--;
				DebugString("AUTOTEST %s %s at %d,%d: %s\n", step.Command.c_str(), type->Name(), step.X, step.Y, placed ? "placed" : "failed");
			}
		} else {
			DebugString("AUTOTEST %s %s: %s\n", step.Command.c_str(), step.Argument.c_str(), type == NULL ? "no such type" : "no civilian house");
		}
	} else if (step.Command == "face") {
		// face <dir256> x y: turns the body and turret of the player's objects on that cell to that facing.
		Cell const cell(step.X, step.Y);
		Dir256 const facing = static_cast<Dir256>(std::atoi(step.Argument.c_str()) & 255);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Get_Cell() == cell) {
				techno->PrimaryFacing.Set(DirType(facing));
				techno->SecondaryFacing.Set(DirType(facing));
				DebugString("AUTOTEST   face %s at %d,%d to %d\n", techno->TClass->Name(), cell.X, cell.Y, (int)facing);
			}
		}
	} else if (step.Command == "pitch") {
		// pitch <dir256> x y: sets the barrel pitch of the player's objects on that cell, 64 level and 0 straight up.
		Cell const cell(step.X, step.Y);
		int const value = std::atoi(step.Argument.c_str()) & 255;
		DirType const pitch = DirType(value);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Get_Cell() == cell) {
				techno->BarrelPitch.Set(pitch);
				DebugString("AUTOTEST   pitch %s at %d,%d to %d\n", techno->TClass->Name(), cell.X, cell.Y, value);
			}
		}
	} else if (step.Command == "barrelpitch") {
		// barrelpitch <dir256> x y: sets the barrel pitch of the player's objects on that cell, 64 level and 0 straight up. The pitch verb takes a raw facing instead.
		Cell const cell(step.X, step.Y);
		Dir256 const value = static_cast<Dir256>(std::atoi(step.Argument.c_str()) & 255);
		DirType const pitch = DirType(value);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Get_Cell() == cell) {
				techno->BarrelPitch.Set(pitch);
				DebugString("AUTOTEST   barrelpitch %s at %d,%d to %d\n", techno->TClass->Name(), cell.X, cell.Y, (int)value);
			}
		}
	} else if (step.Command == "voxelshade") {
		// voxelshade <type>: shade index of each normal in that table under VoxelLightSource, for the diffuse term alone and for the highlight at the rules' strength seen straight on.
		int const normal_type = std::atoi(step.Argument.c_str());
		if (normal_type < 1 || normal_type > 4) {
			DebugString("AUTOTEST voxelshade: type %d is not a normal table\n", normal_type);
		} else {
			int const count = VoxelNormalTableEntryCount[normal_type];
			std::vector<int> diffuse(count);
			std::vector<int> specular(count);
			float const scale = (float)Rule->VoxelShadeScale;
			Precalculate_Normal_Lookup(VoxelLightSource, normal_type, scale);
			for (int index = 0; index < count; index++) {
				diffuse[index] = VoxelNormalTranslateTable[index];
			}
			Precalculate_Normal_Lookup(VoxelLightSource, Vector3(0, 0, 1), (float)Rule->VoxelSpecularStrength, normal_type, scale);
			for (int index = 0; index < count; index++) {
				specular[index] = VoxelNormalTranslateTable[index];
			}
			DebugString("AUTOTEST voxelshade type %d normals %d light %.4f %.4f %.4f\n", normal_type, count, VoxelLightSource.X, VoxelLightSource.Y, VoxelLightSource.Z);
			for (int index = 0; index < count; index++) {
				DebugString("AUTOTEST   normal %d diffuse %d specular %d\n", index, diffuse[index], specular[index]);
			}
			int diffuse_hist[256] = { 0 };
			int specular_hist[256] = { 0 };
			for (int index = 0; index < count; index++) {
				diffuse_hist[diffuse[index] & 255]++;
				specular_hist[specular[index] & 255]++;
			}
			for (int shade = 0; shade < 256; shade++) {
				if (diffuse_hist[shade] != 0 || specular_hist[shade] != 0) {
					DebugString("AUTOTEST   shade %d diffuse %d specular %d\n", shade, diffuse_hist[shade], specular_hist[shade]);
				}
			}
		}
	} else if (step.Command == "hurt") {
		// hurt <TypeID> <percent>: sets the strength of the player's objects of that type to that share.
		int percent = step.X;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Strength = std::max(1, techno->TClass->MaxStrength * percent / 100);
			}
		}
	} else if (step.Command == "warpout") {
		// warpout <TypeID> <0|1>: sets or clears the warping-out flag on the player's objects of that type, without a warp effect.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->IsBeingWarpedOut = step.X != 0;
				DebugString("AUTOTEST   warpout %s at %d,%d: %d strength %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->IsBeingWarpedOut, (int)techno->Strength);
			}
		}
	} else if (step.Command == "cover") {
		// cover x y: how many buildings screen the cell, and whether an object there counts as hidden.
		Cell cell(std::atoi(step.Argument.c_str()), step.X);
		CellClass const & cellptr = Map[cell];
		DebugString("AUTOTEST   cover %d,%d count %d covered %d\n", cell.X, cell.Y, cellptr.OccupyHeightsCoveringMe, (int)cellptr.Is_Covered());
		for (ObjectClass const * object = cellptr.Cell_Occupier(); object != NULL; object = object->Next) {
			if (object->Is_Techno()) {
				TechnoClass const * techno = static_cast<TechnoClass const *>(object);
				DebugString("AUTOTEST   cover   %s hidden %d\n", techno->TClass->Name(), (int)techno->Is_Hidden_Behind_Building());
			}
		}
	} else if (step.Command == "hiddenmarker") {
		// hiddenmarker <mode>: 0 hides the Behind marker, 1 shows it, 2 drops the Behind animation so brackets are drawn.
		int mode = std::atoi(step.Argument.c_str());
		Options.ShowHidden = (mode != 0);
		if (mode == 2) {
			Rule->Behind = NULL;
		}
	} else if (step.Command == "reveal") {
		// reveal: uncovers the whole map, shroud and fog, for the player.
		Map.Reveal_The_Map(PlayerPtr, true);
	} else if (step.Command == "ini") {
		// ini <section>: every entry of that section of the rules file, as the game read it.
		CCINIClass const & ini = *RuleINI;
		int count = ini.Entry_Count(step.Argument.c_str());
		for (int index = 0; index < count; index++) {
			char const * entry = ini.Get_Entry(step.Argument.c_str(), index);
			char value[256] = "";
			ini.Get_String(step.Argument.c_str(), entry, "", value, sizeof(value));
			DebugString("AUTOTEST   ini [%s] %s=%s\n", step.Argument.c_str(), entry, value);
		}
	} else if (step.Command == "speak") {
		// speak <EVA name>: queues that announcer line.
		Speak_Eva(step.Argument.c_str());
	} else if (step.Command == "evafile") {
		// evafile <EVA name>: opens that announcer line's sample for the player's side at no volume.
		std::string const file = Eva_Sample_File(step.Argument.c_str());
		AudioHandle handle = file.empty() ? AudioHandle() : AudioEngine.Open_Stream(file.c_str(), AUDIO_GROUP_SPEECH, 0.0f, false);
		DebugString("AUTOTEST   evafile %s: %s %s\n", step.Argument.c_str(), file.empty() ? "-" : file.c_str(), handle.Is_Valid() ? "opened" : "failed");
		if (handle.Is_Valid()) {
			AudioEngine.Stop_Stream(handle);
		}
	} else if (step.Command == "musicstage") {
		// musicstage: writes sample files into the Music folder unless they are there: two WAV tracks, an empty MP3, a text file posing as an OGG and a text file.
		std::string const folder = Music_Folder();
		for (int stereo = 0; stereo < 2; stereo++) {
			std::string const path = folder + (stereo ? "Stage Stereo.wav" : "Stage Mono.wav");
			std::ifstream exists(path, std::ios::binary);
			if (exists) {
				continue;
			}
			unsigned const rate = 22050, channels = stereo ? 2 : 1, frames = rate;
			std::vector<unsigned char> wav;
			auto put32 = [&wav](unsigned value) { for (int i = 0; i < 4; i++) wav.push_back((unsigned char)(value >> (8 * i))); };
			auto put16 = [&wav](unsigned value) { for (int i = 0; i < 2; i++) wav.push_back((unsigned char)(value >> (8 * i))); };
			wav.insert(wav.end(), { 'R', 'I', 'F', 'F' });
			put32(36 + frames * channels * 2);
			wav.insert(wav.end(), { 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ' });
			put32(16);
			put16(1);
			put16(channels);
			put32(rate);
			put32(rate * channels * 2);
			put16(channels * 2);
			put16(16);
			wav.insert(wav.end(), { 'd', 'a', 't', 'a' });
			put32(frames * channels * 2);
			for (unsigned frame = 0; frame < frames; frame++) {
				int const sample = ((frame / 25) & 1) ? 6000 : -6000;
				for (unsigned channel = 0; channel < channels; channel++) {
					put16((unsigned)(sample & 0xFFFF));
				}
			}
			std::ofstream out(path, std::ios::binary);
			out.write((char const *)wav.data(), (std::streamsize)wav.size());
		}
		{ std::ofstream out(folder + "Stage Empty.mp3", std::ios::binary); }
		{ std::ofstream out(folder + "Stage Broken.ogg", std::ios::binary); out << "This is not an Ogg file."; }
		{ std::ofstream out(folder + "Stage Notes.txt", std::ios::binary); out << "Not music."; }
		DebugString("AUTOTEST   musicstage %s\n", folder.c_str());
	} else if (step.Command == "musicscan") {
		// musicscan: reads the Music folder again, as the game does at start-up.
		Theme.Scan();
	} else if (step.Command == "musicmode") {
		// musicmode <Both|Stock|Custom>: sets the custom music option and logs how many tracks the playlist then offers.
		CustomMusicModeType mode = CUSTOM_MUSIC_BOTH;
		if (!Custom_Music_Mode_From_Name(step.Argument.c_str(), mode)) {
			DebugString("AUTOTEST   musicmode %s: unknown\n", step.Argument.c_str());
		} else {
			Options.Set_Custom_Music(mode);
			int allowed = 0, custom = 0;
			for (ThemeType theme = THEME_FIRST; theme < Theme.Max_Themes(); theme = ThemeType(theme + 1)) {
				if (Theme.Is_Allowed(theme)) {
					allowed++;
					custom += Theme.Is_Custom(theme) ? 1 : 0;
				}
			}
			DebugString("AUTOTEST   musicmode %s: %d allowed, %d custom\n", Custom_Music_Mode_Name(mode), allowed, custom);
		}
	} else if (step.Command == "musiclist") {
		// musiclist: every custom track the theme player holds, with its title, whether the playlist allows it and how long it plays.
		DebugString("AUTOTEST   musiclist %d tracks\n", Theme.Custom_Count());
		for (ThemeType theme = THEME_FIRST; theme < Theme.Max_Themes(); theme = ThemeType(theme + 1)) {
			if (Theme.Is_Custom(theme)) {
				DebugString("AUTOTEST   music %d \"%s\" allowed %d length %d\n", (int)theme, Theme.Full_Name(theme), (int)Theme.Is_Allowed(theme), Theme.Track_Length(theme));
			}
		}
	} else if (step.Command == "musicopen") {
		// musicopen <n>: opens the stream of the n-th custom track, counting from 0, with no volume and closes it again.
		int wanted = std::atoi(step.Argument.c_str());
		int seen = 0;
		bool found = false;
		for (ThemeType theme = THEME_FIRST; theme < Theme.Max_Themes() && !found; theme = ThemeType(theme + 1)) {
			if (Theme.Is_Custom(theme) && seen++ == wanted) {
				found = true;
				AudioHandle handle = AudioEngine.Open_Stream(Theme.File_Name(theme), AUDIO_GROUP_MUSIC, 0.0f, false);
				DebugString("AUTOTEST   musicopen %d \"%s\": %s\n", wanted, Theme.Full_Name(theme), handle.Is_Valid() ? "opened" : "failed");
				if (handle.Is_Valid()) {
					AudioEngine.Stop_Stream(handle);
				}
			}
		}
		if (!found) {
			DebugString("AUTOTEST   musicopen %d: not on the list\n", wanted);
		}
	} else if (step.Command == "maps") {
		// maps: every XMP<nn><letter><digit>.MAP multiplayer map the file system finds.
		for (int number = 0; number < 100; number++) {
			for (char letter = 'A'; letter <= 'Z'; letter++) {
				for (char digit = '0'; digit <= '9'; digit++) {
					char name[32];
					std::snprintf(name, sizeof(name), "XMP%02d%c%c.MAP", number, letter, digit);
					if (CCFileClass(name).Is_Available()) {
						DebugString("AUTOTEST   map %s\n", name);
					}
				}
			}
		}
	} else if (step.Command == "inifile") {
		// inifile <FILE.INI>: every section and entry of that file, as the game's file system finds it.
		CCINIClass ini;
		CCFileClass file(step.Argument.c_str());
		if (!file.Is_Available() || !ini.Load(file, false)) {
			DebugString("AUTOTEST   inifile %s: not found\n", step.Argument.c_str());
		}
		for (int section = 0; section < ini.Section_Count(); section++) {
			char const * name = ini.Section_Name(section);
			int count = ini.Entry_Count(name);
			for (int index = 0; index < count; index++) {
				char const * entry = ini.Get_Entry(name, index);
				char value[256] = "";
				ini.Get_String(name, entry, "", value, sizeof(value));
				DebugString("AUTOTEST   inifile [%s] %s=%s\n", name, entry, value);
			}
		}
	} else if (step.Command == "grant") {
		// grant <SuperWeaponTypeID>: gives the player that super weapon, fully charged.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		if (id != SUPER_NONE && id < PlayerPtr->SuperWeapon.Count()) {
			SuperClass * super = PlayerPtr->SuperWeapon[id];
			// Quiet when the house is short of power, as Enable_Available_Super_Weapons grants it.
			super->Enable(false, true, PlayerPtr->Power_Fraction() < 1.0);
			super->Forced_Charge(true);
			DebugString("AUTOTEST   grant %s type %d ready %d\n", step.Argument.c_str(), (int)super->Class->Type, (int)super->Is_Ready());
		} else {
			DebugString("AUTOTEST   grant %s: no such super weapon\n", step.Argument.c_str());
		}
	} else if (step.Command == "enable") {
		// enable <SuperWeaponTypeID>: gives the player that super weapon the way a structure does, to charge from empty.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		if (id != SUPER_NONE && id < PlayerPtr->SuperWeapon.Count()) {
			PlayerPtr->SuperWeapon[id]->Enable(false, true, false);
			DebugString("AUTOTEST   enable %s ready %d\n", step.Argument.c_str(), (int)PlayerPtr->SuperWeapon[id]->Is_Ready());
		}
	} else if (step.Command == "grantai") {
		// grantai <SuperWeaponTypeID>: gives the first computer house with a construction yard that super weapon, fully charged.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		for (int index = 0; index < Houses.Count() && id != SUPER_NONE; index++) {
			HouseClass * house = Houses[index];
			if (house != PlayerPtr && house->ConYards.Count() > 0 && id < house->SuperWeapon.Count()) {
				SuperClass * super = house->SuperWeapon[id];
				// Quiet when the house is short of power, as Enable_Available_Super_Weapons grants it.
				super->Enable(false, false, house->Power_Fraction() < 1.0);
				super->Forced_Charge(true);
				DebugString("AUTOTEST   grantai %s to %s ready %d\n", step.Argument.c_str(), house->Class->Name(), (int)super->Is_Ready());
				break;
			}
		}
	} else if (step.Command == "chargeai") {
		// chargeai <SuperWeaponTypeID> <percent>: sets how charged the first computer house with a construction yard's super weapon is, as a SetSuperCharge trigger action does (SuperClass::Set_Charge).
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		for (int index = 0; index < Houses.Count() && id != SUPER_NONE; index++) {
			HouseClass * house = Houses[index];
			if (house != PlayerPtr && house->ConYards.Count() > 0 && id < house->SuperWeapon.Count()) {
				SuperClass * super = house->SuperWeapon[id];
				super->Set_Charge(step.X);
				DebugString("AUTOTEST   chargeai %s to %s percent %d ready %d suspended %d\n", step.Argument.c_str(), house->Class->Name(), step.X, (int)super->Is_Ready(), (int)super->Is_Suspended());
				break;
			}
		}
	} else if (step.Command == "fire") {
		// fire <SuperWeaponTypeID> x y: the player fires that super weapon at the cell.
		SuperWeaponType id = SuperWeaponTypeClass::From_Name(step.Argument.c_str());
		if (id != SUPER_NONE) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::SPECIAL_PLACE, id, Cell(step.X, step.Y)));
		}
	} else if (step.Command == "rules") {
		// rules <path>: reads that INI file over the rules, as a map's rule overrides are read.
		CCINIClass ini;
		std::string const resolved = AutoTest_Input_Path(step.Argument);
		RawFileClass file(resolved.c_str());
		if (file.Is_Available() && ini.Load(file, false)) {
			Rule->Addition(ini);
		} else {
			DebugString("AUTOTEST   rules %s: not found\n", resolved.c_str());
		}
	} else if (step.Command == "reload") {
		// reload: reads the rules, art and map overrides again, as the Reload rules command does.
		DebugString("AUTOTEST   reload %d\n", (int)Reload_Rules());
	} else if (step.Command == "art") {
		// art <path>: reads that INI file over the art, then has the types it names read themselves again.
		CCINIClass ini;
		std::string const resolved = AutoTest_Input_Path(step.Argument);
		RawFileClass file(resolved.c_str());
		RawFileClass artfile(resolved.c_str());
		if (file.Is_Available() && ini.Load(file, false) && ArtINI.Load(artfile, false)) {
			Rule->Addition(ini);
		} else {
			DebugString("AUTOTEST   art %s: not found\n", resolved.c_str());
		}
	} else if (step.Command == "pcxcameo") {
		// pcxcameo <path>: the size of that PCX cameo and two of its pixels, as the sidebar gets them.
		Surface const * picture = PCX_Cameo(step.Argument);
		if (picture != NULL) {
			unsigned short const * pixels = (unsigned short const *)picture->Lock();
			DebugString("AUTOTEST   pcxcameo %dx%d bpp %d corner %04X far %04X\n", picture->Get_Width(), picture->Get_Height(), picture->Bytes_Per_Pixel(),
				pixels != NULL ? pixels[0] : 0, pixels != NULL ? pixels[picture->Get_Width() * picture->Get_Height() - 1] : 0);
			picture->Unlock();
		}
	} else if (step.Command == "truecolour") {
		// truecolour <NAME.SHP>: whether a PNG replaces that shape, its sheet size, frames and one pixel.
		TrueColour_Report(step.Argument.c_str());
	} else if (step.Command == "truecolourdir") {
		// truecolourdir <path>: adds a directory the game searches for files, PNG sprites included.
		std::string const resolved = AutoTest_Input_Path(step.Argument);
		TrueColour_Add_Directory(resolved.c_str());
	} else if (step.Command == "extract") {
		// extract <FILE> <out path>: copies a game file, wherever the game finds it, to the path given.
		std::string const & argument = step.Argument;
		size_t const gap = argument.find_first_of(" \t");
		size_t const start = gap != std::string::npos ? argument.find_first_not_of(" \t", gap) : std::string::npos;
		CCFileClass in(argument.substr(0, gap).c_str());
		if (start == std::string::npos || !in.Is_Available()) {
			DebugString("AUTOTEST   extract %s: not found\n", argument.c_str());
		} else {
			std::vector<unsigned char> bytes((std::size_t)in.Size());
			in.Read(bytes.data(), (int)bytes.size());
			FILE * out = std::fopen(argument.substr(start).c_str(), "wb");
			if (out != NULL) {
				std::fwrite(bytes.data(), 1, bytes.size(), out);
				std::fclose(out);
			}
			DebugString("AUTOTEST   extract %s: %d bytes\n", argument.substr(0, gap).c_str(), (int)bytes.size());
		}
	} else if (step.Command == "exportshape") {
		// exportshape <NAME.SHP> <out.png>: writes the shape's frames as a PNG sheet, plus a house-colour mask.
		std::string const & argument = step.Argument;
		size_t const gap = argument.find_first_of(" \t");
		size_t const start = gap != std::string::npos ? argument.find_first_not_of(" \t", gap) : std::string::npos;
		if (start != std::string::npos) {
			TrueColour_Export(argument.substr(0, gap).c_str(), argument.substr(start).c_str());
		} else {
			DebugString("AUTOTEST   exportshape %s: needs a shape name and an output path\n", argument.c_str());
		}
	} else if (step.Command == "exportpal") {
		// exportpal <NAME.SHP> <theater|unit|anim|house|house:N> <out.png>: writes the shape's frames as a PNG sheet, shown with that palette.
		// "house" is the player's own colour scheme, as the scheme a building animation draws with.
		std::string const & argument = step.Argument;
		size_t const first = argument.find_first_of(" \t");
		size_t const second = first != std::string::npos ? argument.find_first_not_of(" \t", first) : std::string::npos;
		size_t const third = second != std::string::npos ? argument.find_first_of(" \t", second) : std::string::npos;
		size_t const last = third != std::string::npos ? argument.find_first_not_of(" \t", third) : std::string::npos;
		if (last != std::string::npos) {
			std::string palette = argument.substr(second, third - second);
			if (palette == "house") {
				palette = "house:" + std::to_string(PlayerPtr->Scheme);
			}
			TrueColour_Export_Palette(argument.substr(0, first).c_str(), palette.c_str(), argument.substr(last).c_str());
		} else {
			DebugString("AUTOTEST   exportpal %s: needs a shape name, a palette and an output path\n", argument.c_str());
		}
	} else if (step.Command == "versus") {
		// versus <WarheadID>:<TypeID>: the warhead's multiplier and targeting switches against that type's armor.
		std::string const & argument = step.Argument;
		size_t const colon = argument.find(':');
		WarheadTypeClass const * warhead = colon != std::string::npos ? WarheadTypeClass::Find_Or_Make(argument.substr(0, colon).c_str()) : NULL;
		TechnoTypeClass const * type = colon != std::string::npos ? Find_Type(argument.substr(colon + 1)) : NULL;
		if (warhead != NULL && type != NULL) {
			DebugString("AUTOTEST   versus %s armor %s %.4f forcefire %d retaliate %d passive %d\n", argument.c_str(), Armor_Type_Name(type->Armor),
				warhead->Versus(type->Armor), (int)warhead->Can_Force_Fire(type->Armor), (int)warhead->Can_Retaliate(type->Armor), (int)warhead->Can_Passive_Acquire(type->Armor));
		}
	} else if (step.Command == "damage") {
		// damage <TypeID> <amount>: hits every player object of the type for that much unforced damage.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int damage = step.X;
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, false);
				DebugString("AUTOTEST   damage %s at %d,%d took %d strength %d curtain %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, damage, (int)techno->Strength, (int)techno->IronCurtainTimer);
			}
		}
	} else if (step.Command == "hollow") {
		// hollow <TypeID>: sets the strength of the player's factory-held object of that type to zero, the state
		// of an internal dummy unit, so that the unit leaves with no radio contact.
		for (int index = 0; index < Factories.Count(); index++) {
			FactoryClass * factory = Factories[index];
			TechnoClass * object = factory->Get_Object();
			if (factory->Get_House() == PlayerPtr && object != NULL && stricmp(object->TClass->Name(), step.Argument.c_str()) == 0) {
				object->Strength = 0;
				DebugString("AUTOTEST   hollow %s: strength set to 0 while in production\n", object->TClass->Name());
			}
		}
	} else if (step.Command == "rule") {
		if (stricmp(step.Argument.c_str(), "CanDetonateTimeBomb") == 0) {
			Rule->IsCanDetonateTimeBomb = step.X != 0;
		}
		if (stricmp(step.Argument.c_str(), "BallisticScatter") == 0) {
			Rule->BallisticScatter = step.X;
		}
		if (stricmp(step.Argument.c_str(), "AircraftWaypointReload") == 0) {
			Rule->AircraftWaypointReload = step.X != 0;
		}
		DebugString("AUTOTEST   rule %s=%d\n", step.Argument.c_str(), step.X);
	} else if (step.Command == "disguise") {
		// disguise <TypeID>: every computer-owned soldier of that type disguises as the player's AlliedDisguise soldier.
		for (int index = 0; index < Infantry.Count(); index++) {
			InfantryClass * soldier = Infantry[index];
			if (soldier->House != PlayerPtr && stricmp(soldier->Class->Name(), step.Argument.c_str()) == 0) {
				soldier->DisguiseType = Rule->AlliedDisguise;
				soldier->DisguiseHouse = PlayerPtr;
				DebugString("AUTOTEST   disguise %s at %d,%d as %s fools player %d cursor rule %d\n", soldier->Class->Name(), soldier->Get_Cell().X, soldier->Get_Cell().Y, Rule->AlliedDisguise != NULL ? Rule->AlliedDisguise->Name() : "-", (int)soldier->Is_Disguised_As(PlayerPtr), (int)Rule->IsAttackCursorOnDisguise);
			}
		}
	} else if (step.Command == "action" || step.Command == "clickon") {
		ObjectClass * target = Techno_Over_Cell(Cell(step.X, step.Y));
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				ActionType const action = target != NULL ? techno->What_Action(target, false) : ACTION_NONE;
				TechnoClass const * victim = dynamic_cast<TechnoClass const *>(target);
				DebugString("AUTOTEST   action %s on %s: %s (bomb planted at %d)\n", techno->TClass->Name(), target != NULL ? target->Class_Of()->Name() : "-", ActionName[action],
					victim != NULL && victim->BombDetonateFrame != -1 ? victim->BombPlantFrame : -1);
				if (step.Command == "clickon" && target != NULL) {
					// Test hook: a click in planning mode is recorded as a planned order, as the real click path does.
					PlanningRecordScope const recording(Map.IsPlanningMode);
					techno->Active_Click_With(action, target, false);
				}
				break;
			}
		}
	} else if (step.Command == "hit") {
		// hit <TypeID>:<Warhead>[@<FirerTypeID>] <amount>: the firer is the player's first object of the named type, else of another type; a named type the player lacks may belong to any house.
		std::string const argument = step.Argument;
		std::size_t const at = argument.find('@');
		std::string const head = argument.substr(0, at);
		std::string const firer_name = at != std::string::npos ? argument.substr(at + 1) : std::string();
		std::size_t const colon = head.find(':');
		std::string const typename_ = head.substr(0, colon);
		WarheadTypeClass const * warhead = colon != std::string::npos ? WarheadTypeClass::From_Name(head.substr(colon + 1).c_str()) : NULL;
		TechnoClass * firer = NULL;
		for (int index = 0; index < Technos.Count() && firer == NULL; index++) {
			TechnoClass * candidate = Technos[index];
			bool const match = firer_name.empty() ? stricmp(candidate->TClass->Name(), typename_.c_str()) != 0 : stricmp(candidate->TClass->Name(), firer_name.c_str()) == 0;
			if (candidate->House == PlayerPtr && !candidate->IsInLimbo && match) {
				firer = candidate;
			}
		}
		for (int index = 0; !firer_name.empty() && firer == NULL && index < Technos.Count(); index++) {
			TechnoClass * candidate = Technos[index];
			if (!candidate->IsInLimbo && stricmp(candidate->TClass->Name(), firer_name.c_str()) == 0) {
				firer = candidate;
			}
		}
		for (int index = Technos.Count() - 1; warhead != NULL && index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			if (!techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), typename_.c_str()) == 0) {
				int damage = step.X;
				techno->Take_Damage(damage, 0, warhead, firer, false);
				DebugString("AUTOTEST   hit %s of %s took %d strength %d berzerk %d for %d firer %s\n", techno->TClass->Name(), techno->House->Class->Name(), damage, (int)techno->Strength, (int)techno->IsBerzerk, techno->BerzerkDuration, firer != NULL ? firer->TClass->Name() : "none");
			}
		}
	} else if (step.Command == "clickcell") {
		Cell const cell(step.X, step.Y);
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && !techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				ActionType const action = techno->What_Action(cell, false, false);
				DebugString("AUTOTEST   clickcell %s at %d,%d: %s\n", techno->TClass->Name(), step.X, step.Y, ActionName[action]);
				// Test hook: a click in planning mode is recorded as a planned order, as the real click path does.
				// The real click path refuses the whole order when a selected object cannot take a plan (DisplayClass::Active_Click).
				if (Map.IsPlanningMode && Planning_Selection_Label() != NULL) {
					Planning_Refusal(Planning_Selection_Label());
					break;
				}
				PlanningRecordScope const recording(Map.IsPlanningMode);
				techno->Active_Click_With(action, cell, false);
				break;
			}
		}
	} else if (step.Command == "typesounds") {
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   typesounds %s create %d enter %d leave %d primaryattack %d secondaryattack %d\n", type->Name(), (int)type->CreateSound, (int)type->EnterTransportSound, (int)type->LeaveTransportSound, (int)type->VoicePrimaryWeaponAttack, (int)type->VoiceSecondaryWeaponAttack);
		}
	} else if (step.Command == "cratesounds") {
		DebugString("AUTOTEST   cratesounds money %d reveal %d fire %d armour %d speed %d unit %d promote %d\n", (int)Rule->CrateMoneySound, (int)Rule->CrateRevealSound, (int)Rule->CrateFireSound, (int)Rule->CrateArmourSound, (int)Rule->CrateSpeedSound, (int)Rule->CrateUnitSound, (int)Rule->CratePromoteSound);
	} else if (step.Command == "elite") {
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Veterancy.Set_Elite(true);
			}
		}
	} else if (step.Command == "load") {
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Storage.Increase_Amount(techno->TClass->Capacity, 0);
				DebugString("AUTOTEST   load %s: %d held\n", techno->TClass->Name(), techno->Storage.Get_Total_Amount());
			}
		}
	} else if (step.Command == "loadgems") {
		// loadgems <TypeID>: puts a quarter of its capacity in gems, the second kind of Tiberium, into each of the player's objects of that type.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Storage.Increase_Amount(techno->TClass->Capacity / 4, 1);
				DebugString("AUTOTEST   loadgems %s: %d held\n", techno->TClass->Name(), techno->Storage.Get_Total_Amount());
			}
		}
	} else if (step.Command == "canrepair") {
		BuildingClass const * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		if (building != NULL) {
			DebugString("AUTOTEST   canrepair %s %d\n", building->Class->Name(), (int)building->Can_Repair());
		}
	} else if (step.Command == "overpower") {
		BuildingClass const * building = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Cell_Building();
		if (building != NULL) {
			DebugString("AUTOTEST   overpower %s chargers %d overpowered %d powered %d\n", building->Class->Name(), building->Overpowerer_Count(), (int)building->IsOverpowered, (int)building->Is_Powered_On());
		}
	} else {
		handled = false;
	}
	if (handled) return;

	if (step.Command == "ruleanims") {
		auto name = [](AnimTypeClass const * type) { return type != NULL ? type->Name() : "-"; };
		DebugString("AUTOTEST   ruleanims MoveFlash %s InfantryExplode %s InfantryNuked %s FlamingInfantry %s IonBlast %s DropZoneAnim %s BarrelExplode %s money %d\n",
			name(Rule->MoveFlash), name(Rule->InfantryExplode), name(Rule->InfantryNuked), name(Rule->FlamingInfantry), name(Rule->IonBlast), name(Rule->FlareAnim), name(Rule->BarrelExplode), Rule->MPMoney);
	} else if (step.Command == "canfire") {
		TechnoClass * target = Techno_Over_Cell(Cell(step.X, step.Y));
		for (int index = 0; target != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			// An open-topped passenger is limboed as it boards (gamemd limboes it before EnteredOpenTopped), so it still counts.
			if (techno->House == PlayerPtr && (!techno->IsInLimbo || techno->IsInOpenToppedTransport) && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int const which = techno->What_Weapon_Should_I_Use(target);
				DebugString("AUTOTEST   canfire %s at %s: %d facing %d weapon %d fire %d visual %d sensed %d\n", techno->TClass->Name(), target->TClass->Name(), (int)techno->Can_Fire(target, 0), (int)techno->PrimaryFacing.Current().As_Facing(), which, (int)techno->Can_Fire(target, which), (int)target->Visual_Character(true, techno->House), (int)Map[target->Center_Coord()].Is_Sensed(techno->House));
				break;
			}
		}
	} else if (step.Command == "blast") {
		// blast <Warhead|ion> x y: explodes that warhead (ion is the ion cannon warhead) on the centre of the cell, as a weapon impact does (Explosion_Damage,
		// the call a wood low bridge takes its hit in). Strength is high enough that the bridge roll always passes.
		WarheadTypeClass const * warhead = stricmp(step.Argument.c_str(), "ion") == 0 ? Rule->IonCannonWarhead : WarheadTypeClass::From_Name(step.Argument.c_str());
		if (warhead != NULL) {
			DebugString("AUTOTEST   blast %s at %d,%d\n", warhead->Name(), step.X, step.Y);
			Explosion_Damage(Map[Cell(step.X, step.Y)].Center_Coord(), 1000, NULL, warhead, false, NULL);
		} else {
			DebugString("AUTOTEST   blast %s: no such warhead\n", step.Argument.c_str());
		}
	} else if (step.Command == "shake") {
		WarheadTypeClass const * warhead = WarheadTypeClass::From_Name(step.Argument.c_str());
		if (warhead != NULL) {
			Map.ScreenX = warhead->ShakeXhi;
			Map.ScreenY = warhead->ShakeYhi;
		}
	} else if (step.Command == "screen") {
		DebugString("AUTOTEST   screen shake %d,%d frame %d\n", Map.ScreenX, Map.ScreenY, Frame);
	} else if (step.Command == "kill") {
		// kill <TypeID> [1]: destroys the computer's objects of the type with no attacker, or the player's with a 1.
		for (int index = Technos.Count() - 1; index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			bool const player = techno->House == PlayerPtr;
			if (player == (step.X == 1) && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				int damage = techno->Strength;
				DebugString("AUTOTEST   kill %s at %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y);
				techno->Take_Damage(damage, 0, Rule->C4Warhead, NULL, true);
			}
		}
	} else if (step.Command == "ironcurtain") {
		// ironcurtain <TypeID> <frames>: puts each object of the type, whoever owns it, under an Iron Curtain for that many frames.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (!techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				DebugString("AUTOTEST   ironcurtain %s at %d,%d frames %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, step.X);
				techno->Iron_Curtain(step.X, techno->House, false);
			}
		}
	} else if (step.Command == "setlocal" || step.Command == "setglobal") {
		// setlocal <index> <0|1>, setglobal <index> <0|1>: sets or, with 0, clears that scenario variable, as a trigger action would.
		int const index = std::atoi(step.Argument.c_str());
		bool const value = step.X != 0;
		if (step.Command == "setlocal") {
			Scen->Set_Local_To(index, value);
		} else {
			Scen->Set_Global_To(index, value);
		}
		DebugString("AUTOTEST   %s %d: %d\n", step.Command.c_str(), index, (int)value);
	} else if (step.Command == "killhouse") {
		// killhouse <House> [1|2|3]: (an underscore stands for a space in the name) destroys the buildings (1), the units, infantry and aircraft (2) or everything (0, the default) that house owns, as a player's attack would. Kind 3 leaves out the objects on cells the player cannot see.
		std::string house_name = step.Argument;
		std::replace(house_name.begin(), house_name.end(), '_', ' ');
		HouseClass * victim = House_From_Name(house_name.c_str());
		// A skirmish's computer players are found by their names in the lobby, such as "Medium AI".
		for (int index = 0; victim == NULL && index < Houses.Count(); index++) {
			if (stricmp(Houses[index]->IniName, house_name.c_str()) == 0) {
				victim = Houses[index];
			}
		}
		int const kinds = step.X;
		TechnoClass * attacker = NULL;
		for (int index = 0; index < Technos.Count() && attacker == NULL; index++) {
			if (Technos[index]->House == PlayerPtr && !Technos[index]->IsInLimbo && Technos[index]->Strength > 0) attacker = Technos[index];
		}
		int killed = 0;
		for (int index = Technos.Count() - 1; victim != NULL && index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			bool const building = techno->What_Am_I() == RTTI_BUILDING;
			if (techno->House != victim || techno->IsInLimbo || techno->Strength <= 0) continue;
			if (kinds == 1 && !building) continue;
			if (kinds == 2 && building) continue;
			// Kind 3 is a player's sweep: the units, infantry and aircraft on cells the player can see now.
			if (kinds == 3 && (building || !Map[techno->Get_Cell()].IsVisible[PlayerPtr])) continue;
			int damage = techno->Strength;
			techno->Take_Damage(damage, 0, Rule->C4Warhead, attacker, true);
			killed++;
		}
		DebugString("AUTOTEST   killhouse %s %d: %d objects\n", step.Argument.c_str(), kinds, killed);
	} else if (step.Command == "killtag") {
		// killtag <Tag>: destroys every object that carries a tag of that type, with a player's object as the attacker.
		TechnoClass * attacker = NULL;
		for (int index = 0; index < Technos.Count() && attacker == NULL; index++) {
			if (Technos[index]->House == PlayerPtr && !Technos[index]->IsInLimbo && Technos[index]->Strength > 0) attacker = Technos[index];
		}
		int killed = 0;
		for (int index = Technos.Count() - 1; index >= 0; index--) {
			TechnoClass * techno = Technos[index];
			if (techno->Tag == NULL || techno->Tag->Class == NULL || techno->IsInLimbo || techno->Strength <= 0) continue;
			if (stricmp(techno->Tag->Class->IniName, step.Argument.c_str()) != 0 && stricmp(techno->Tag->Class->GivenName, step.Argument.c_str()) != 0) continue;
			int damage = techno->Strength;
			techno->Take_Damage(damage, 0, Rule->C4Warhead, attacker, true);
			killed++;
		}
		DebugString("AUTOTEST   killtag %s: %d objects\n", step.Argument.c_str(), killed);
	} else if (step.Command == "price") {
		// price <TypeID>: what the player pays for one object of the type.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   price %s %d (listed %d)\n", type->Name(), type->Cost_Of(PlayerPtr), type->Raw_Cost());
		}
	} else if (step.Command == "buildtime") {
		// buildtime <TypeID>: the build time, in game frames, of the player's first object of that type.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House == PlayerPtr && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				DebugString("AUTOTEST   buildtime %s %d\n", techno->TClass->Name(), techno->Time_To_Build());
				break;
			}
		}
	} else if (step.Command == "highcell") {
		// highcell: the highest cell on the map.
		CellClass const * highest = NULL;
		for (int y = 0; y < 512; y++) {
			for (int x = 0; x < 512; x++) {
				if (!Map.In_Radar(Cell(x, y))) continue;
				CellClass const * cell = &Map[Cell(x, y)];
				if (highest == NULL || cell->Height > highest->Height) highest = cell;
			}
		}
		if (highest != NULL) DebugString("AUTOTEST   highcell %d,%d height %d\n", highest->Fetch_CellID().X, highest->Fetch_CellID().Y, highest->Height);
	} else if (step.Command == "teamini") {
		// teamini <path>: reads the task forces, scripts and team types in that INI file, as a map's are read.
		CCINIClass ini;
		std::string const resolved = AutoTest_Input_Path(step.Argument);
		RawFileClass file(resolved.c_str());
		if (file.Is_Available() && ini.Load(file, false)) {
			TaskForceClass::Read_All(ini, SCOPE_LOCAL);
			ScriptTypeClass::Read_All(ini, SCOPE_LOCAL);
			TeamTypeClass::Read_All(ini, SCOPE_LOCAL);
		} else {
			DebugString("AUTOTEST   teamini %s: not found\n", resolved.c_str());
		}
	} else if (step.Command == "reinforce") {
		// reinforce <TeamType>: brings that team type on as a reinforcement for the player, as the trigger action does.
		TeamTypeClass * type = TeamTypeClass::From_Name(step.Argument.c_str());
		if (type != NULL) {
			type->House = PlayerPtr;
		}
		DebugString("AUTOTEST   reinforce %s: %d\n", step.Argument.c_str(), type != NULL ? (int)Do_Reinforcements(type) : -1);
	} else if (step.Command == "runtrigger") {
		// runtrigger <TriggerType>: carries out the actions of that trigger type for its house, as if its events had all happened.
		TriggerTypeClass * type = TriggerTypeClass::From_Name(step.Argument.c_str());
		int ran = -1;
		if (type != NULL) {
			ran = 0;

			// The trigger the map made from the type, if any, is what actions that look at the objects it is attached to see.
			TriggerClass * trigger = NULL;
			for (int index = 0; index < Triggers.Count() && trigger == NULL; index++) {
				if (Triggers[index]->Class == type) trigger = Triggers[index];
			}
			for (TActionClass * action = type->FirstAction; action != NULL; action = action->Next) {
				(*action)(type->House, NULL, trigger, CELL_NONE);
				ran++;
			}
		}
		DebugString("AUTOTEST   runtrigger %s: %d actions\n", step.Argument.c_str(), ran);
	} else if (step.Command == "restart") {
		// restart: asks for the Restart mission command, as the options menu does in a campaign.
		PlayerRestarts = true;
	} else if (step.Command == "triggers") {
		// triggers [all]: what the map's trigger system holds right now.
		bool const everything = step.Argument == "all";
		for (int index = 0; index < TriggerTypes.Count(); index++) {
			TriggerTypeClass const * type = TriggerTypes[index];
			int live = 0;
			int enabled = 0;
			for (int t = 0; t < Triggers.Count(); t++) {
				if (Triggers[t]->Class == type) {
					live++;
					if (Triggers[t]->Is_Enabled()) enabled++;
				}
			}
			if (!everything && enabled == 0) continue;
			int event_count = 0;
			for (TEventClass const * event = type->FirstEvent; event != NULL; event = event->Next) event_count++;
			int action_count = 0;
			for (TActionClass const * action = type->FirstAction; action != NULL; action = action->Next) action_count++;
			std::string events;
			for (TEventClass const * event = type->FirstEvent; event != NULL; event = event->Next) {
				char text[160];
				std::snprintf(text, sizeof(text), " %d(%d", (int)event->Event, event->Data.Value);
				events += text;
				if (Event_Needs(event->Event) == NEED_HOUSE) {
					HouseClass const * owner = House_From_HousesType(event->Data.House);
					events += std::string("=") + (owner != NULL ? owner->Class->Name() : "NOHOUSE");
				}
				if (event->TechnoName[0] != '\0') events += std::string(" ") + event->TechnoName;
				NeedType const need = Event_Needs(event->Event);
				if (need == NEED_STRUCTURE) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < BuildingTypes.Count() ? BuildingTypes[event->Data.Value]->Name() : "NOSTRUCT");
				if (need == NEED_UNIT) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < UnitTypes.Count() ? UnitTypes[event->Data.Value]->Name() : "NOUNIT");
				if (need == NEED_INFANTRY) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < InfantryTypes.Count() ? InfantryTypes[event->Data.Value]->Name() : "NOINFANTRY");
				if (need == NEED_AIRCRAFT) events += std::string(" ") + (event->Data.Value >= 0 && event->Data.Value < AircraftTypes.Count() ? AircraftTypes[event->Data.Value]->Name() : "NOAIRCRAFT");
				events += ")";
			}
			DebugString("AUTOTEST   trigger %s '%s' owner %s live %d enabled %d event count %d action count %d events:%s\n", (char const *)type->IniName, (char const *)type->GivenName, type->House != NULL ? type->House->Class->Name() : "NOHOUSE", live, enabled, event_count, action_count, events.c_str());
		}
		for (int index = 0; index < Tags.Count(); index++) {
			TagClass const * tag = Tags[index];
			std::string chain;
			for (TriggerClass const * trigger = tag->Trigger; trigger != NULL; trigger = trigger->LinkedTo) {
				chain += std::string(" ") + (trigger->Class != NULL ? (char const *)trigger->Class->GivenName : "?") + (trigger->Is_Enabled() ? "+" : "-");
			}
			DebugString("AUTOTEST   tag %s '%s' attached %d cell %d,%d chain:%s\n", tag->Class != NULL ? (char const *)tag->Class->IniName : "?", tag->Class != NULL ? (char const *)tag->Class->GivenName : "?", tag->AttachCount, tag->CellID.X, tag->CellID.Y, chain.c_str());
		}
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->Tag != NULL) {
				DebugString("AUTOTEST   tagged %s of %s at %d,%d tag %s\n", techno->TClass->Name(), techno->House != NULL ? techno->House->Class->Name() : "?", techno->Get_Cell().X, techno->Get_Cell().Y, techno->Tag->Class != NULL ? (char const *)techno->Tag->Class->IniName : "?");
			}
		}
		for (int y = 0; y < 512; y++) {
			for (int x = 0; x < 512; x++) {
				if (!Map.In_Radar(Cell(x, y))) continue;
				TagClass const * tag = Map[Cell(x, y)].Tag;
				if (tag != NULL) DebugString("AUTOTEST   tagged cell %d,%d tag %s\n", x, y, tag->Class != NULL ? (char const *)tag->Class->IniName : "none");
			}
		}
		for (int index = 0; index < SCEN_LOCAL_COUNT; index++) {
			if (Scen->LocalFlags[index].VariableName[0] != '\0' && Scen->LocalFlags[index].Value) DebugString("AUTOTEST   local %d %s is set\n", index, Scen->LocalFlags[index].VariableName);
		}
		for (int index = 0; index < SCEN_GLOBAL_COUNT; index++) {
			if (Scen->GlobalFlags[index].VariableName[0] != '\0' && Scen->GlobalFlags[index].Value) DebugString("AUTOTEST   global %d %s is set\n", index, Scen->GlobalFlags[index].VariableName);
		}
	} else if (step.Command == "planes") {
		// planes: each aircraft on the map, its cell, mission and passenger count.
		for (int index = 0; index < Aircraft.Count(); index++) {
			AircraftClass const * plane = Aircraft[index];
			DebugString("AUTOTEST   plane %s cell %d,%d mission %s passengers %d\n", plane->Class->Name(), plane->Get_Cell().X, plane->Get_Cell().Y, MissionClass::Mission_Name(plane->Get_Mission()), plane->Cargo.How_Many());
		}
	} else if (step.Command == "inrange") {
		// inrange <TypeID>: whether each of the player's objects of that type has each other player object in primary weapon range.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House != PlayerPtr || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			for (int other = 0; other < Technos.Count(); other++) {
				TechnoClass * target = Technos[other];
				if (target == techno || target->House != PlayerPtr) continue;
				DebugString("AUTOTEST   inrange %s -> %s at %d leptons: %d fire error %d turret %d wants %d\n", techno->TClass->Name(), target->TClass->Name(), (int)(Point2D(techno->Center_Coord()) - Point2D(target->Center_Coord())).Length(), (int)techno->In_Range(target, 0), (int)techno->Can_Fire(target, 0), (int)techno->SecondaryFacing.Current().As_Dir256(), (int)techno->SecondaryFacing.Desired().As_Dir256());
			}
		}
	} else if (step.Command == "elevation") {
		// elevation <TypeID>: the elevation range bonus each of the player's objects of that type has against each other one.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House != PlayerPtr || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			for (int other = 0; other < Technos.Count(); other++) {
				TechnoClass * target = Technos[other];
				if (target == techno || target->House != PlayerPtr || stricmp(target->TClass->Name(), step.Argument.c_str()) != 0) continue;
				DebugString("AUTOTEST   elevation %d,%d height %d -> %d,%d height %d bonus %d\n", techno->Get_Cell().X, techno->Get_Cell().Y, Map[techno->Get_Cell()].Height, target->Get_Cell().X, target->Get_Cell().Y, Map[target->Get_Cell()].Height, techno->Elevation_Range_Bonus(target));
			}
		}
	} else if (step.Command == "types") {
		// types <prefix>: every structure type whose ID starts with the prefix.
		for (int index = 0; index < BuildingTypes.Count(); index++) {
			if (strnicmp(BuildingTypes[index]->Name(), step.Argument.c_str(), step.Argument.size()) == 0) {
				DebugString("AUTOTEST   type %d %s\n", index, BuildingTypes[index]->Name());
			}
		}
	} else if (step.Command == "typecounts") {
		// typecounts: how many types of each kind there are, and any whose ID is not a plain name (a list read as one name).
		DebugString("AUTOTEST   typecounts structures %d units %d infantry %d aircraft %d\n", BuildingTypes.Count(), UnitTypes.Count(), InfantryTypes.Count(), AircraftTypes.Count());
		for (int index = 0; index < BuildingTypes.Count(); index++) if (std::strpbrk(BuildingTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray structure %d %s\n", index, BuildingTypes[index]->Name());
		for (int index = 0; index < UnitTypes.Count(); index++) if (std::strpbrk(UnitTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray unit %d %s\n", index, UnitTypes[index]->Name());
		for (int index = 0; index < InfantryTypes.Count(); index++) if (std::strpbrk(InfantryTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray infantry %d %s\n", index, InfantryTypes[index]->Name());
		for (int index = 0; index < AircraftTypes.Count(); index++) if (std::strpbrk(AircraftTypes[index]->Name(), ",; ") != NULL) DebugString("AUTOTEST   stray aircraft %d %s\n", index, AircraftTypes[index]->Name());
	} else if (step.Command == "capture") {
		// capture <TypeID> x y: sends the player's objects of that type that are not already capturing into the structure on that cell.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		DebugString("AUTOTEST capture %s -> %s\n", step.Argument.c_str(), building != NULL ? building->Class->Name() : "(none)");
		for (int index = 0; building != NULL && index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Is_Foot() && techno->House == PlayerPtr && !techno->IsInLimbo && techno->Get_Mission() != MISSION_CAPTURE && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				techno->Assign_Mission(MISSION_CAPTURE);
				techno->Assign_Destination(building);
			}
		}
	} else if (step.Command == "aiscan") {
		// aiscan: each foot object no player owns, with its mission, queued mission, team, the target it holds,
		// the target its range scan picks now, and the nearest player object with whether that object is in its
		// weapon range and can be fired at, and the mission-level flags a threat check reads.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->House == PlayerPtr || techno->IsInLimbo || !techno->Is_Foot()) continue;
			FootClass * foot = static_cast<FootClass *>(techno);
			ObjectClass const * tarobject = dynamic_cast<ObjectClass const *>(techno->TarCom);
			bool const tarcell = techno->TarCom != NULL && tarobject == NULL;
			// A scan clears IsScanLimited when it finds nothing. Restoring the flag keeps the dump from changing the unit's
			// state or the scan that follows.
			bool const scan_limited = foot->IsScanLimited;
			AbstractClass * scan = techno->Greatest_Threat(THREAT_RANGE, techno->Center_Coord(), false);
			foot->IsScanLimited = scan_limited;
			ObjectClass const * scanobject = dynamic_cast<ObjectClass const *>(scan);
			Cell const scancell = scan != NULL ? scan->Center_Coord().As_Cell() : Cell(-1, -1);
			TechnoClass * nearest = NULL;
			int nearest_dist = 0;
			for (int other = 0; other < Technos.Count(); other++) {
				TechnoClass * target = Technos[other];
				if (target->House != PlayerPtr || target->IsInLimbo) continue;
				int const dist = (int)(Point2D(techno->Center_Coord()) - Point2D(target->Center_Coord())).Length();
				if (nearest == NULL || dist < nearest_dist) {
					nearest = target;
					nearest_dist = dist;
				}
			}
			int inrange = -1;
			int fire = -1;
			int nothreat = -1;
			int locked = -1;
			if (nearest != NULL) {
				int const weapon = techno->What_Weapon_Should_I_Use(nearest);
				inrange = (int)techno->In_Range(nearest, weapon);
				fire = (int)techno->Can_Fire(nearest, weapon);
				nothreat = (int)nearest->Current_Mission_Control().IsNoThreat;
				locked = (int)nearest->IsLocked;
			}
			AbstractClass * home = techno->ArchiveTarget;
			Coord const home_coord = home != NULL ? home->Center_Coord() : techno->Center_Coord();
			AbstractClass * area = techno->Greatest_Threat(THREAT_AREA, home_coord, false);
			foot->IsScanLimited = scan_limited;
			ObjectClass const * areaobject = dynamic_cast<ObjectClass const *>(area);
			Cell const areacell = area != NULL ? area->Center_Coord().As_Cell() : Cell(-1, -1);
			Cell const homecell = home != NULL ? home->Center_Coord().As_Cell() : Cell(-1, -1);
			DebugString("AUTOTEST   aiscan %s of %s cell %d,%d mission %s queued %d team %s tarcom %s tarcell %d,%d scan %s scancell %d,%d passive %d slave %d near %s dist %d inrange %d fire %d nearnothreat %d nearlocked %d home %d,%d navcom %d firing %d area %s areacell %d,%d\n",
				techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()), (int)foot->MissionQueue,
				foot->Team != NULL ? foot->Team->Class->Name() : "-", tarobject != NULL ? tarobject->Class_Of()->Name() : (tarcell ? "cell" : "-"),
				techno->TarCom != NULL ? techno->TarCom->Center_Coord().As_Cell().X : -1, techno->TarCom != NULL ? techno->TarCom->Center_Coord().As_Cell().Y : -1,
				scanobject != NULL ? scanobject->Class_Of()->Name() : (scan != NULL ? "cell" : "-"), scancell.X, scancell.Y,
				(int)techno->TClass->IsCanPassiveAquire, (int)(techno->SlaveOwner != NULL), nearest != NULL ? nearest->TClass->Name() : "-", nearest_dist, inrange, fire, nothreat, locked,
				homecell.X, homecell.Y, (int)(foot->NavCom != NULL), (int)foot->IsFiring,
				areaobject != NULL ? areaobject->Class_Of()->Name() : (area != NULL ? "cell" : "-"), areacell.X, areacell.Y);
		}
	} else if (step.Command == "lockscan") {
		// lockscan: each object outside limbo whose IsLocked flag is clear, whoever owns it, with its cell, whether that cell
		// is in the local radar now (a flag that stayed clear there would be stale), its mission and its strength.
		int unlocked = 0;
		int stale = 0;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->IsInLimbo || techno->IsLocked) continue;
			bool const inlocal = Map.In_Local_Radar(techno->PositionCell);
			unlocked++;
			stale += (int)inlocal;
			DebugString("AUTOTEST   lockscan %s of %s cell %d,%d mission %s inlocal %d strength %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()), (int)inlocal, (int)techno->Strength);
		}
		DebugString("AUTOTEST   lockscan frame %d unlocked %d stale %d\n", (int)Frame, unlocked, stale);
	} else if (step.Command == "gattling") {
		// gattling <TypeID>: the state a gattling structure's draw and fire code read, for each object of that type.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			Cell const tar = techno->TarCom != NULL ? techno->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
			int const aim = techno->TarCom != NULL && techno->RTTI == RTTI_BUILDING ? (int)static_cast<BuildingClass const *>(techno)->Aim_Direction(techno->TarCom).As_Int() : -1;
			int const center = techno->TarCom != NULL ? (int)::Direction(techno->Center_Coord(), techno->TarCom->Center_Coord()).As_Int() : -1;
			int const fire = techno->RTTI == RTTI_BUILDING && techno->TarCom != NULL ? (int)static_cast<BuildingClass const *>(techno)->Can_Fire(techno->TarCom, 0) : -1;
			int turret_frames = -1;
			int barrel_frames = -1;
			if (techno->RTTI == RTTI_BUILDING) {
				BuildingTypeClass const * btype = static_cast<BuildingClass const *>(techno)->Class;
				turret_frames = btype->AuxVoxel.MotLib != NULL ? btype->AuxVoxel.MotLib->Get_Frame_Count() : -1;
				barrel_frames = btype->AuxVoxel2.MotLib != NULL ? btype->AuxVoxel2.MotLib->Get_Frame_Count() : -1;
			}
			DebugString("AUTOTEST   gattling %s of %s cell %d,%d mission %s facing %d desired %d dir32 %d aim %d center %d fire %d pitch %d spin %d stage %d frame %d recoil %d target %d,%d at %d hva %d %d\n",
				techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()),
				(int)techno->PrimaryFacing.Current().As_Int(), (int)techno->PrimaryFacing.Desired().As_Int(), (int)techno->PrimaryFacing.Current().As_Dir32(), aim, center, fire,
				(int)techno->BarrelPitch.Current().As_Int(), techno->GattlingValue, techno->CurrentGattlingStage, techno->TurretAnimFrame, (int)techno->IsInRecoilState, tar.X, tar.Y, (int)Frame, turret_frames, barrel_frames);
			Coord const turret = techno->Turret_Coord();
			Coord const lead = techno->Predict_Target_Coord();
			DebugString("AUTOTEST   barrelstate %s barrel desired %d current %d center z %d tarz %d turret %d,%d,%d lead %d,%d,%d\n",
				techno->TClass->Name(), (int)techno->BarrelPitch.Desired().As_Int(), (int)techno->BarrelPitch.Current().As_Int(), techno->Center_Coord().Z,
				techno->TarCom != NULL ? techno->TarCom->Center_Coord().Z : -1, turret.X, turret.Y, turret.Z, lead.X, lead.Y, lead.Z);
		}
	} else if (step.Command == "animlog") {
		// animlog <TypeID>: the animation state of each object of that type, whoever owns it. The body draws the voxel frame
		// TotalFramesWalked modulo its frame count (UnitClass::Unit_Draw_Voxel, AircraftClass::Draw_It), the turret animates from its facing and
		// TurretAnimFrame, and a DiskLaser ring is drawn while DiskLaser is active.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			FootClass const * foot = techno->Is_Foot() ? static_cast<FootClass const *>(techno) : NULL;
			int const walked = foot != NULL ? foot->TotalFramesWalked : -1;
			int body_frames = -1;
			int secondary = -1;
			int secondary_desired = -1;
			if (techno->RTTI == RTTI_UNIT) {
				UnitClass const * vehicle = static_cast<UnitClass const *>(techno);
				body_frames = vehicle->Class->Voxel.MotLib != NULL ? vehicle->Class->Voxel.MotLib->Get_Frame_Count() : -1;
				secondary = (int)vehicle->SecondaryFacing.Current().As_Int();
				secondary_desired = (int)vehicle->SecondaryFacing.Desired().As_Int();
			} else if (techno->RTTI == RTTI_AIRCRAFT) {
				AircraftClass const * aircraft = static_cast<AircraftClass const *>(techno);
				body_frames = aircraft->Class->Voxel.MotLib != NULL ? aircraft->Class->Voxel.MotLib->Get_Frame_Count() : -1;
			}
			int const body_frame = body_frames > 0 && walked >= 0 ? walked % body_frames : -1;
			ObjectClass const * tarobject = dynamic_cast<ObjectClass const *>(techno->TarCom);
			char tar[48] = "none";
			if (tarobject != NULL) {
				std::snprintf(tar, sizeof(tar), "object %s", tarobject->Class_Of()->Name());
			} else if (techno->TarCom != NULL) {
				Cell const cell = techno->TarCom->Center_Coord().As_Cell();
				std::snprintf(tar, sizeof(tar), "cell %d,%d", cell.X, cell.Y);
			}
			bool const moving = foot != NULL && foot->Locomotion != NULL && foot->Locomotion->Is_Moving();
			bool const moving_now = foot != NULL && foot->Locomotion != NULL && foot->Locomotion->Is_Moving_Now();
			DebugString("AUTOTEST   anim %s of %s frame %d cell %d,%d mission %s walked %d body %d/%d turretframe %d secondary %d desired %d primary %d moving %d now %d firing %d target %s laser %d height %d strength %d\n",
				techno->TClass->Name(), techno->House->Class->Name(), (int)Frame, techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()),
				walked, body_frame, body_frames, techno->TurretAnimFrame, secondary, secondary_desired, (int)techno->PrimaryFacing.Current().As_Int(),
				(int)moving, (int)moving_now, foot != NULL ? (int)foot->IsFiring : -1, tar, (int)techno->DiskLaser.Is_Active(), foot != NULL ? foot->HeightAGL : -1, (int)techno->Strength);
		}
	} else if (step.Command == "pickable") {
		// pickable <TypeID>: each object of that type the player owns, and whether the clickable list
		// holds it. The list is the one the last render built, which the on-screen type select reads.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->House != PlayerPtr || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			bool listed = false;
			Point2D window(-1, -1);
			for (Tactical::Selectable const & entry : Tactical::SelectableObjects) {
				if (!listed && entry.Object == static_cast<ObjectClass const *>(techno)) {
					listed = true;
					window = ScreenTacticalRect.Top_Left() + View_To_Screen_Offset(entry.Position - Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY));
				}
			}
			DebugString("AUTOTEST   pickable %s cell %d,%d selected %d listed %d window %d,%d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->IsSelected, (int)listed, window.X, window.Y);
		}
	} else if (step.Command == "helptext") {
		// helptext <id>: the tooltip text the map gives for the mouse position now, as the tooltip manager asks for it.
		char const * text = Map.Help_Text(std::atoi(step.Argument.c_str()));
		DebugString("AUTOTEST   helptext %d: %s\n", std::atoi(step.Argument.c_str()), text != NULL ? text : "(none)");
	} else if (step.Command == "cellpixel") {
		// cellpixel x y: the window pixel of the centre of that cell under the current view, for windowclick.
		Coord const centre = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Center_Coord();
		Point2D const absolute = TacticalMap->Coord_To_Pixel_Absolute(centre);
		Point2D const window = ScreenTacticalRect.Top_Left() + View_To_Screen_Offset(absolute - Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY));
		DebugString("AUTOTEST   cellpixel %d,%d at window %d,%d\n", std::atoi(step.Argument.c_str()), step.X, window.X, window.Y);
	} else if (step.Command == "hoverat") {
		// hoverat x y: moves the pointer to the centre pixel of that cell, as windowmove does, so the object there hovers from the next frame.
		Coord const centre = Map[Cell(std::atoi(step.Argument.c_str()), step.X)].Center_Coord();
		Point2D const absolute = TacticalMap->Coord_To_Pixel_Absolute(centre);
		Point2D const window = ScreenTacticalRect.Top_Left() + View_To_Screen_Offset(absolute - Point2D(TacticalMap->TacPixelX, TacticalMap->TacPixelY));
		WindowEvent event;
		event.Type = WINDOW_EVENT_MOUSE_MOVE;
		event.X = window.X;
		event.Y = window.Y;
		_CursorX = event.X;
		_CursorY = event.Y;
		Game_Window_Handle_Event(event);
		DebugString("AUTOTEST   hoverat %d,%d pointer %d,%d\n", std::atoi(step.Argument.c_str()), step.X, window.X, window.Y);
	} else if (step.Command == "pips") {
		// pips <TypeID> x y: the inputs of the occupant-pip gate for the building of that type on that cell, and whether the pointer hovers it.
		BuildingClass * building = Map[Cell(step.X, step.Y)].Cell_Building();
		if (building == NULL || stricmp(building->Class->Name(), step.Argument.c_str()) != 0) {
			DebugString("AUTOTEST   pips %s at %d,%d: no such building\n", step.Argument.c_str(), step.X, step.Y);
		} else {
			bool const visible = building->Pips_Visible_To_Player();
			bool const occupant_pips = visible && building->Class->IsShowOccupantPips && building->Class->MaxNumberOccupants > 0;
			DebugString("AUTOTEST   pips %s at %d,%d house %s passive %d allied %d pipsforall %d canoccupy %d showpips %d max %d occupants %d hover %d selected %d visible %d occupant pips %d\n",
				building->Class->Name(), step.X, step.Y, building->House->Class->Name(), (int)building->House->Class->IsMultiplayPassive,
				(int)(building->House->Shares_View_With(PlayerPtr) || building->SpiedBy[PlayerPtr]), (int)building->TClass->IsPipsDrawForAll,
				(int)building->Class->IsCanBeOccupied, (int)building->Class->IsShowOccupantPips, building->Class->MaxNumberOccupants,
				building->Occupants.Count(), (int)(Map.HoverObject == building), (int)building->IsSelected, (int)visible, (int)occupant_pips);
		}
	} else if (step.Command == "pickat") {
		// pickat x y: the object a click at that window pixel would pick, and the cell under it. Selection is left alone.
		Point2D const view = Screen_To_View_Offset(Point2D(std::atoi(step.Argument.c_str()), step.X) - ScreenTacticalRect.Top_Left());
		ObjectClass * object = TacticalMap->Get_Selectable_Object(view);
		Cell const cell = TacticalMap->Pixel_To_Cell(view + TacticalRect.Top_Left());
		DebugString("AUTOTEST   pickat %d,%d picks %s cell %d,%d\n", std::atoi(step.Argument.c_str()), step.X,
			object != NULL ? object->Class_Of()->Name() : "-", cell.X, cell.Y);
	} else if (step.Command == "rallyof") {
		// rallyof <TypeID>: the rally point (ArchiveTarget) of each of the player's structures of that type.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * building = Buildings[index];
			if (building->House != PlayerPtr || stricmp(building->Class->Name(), step.Argument.c_str()) != 0) continue;
			Cell const rally = building->ArchiveTarget != NULL ? building->ArchiveTarget->Center_Coord().As_Cell() : Cell(-1, -1);
			DebugString("AUTOTEST   rallyof %s cell %d,%d rally %d,%d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y, rally.X, rally.Y);
		}
	} else if (step.Command == "lift") {
		// lift <TypeID>: each object of the type with its height above the ground, its locomotor, its jumpjet state
		// and the state of its magnetron hold (IsAttackedByLocomotor, IsLetGoByLocomotor, IsCrashing).
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			char const * state = "-";
			char const * locomotor = "-";
			if (techno->Is_Foot()) {
				FootClass const * foot = static_cast<FootClass const *>(techno);
				if (foot->Locomotion != NULL) {
					locomotor = Locomotion_Class_ID(foot->Locomotion.get()) == ClassID_JumpjetLocomotion ? "jumpjet" : (Locomotion_Class_ID(foot->Locomotion.get()) == ClassID_DriveLocomotion ? "drive" : "other");
					JumpjetLocomotionClass const * jump = dynamic_cast<JumpjetLocomotionClass const *>(foot->Locomotion.get());
					if (jump != NULL) {
						state = jump->State_Name();
					}
				}
			}
			DebugString("AUTOTEST   lift %s cell %d,%d height %d loco %s jump %s attacked %d letgo %d crashing %d source %s target %s strength %d limbo %d weight %d layer %d ironcurtain %d sinking %d facing %d onbridge %d z %d doing %d\n", techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, techno->HeightAGL, locomotor, state, (int)techno->IsAttackedByLocomotor, (int)techno->IsLetGoByLocomotor, (int)techno->IsCrashing, techno->LocomotorSource != NULL ? techno->LocomotorSource->TClass->Name() : "-", techno->LocomotorTarget != NULL ? techno->LocomotorTarget->TClass->Name() : "-", (int)techno->Strength, (int)techno->IsInLimbo, (int)techno->TClass->Weight, (int)(techno->Is_Foot() ? static_cast<FootClass const *>(techno)->In_Which_Layer() : LAYER_NONE), (int)techno->Is_Iron_Curtained(), (int)techno->IsSinking, static_cast<int>(techno->PrimaryFacing.Current().As_Dir256()), (int)techno->IsOnBridge, techno->PositionCoord.Z, techno->RTTI == RTTI_INFANTRY ? (int)static_cast<InfantryClass const *>(techno)->Doing : -1);
		}
	} else if (step.Command == "spots") {
		// spots <TypeID>: each live object of that type, whoever owns it, with its cell, the sub-cell spot its
		// coordinate falls in (CellClass::Spot_Index) and its lepton offset inside the cell. Then the pairs of
		// them within 32 leptons of each other, which is what two objects sharing a spot looks like.
		std::vector<TechnoClass const *> found;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (!techno->IsInLimbo && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				found.push_back(techno);
			}
		}
		int pairs = 0;
		for (size_t first = 0; first < found.size(); first++) {
			Coord const at = found[first]->Get_Coord();
			DebugString("AUTOTEST   spot %s of %s cell %d,%d spot %d offset %d,%d mission %s\n", found[first]->TClass->Name(), found[first]->House->Class->Name(), at.As_Cell().X, at.As_Cell().Y, CellClass::Spot_Index(at), at.X & (CELL_LEPTON_W - 1), at.Y & (CELL_LEPTON_H - 1), MissionClass::Mission_Name(found[first]->Get_Mission()));
			for (size_t second = first + 1; second < found.size(); second++) {
				Coord const other = found[second]->Get_Coord();
				long long const dx = (long long)at.X - (long long)other.X;
				long long const dy = (long long)at.Y - (long long)other.Y;
				if (dx * dx + dy * dy < 32 * 32) {
					pairs++;
					DebugString("AUTOTEST   spot overlap %s %d,%d and %d,%d\n", found[first]->TClass->Name(), at.X, at.Y, other.X, other.Y);
				}
			}
		}
		DebugString("AUTOTEST spots %s count %d overlapping %d\n", step.Argument.c_str(), (int)found.size(), pairs);
	} else if (step.Command == "airstrike") {
		// airstrike <TypeID>: each object of that type, whoever owns it, with the frame its next strike is ready, and each
		// plane that strike still has out, with its cell, mission, ammunition and target.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   airstrike %s of %s cell %d,%d ready %d frame %d planes %d marked by %s\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, techno->AirstrikeReadyFrame, Frame, (int)techno->AirstrikePlanes.size(), techno->AirstrikeMarkedBy != NULL ? techno->AirstrikeMarkedBy->TClass->Name() : "-");
			{
				// The weapon chosen for the target, its range, and whether it reaches and can fire at the target now.
				int const which = techno->What_Weapon_Should_I_Use(techno->TarCom);
				WeaponTypeClass const * weapon = techno->Get_Class_Weapon_Data(which)->Weapon;
				bool const airstrike = weapon != NULL && weapon->WarheadPtr != NULL && weapon->WarheadPtr->IsAirstrike;
				DebugString("AUTOTEST   airstrike weapon %d %s airstrike %d range %d tarcom %s closeenough %d inrange %d fire %d\n", which, weapon != NULL ? weapon->Name() : "none", (int)airstrike, techno->Weapon_Range(which), dynamic_cast<ObjectClass const *>(techno->TarCom) != NULL ? dynamic_cast<ObjectClass const *>(techno->TarCom)->Class_Of()->Name() : "-", techno->TarCom != NULL ? (int)techno->Is_Close_Enough(techno->TarCom, which) : -1, techno->TarCom != NULL ? (int)techno->In_Range(techno->TarCom, which) : -1, techno->TarCom != NULL ? (int)techno->Can_Fire(techno->TarCom, which) : -1);
			}
			for (AircraftClass const * plane : techno->AirstrikePlanes) {
				ObjectClass const * tarobject = dynamic_cast<ObjectClass const *>(plane->TarCom);
				DebugString("AUTOTEST   airstrike plane %s cell %d,%d mission %s ammo %d target %s at %d,%d limbo %d\n", plane->TClass->Name(), plane->Get_Cell().X, plane->Get_Cell().Y, MissionClass::Mission_Name(plane->Get_Mission()), plane->Ammo, tarobject != NULL ? tarobject->Class_Of()->Name() : "-", tarobject != NULL ? tarobject->Get_Cell().X : -1, tarobject != NULL ? tarobject->Get_Cell().Y : -1, (int)plane->IsInLimbo);
			}
		}
	} else if (step.Command == "where") {
		// where <TypeID>: each object of that type, whoever owns it, with its cell, mission, destination and target.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			Cell const nav = techno->Is_Foot() && static_cast<FootClass const *>(techno)->NavCom != NULL ? static_cast<FootClass const *>(techno)->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
			Cell const tar = techno->TarCom != NULL ? techno->TarCom->Center_Coord().As_Cell() : Cell(-1, -1);
			ObjectClass const * tarobject = dynamic_cast<ObjectClass const *>(techno->TarCom);
			DebugString("AUTOTEST   where %s of %s cell %d,%d mission %s nav %d,%d target %d,%d strength %d limbo %d tarcom %s tarlimbo %d warped %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()), nav.X, nav.Y, tar.X, tar.Y, (int)techno->Strength, (int)techno->IsInLimbo, tarobject != NULL ? tarobject->Class_Of()->Name() : "-", tarobject != NULL ? (int)tarobject->IsInLimbo : -1, (int)techno->IsBeingWarpedOut);
			DebugString("AUTOTEST   slaveof %s owner %s\n", techno->TClass->Name(), techno->SlaveOwner != NULL ? techno->SlaveOwner->TClass->Name() : "-");
			DebugString("AUTOTEST   rank %s %d\n", techno->TClass->Name(), techno->Veterancy.Is_Elite() ? 2 : (techno->Veterancy.Is_Veteran() ? 1 : 0));
			if (techno->RTTI == RTTI_UNIT) {
				UnitClass const * vehicle = static_cast<UnitClass const *>(techno);
				DebugString("AUTOTEST   mirage look %s blink %d owner %d ally %d cycle %d terrain for player %d shimmer %d\n", vehicle->MirageType != NULL ? vehicle->MirageType->Name() : "none", (int)vehicle->Is_Blinking_Disguise(), (int)vehicle->House->Is_Player_Control(), (int)vehicle->House->Is_Ally(PlayerPtr), vehicle->Mirage_Cycle(), (int)vehicle->Is_Shown_As_Terrain_To(PlayerPtr), vehicle->Mirage_Shimmer_Flags());
			}
			if (techno->RTTI == RTTI_AIRCRAFT) {
				AircraftClass const * plane = static_cast<AircraftClass const *>(techno);
				DebugString("AUTOTEST   flight %s straight %d loaner %d flyby %d flyback %d entered %d inlocal %d legal %d\n", techno->TClass->Name(), (int)plane->IsLockedStraight, (int)plane->IsALoaner, (int)plane->Class->IsFlyBy, (int)plane->Class->IsFlyBack, (int)plane->IsLocked, (int)Map.In_Local_Radar(plane->PositionCell), (int)Map.In_Radar(plane->PositionCell));
				Cell yard = CELL_NONE;
				int yards = 0;
				for (int index = 0; index < Buildings.Count(); index++) {
					BuildingClass const * building = Buildings[index];
					if (building->House == plane->House && building->Class->IsConstructionYard && !building->IsInLimbo) {
						if (yards == 0) yard = building->Get_Cell();
						yards++;
					}
				}
				DebugString("AUTOTEST   edges start %d base %d entry %d exit %d yards %d first %d,%d local %d,%d,%d,%d\n", (int)plane->House->Control.Edge, (int)plane->House->Base_Edge, (int)plane->House->Entry_Edge(), (int)plane->House->Opposite_Base_Edge(), yards, (int)yard.X, (int)yard.Y, Map.LocalRect.X, Map.LocalRect.Y, Map.LocalRect.Width, Map.LocalRect.Height);
			}
			{
				bool const firing = techno->Is_Foot() && static_cast<FootClass const *>(techno)->IsFiring;
				int const which = techno->What_Weapon_Should_I_Use(techno->TarCom);
				bool const inrange = techno->TarCom != NULL && techno->In_Range(techno->TarCom, which);
				int const fire = techno->TarCom != NULL ? (int)techno->Can_Fire(techno->TarCom, which) : -1;
				DebugString("AUTOTEST   cloak %s cloak %d firing %d inrange %d fire %d\n", techno->TClass->Name(), (int)techno->Cloak, (int)firing, (int)inrange, fire);
				// visual: how the local player is shown the object (VISUAL_*: 0 normal, 1 indistinct, 2 darken, 3 shadowy, 4 ripple, 5 hidden),
				// with the cloak facts that decide it.
				DebugString("AUTOTEST   visual %s character %d raw %d sensed %d height %d locked %d cloakable %d recloak %d ready %d delay %d stage %d owned %d\n",
					techno->TClass->Name(), (int)techno->Visual_Character(false), (int)techno->Visual_Character(true), (int)techno->Is_Sensed_By_Player(),
					(int)techno->HeightAGL, (int)techno->IsLocked, (int)techno->IsCloakable, (int)techno->Is_Allowed_To_Recloak(), (int)techno->Is_Ready_To_Cloak(), (int)techno->CloakDelay, techno->CloakingDevice.Fetch_Stage(), (int)techno->IsOwnedByPlayer);
				// radar: whether the local player's radar plots the object, and how it counts as detected when it only shows because
				// the player senses it (the DetectedType).
				DetectedType detected = DETECTED_NONE;
				bool const plotted = techno->Is_Radar_Visible(detected);
				DebugString("AUTOTEST   radar %s plotted %d detected %d\n", techno->TClass->Name(), (int)plotted, (int)detected);
				DebugString("AUTOTEST   sight %s range %d increase %d\n", techno->TClass->Name(), techno->TClass->SightRange, (int)techno->SightIncrease);
				DebugString("AUTOTEST   facing %s body %d desired %d turret %d desired %d rotating %d paralysis %d moving %d\n", techno->TClass->Name(), (int)techno->PrimaryFacing.Current().As_Dir256(), (int)techno->PrimaryFacing.Desired().As_Dir256(),
					(int)techno->SecondaryFacing.Current().As_Dir256(), (int)techno->SecondaryFacing.Desired().As_Dir256(), (int)techno->PrimaryFacing.Is_Rotating(), techno->Is_Foot() ? (int)static_cast<FootClass const *>(techno)->ParalysisTimer : -1,
					techno->Is_Foot() ? (int)static_cast<FootClass const *>(techno)->Locomotion->Is_Moving() : -1);
				DebugString("AUTOTEST   recloak %s arm %d immobilized %d stun %d deactivated %d locomotor %d radio %d tarcom %d closeenough %d\n", techno->TClass->Name(), (int)techno->Arm, (int)techno->Is_Immobilized(), (int)techno->StunDuration, (int)techno->IsDeactivated, (int)techno->IsAttackedByLocomotor,
					techno->Contact_With_Whom() != NULL ? 1 : 0, techno->TarCom != NULL ? 1 : 0, techno->TarCom != NULL ? (int)techno->Is_Close_Enough(techno->TarCom, techno->What_Weapon_Should_I_Use(techno->TarCom)) : -1);
			}
			if (techno->ParasiteImUsing) {
				ParasiteClass const & parasite = *techno->ParasiteImUsing;
				DebugString("AUTOTEST   grapple stage %d frame %d delay %d anim %d victim %s angle %.4f\n", (int)parasite.GrappleState, parasite.GrappleAnimFrame, parasite.GrappleAnimDelay, parasite.GrappleAnim != NULL ? 1 : 0,
					parasite.Victim != NULL ? parasite.Victim->TClass->Name() : "-", parasite.Victim != NULL ? (double)parasite.Victim->AngleRotatedSideways : 0.0);
			}
			if (techno->RTTI == RTTI_INFANTRY) {
				InfantryClass const * soldier = static_cast<InfantryClass const *>(techno);
				DebugString("AUTOTEST   soldier %s do %d deployed %d\n", soldier->Class->Name(), (int)soldier->Doing, (int)soldier->Is_Deployed());
			}
			if (techno->Is_Foot()) {
				FootClass const * foot = static_cast<FootClass const *>(techno);
				TeleportLocomotionClass const * teleport = dynamic_cast<TeleportLocomotionClass const *>(foot->Locomotion.get());
				if (teleport != NULL) {
					DebugString("AUTOTEST   warp %s phase %d end %d warping %d chrono %d selected %d\n", techno->TClass->Name(), (int)teleport->Warp_Phase(), teleport->Warp_End(), (int)techno->IsBeingWarpedOut, (int)foot->Is_Chrono_Warping(), (int)techno->IsSelected);
				}
			}
		}
	} else if (step.Command == "turretcoord") {
		// turretcoord <TypeID>: the body and turret facings, TurretOffset and mounting that Turret_Coord(0) is built from.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			TPoint3D<int> const flh = techno->Weapon_FLH(0);
			Coord const render = techno->Render_Coord();
			Coord const mount = techno->Turret_Coord(0);
			DebugString("AUTOTEST   turretcoord %s of %s cell %d,%d body %d turret %d offset %d flh %d,%d,%d relative %d,%d,%d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, (int)techno->PrimaryFacing.Current().As_Dir256(), (int)techno->Turret_Facing().As_Dir256(), (int)techno->TClass->TurretOffset, flh.X, flh.Y, flh.Z, mount.X - render.X, mount.Y - render.Y, mount.Z - render.Z);
			if (techno->Is_Foot()) {
				FootClass const * foot = static_cast<FootClass const *>(techno);
				Matrix3D const draw = foot->Locomotion->Draw_Matrix(NULL);
				DebugString("AUTOTEST   turretcoord draw %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f %.4f\n", (double)draw[0].X, (double)draw[0].Y, (double)draw[0].Z, (double)draw[0].W, (double)draw[1].X, (double)draw[1].Y, (double)draw[1].Z, (double)draw[1].W, (double)draw[2].X, (double)draw[2].Y, (double)draw[2].Z, (double)draw[2].W);
			}
		}
	} else if (step.Command == "recoil") {
		// recoil <TypeID>: the recoil phase, frames left and travel of the turret body and barrel of each building of that type.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * building = Buildings[index];
			if (building->IsInLimbo || stricmp(building->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   recoil %s at %d,%d turret phase %d left %d travel %.3f barrel phase %d left %d travel %.3f\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
				(int)building->TurretRecoil.CurrentPhase, building->TurretRecoil.FramesLeft, (double)building->TurretRecoil.TravelSoFar,
				(int)building->BarrelRecoil.CurrentPhase, building->BarrelRecoil.FramesLeft, (double)building->BarrelRecoil.TravelSoFar);
		}
	} else if (step.Command == "unitrecoil") {
		// unitrecoil <TypeID>: the recoil phase, frames left and travel of the turret body and barrel of each vehicle of that type.
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass const * unit = Units[index];
			if (unit->IsInLimbo || stricmp(unit->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   unitrecoil %s at %d,%d turret phase %d left %d travel %.3f barrel phase %d left %d travel %.3f\n", unit->Class->Name(), unit->Get_Cell().X, unit->Get_Cell().Y,
				(int)unit->TurretRecoil.CurrentPhase, unit->TurretRecoil.FramesLeft, (double)unit->TurretRecoil.TravelSoFar,
				(int)unit->BarrelRecoil.CurrentPhase, unit->BarrelRecoil.FramesLeft, (double)unit->BarrelRecoil.TravelSoFar);
		}
	} else if (step.Command == "rock") {
		// rock <TypeID>: the sideways and forwards tilt of each object of that type, the tilt per frame still to come, its facing and strength, and whether a parasite is inside it.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->IsInLimbo || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   rock %s frame %d at %d,%d sideways %.5f forwards %.5f sideways per frame %.5f forwards per frame %.5f facing %d strength %d parasite %d\n", techno->TClass->Name(), Frame, techno->Get_Cell().X, techno->Get_Cell().Y,
				(double)techno->AngleRotatedSideways, (double)techno->AngleRotatedForwards, (double)techno->RockingSidewaysPerFrame, (double)techno->RockingForwardsPerFrame, (int)techno->PrimaryFacing.Current().Facing, (int)techno->Strength, (int)(techno->ParasiteEatingMe != NULL));
		}
	} else if (step.Command == "statics") {
		// statics: how many positioned sounds a trigger started are still going.
		DebugString("AUTOTEST   statics %d\n", Static_Sounds_Active(STATIC_SOUND_TRIGGER));
	} else if (step.Command == "enterprobe") {
		// enterprobe: each vehicle on the enter mission, with the building it heads for, its dock cell and the radio state.
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass * unit = Units[index];
			if (unit->IsInLimbo || unit->CurrentMission != MISSION_ENTER) continue;
			BuildingClass * target = unit->NavCom != NULL ? dynamic_cast<BuildingClass *>(unit->NavCom) : NULL;
			TechnoClass * contact = unit->Contact_With_Whom();
			DebugString("AUTOTEST   enterprobe %s %s cell %d,%d nav %s contact %s tethered %d moving %d mzone %d naval %d\n", unit->House->Class->Name(), unit->Class->Name(),
				unit->Get_Cell().X, unit->Get_Cell().Y, unit->NavCom != NULL ? "set" : "-", contact != NULL ? contact->TClass->Name() : "-", (int)unit->IsTethered,
				(int)unit->Locomotion->Is_Moving(), (int)unit->Class->MZone, (int)unit->Class->IsNaval);
			if (target != NULL) {
				Cell const dock = target->Destination_Coord_For(unit).As_Cell();
				DebugString("AUTOTEST     target %s cell %d,%d naval %d dock %d,%d land %d links %d\n", target->Class->Name(), target->Get_Cell().X, target->Get_Cell().Y,
					(int)target->Class->IsNaval, dock.X, dock.Y, (int)Map[dock].Land_Type(), (int)target->In_Radio_Contact());
			}
		}
	} else if (step.Command == "supers") {
		// supers: each super weapon a house has, how much of its charge is left and the time it charges in.
		DebugString("AUTOTEST   super defense frames %d distance %d probability %d,%d,%d\n", Rule->AISuperDefenseFrames, (int)Rule->AISuperDefenseDistance, Rule->AISuperDefenseProbability.Count() > 0 ? Rule->AISuperDefenseProbability[0] : -1, Rule->AISuperDefenseProbability.Count() > 1 ? Rule->AISuperDefenseProbability[1] : -1, Rule->AISuperDefenseProbability.Count() > 2 ? Rule->AISuperDefenseProbability[2] : -1);
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			DebugString("AUTOTEST   aim house %s target %d,%d base center %d,%d defensive cell %d,%d since frame %d\n", house->Class->Name(), (int)house->PreferredTargetCell.X, (int)house->PreferredTargetCell.Y, (int)house->Center.As_Cell().X, (int)house->Center.As_Cell().Y, (int)house->PreferredDefensiveCell.X, (int)house->PreferredDefensiveCell.Y, house->PreferredDefensiveCellStartTime);
			for (int s = 0; s < house->SuperWeapon.Count(); s++) {
				SuperClass * super = house->SuperWeapon[s];
				if (super == NULL || !super->Is_Present()) continue;
				DebugString("AUTOTEST   super house %s index %d %s ready %d left %d time %d custom %d flashtab %d suspended %d usable %d\n", house->Class->Name(), s, super->Class->Name(), (int)super->Is_Ready(), (int)super->Control.Value(), super->Get_Recharge_Time(), super->CustomRechargeTime, (int)super->Should_Flash_Tab(), (int)super->Is_Suspended(), (int)super->Can_Place());
			}
		}
	} else if (step.Command == "tabflash") {
		// tabflash: the tabs the modern sidebar panel flashes now, one bit per tab.
		DebugString("AUTOTEST   tabflash mask %u frame %d\n", SidebarClass::Tab_Flash_Mask(), Frame);
	} else if (step.Command == "ambient") {
		// ambient <TypeID>: the sound the ambient table keeps on each object of that type, and how many objects it serves.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   ambient %s type sound %d attached %d limbo %d table %d frame %d\n", techno->TClass->Name(), (int)techno->TClass->AmbientSound, (int)AmbientSounds.Attached(techno), (int)techno->IsInLimbo, AmbientSounds.Count(), Frame);
		}
	} else if (step.Command == "trails") {
		// trails: how many line trails exist and how many of their places still show.
		DebugString("AUTOTEST   trails lines %d lit %d frame %d\n", LineTrailClass::Count(), LineTrailClass::Lit_Places(), Frame);
	} else if (step.Command == "ammo") {
		// ammo <TypeID>: each unit of that type with its ammunition and the frames left before it regains a shot.
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass const * unit = Units[index];
			if (stricmp(unit->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   ammo %s of %s ammo %d of %d reload %d trail type %d art %d graphic %s line %d frame %d\n", unit->Class->Name(), unit->House->Class->Name(), unit->Ammo, unit->Class->MaxAmmo, (int)unit->Reload, (int)unit->Class->IsUseLineTrail, (int)ArtINI.Get_Bool(unit->Class->Graphic_Name(), "UseLineTrail", false), unit->Class->Graphic_Name(), (int)(unit->LineTrailer != NULL), Frame);
		}
	} else if (step.Command == "houses") {
		// houses: each house's money, power and spy effects, then a "seat" line for who plays it. The
		// "who" line names the country the house acts as and counts the objects it owns.
		// Each tech secret lab follows, with its owner, the item it offers and whether the owner lists it.
		for (int index = 0; index < Houses.Count(); index++) {
			HouseClass * house = Houses[index];
			int owned = 0;
			for (int object = 0; object < Technos.Count(); object++) {
				if (Technos[object]->House == house && !Technos[object]->IsInLimbo) {
					owned++;
				}
			}
			DebugString("AUTOTEST   who %s country %s player %d objects %d\n", house->Class->Name(), house->ActLike != HOUSE_NONE ? HouseTypes[house->ActLike]->Name() : "<none>", (int)(house == PlayerPtr), owned);
			int allies = 0;
			for (int other = 0; other < Houses.Count(); other++) {
				if (other != index && house->Is_Ally(Houses[other])) {
					allies |= 1 << other;
				}
			}
			DebugString("AUTOTEST   seat %d %s name %s human %d scheme %d start %d difficulty %d allies %x\n", index, house->Class->Name(), (char const *)house->IniName, (int)house->IsHuman, house->Scheme, house->SpawnWaypoint, (int)house->Difficulty, allies);
			DebugString("AUTOTEST   counts %s buildings %d units %d infantry %d aircraft %d lost %d/%d\n", house->Class->Name(), house->CurBuildings, house->CurUnits, house->CurInfantry, house->CurAircraft, house->BuildingsLost, house->UnitsLost);
			DebugString("AUTOTEST   house %s money %d power %d drain %d blackout %d stolen %d%d%d barracks %d factory %d fraction %.2f\n", house->Class->Name(), house->Available_Money(), house->Power, house->Drain,
				(int)house->PowerBlackout, (int)house->IsSide0TechStolen, (int)house->IsSide1TechStolen, (int)house->IsSide2TechStolen, (int)house->IsBarracksInfiltrated, (int)house->IsWarFactoryInfiltrated, house->Power_Fraction());
			DebugString("AUTOTEST   gatherers %s %d powered %d\n", house->Class->Name(), house->Count_Resource_Gatherers(), house->PoweredUnitCenters);
		}
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass * lab = Buildings[index];
			if (!lab->Class->IsSecretLab) continue;
			TechnoTypeClass const * item = lab->Secret_Item();
			DebugString("AUTOTEST   lab %s of %s cell %d,%d limbo %d offers %s listed %d\n", lab->Class->Name(), lab->House->Class->Name(), lab->Get_Cell().X, lab->Get_Cell().Y, (int)lab->IsInLimbo, item != NULL ? item->Name() : "-", (int)(lab->House->SecretLabs.ID(lab) != -1));
		}
	} else if (step.Command == "census") {
		// census: per house, how many structures, vehicles, infantry and aircraft it owns, how many of the
		// mobile ones are moving or have a target, and the size of the object lists, for watching long games.
		// census ore writes the tiberium report of Log_Ore instead.
		if (step.Argument == "ore") {
			Log_Ore();
			return;
		}
		if (step.Argument == "cells") {
			Log_Ore_Cells(8);
			return;
		}
		DebugString("AUTOTEST   census frame %d technos %d anims %d bullets %d waves %d teams %d\n", Frame, Technos.Count(), Anims.Count(), Bullets.Count(), Waves.Count(), Teams.Count());
		for (int house = 0; house < Houses.Count(); house++) {
			int buildings = 0, units = 0, infantry = 0, aircraft = 0, busy = 0, harvesters = 0;
			int destinations = 0, gatherers_on_map = 0, slave_miners = 0;
			std::string types;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass * techno = Technos[index];
				if (techno->House != Houses[house] || techno->IsInLimbo || techno->Strength <= 0) {
					continue;
				}
				if (techno->TClass->IsResourceDestination) destinations++;
				if (techno->TClass->IsResourceGatherer) gatherers_on_map++;
				if (stricmp(techno->TClass->Name(), "SMIN") == 0) slave_miners++;
				if (techno->What_Am_I() == RTTI_BUILDING) {
					buildings++;
					types += std::string(" ") + techno->TClass->Name();
					continue;
				}
				if (techno->What_Am_I() == RTTI_INFANTRY) infantry++;
				else if (techno->What_Am_I() == RTTI_AIRCRAFT) aircraft++;
				else units++;
				if (step.Argument == "units" && (techno->What_Am_I() != RTTI_INFANTRY || stricmp(techno->TClass->Name(), "SLAV") == 0)) {
					FootClass const * foot = static_cast<FootClass const *>(techno);
					Cell const nav = foot->NavCom != NULL ? foot->NavCom->Center_Coord().As_Cell() : Cell(-1, -1);
					DebugString("AUTOTEST     %s %s cell %d,%d mission %s nav %d,%d strength %d\n", Houses[house]->Class->Name(), techno->TClass->Name(), techno->Get_Cell().X, techno->Get_Cell().Y, MissionClass::Mission_Name(techno->Get_Mission()), nav.X, nav.Y, (int)techno->Strength);
				}
				if (techno->Is_Foot()) {
					FootClass * foot = static_cast<FootClass *>(techno);
					if (foot->NavCom != NULL || foot->TarCom != NULL) {
						busy++;
					}
					if (techno->What_Am_I() == RTTI_UNIT && static_cast<UnitClass *>(techno)->Class->IsToHarvest) {
						harvesters++;
					}
				}
			}
			if (buildings + units + infantry + aircraft > 0 && Houses[house]->Class->IsMultiplayPassive == false) {
				DebugString("AUTOTEST   census %s money %d power %d/%d defeated %d: buildings %d units %d (harvesters %d) infantry %d aircraft %d busy %d |%s\n", Houses[house]->Class->Name(), Houses[house]->Available_Money(),
					Houses[house]->Power, Houses[house]->Drain, (int)Houses[house]->IsDefeated, buildings, units, harvesters, infantry, aircraft, busy, types.c_str());
				HouseClass const * owner = Houses[house];
				if (!owner->Is_Human_Player()) {
					DebugString("AUTOTEST     builds struct %s unit %s infantry %s aircraft %s state %d mode %d tiberiumshort %d tech %d\n",
						owner->BuildStructure != STRUCT_NONE ? BuildingTypes[owner->BuildStructure]->Name() : "-", owner->BuildUnit != UNIT_NONE ? UnitTypes[owner->BuildUnit]->Name() : "-",
						owner->BuildInfantry != INFANTRY_NONE ? InfantryTypes[owner->BuildInfantry]->Name() : "-", owner->BuildAircraft != AIRCRAFT_NONE ? AircraftTypes[owner->BuildAircraft]->Name() : "-",
						(int)owner->State, (int)owner->ProductionMode, (int)owner->IsTiberiumShort, owner->Control.TechLevel);
					ProducedTallyStruct const produced = _ProducedBy[owner];
					DebugString("AUTOTEST     economy difficulty %d gatherers %d on map %d slave miners %d destinations %d produced units %d infantry %d aircraft %d structures %d factories units %d infantry %d aircraft %d\n",
						(int)owner->Difficulty, owner->Count_Resource_Gatherers(), gatherers_on_map, slave_miners, destinations, produced.Count[0], produced.Count[1], produced.Count[2], produced.Count[3],
						owner->Factory_Count(RTTI_UNIT), owner->Factory_Count(RTTI_INFANTRY), owner->Factory_Count(RTTI_AIRCRAFT));
				}
				for (int f = 0; f < Factories.Count(); f++) {
					FactoryClass * factory = Factories[f];
					if (factory->Get_Object() == NULL || factory->Get_Object()->House != owner) continue;
					DebugString("AUTOTEST     factory %s complete %d building %d suspended %d\n", factory->Get_Object()->TClass->Name(), factory->Completion(), (int)factory->Is_Building(), (int)factory->Is_Suspended());
				}
			}
		}
	} else if (step.Command == "count") {
		// count <TypeID>: the number of live objects of the type on the map, per owner.
		for (int house = 0; house < Houses.Count(); house++) {
			int count = 0;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass const * techno = Technos[index];
				if (techno->House == Houses[house] && !techno->IsInLimbo && techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
					count++;
				}
			}
			if (count > 0) {
				DebugString("AUTOTEST   count %s house %s: %d\n", step.Argument.c_str(), Houses[house]->Class->Name(), count);
			}
		}
	} else if (step.Command == "sensors") {
		// sensors: each vehicle, soldier or aircraft laying down SensorsSight coverage, with the cell and radius it
		// last laid it for and whether its own cell counts as sensed for its owner.
		int total = 0;
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (!techno->Is_Foot() || techno->TClass->SensorsSight <= 0) {
				continue;
			}
			FootClass const * foot = static_cast<FootClass const *>(techno);
			bool const sensed = foot->SensorCell != CELL_NONE && foot->SensorHouse != NULL && Map[foot->SensorCell].Is_Sensed(foot->SensorHouse);
			total += foot->SensorCell != CELL_NONE;
			DebugString("AUTOTEST   sensor frame %d %s of %s cell %d,%d sensor cell %d,%d radius %d of %d sensed %d\n", Frame, techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y,
				foot->SensorCell.X, foot->SensorCell.Y, foot->SensorRadius, techno->TClass->SensorsSight, (int)sensed);
		}
		int sensed_cells = 0;
		Map.Reset_Iterator();
		for (CellClass * cell = Map.Iterate(); cell != NULL; cell = Map.Iterate()) {
			sensed_cells += cell->Is_Sensed(PlayerPtr);
		}
		DebugString("AUTOTEST   sensors frame %d laying coverage %d, cells sensed for the player %d\n", Frame, total, sensed_cells);
	} else if (step.Command == "partsys") {
		// partsys: each live particle system, with the object it rides on.
		for (int index = 0; index < ParticleSystems.Count(); index++) {
			ParticleSystemClass const * system = ParticleSystems[index];
			AbstractClass const * source = system->Source_Object();
			ObjectClass const * object = source != NULL ? source->As_ObjectClass() : NULL;
			DebugString("AUTOTEST   partsys frame %d %s lifetime %d at %d,%d,%d on %s\n", Frame, system->Class != NULL ? system->Class->Name() : "-", system->Lifetime,
				system->Get_Coord().X, system->Get_Coord().Y, system->Get_Coord().Z, object != NULL ? object->Class_Of()->Name() : "-");
		}
	} else if (step.Command == "bullets") {
		// bullets: each live projectile.
		for (int index = 0; index < Bullets.Count(); index++) {
			BulletClass const * bullet = Bullets[index];
			if (bullet->Class != NULL) {
				DebugString("AUTOTEST   bullet frame %d %s firerhouse %d scalable %d firerspalette %d spawndelay %d scaled %d target %d,%d\n", Frame, bullet->Class->Name(), bullet->FirerHouse,
					(int)bullet->Class->IsScalable, (int)bullet->Class->IsFirersPalette, bullet->Class->SpawnDelay, bullet->Class->ScaledSpawnDelay,
					bullet->TarCom != NULL ? bullet->TarCom->Center_Coord().As_Cell().X : -1, bullet->TarCom != NULL ? bullet->TarCom->Center_Coord().As_Cell().Y : -1);
			}
		}
	} else if (step.Command == "radbeams") {
		// radbeams: how many beams of each kind are live.
		DebugString("AUTOTEST   radbeams frame %d wave %d eruption %d visible %d\n", Frame, RadBeamClass::Count(false), RadBeamClass::Count(true), RadBeamClass::Count_Visible_Eruptions());
		} else if (step.Command == "radlevel") {
			// radlevel x y: the radiation level of that cell, on a line of its own so the cell line's length does not cut it off.
			int const x = std::atoi(step.Argument.c_str());
			DebugString("AUTOTEST   radlevel %d,%d level %.3f\n", x, step.X, Map[Cell(x, step.X)].RadLevel);
	} else if (step.Command == "quantity") {
		// quantity <TypeID>: how many of that type each house is counted as having, owned and active. A
		// structure uses the building counts, and a unit, infantry or aircraft type its own counts.
		TechnoTypeClass const * ttype = Find_Type(step.Argument);
		if (ttype != NULL) {
			int const id = ttype->Fetch_Heap_ID();
			bool const building = dynamic_cast<BuildingTypeClass const *>(ttype) != NULL;
			bool const aircraft = dynamic_cast<AircraftTypeClass const *>(ttype) != NULL;
			bool const infantry = dynamic_cast<InfantryTypeClass const *>(ttype) != NULL;
			for (int house = 0; house < Houses.Count(); house++) {
				HouseClass * owner = Houses[house];
				CounterClass const & owned = building ? owner->BQuantity : aircraft ? owner->AQuantity : infantry ? owner->IQuantity : owner->UQuantity;
				CounterClass const & active = building ? owner->ABQuantity : aircraft ? owner->AAQuantity : infantry ? owner->AIQuantity : owner->AUQuantity;
				if (owned.Value(id) != 0 || active.Value(id) != 0) DebugString("AUTOTEST   quantity %s (%d) house %s owned %d active %d\n", step.Argument.c_str(), id, owner->Class->Name(), owned.Value(id), active.Value(id));
			}
		}
	} else if (step.Command == "effects") {
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass * techno = Technos[index];
			if (techno->Strength > 0 && stricmp(techno->TClass->Name(), step.Argument.c_str()) == 0) {
				AttachedEffectsClass const & effects = techno->AttachedEffects;
				int const speed = techno->Is_Foot() ? static_cast<FootClass *>(techno)->Current_Speed() : 0;
				DebugString("AUTOTEST   effects %s of %s: count %d speed x%.3f armor x%.3f firepower x%.3f rof x%.3f cloakable %d | bias armor %.3f speed %.3f firepower %.3f | speed %d strength %d arm %d limbo %d\n",
					techno->TClass->Name(), techno->House->Class->Name(), effects.Count(), effects.Speed_Multiplier(), effects.Armor_Multiplier(),
					effects.Firepower_Multiplier(), effects.ROF_Multiplier(), (int)effects.Is_Cloakable(), techno->ArmorBias, techno->Is_Foot() ? static_cast<FootClass *>(techno)->SpeedBias : 1.0, techno->FirepowerBias, speed, (int)techno->Strength, (int)techno->Arm, (int)techno->IsInLimbo);
			}
		}
	} else if (step.Command == "schemes") {
		// schemes: every color scheme by list position, then the scheme each house draws with.
		for (int index = 0; index < ColorSchemes.Count(); index++) {
			DebugString("AUTOTEST   scheme %d %s\n", index, ColorSchemes[index]->Name != NULL ? ColorSchemes[index]->Name : "-");
		}
		for (int index = 0; index < Houses.Count(); index++) {
			DebugString("AUTOTEST   house %s scheme %d\n", Houses[index]->Class->Name(), Houses[index]->Scheme);
		}
	} else if (step.Command == "seq") {
		// seq <InfantryTypeID>: the art sequences the type has, by DoType number.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL && type->Fetch_RTTI() == RTTI_INFANTRYTYPE && ((InfantryTypeClass const *)type)->DoControls != NULL) {
			DoInfoStruct const * controls = ((InfantryTypeClass const *)type)->DoControls;
			for (int index = 0; index < DO_COUNT; index++) {
				if (controls[index].Count > 0) {
					DebugString("AUTOTEST   seq %s %d frame %d count %d jump %d\n", type->Name(), index, controls[index].Frame, controls[index].Count, controls[index].Jump);
				}
			}
		}
	} else if (step.Command == "plan") {
		// plan: each computer house's base plan, node by node; a negative type is a placeholder.
		for (int house = 0; house < Houses.Count(); house++) {
			HouseClass * owner = Houses[house];
			if (owner == PlayerPtr || owner->Base.Nodes.Count() == 0) {
				continue;
			}
			std::string line;
			for (int index = 0; index < owner->Base.Nodes.Count(); index++) {
				int type = owner->Base.Nodes[index].Type;
				line += (type >= 0 && type < BuildingTypes.Count()) ? BuildingTypes[type]->Name() : std::to_string(type);
				line += " ";
			}
			DebugString("AUTOTEST   plan %s difficulty %d: %s\n", owner->Class->Name(), (int)owner->Difficulty, line.c_str());
		}
	} else if (step.Command == "team") {
		// team <TeamTypeID>: makes a team of that type for the first computer house and puts in every unit of that house no team holds.
		HouseClass * enemy = NULL;
		for (int index = 0; index < Houses.Count(); index++) {
			if (Houses[index] != PlayerPtr && Houses[index]->ConYards.Count() > 0) {
				enemy = Houses[index];
				break;
			}
		}
		TeamTypeClass * type = TeamTypeClass::From_Name(step.Argument.c_str());
		TeamClass * team = enemy != NULL && type != NULL ? type->Create_One_Of(enemy) : NULL;
		int added = 0;
		if (team != NULL && step.Line.size() > 6 && step.Line.compare(step.Line.size() - 6, 6, " plane") == 0) {
			// The first free aircraft joins with Team::Add, not Recruit, which would clear its target (TeamClass::Recruit, 0x6EAA90).
			AircraftClass * plane = NULL;
			for (int index = 0; plane == NULL && index < Aircraft.Count(); index++) {
				if (Aircraft[index]->House == enemy && !Aircraft[index]->IsInLimbo && Aircraft[index]->Team == NULL) {
					plane = Aircraft[index];
				}
			}
			if (plane != NULL && team->Add(plane)) {
				added++;
			}
			DebugString("AUTOTEST team %s plane: %s, %d members, heap %d\n", step.Argument.c_str(), plane != NULL ? plane->TClass->Name() : "no free aircraft", added, type->Fetch_Heap_ID());
		} else {
			if (team != NULL) {
				team->IsForcedActive = true;
				for (int index = 0; index < Technos.Count(); index++) {
					TechnoClass * techno = Technos[index];
					if (techno->House == enemy && techno->Is_Foot() && !techno->IsInLimbo && ((FootClass *)techno)->Team == NULL && techno->RTTI != RTTI_AIRCRAFT) {
						if (team->Add((FootClass *)techno)) {
							added++;
						}
					}
				}
			}
			DebugString("AUTOTEST team %s: %s, %d members, heap %d\n", step.Argument.c_str(), team != NULL ? "made" : "not made", added, type != NULL ? type->Fetch_Heap_ID() : -1);
		}
	} else if (step.Command == "teamtrace" || step.Command == "buildtrace") {
		// teamtrace <0|1>: logs each team script line a team starts, as TEAMTRACE lines.
		// buildtrace <0|1>: logs each structure a computer house starts and places, as BUILDTRACE lines.
		if (step.Command == "teamtrace") {
			TeamClass::TraceScripts = std::atoi(step.Argument.c_str()) != 0;
		} else {
			HouseClass::TraceBuilding = std::atoi(step.Argument.c_str()) != 0;
		}
	} else if (step.Command == "teams") {
		for (int index = 0; index < Teams.Count(); index++) {
			TeamClass * team = Teams[index];
			int members = 0;
			for (FootClass * member = team->Get_Member(); member != NULL; member = member->Member) {
				members++;
			}
			TeamMissionClass mission = team->Script != NULL ? team->Script->Get_Current_Mission() : TeamMissionClass(TMISSION_NONE, 0);
			ScriptTypeClass const * script = team->Script != NULL ? team->Script->Get_Type() : NULL;
			DebugString("AUTOTEST   team %s house %s members %d mission %d data %d moving %d hasbeen %d full %d under %d script %s line %d/%d\n",
				team->Class->Name(), team->House->Class->Name(), members, (int)mission.Mission, mission.Data.Value,
				(int)team->IsMoving, (int)team->IsHasBeen, (int)team->IsFullStrength, (int)team->IsUnderStrength,
				script != NULL ? script->Name() : "-", team->Script != NULL ? team->Script->Get_Line() : -1, script != NULL ? script->MissionCount : 0);
			if (team->IsUnderStrength || !team->IsFullStrength) {
				TEAM_MEMBER_LIST missing;
				team->Team_Members(missing);
				std::string names;
				for (int m = 0; m < missing.Count(); m++) {
					names += std::string(" ") + missing[m]->Name() + (team->House->Can_Build(missing[m], false, false) == 0 ? ("(cannot lvl" + std::to_string(missing[m]->Level) + " req" + std::to_string(missing[m]->RequiredHouses) + " forb" + std::to_string(missing[m]->ForbiddenHouses) + " unb" + std::to_string((int)missing[m]->IsUnbuildable) + " own" + std::to_string(missing[m]->Ownable) + " mask" + std::to_string(team->House->Own_Mask()) + ")") : std::string(""));
				}
				DebugString("AUTOTEST     team %s missing%s\n", team->Class->Name(), names.c_str());
				DebugString("AUTOTEST     team %s group %d recruiter %d priority %d\n", team->Class->Name(), team->Class->Get_Group(), (int)team->Class->IsRecruiter, (int)team->Class->RecruitPriority);
				for (int t = 0; t < team->Class->TaskForce->ClassCount; t++) {
					DebugString("AUTOTEST       force %d %s want %d have %d\n", t, team->Class->TaskForce->Members[t].Class->Name(), team->Class->TaskForce->Members[t].Quantity, team->Quantity[t]);
				}
				for (int i = 0; i < Infantry.Count(); i++) {
					InfantryClass * soldier = Infantry[i];
					if (soldier->House != team->House) continue;
					bool wanted = false;
					for (int t = 0; t < team->Class->TaskForce->ClassCount; t++) {
						if (team->Class->TaskForce->Members[t].Class == soldier->Class_Of()) wanted = true;
					}
					if (!wanted) continue;
					int typeindex = 0;
					DebugString("AUTOTEST       cand %s cell %d,%d group %d team %s mission %d radio %d recruitable %d limbo %d canadd %d\n", soldier->Class->Name(),
						soldier->Get_Cell().X, soldier->Get_Cell().Y, soldier->Group, soldier->Team != NULL ? soldier->Team->Class->Name() : "-", (int)soldier->Mission, (int)soldier->In_Radio_Contact(),
						(int)soldier->IsTeamRecruitable, (int)soldier->IsInLimbo, (int)team->Can_Add(soldier, typeindex));
				}
			}
		}
	} else if (step.Command == "banims") {
		for (int slot = 0; slot < BANIM_COUNT; slot++) {
			int count = 0;
			int garrisoned = 0;
			int effect = 0;
			char const * example = "";
			for (int index = 0; index < BuildingTypes.Count(); index++) {
				BuildingTypeClass::AnimDataType const & data = BuildingTypes[index]->AnimData[slot];
				if (data.Anim[0] != '\0') {
					count++;
					example = BuildingTypes[index]->Name();
				}
				if (std::strcmp(data.AnimGarrisoned, data.Anim) != 0) garrisoned++;
				if (data.PoweredEffect || data.PoweredSpecial) effect++;
			}
			DebugString("AUTOTEST   banim slot %d used %d garrisoned %d effect %d e.g. %s\n", slot, count, garrisoned, effect, example);
		}
	} else if (step.Command == "banimstate") {
		// banimstate <TypeID>: for each building of that type, the animation in each slot with its stage, rate and whether it is held still.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * building = Buildings[index];
			if (building->IsInLimbo || stricmp(building->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   banimstate %s at %d,%d health %d/%d bstate %d stage %d rate %d power %d house %s\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
				(int)building->Strength, (int)building->Class->MaxStrength, (int)building->BState, building->Fetch_Stage(), building->Fetch_Rate(), (int)building->Is_Powered_On(), building->House->Class->Name());
			DebugString("AUTOTEST     buildup start %d count %d rate %d ready %d mission %d queue %d\n", building->Class->Anims[BSTATE_CONSTRUCTION].Start, building->Class->Anims[BSTATE_CONSTRUCTION].Count,
				building->Class->Anims[BSTATE_CONSTRUCTION].Rate, (int)building->IsReadyToCommence, (int)building->Mission, (int)building->MissionQueue);
			for (int slot = 0; slot < BANIM_COUNT; slot++) {
				AnimClass const * anim = building->Anims[slot];
				char const * wanted = building->Class->AnimData[slot].Anim;
				if (anim == NULL && wanted[0] == '\0') continue;
				DebugString("AUTOTEST     slot %d wants %s damaged %s: %s stage %d rate %d disabled %d\n", slot, wanted, building->Class->AnimData[slot].AnimDamaged,
					anim != NULL ? anim->Class->Name() : "(none)", anim != NULL ? anim->Fetch_Stage() : -1, anim != NULL ? anim->Fetch_Rate() : -1, anim != NULL ? (int)anim->Is_Disabled() : -1);
			}
		}
	} else if (step.Command == "bshape") {
		// bshape <TypeID>: for each building of that type, its health, state and mission, whether its animations are the damaged forms, and the body frame drawn, which is the shape clamped to half the image's frame count as BuildingClass::DrawIt does.
		for (int index = 0; index < Buildings.Count(); index++) {
			BuildingClass const * building = Buildings[index];
			if (building->IsInLimbo || stricmp(building->Class->Name(), step.Argument.c_str()) != 0) continue;
			ShapeSet const * image = (ShapeSet const *)building->Get_Image_Data();
			int shape = building->Shape_Number();
			int frames = image != NULL ? image->Get_Count() : 0;
			int drawn = (frames > 0 && shape >= frames / 2) ? frames / 2 : shape;
			DebugString("AUTOTEST   bshape %s at %d,%d health %.3f yellow %.3f bstate %d mission %d shape %d drawn %d frames %d damaged anims %d\n", building->Class->Name(), building->Get_Cell().X, building->Get_Cell().Y,
				building->HealthRatio, Rule->ConditionYellow, (int)building->BState, (int)building->Mission, shape, drawn, frames, (int)building->IsDamagedAnims);
		}
	} else if (step.Command == "infstate") {
		// infstate <TypeID>: for each soldier of that type, the sequence it plays with its stage, the shape drawn, its facing and where it is.
		for (int index = 0; index < Infantry.Count(); index++) {
			InfantryClass const * soldier = Infantry[index];
			if (soldier->IsInLimbo || stricmp(soldier->Class->Name(), step.Argument.c_str()) != 0) continue;
			DebugString("AUTOTEST   infstate %s at %d,%d doing %d stage %d shape %d facing %d prone %d mission %d height %d fear %d jjturn %d tardir %d\n", soldier->Class->Name(), soldier->Get_Cell().X, soldier->Get_Cell().Y,
				(int)soldier->Doing, soldier->Fetch_Stage(), soldier->Shape_Number(), (int)soldier->PrimaryFacing.Current().As_Dir8(), (int)soldier->IsProne, (int)soldier->Mission, (int)soldier->HeightAGL, (int)soldier->Fear, (int)soldier->Class->IsJumpJetTurn, soldier->TarCom != NULL ? (int)soldier->Direction(soldier->TarCom).As_Dir8() : -1);
		}
	} else if (step.Command == "unitstate") {
		// unitstate <TypeID>: for each vehicle of that type, its house, cell, mission, team and whether its DeploysInto structure fits where it stands.
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass const * unit = Units[index];
			if (unit->IsInLimbo || stricmp(unit->Class->Name(), step.Argument.c_str()) != 0) continue;
			int fits = -1;
			if (unit->Class->DeploysInto != NULL) {
				fits = unit->Class->DeploysInto->Legal_Placement(unit->PositionCell, NULL) ? 1 : 0;
			}
			DebugString("AUTOTEST   unitstate %s house %s at %d,%d mission %d queued %d team %s navcom %d strength %d deployfits %d\n", unit->Class->Name(), unit->House->Class->Name(), unit->Get_Cell().X, unit->Get_Cell().Y,
				(int)unit->Mission, (int)unit->MissionQueue, unit->Team != NULL ? unit->Team->Class->Name() : "-", unit->NavCom != NULL ? 1 : 0, (int)unit->Strength, fits);
				DebugString("AUTOTEST   rank %s %d\n", unit->Class->Name(), unit->Veterancy.Is_Elite() ? 2 : (unit->Veterancy.Is_Veteran() ? 1 : 0));
		}
	} else if (step.Command == "rockerlink") {
		// rockerlink <TypeID>: for each object of that type, the house and cell of the unit it is DirectRocker-linked to (or "-"), and whether its type is Pushy.
		for (int index = 0; index < Technos.Count(); index++) {
			TechnoClass const * techno = Technos[index];
			if (techno->IsInLimbo || stricmp(techno->TClass->Name(), step.Argument.c_str()) != 0) continue;
			TechnoClass const * partner = techno->DirectRockerLinkedUnit;
			DebugString("AUTOTEST   rockerlink %s house %s at %d,%d linked %s house %s at %d,%d pushy %d\n", techno->TClass->Name(), techno->House->Class->Name(), techno->Get_Cell().X, techno->Get_Cell().Y,
				partner != NULL ? partner->TClass->Name() : "-", partner != NULL ? partner->House->Class->Name() : "-", partner != NULL ? partner->Get_Cell().X : -1, partner != NULL ? partner->Get_Cell().Y : -1, techno->TClass->IsPushy ? 1 : 0);
		}
	} else if (step.Command == "playanim") {
		// playanim <AnimTypeID> x y: plays one loop of that animation over the cell.
		AnimTypeClass const * type = AnimTypeClass::Find_Or_Make(step.Argument.c_str());
		if (type != NULL) {
			new AnimClass(type, Map[Cell(step.X, step.Y)].Center_Coord());
		}
	} else if (step.Command == "blackout") {
		// blackout <frames>: a spy's power blackout of that many frames on the player's house.
		PlayerPtr->PowerBlackout = std::atoi(step.Argument.c_str());
		PlayerPtr->IsPowerBlackout = true;
		PlayerPtr->RecalcPower = true;
	} else if (step.Command == "emp") {
		// emp <duration> x y: an EM pulse of radius 2 and that duration on the cell, from no source.
		new EMPulseClass(Cell(step.X, step.Y), 2, std::atoi(step.Argument.c_str()), NULL);
	} else if (step.Command == "cameos") {
		// cameos: each sidebar strip's cameos in display order, with the type and whether the player may build it now.
		for (int column = 0; column < SidebarClass::COLUMNS; column++) {
			for (int index = 0; index < Map.Column[column].BuildableCount; index++) {
				SidebarClass::StripClass::BuildType const & entry = Map.Column[column].Buildables[index];
				TechnoTypeClass const * tech = Fetch_Techno_Type(entry.BuildableType, entry.BuildableID);
				DebugString("AUTOTEST   cameo column %d slot %d %s id %d can %d\n", column, index, tech != NULL ? tech->Name() : "(superweapon)",
					entry.BuildableID, tech != NULL ? PlayerPtr->Can_Build(tech, false, true) : -2);
			}
		}
	} else if (step.Command == "clickcameo") {
		// clickcameo <TypeID>: starts production of the type, as a click on its cameo does, when a sidebar strip still offers the cameo.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		bool offered = false;
		for (int column = 0; type != NULL && column < SidebarClass::COLUMNS; column++) {
			for (int index = 0; index < Map.Column[column].BuildableCount; index++) {
				SidebarClass::StripClass::BuildType const & entry = Map.Column[column].Buildables[index];
				offered = offered || Fetch_Techno_Type(entry.BuildableType, entry.BuildableID) == type;
			}
		}
		if (type != NULL && offered) {
			OutList.push_back(EventClass(PlayerPtr->HeapID, EventClass::PRODUCE, type->Fetch_RTTI(), type->Fetch_Heap_ID()));
		}
		DebugString("AUTOTEST   clickcameo %s: %s\n", step.Argument.c_str(), offered ? "offered" : "no cameo");
	} else if (step.Command == "cameopress") {
		// cameopress <TypeID> left|right: presses the type's visible cameo through the cameo's own action, so the Shift
		// state the script holds decides what the press does, as it does for a player.
		std::size_t const space = step.Argument.find_first_of(" \t");
		std::string const name = step.Argument.substr(0, space);
		bool const right = space != std::string::npos && step.Argument.find("right", space) != std::string::npos;
		TechnoTypeClass const * type = Find_Type(name);
		bool pressed = false;
		for (int column = 0; type != NULL && !pressed && column < SidebarClass::COLUMNS; column++) {
			SidebarClass::StripClass & strip = Map.Column[column];
			for (int index = 0; index < strip.BuildableCount && !pressed; index++) {
				if (Fetch_Techno_Type(strip.Buildables[index].BuildableType, strip.Buildables[index].BuildableID) != type) {
					continue;
				}
				int const slot = index - strip.TopIndex;
				if (slot < 0 || slot >= Map.Max_Visible()) {
					continue;
				}
				SidebarClass::StripClass::SelectClass & cameo = SidebarClass::StripClass::SelectButton[column][slot];
				cameo.Set_Owner(strip, slot);
				if (right) {
					cameo.Press_Right();
				} else {
					cameo.Press_Left();
				}
				pressed = true;
			}
		}
		DebugString("AUTOTEST   cameopress %s %s: %s\n", name.c_str(), right ? "right" : "left",
			type == NULL ? "no such type" : (pressed ? "pressed" : "no visible cameo"));
	} else if (step.Command == "queuestate") {
		// queuestate <TypeID>: how many of the type the player's factory holds, in production and queued, how many
		// are queued behind the one in production, and the player's credits.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			RTTIType const rtti = type->Fetch_RTTI();
			FactoryClass * factory = PlayerPtr->Fetch_Factory(rtti, HouseClass::Build_Category(rtti, type->Fetch_Heap_ID()), HouseClass::Is_Naval_Type(rtti, type->Fetch_Heap_ID()));
			DebugString("AUTOTEST   queuestate %s: pending %d queued %d credits %d\n", type->Name(),
				factory != NULL ? factory->Total(type) : 0, factory != NULL ? factory->Queued_Count() : 0, PlayerPtr->Available_Money());
		}
	} else if (step.Command == "prereqs") {
		// prereqs <TypeID>: the prerequisites the type lists, then each type of any kind whose Prerequisite list names it.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			for (int index = 0; index < type->Prerequisite.Count(); index++) {
				int const pre = type->Prerequisite[index];
				DebugString("AUTOTEST   prereq of %s: %d %s\n", type->Name(), pre, pre >= 0 && pre < BuildingTypes.Count() ? BuildingTypes[pre]->Name() : "(group)");
			}
			// Prerequisite entries name structures by heap ID, so only a structure can be named by another type.
			if (dynamic_cast<BuildingTypeClass const *>(type) != NULL) {
				int const id = type->Fetch_Heap_ID();
				for (int index = 0; index < BuildingTypes.Count(); index++) Note_If_Gated(BuildingTypes[index], id, type->Name());
				for (int index = 0; index < UnitTypes.Count(); index++) Note_If_Gated(UnitTypes[index], id, type->Name());
				for (int index = 0; index < InfantryTypes.Count(); index++) Note_If_Gated(InfantryTypes[index], id, type->Name());
				for (int index = 0; index < AircraftTypes.Count(); index++) Note_If_Gated(AircraftTypes[index], id, type->Name());
			}
		}
	} else if (step.Command == "canbuild") {
		// canbuild <TypeID>: whether the player may build the type and which factory would.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			BuildingClass const * factory = type->Who_Can_Build_Me(true, false, true, PlayerPtr);
			DebugString("AUTOTEST   canbuild %s %d factory %s cameo %d\n", type->Name(), PlayerPtr->Can_Build(type, false, true), factory != NULL ? factory->Class->Name() : "-", (int)(type->Get_Cameo_Data() != type->CameoData));
		}
	} else if (step.Command == "selected") {
		// selected: the number of selected objects and their types.
		std::string types;
		for (int index = 0; index < CurrentObject.Count(); index++) {
			types += " ";
			types += CurrentObject[index]->Class_Of()->Name();
		}
		DebugString("AUTOTEST   selected %d:%s\n", CurrentObject.Count(), types.c_str());
	} else if (step.Command == "defeat") {
		// defeat <house>: runs the defeat HouseClass::AI runs once a house has nothing left, on the named
		// house ("player" or a house name as the waits name one). Every copy of a network game must run it on the same frame.
		HouseClass * house = Wait_House(step.Argument);
		if (house == NULL || house->IsDefeated) {
			DebugString("AUTOTEST defeat %s: %s\n", step.Argument.c_str(), house == NULL ? "no such house" : "already defeated");
		} else {
			house->Destroy_All();
			house->MPlayer_Defeated();
			DebugString("AUTOTEST defeat %s: defeated frame %d, local %d\n", house->Class->Name(), Frame, (int)(house == PlayerPtr));
		}
	} else if (step.Command == "houseobjects") {
		// houseobjects <house>: the objects the house owns, by kind, and the ones it originally owns.
		HouseClass * house = Wait_House(step.Argument);
		if (house == NULL) {
			DebugString("AUTOTEST houseobjects %s: no such house\n", step.Argument.c_str());
		} else {
			int units = 0;
			int infantry = 0;
			int aircraft = 0;
			int buildings = 0;
			int limbo = 0;
			int original = 0;
			int living_units = 0;
			for (int index = 0; index < Technos.Count(); index++) {
				TechnoClass * techno = Technos[index];
				if (techno->Original_Owner() == house) {
					original++;
				}
				if (techno->House != house) {
					continue;
				}
				if (techno->IsInLimbo) {
					limbo++;
					continue;
				}
				switch (techno->RTTI) {
					case RTTI_UNIT: units++; break;
					case RTTI_INFANTRY: infantry++; break;
					case RTTI_AIRCRAFT: aircraft++; break;
					case RTTI_BUILDING: buildings++; break;
					default: break;
				}
			}
			for (int index = 0; index < Units.Count(); index++) {
				if (Units[index]->House == house && Units[index]->IsActive) {
					living_units++;
				}
			}
			DebugString("AUTOTEST houseobjects %s frame %d: units %d infantry %d aircraft %d buildings %d limbo %d original %d living units %d defeated %d\n",
				house->Class->Name(), Frame, units, infantry, aircraft, buildings, limbo, original, living_units, (int)house->IsDefeated);
		}
	} else if (step.Command == "shortgame") {
		// shortgame <0|1>: the Short Game option on this copy.
		Session.Options.ShortGame = std::atoi(step.Argument.c_str()) != 0;
		DebugString("AUTOTEST shortgame %d frame %d\n", (int)Session.Options.ShortGame, Frame);
	} else if (step.Command == "observer") {
		// observer: the local player's defeat and observer state, what it owns and selects, and its beacon mode.
		int owned = 0;
		if (PlayerPtr != NULL) {
			for (int object = 0; object < Technos.Count(); object++) {
				if (Technos[object]->House == PlayerPtr && !Technos[object]->IsInLimbo) {
					owned++;
				}
			}
		}
		int alive = 0;
		for (int index = 0; index < Houses.Count(); index++) {
			if (Houses[index] != NULL && !Houses[index]->IsDefeated && !Houses[index]->Class->IsMultiplayPassive) {
				alive++;
			}
		}
		DebugString("AUTOTEST observer frame %d player %s defeated %d observer %d watching %d whole %d obiwan %d owned %d alive %d selected %d beacon %d planning %d\n",
			Frame, PlayerPtr != NULL ? PlayerPtr->Class->Name() : "none", PlayerPtr != NULL ? (int)PlayerPtr->IsDefeated : 0,
			PlayerPtr != NULL ? (int)PlayerPtr->IsObserver : 0, PlayerPtr != NULL ? (int)PlayerPtr->Is_Watching() : 0,
			PlayerPtr != NULL ? (int)PlayerPtr->Sees_Whole_Map() : 0, (int)Session.ObiWan, owned, alive, CurrentObject.Count(),
			(int)Map.IsPlaceBeaconMode, (int)Map.IsPlanningMode);
	} else if (step.Command == "fullname") {
		// fullname <TypeID>: the name players see for the type, and the string label it comes from.
		TechnoTypeClass const * type = Find_Type(step.Argument);
		if (type != NULL) {
			DebugString("AUTOTEST   fullname %s [%s] label [%s]\n", type->Name(), type->Full_Name(), type->UINameLabel.c_str());
		}
	} else if (step.Command == "anims") {
		for (int index = 0; index < 4 && index < AnimTypes.Count(); index++) {
			DebugString("AUTOTEST   anim %d %s\n", index, AnimTypes[index] != NULL ? AnimTypes[index]->Name() : "(null)");
		}
		DebugString("AUTOTEST   anim count %d\n", AnimTypes.Count());
	} else if (step.Command == "wall") {
		// wall <OverlayType> x y: builds a section of that wall, owned by the player, on that cell.
		OverlayType const type = OverlayTypeClass::From_Name(step.Argument.c_str());
		if (type != OVERLAY_NONE) {
			Map.PendingHouse = HousesType(PlayerPtr->HeapID);
			new OverlayClass(OverlayTypes[type], Cell(step.X, step.Y), HousesType(PlayerPtr->HeapID));
			DebugString("AUTOTEST wall %s at %d,%d: overlay %d\n", step.Argument.c_str(), step.X, step.Y, (int)Map[Cell(step.X, step.Y)].Overlay);
		}
	} else if (step.Command == "ewall") {
		// ewall <OverlayType> x y: builds a section of that wall on that cell, owned by the first computer house with a
		// construction yard that the player is not allied with, as enemy does for a structure.
		OverlayType const type = OverlayTypeClass::From_Name(step.Argument.c_str());
		HouseClass * owner = NULL;
		for (int index = 0; owner == NULL && index < Houses.Count(); index++) {
			if (Houses[index] != PlayerPtr && Houses[index]->ConYards.Count() > 0 && !PlayerPtr->Is_Ally(Houses[index])) {
				owner = Houses[index];
			}
		}
		if (type != OVERLAY_NONE && owner != NULL) {
			Map.PendingHouse = HousesType(owner->HeapID);
			new OverlayClass(OverlayTypes[type], Cell(step.X, step.Y), HousesType(owner->HeapID));
			DebugString("AUTOTEST ewall %s at %d,%d: overlay %d owner %s\n", step.Argument.c_str(), step.X, step.Y, (int)Map[Cell(step.X, step.Y)].Overlay, owner->Class->Name());
		} else {
			DebugString("AUTOTEST ewall %s: %s\n", step.Argument.c_str(), type == OVERLAY_NONE ? "no such overlay" : "no computer house");
		}
	} else if (step.Command == "holdmove") {
		// holdmove 0|1: turns MissileLaunchHoldsMove off or on for the rest of the run.
		Rule->IsMissileLaunchHoldsMove = step.Argument != "0";
		DebugString("AUTOTEST holdmove %d\n", (int)Rule->IsMissileLaunchHoldsMove);
	} else if (step.Command == "liveanims") {
		// liveanims <AnimType>: how many animations of that type are playing.
		int count = 0;
		for (int index = 0; index < Anims.Count(); index++) {
			if (Anims[index]->Class != NULL && stricmp(Anims[index]->Class->Name(), step.Argument.c_str()) == 0) {
				count++;
			}
		}
		DebugString("AUTOTEST   liveanims %s %d\n", step.Argument.c_str(), count);
	} else if (step.Command == "record") {
		RecordInterval = std::max(0, std::atoi(step.Argument.c_str()));
	} else if (step.Command == "fps") {
		// fps: the game frames counted in the last whole second, which the game's FPS readout shows.
		DebugString("AUTOTEST fps frame %d %u\n", Frame, LastFramesPerSecond);
	} else if (step.Command == "dump") {
		Dump();
	} else if (step.Command == "sounds") {
		// sounds <0|1>: stops or starts writing every sound effect played to the log.
		LogSoundEffects = std::atoi(step.Argument.c_str()) != 0;
	} else if (step.Command == "movie") {
		// movie <name>: plays that movie to its end, with its sound at the Movies option's level.
		DebugString("AUTOTEST movie %s\n", step.Argument.c_str());
		Play_Movie(step.Argument.c_str());
		DebugString("AUTOTEST movie %s done\n", step.Argument.c_str());
	} else if (step.Command == "ranking") {
		// ranking: the scenario's par times (easy, normal, hard, in seconds), its four rank labels and the elapsed mission time.
		DebugString("AUTOTEST   ranking par %d %d %d elapsed %d under [%s] [%s] over [%s] [%s]\n", Scen->ParTime[0] / TIMER_SECOND, Scen->ParTime[1] / TIMER_SECOND, Scen->ParTime[2] / TIMER_SECOND, (int)Scen->ElapsedTimer / TIMER_SECOND, Scen->UnderParTitle, Scen->UnderParMessage, Scen->OverParTitle, Scen->OverParMessage);
	} else if (step.Command == "skipscore") {
		// skipscore: a multiplayer or skirmish win passes over its score screen, as a launch file's
		// SkipScoreScreen does, so the run reaches the lobby without a key press.
		Session.SkipScoreScreen = true;
		DebugString("AUTOTEST skipscore\n");
	} else if (step.Command == "lansession") {
		// lansession: marks the game as a network game, so steps can see the multiplayer limits.
		Session.Type = GAME_IPX;
	} else if (step.Command == "zoomlevel") {
		// zoomlevel: the current map view zoom.
		DebugString("AUTOTEST zoomlevel %.2f\n", ViewZoom * InterfaceScale);
	} else if (step.Command == "zoom") {
		// zoom <factor>: sets the map view zoom at once (0.5 shows twice as much), measured from the screen's own pixels.
		Set_View_Zoom(std::atof(step.Argument.c_str()));
		DebugString("AUTOTEST zoom %.2f view %dx%d on screen %dx%d\n", ViewZoom * InterfaceScale, TacticalRect.Width, TacticalRect.Height, ScreenTacticalRect.Width, ScreenTacticalRect.Height);
	} else if (step.Command == "wheel") {
		// wheel <step> [x y]: glides the zoom by that step around a screen point, as the mouse wheel
		// does; without a point the zoom centers on the middle of the map view.
		Point2D point = ScreenTacticalRect.Top_Left() + Point2D(ScreenTacticalRect.Width / 2, ScreenTacticalRect.Height / 2);
		if (step.X != 0 || step.Y != 0) {
			point = Point2D(step.X, step.Y);
		}
		Request_View_Zoom_Step(std::atof(step.Argument.c_str()), point);
	} else if (step.Command == "options") {
		SpecialDialog = SDLG_OPTIONS;
	} else if (step.Command == "windowshot") {
		// windowshot <name>: saves what the window shows, map layer included, as <name>.tga in the screenshots folder.
		static std::string path;
		path = Screenshot_Name((step.Argument + ".tga").c_str());
		Backend_Request_Window_Capture(path.c_str());
	} else if (step.Command == "glideclock") {
		// glideclock: times zoom glides by game frames, for recordings.
		ViewZoomGameClock = true;
	} else if (step.Command == "scriptedzoom") {
		// scriptedzoom <0|1>: turns the scripted scene zoom option off or on.
		Options.ScriptedCameraZoom = std::atoi(step.Argument.c_str()) != 0;
		DebugString("AUTOTEST scriptedzoom %d\n", (int)Options.ScriptedCameraZoom);
	} else if (step.Command == "defencerange") {
		// defencerange <0|1>: turns the Show defence range option off or on, as the options screen does.
		Options.ShowDefenceRange = std::atoi(step.Argument.c_str()) != 0;
		DebugString("AUTOTEST defencerange %d\n", (int)Options.ShowDefenceRange);
	} else if (step.Command == "scriptzoom") {
		// scriptzoom: the zoom on screen, the map pixels the view shows at it, and whether a scene holds the player's zoom.
		Point2D const centre = TacticalMap != NULL ? TacticalMap->Get_Tactical_Position() : Point2D(0, 0);
		DebugString("AUTOTEST scriptzoom frame %d drawn %.4f shown %.4f visible %.1fx%.1f centre %d,%d saved %d locked %d ignore %d\n", Frame, ViewZoom, DisplayZoom,
			ScreenTacticalRect.Width / DisplayZoom, ScreenTacticalRect.Height / DisplayZoom, centre.X, centre.Y, (int)Scripted_View_Zoom_Saved(),
			Scen != NULL ? (int)Scen->IsInputLocked : -1, (int)IgnoreInput);
	} else if (step.Command == "statedump") {
		// statedump <path>: writes the state a save would hold, and an index of its records, for comparing two runs.
		DebugString("AUTOTEST statedump %s: %s\n", step.Argument.c_str(), Dump_Game_State(step.Argument.c_str()) ? "written" : "failed");
	} else if (step.Command == "hashparts") {
		// hashparts: logs each value the state hash covers, one object per line.
		Log_State_Parts();
	} else if (step.Command == "perflog") {
		// perflog <0|1>: turns the once-a-second Perf lines on or off, as [Options] PerfLog does.
		Perf_Set_Enabled(std::atoi(step.Argument.c_str()) != 0);
		DebugString("AUTOTEST perflog %d\n", (int)Perf_Enabled);
	} else if (step.Command == "uploadverify") {
		// uploadverify <0|1>: 1 checks, at every present, that the copy kept of each texture matches its picture; 0 logs the checks and mismatches so far.
		bool const on = std::atoi(step.Argument.c_str()) != 0;
		unsigned int checks = 0;
		unsigned int mismatches = 0;
		if (!on) {
			Backend_Upload_Verify_Totals(&checks, &mismatches);
			DebugString("AUTOTEST uploadverify checks %u mismatches %u\n", checks, mismatches);
		}
		Backend_Verify_Uploads(on);
	} else if (step.Command == "hash") {
		// hash [interval]: logs the game-state hash now, and every interval frames after when one is given.
		HashInterval = std::max(0, std::atoi(step.Argument.c_str()));
		Log_State_Hash();
	} else if (step.Command == "statecrc") {
		// statecrc [interval]: logs the object and target checksums now, and every interval frames after when one is given.
		StateCRCInterval = std::max(0, std::atoi(step.Argument.c_str()));
		Log_State_CRC();
	} else if (step.Command == "syncrec") {
		// syncrec on|<path>: "on" starts the desync recorder's histories afresh and the outside-frame-logic check; a path writes the histories to that file.
		if (step.Argument == "on") {
			SyncRecorder.Reset();
			SyncRecorder.Set_Recording(true);
			Sync_Check_Outside_Logic(true);
			DebugString("AUTOTEST syncrec on frame %d\n", Frame);
		} else {
			FILE * fp = std::fopen(step.Argument.c_str(), "wt");
			if (fp != NULL) {
				SyncRecorder.Print_All(fp, Sync_Engine_Names());
				std::fclose(fp);
			}
			DebugString("AUTOTEST syncrec %s: %s\n", step.Argument.c_str(), fp != NULL ? "written" : "failed");
		}
	} else if (step.Command == "log") {
		// The step line itself is the log entry.
	} else if (step.Command == "searchdir") {
		// searchdir <path>: added to the search path when the script was read (AutoTest_Load).
	} else if (step.Command == "loadshot") {
		// loadshot <name>: kept when the script was read (AutoTest_Load).
	} else if (step.Command == "stall") {
		// stall <ms>: keeps the main thread busy without pumping messages, so the hang watchdog reports it.
		DebugString("AUTOTEST stall %s\n", step.Argument.c_str());
		Hang_Watchdog_Test_Stall((unsigned int)std::max(0, std::atoi(step.Argument.c_str())));
	} else if (step.Command == "rounding") {
		// rounding <near|chop|up|down|guard>: sets the game thread's rounding mode for the rest of the run, or turns the per-frame guard back on.
		DebugString("AUTOTEST rounding %s\n", step.Argument.c_str());
		Rounding_Test_Set(step.Argument.c_str());
	} else if (step.Command == "quit") {
		DebugString("AUTOTEST quit\n");
		std::exit(0);
	} else {
		DebugString("AUTOTEST unknown command %s\n", step.Command.c_str());
	}
}

}


void AutoTest_Cursor_Position(int & x, int & y)
{
	x = _CursorX;
	y = _CursorY;
}


bool AutoTest_Active(void)
{
	return(Active);
}


bool AutoTest_Key_Held(int virtualkey)
{
	return(virtualkey > 0 && virtualkey < 256 && ScriptHeldKeys[virtualkey]);
}


/// <summary>
/// Saves the window as the script's next loading screen capture, when it asked for them.
/// Call once the loading screen is drawn as it should be seen.
/// </summary>
void AutoTest_Loading_Screen_Shown(void)
{
	if (!Active || LoadShotName.empty()) {
		return;
	}

	// The capture request keeps the path's address until a frame is shown.
	static std::string path;
	path = Screenshot_Name((LoadShotName + "-" + std::to_string(++LoadShotCount) + ".tga").c_str());
	DebugString("AUTOTEST loadshot %s\n", path.c_str());
	Backend_Request_Window_Capture(path.c_str());
	Video_Present();
	Video_Present();
}


/// <summary>
/// Reads a test script and turns unattended mode on. Steps are played in file order.
/// </summary>
/// <returns>False when the file cannot be read.</returns>
bool AutoTest_Load(char const * filename)
{
	FILE * file = std::fopen(filename, "r");
	if (file == NULL) {
		return(false);
	}

	AutoTest_Script_Folder = AutoTest_Folder_Of(filename);

	char line[512];
	while (std::fgets(line, sizeof(line), file) != NULL) {
		char command[64] = "";
		char argument[256] = "";
		int frame = 0;
		int x = 0;
		int y = 0;
		// A menu step has no frame, since the menus run before any game does.
		if (std::strncmp(line, "ui ", 3) == 0) {
			std::string rest = line + 3;
			rest.erase(rest.find_last_not_of(" \t\r\n") + 1);
			std::string::size_type const space = rest.find(' ');
			UIScript_Add(rest.substr(0, space), space == std::string::npos ? std::string() : rest.substr(space + 1));
			continue;
		}
		if (line[0] == ';' || std::sscanf(line, "%d %63s %255s %d %d", &frame, command, argument, &x, &y) < 2) {
			continue;
		}
		std::string text = argument;
		// A rules step's path and a log line's text are the rest of the line, so they may hold spaces.
		if (std::strcmp(command, "rules") == 0 || std::strcmp(command, "art") == 0 || std::strcmp(command, "pcxcameo") == 0
			|| std::strcmp(command, "truecolourdir") == 0 || std::strcmp(command, "exportshape") == 0 || std::strcmp(command, "exportpal") == 0 || std::strcmp(command, "teamini") == 0
			|| std::strcmp(command, "searchdir") == 0 || std::strcmp(command, "statedump") == 0 || std::strcmp(command, "extract") == 0
			|| std::strcmp(command, "log") == 0 || std::strcmp(command, "beacontext") == 0
			|| std::strcmp(command, "wintype") == 0 || std::strcmp(command, "key") == 0 || std::strcmp(command, "winkey") == 0
			|| std::strcmp(command, "hold") == 0 || std::strcmp(command, "release") == 0 || std::strcmp(command, "cameopress") == 0
			|| std::strcmp(command, "binding") == 0 || std::strcmp(command, "scheme") == 0) {
			char const * rest = std::strstr(line, command) + std::strlen(command);
			text = rest + std::strspn(rest, " \t");
			text.erase(text.find_last_not_of(" \t\r\n") + 1);
		}
		// A search directory has to be in place before startup reads the files it holds, such
		// as the extra string tables, so it is added now rather than at its frame.
		if (std::strcmp(command, "searchdir") == 0 && !text.empty()) {
			std::string path = AutoTest_Input_Path(text);
			if (path.back() != '\\' && path.back() != '/') {
				path += '\\';
			}
			CDFileClass::Add_Search_Drive(path.c_str());
		}
		// The first loading screen comes before any game frame.
		if (std::strcmp(command, "loadshot") == 0) {
			LoadShotName = text;
		}
		// recordreplay and playreplay take effect when the script is read, before the game starts.
		if (std::strcmp(command, "recordreplay") == 0) {
			Replay_Set_Recording(true, text.c_str());
			continue;
		}
		if (std::strcmp(command, "playreplay") == 0) {
			Replay_Request_Playback(text.c_str());
			Spawner_Request();
			continue;
		}
		// requirera2 is kept when the script is read, so it needs no step.
		if (std::strcmp(command, "requirera2") == 0) {
			RequireRA2 = true;
			continue;
		}
		// continuewin and continuelose are kept when the script is read, so they need no step.
		if (std::strcmp(command, "continuewin") == 0) {
			ContinueAfterWin = true;
			continue;
		}
		if (std::strcmp(command, "continuelose") == 0) {
			ContinueAfterLose = true;
			continue;
		}
		std::string source = line;
		source.erase(source.find_last_not_of(" \t\r\n") + 1);
		Steps.push_back(StepType{frame, command, text, x, y, source});
	}
	std::fclose(file);

	Active = true;
	return(true);
}


/// <summary>
/// Plays every step whose frame has been reached. Call once per game logic frame.
/// </summary>
void AutoTest_Frame(void)
{
	if (!Active || PlayerPtr == NULL) {
		return;
	}
	while (Next < Steps.size() && Steps[Next].Frame + WaitShift <= Frame) {
		StepType const & step = Steps[Next];
		if (Is_Wait_Command(step.Command)) {
			if (WaitChecked != Next) {
				WaitChecked = Next;
				std::string const problem = Wait_Problem(Line_Words(step.Line));
				if (!problem.empty()) {
					DebugString("AUTOTEST wait problem: %s: %s\n", problem.c_str(), step.Line.c_str());
				}
			}
			if (!Wait_Over(step)) {
				break;
			}
		} else {
			Run(step);
		}
		Next++;
	}

	C4_Watch_Frame();

	if (!FollowType.empty() && (Frame % 15) == 0) {
		for (int index = 0; index < Infantry.Count(); index++) {
			InfantryClass * object = Infantry[index];
			if (object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->Class->Name(), FollowType.c_str()) == 0) {
				TacticalMap->Set_Tactical_Position(object->Center_Coord());
				break;
			}
		}
		for (int index = 0; index < Units.Count(); index++) {
			UnitClass * object = Units[index];
			if (object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->Class->Name(), FollowType.c_str()) == 0) {
				TacticalMap->Set_Tactical_Position(object->Center_Coord());
				if ((Frame % 300) == 0) {
					Point2D actual = TacticalMap->Get_Tactical_Position();
					DebugString("AUTOTEST   follow %s cell %d,%d view %d,%d\n", object->Class->Name(), object->Get_Cell().X, object->Get_Cell().Y, actual.X, actual.Y);
				}
				break;
			}
		}
		for (int index = 0; index < Aircraft.Count(); index++) {
			AircraftClass * object = Aircraft[index];
			if (object->House == PlayerPtr && !object->IsInLimbo && stricmp(object->Class->Name(), FollowType.c_str()) == 0) {
				TacticalMap->Set_Tactical_Position(object->Center_Coord());
				break;
			}
		}
	}

	if (RecordInterval > 0 && (Frame % RecordInterval) == 0) {
		Execute_Command("ScreenCapture");
	}

	if (HashInterval > 0 && (Frame % HashInterval) == 0) {
		Log_State_Hash();
	}

	if (StateCRCInterval > 0 && (Frame % StateCRCInterval) == 0) {
		Log_State_CRC();
	}

	if (SpeedProbeInterval > 0 && (Frame % SpeedProbeInterval) == 0) {
		Log_Speed_Probe();
	}
}


/// <summary>
/// Counts a spring of a trigger type for triggerlog, triggercoverage and waittrigger. TriggerClass::Spring
/// calls it only while an autotest run is active.
/// </summary>
void AutoTest_Trigger_Sprung(TriggerTypeClass const * type)
{
	if (!Active || type == NULL) {
		return;
	}
	TriggerSprings[type]++;
	if (TriggerLog) {
		DebugString("AUTOTEST trigger fired frame %d: %s %s house %s\n", Frame, (char const *)type->IniName, (char const *)type->GivenName, type->House != NULL ? type->House->Class->Name() : "NOHOUSE");
	}
}


void AutoTest_Score_Shown(void)
{
	if (!Active) {
		return;
	}
	DebugString("AUTOTEST score screen shown frame %d\n", Frame);
	Execute_Command("ScreenCapture");
	Video_Present();
}


void AutoTest_Game_Over(bool won)
{
	if (!Active) {
		return;
	}
	DebugString("AUTOTEST game over frame %d: %s\n", Frame, won ? "won" : "lost");
	if (won && ContinueAfterWin) {
		PostWinContinue = true;
		return;
	}
	if (!won && ContinueAfterLose) {
		PostLoseContinue = true;
		return;
	}
	std::exit(0);
}


/// <summary>
/// Ends an unattended run whose script has requirera2 when the mission has no Red Alert 2 house numbering.
/// Call once the mission's INI is read, before any game frame runs.
/// </summary>
/// <param name="numbered">True when Red Alert 2 house numbers are converted for the mission.</param>
void AutoTest_Require_RA2_Numbering(bool numbered)
{
	if (!Active || !RequireRA2 || numbered) {
		return;
	}
	DebugString("AUTOTEST error: %s is a Red Alert 2 mission, but its house numbers were not converted (no Red Alert 2 campaign entry, or the RA2Rules file is missing); run with -Mods tests/autotest/ra2-campaigns\n", Scen->ScenarioName);
	std::exit(EXIT_FAILURE);
}


bool AutoTest_Post_Win(void)
{
	return(Active && (PostWinContinue || PostLoseContinue));
}
