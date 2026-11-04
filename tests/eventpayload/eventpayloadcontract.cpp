/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Pins, for every event type, the payload bytes that a compressed packet, a replay and a sync
// record each copy from an event. Run with --print to list the current values as table rows.
//
// The replay rule lives in code/replay.cpp (Capture_Event), which needs the whole engine, so the
// harness repeats its length rule beside the table. The sync record bytes come from
// EventClass::Sync_Record_Bytes and the packet bytes from the real decoder.

#include "event.h"
#include "fnv.h"
#include "netpacket.h"
#include "replayfile.h"
#include "syncrec.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <vector>


namespace {

using Bytes = std::vector<std::byte>;

constexpr int Sender = 3;
constexpr int EventFrame = 120;

// A length of NOT_COPIED marks a path that keeps nothing for the event: a replay refuses it, or
// the sync recorder skips it.
constexpr int NOT_COPIED = -1;

int Failures = 0;


struct ExpectedPayload
{
	char const * Name;
	int EventLength;
	int ReplayLength;
	std::uint32_t ReplayHash;
	int SyncLength;
	std::uint32_t SyncHash;
	int PacketLength;
	std::uint32_t PacketHash;
};


// Recorded from dev eddea67b. Each hash is the FNV-1a of the bytes that path copies. The packet
// length counts the type byte, the MEGAMISSION count byte and the variable payload that a
// compressed packet carries. The rules differ on purpose and stay so: a replay keeps BEACON_TEXT's
// text and refuses ADDPLAYER, a sync record keeps the payload size and payload hash of both, and
// RESPONSE_TIME's packet byte is FrameInfo.Delay while a replay
// keeps the first Data byte.
ExpectedPayload const Expected[EventClass::LAST_EVENT] = {
	{"EMPTY",            0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x050C5D1F},
	{"POWERON",          8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x908BD42C},
	{"POWEROFF",         8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x1106DA65},
	{"ALLY",             4,  4, 0x512492E1,  4, 0x512492E1,  5, 0x39ED3F02},
	{"MEGAMISSION",     28, 28, 0x76E07D61, 28, 0x76E07D61, 30, 0xCBF0D0B6},
	{"MEGAMISSION_F",   36, 36, 0x5562A5C1, 36, 0x5562A5C1, 37, 0xF621F5F8},
	{"IDLE",             8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x73BB3399},
	{"SCATTER",          8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x2A55FA2E},
	{"DESTRUCT",         0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x0D0C69B7},
	{"DEPLOY",           8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0xC450CF34},
	{"PLACE",           12, 12, 0x3BB1CF71, 12, 0x3BB1CF71, 13, 0x5A028359},
	{"OPTIONS",          0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x0E0C6B4A},
	{"GAMESPEED",        4,  4, 0x512492E1,  4, 0x512492E1,  5, 0x30DE9C5F},
	{"PRODUCE",          8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0xDF77BC28},
	{"SUSPEND",          8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0xB8D7F441},
	{"ABANDON",          8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x9C415896},
	{"PRIMARY",          8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x6A506C47},
	{"SPECIAL_PLACE",    8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x696C5C5C},
	{"EXIT",             0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x170C7975},
	{"ANIMATION",       16, 16, 0xDE58A475, 16, 0xDE58A475, 17, 0x4D756912},
	{"REPAIR",           8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0xB84764EB},
	{"SELL",             8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x4E616110},
	{"SELLCELL",         4,  4, 0x512492E1,  4, 0x512492E1,  5, 0xC214A59D},
	{"SPECIAL",          4,  4, 0x512492E1,  4, 0x512492E1,  5, 0x87C4E4AE},
	{"FRAMESYNC",        0,  0, 0x811C9DC5, -1, 0x00000000,  1, 0x1D0C82E7},
	{"MESSAGE",          0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x1C0C8154},
	{"RESPONSE_TIME",    1,  1, 0x0E0C6B4A, -1, 0x00000000,  2, 0xC4B858EC},
	{"FRAMEINFO",        7,  7, 0x2B00E6A9, -1, 0x00000000,  8, 0xB8699BA8},
	{"SAVEGAME",         0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x190C7C9B},
	{"ARCHIVE",         16, 16, 0xDE58A475, 16, 0xDE58A475, 17, 0xA998F528},
	{"ADDPLAYER",        4, -1, 0x00000000,  8, 0x50F3C976, 16, 0x656A5B72},
	{"TIMING",           5,  5, 0x3A929C5A, -1, 0x00000000,  6, 0xCD92B8DB},
	{"PROCESS_TIME",     2,  2, 0x978D310E, -1, 0x00000000,  3, 0x956C37C4},
	{"PAGEUSER",         0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x240C8DEC},
	{"REMOVEPLAYER",     4,  4, 0x512492E1,  4, 0x512492E1,  5, 0x002A87C1},
	{"LATENCYFUDGE",     4,  4, 0x512492E1,  4, 0x512492E1,  5, 0x08836EE2},
	{"NETWORK_REPORT",   6,  6, 0xD2CC8CBA,  6, 0xD2CC8CBA,  7, 0x1B5CFDC4},
	{"ABANDON_COUNT",   12, 12, 0x3BB1CF71, 12, 0x3BB1CF71, 13, 0x9DA3B8A8},
	{"DETONATE",         8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0xAF24B639},
	{"ALL_CHEER",        0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x220C8AC6},
	{"PLAN_ORDER",      28, 28, 0x76E07D61, 28, 0x76E07D61, 29, 0x58A8651B},
	{"PLAN_COMMIT",      0,  0, 0x811C9DC5,  0, 0x811C9DC5,  1, 0x2C0C9A84},
	{"BEACON_PLACE",     8,  8, 0x596B18E5,  8, 0x596B18E5,  9, 0x8CAD216D},
	{"BEACON_DELETE",    4,  4, 0x512492E1,  4, 0x512492E1,  5, 0x514C9D9A},
	{"BEACON_TEXT",      4, 11, 0x6BF8D67B,  8, 0x50F3C976, 16, 0x599A08A0},
};


void Check(bool condition, char const * what)
{
	if (!condition) {
		std::printf("%-72s FAILED\n", what);
		Failures++;
	}
}


template<typename T>
void Append_Value(Bytes & bytes, T const & value)
{
	std::byte const * first = reinterpret_cast<std::byte const *>(&value);
	bytes.insert(bytes.end(), first, first + sizeof(value));
}


void Append_Raw(Bytes & bytes, void const * data, std::size_t length)
{
	std::byte const * first = static_cast<std::byte const *>(data);
	bytes.insert(bytes.end(), first, first + length);
}


std::uint32_t Hash(Bytes const & bytes)
{
	return(Fnv_Add_Bytes(FNV_BASIS, bytes.data(), bytes.size()));
}


// Every Data byte holds a value derived from its offset, so a copy from the wrong offset or of
// the wrong length changes the hash. A variable-payload event points at its text instead.
char const SampleText[] = "\x02Rally here";
constexpr unsigned int SAMPLE_TEXT_SIZE = sizeof(SampleText) - 1;


EventClass Pattern_Event(std::uint8_t type)
{
	EventClass event;
	std::memset(&event, 0, sizeof(event));
	event.Type = type;
	event.Frame = EventFrame;
	event.ID = Sender;
	unsigned char * data = reinterpret_cast<unsigned char *>(&event.Data);
	for (std::size_t index = 0; index < sizeof(event.Data); index++) {
		data[index] = static_cast<unsigned char>(index * 37 + 11);
	}
	return(event);
}


EventClass Sample_Event(std::uint8_t type)
{
	EventClass event = Pattern_Event(type);
	if (EventClass::Has_Variable_Payload(type)) {
		event.Data.Variable.Pointer = const_cast<char *>(SampleText);
		event.Data.Variable.Size = SAMPLE_TEXT_SIZE;
	}
	return(event);
}


// The bytes Capture_Event keeps in ReplayFile::EventType::Data.
std::optional<Bytes> Replay_Payload(EventClass const & event)
{
	if (event.Type >= EventClass::LAST_EVENT) {
		return(std::nullopt);
	}

	Bytes bytes;
	if (event.Type == EventClass::BEACON_TEXT) {
		char const * text = static_cast<char const *>(event.Data.Variable.Pointer);
		if (text == NULL || event.Data.Variable.Size > ReplayFile::FIELD_LIMIT) {
			return(std::nullopt);
		}
		Append_Raw(bytes, text, event.Data.Variable.Size);
	} else if (EventClass::Has_Variable_Payload(event.Type)) {
		return(std::nullopt);
	} else {
		Append_Raw(bytes, &event.Data, EventClass::Fixed_Length(event.Type));
	}
	return(bytes);
}


// The bytes Sync_Record_Event_Impl keeps in SyncEventEntryType::Bytes.
std::optional<Bytes> Sync_Payload(EventClass const & event)
{
	switch (event.Type) {
		case EventClass::FRAMEINFO:
		case EventClass::FRAMESYNC:
		case EventClass::RESPONSE_TIME:
		case EventClass::PROCESS_TIME:
		case EventClass::TIMING:
			return(std::nullopt);
		default:
			break;
	}

	SyncEventEntryType entry {};
	std::size_t const length = event.Sync_Record_Bytes(entry.Bytes, sizeof(entry.Bytes));

	Bytes bytes;
	Append_Raw(bytes, entry.Bytes, length);
	return(bytes);
}


// The bytes Add_Compressed_Events in code/queue.cpp writes for an event that starts a packet.
Bytes Packet_Payload(EventClass const & event)
{
	std::size_t const datasize = EventClass::EventLength[event.Type];
	Bytes bytes;
	bytes.push_back(static_cast<std::byte>(event.Type));

	switch (event.Type) {
		case EventClass::RESPONSE_TIME:
			Append_Raw(bytes, &event.Data.FrameInfo.Delay, datasize);
			break;

		case EventClass::MEGAMISSION:
			bytes.push_back(std::byte{1});
			Append_Raw(bytes, &event.Data.MegaMission, datasize);
			break;

		case EventClass::ADDPLAYER:
		case EventClass::BEACON_TEXT:
			Append_Raw(bytes, &event.Data.Variable.Size, datasize);
			Append_Raw(bytes, event.Data.Variable.Pointer, event.Data.Variable.Size);
			break;

		default:
			Append_Raw(bytes, &event.Data, datasize);
			break;
	}
	return(bytes);
}


// Decodes the event's packet bytes after a FRAMEINFO envelope and compares the event they build.
void Check_Packet_Decodes(EventClass const & event, Bytes const & payload)
{
	if (event.Type == EventClass::FRAMEINFO || event.Type == EventClass::FRAMESYNC) {
		return;
	}

	EventClass envelope;
	std::memset(&envelope, 0, sizeof(envelope));
	envelope.Type = EventClass::FRAMEINFO;
	envelope.Frame = EventFrame;
	envelope.ID = Sender;
	envelope.Data.FrameInfo.Delay = 4;

	Bytes packet;
	Append_Raw(packet, &envelope, offsetof(EventClass, Data) + sizeof(envelope.Data.FrameInfo));
	packet.insert(packet.end(), payload.begin(), payload.end());

	NetPacket::DecodeResult const result = NetPacket::Decode_Event_Packet(packet, NetPacket::Encoding::COMPRESSED, Sender);

	char label[96];
	std::snprintf(label, sizeof(label), "packet bytes of %s decode to one event", EventClass::EventNames[event.Type]);
	bool const decoded = result.Succeeded() && result.Events.size() == 2 && result.Events[1].Event.Type == event.Type;
	Check(decoded, label);
	if (!decoded) {
		return;
	}

	NetPacket::DecodedEvent const & got = result.Events[1];
	std::size_t const length = EventClass::EventLength[event.Type];
	bool same = got.Event.Frame == event.Frame && got.Event.ID == event.ID;
	if (event.Type == EventClass::RESPONSE_TIME) {
		same = same && got.Event.Data.FrameInfo.Delay == event.Data.FrameInfo.Delay;
	} else if (EventClass::Has_Variable_Payload(event.Type)) {
		same = same && got.Event.Data.Variable.Size == event.Data.Variable.Size
			&& got.VariablePayload.size() == event.Data.Variable.Size
			&& std::memcmp(got.VariablePayload.data(), event.Data.Variable.Pointer, event.Data.Variable.Size) == 0;
	} else {
		same = same && std::memcmp(&got.Event.Data, &event.Data, length) == 0;
	}
	std::snprintf(label, sizeof(label), "packet bytes of %s rebuild the sent event data", EventClass::EventNames[event.Type]);
	Check(same, label);
}


int Length_Of(std::optional<Bytes> const & bytes)
{
	return(bytes ? static_cast<int>(bytes->size()) : NOT_COPIED);
}


std::uint32_t Hash_Of(std::optional<Bytes> const & bytes)
{
	return(bytes ? Hash(*bytes) : 0);
}


void Print_Table(void)
{
	for (int index = 0; index < EventClass::LAST_EVENT; index++) {
		std::uint8_t const type = static_cast<std::uint8_t>(index);
		EventClass const sample = Sample_Event(type);
		std::optional<Bytes> const replay = Replay_Payload(sample);
		std::optional<Bytes> const sync = Sync_Payload(sample);
		Bytes const packet = Packet_Payload(sample);
		std::printf("\t{%-18s %2d, %2d, 0x%08X, %2d, 0x%08X, %2d, 0x%08X},\n",
			(std::string("\"") + EventClass::EventNames[type] + "\",").c_str(),
			EventClass::EventLength[type],
			Length_Of(replay), Hash_Of(replay),
			Length_Of(sync), Hash_Of(sync),
			static_cast<int>(packet.size()), Hash(packet));
	}
}


void Test_Table(void)
{
	for (int index = 0; index < EventClass::LAST_EVENT; index++) {
		std::uint8_t const type = static_cast<std::uint8_t>(index);
		ExpectedPayload const & expected = Expected[index];
		char const * name = EventClass::EventNames[type];
		char label[96];

		std::snprintf(label, sizeof(label), "table row %d names %s", index, name);
		Check(expected.Name != nullptr && std::strcmp(expected.Name, name) == 0, label);

		std::snprintf(label, sizeof(label), "%s keeps its EventLength", name);
		Check(EventClass::EventLength[type] == expected.EventLength, label);

		EventClass const sample = Sample_Event(type);

		std::optional<Bytes> const replay = Replay_Payload(sample);
		std::optional<Bytes> const sync = Sync_Payload(sample);
		Bytes const packet = Packet_Payload(sample);

		std::snprintf(label, sizeof(label), "%s replay payload keeps its length and bytes", name);
		Check(Length_Of(replay) == expected.ReplayLength && Hash_Of(replay) == expected.ReplayHash, label);

		std::snprintf(label, sizeof(label), "%s sync record payload keeps its length and bytes", name);
		Check(Length_Of(sync) == expected.SyncLength && Hash_Of(sync) == expected.SyncHash, label);

		std::snprintf(label, sizeof(label), "%s packet payload keeps its length and bytes", name);
		Check(static_cast<int>(packet.size()) == expected.PacketLength && Hash(packet) == expected.PacketHash, label);

		Check_Packet_Decodes(sample, packet);
	}
}


// Two peers hold the same text at different addresses, and their sync records must match.
void Test_Sync_Record_Ignores_Pointer(void)
{
	for (std::uint8_t type : {(std::uint8_t)EventClass::ADDPLAYER, (std::uint8_t)EventClass::BEACON_TEXT}) {
		char const * name = EventClass::EventNames[type];
		char other[sizeof(SampleText)];
		std::memcpy(other, SampleText, sizeof(SampleText));

		EventClass first = Sample_Event(type);
		EventClass second = Sample_Event(type);
		second.Data.Variable.Pointer = other;

		std::optional<Bytes> const a = Sync_Payload(first);
		std::optional<Bytes> const b = Sync_Payload(second);
		char label[96];
		std::snprintf(label, sizeof(label), "%s sync record is the same for the same text at another address", name);
		Check(first.Data.Variable.Pointer != second.Data.Variable.Pointer && a && b && *a == *b && a->size() == 8, label);

		other[0] = 'X';
		std::optional<Bytes> const c = Sync_Payload(second);
		std::snprintf(label, sizeof(label), "%s sync record changes with the text", name);
		Check(a && c && *a != *c, label);
	}
}

}


int main(int argc, char ** argv)
{
	if (argc > 1 && std::strcmp(argv[1], "--print") == 0) {
		Print_Table();
		return(0);
	}

	Test_Table();
	Test_Sync_Record_Ignores_Pointer();
	Check(EventClass::Fixed_Length(EventClass::LAST_EVENT) == 0 && EventClass::Fixed_Length(0xFF) == 0,
		"a type past LAST_EVENT copies no Data bytes");

	std::printf("\n%s\n", Failures == 0 ? "All checks passed." : "Some checks FAILED.");
	return(Failures == 0 ? 0 : 1);
}
