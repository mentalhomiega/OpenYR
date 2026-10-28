/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "replayfile.h"

#include <cstring>


namespace ReplayFile {

namespace {

constexpr char MAGIC[8] = {'O', 'Y', 'R', 'R', 'E', 'P', 'L', 'Y'};

constexpr std::uint8_t TAG_EVENT = 'E';
constexpr std::uint8_t TAG_HASH = 'H';
constexpr std::uint8_t TAG_END = 'Z';


void Put_U8(std::vector<std::uint8_t> & out, std::uint8_t value)
{
	out.push_back(value);
}


void Put_U32(std::vector<std::uint8_t> & out, std::uint32_t value)
{
	for (int shift = 0; shift < 32; shift += 8) {
		out.push_back(static_cast<std::uint8_t>(value >> shift));
	}
}


void Put_Text(std::vector<std::uint8_t> & out, std::string const & text)
{
	Put_U32(out, static_cast<std::uint32_t>(text.size()));
	out.insert(out.end(), text.begin(), text.end());
}


void Put_Header(std::vector<std::uint8_t> & out, HeaderType const & header)
{
	out.insert(out.end(), MAGIC, MAGIC + sizeof(MAGIC));
	Put_U32(out, REVISION);
	Put_U32(out, header.Version);
	Put_Text(out, header.Commit);
	Put_U8(out, header.Modified ? 1 : 0);
	Put_Text(out, header.Architecture);
	Put_Text(out, header.Map);
	Put_U32(out, header.MapCrc);
	Put_U32(out, header.RulesCrc);
	Put_U32(out, header.ArtCrc);
	Put_U32(out, header.AiCrc);
	Put_Text(out, header.Mods);
	Put_U32(out, header.Seed);
	Put_U32(out, static_cast<std::uint32_t>(header.GameSpeed));
	Put_U32(out, header.HashInterval);
	Put_U32(out, static_cast<std::uint32_t>(header.LocalHouse));
	Put_Text(out, header.SpawnIni);
}


// Reads fields from a byte range. A field that would run past the end fails and reads nothing.
class ReaderClass
{
	public:

		explicit ReaderClass(std::vector<std::uint8_t> const & bytes) : Bytes(bytes) {}

		bool At_End(void) const { return(Position >= Bytes.size()); }

		bool Get_U8(std::uint8_t & value)
		{
			if (Bytes.size() - Position < 1) {
				return(false);
			}
			value = Bytes[Position++];
			return(true);
		}

		bool Get_U32(std::uint32_t & value)
		{
			if (Bytes.size() - Position < 4) {
				return(false);
			}
			value = 0;
			for (int shift = 0; shift < 32; shift += 8) {
				value |= static_cast<std::uint32_t>(Bytes[Position++]) << shift;
			}
			return(true);
		}

		bool Get_I32(std::int32_t & value)
		{
			std::uint32_t raw = 0;
			if (!Get_U32(raw)) {
				return(false);
			}
			value = static_cast<std::int32_t>(raw);
			return(true);
		}

		bool Get_Bytes(std::vector<std::uint8_t> & out)
		{
			std::uint32_t length = 0;
			if (!Get_U32(length) || length > FIELD_LIMIT || Bytes.size() - Position < length) {
				return(false);
			}
			out.assign(Bytes.begin() + Position, Bytes.begin() + Position + length);
			Position += length;
			return(true);
		}

		bool Get_Text(std::string & out)
		{
			std::vector<std::uint8_t> raw;
			if (!Get_Bytes(raw)) {
				return(false);
			}
			out.assign(raw.begin(), raw.end());
			return(true);
		}

		bool Get_Magic(void)
		{
			if (Bytes.size() < sizeof(MAGIC) || std::memcmp(Bytes.data(), MAGIC, sizeof(MAGIC)) != 0) {
				return(false);
			}
			Position = sizeof(MAGIC);
			return(true);
		}

	private:

		std::vector<std::uint8_t> const & Bytes;
		std::size_t Position = 0;
};


bool Get_Header(ReaderClass & reader, HeaderType & header)
{
	std::uint8_t modified = 0;
	bool const read = reader.Get_U32(header.Version)
		&& reader.Get_Text(header.Commit)
		&& reader.Get_U8(modified)
		&& reader.Get_Text(header.Architecture)
		&& reader.Get_Text(header.Map)
		&& reader.Get_U32(header.MapCrc)
		&& reader.Get_U32(header.RulesCrc)
		&& reader.Get_U32(header.ArtCrc)
		&& reader.Get_U32(header.AiCrc)
		&& reader.Get_Text(header.Mods)
		&& reader.Get_U32(header.Seed)
		&& reader.Get_I32(header.GameSpeed)
		&& reader.Get_U32(header.HashInterval)
		&& reader.Get_I32(header.LocalHouse)
		&& reader.Get_Text(header.SpawnIni);
	header.Modified = modified != 0;
	return(read);
}


bool Get_Hash(ReaderClass & reader, HashType & hash)
{
	return(reader.Get_U32(hash.Frame) && reader.Get_U32(hash.State) && reader.Get_U32(hash.Targets));
}


std::string Hex(std::uint32_t value)
{
	char text[16];
	std::snprintf(text, sizeof(text), "%08X", static_cast<unsigned int>(value));
	return(text);
}


std::string Or_None(std::string const & text)
{
	return(text.empty() ? std::string("none") : text);
}


std::string Difference(char const * what, std::string const & recorded, std::string const & current)
{
	return(std::string("The replay was made with ") + what + " " + Or_None(recorded)
		+ ", and this game has " + Or_None(current) + ".");
}

}


/// <summary>
/// Reads a replay from its bytes. A recording that stops between records, as one does when the
/// game crashed, loads as far as it goes and is not complete.
/// </summary>
LoadResult Parse(std::vector<std::uint8_t> const & bytes, ReplayType & replay)
{
	replay = ReplayType();

	ReaderClass reader(bytes);
	if (!reader.Get_Magic()) {
		return(LoadResult::NOT_A_REPLAY);
	}

	std::uint32_t revision = 0;
	if (!reader.Get_U32(revision)) {
		return(LoadResult::DAMAGED);
	}
	if (revision != REVISION) {
		return(LoadResult::OTHER_REVISION);
	}
	if (!Get_Header(reader, replay.Header)) {
		return(LoadResult::DAMAGED);
	}

	while (!reader.At_End()) {
		std::uint8_t tag = 0;
		reader.Get_U8(tag);

		if (tag == TAG_EVENT) {
			EventType event;
			if (!reader.Get_U32(event.Frame) || !reader.Get_U8(event.Type) || !reader.Get_I32(event.EventFrame)
				|| !reader.Get_I32(event.House) || !reader.Get_Bytes(event.Data)) {
				break;
			}
			replay.Events.push_back(std::move(event));
		} else if (tag == TAG_HASH) {
			HashType hash;
			if (!Get_Hash(reader, hash)) {
				break;
			}
			replay.Hashes.push_back(hash);
		} else if (tag == TAG_END) {
			if (!Get_Hash(reader, replay.End)) {
				break;
			}
			replay.IsComplete = true;
			if (!reader.At_End()) {
				return(LoadResult::DAMAGED);
			}
		} else {
			return(LoadResult::DAMAGED);
		}
	}

	return(LoadResult::OK);
}


LoadResult Load(char const * path, ReplayType & replay)
{
	std::FILE * file = std::fopen(path, "rb");
	if (file == nullptr) {
		return(LoadResult::CANNOT_OPEN);
	}

	std::vector<std::uint8_t> bytes;
	std::uint8_t block[4096];
	std::size_t got = 0;
	while ((got = std::fread(block, 1, sizeof(block), file)) > 0) {
		bytes.insert(bytes.end(), block, block + got);
	}
	std::fclose(file);

	return(Parse(bytes, replay));
}


char const * Load_Result_Text(LoadResult result)
{
	switch (result) {
		case LoadResult::OK:
			return("The replay was read.");
		case LoadResult::CANNOT_OPEN:
			return("The replay file could not be opened.");
		case LoadResult::NOT_A_REPLAY:
			return("The file is not a replay.");
		case LoadResult::OTHER_REVISION:
			return("The replay was written in a file format this game does not read.");
		case LoadResult::DAMAGED:
			return("The replay file is damaged.");
	}
	return("");
}


std::string Find_Incompatibility(HeaderType const & recorded, HeaderType const & current)
{
	if (recorded.Version != current.Version || recorded.Commit != current.Commit) {
		return(Difference("build", recorded.Commit, current.Commit));
	}
	if (recorded.Map != current.Map) {
		return(Difference("the map", recorded.Map, current.Map));
	}
	if (recorded.MapCrc != current.MapCrc) {
		return(Difference("a map file with checksum", Hex(recorded.MapCrc), Hex(current.MapCrc)));
	}
	if (recorded.RulesCrc != current.RulesCrc) {
		return(Difference("rules with checksum", Hex(recorded.RulesCrc), Hex(current.RulesCrc)));
	}
	if (recorded.ArtCrc != current.ArtCrc) {
		return(Difference("art rules with checksum", Hex(recorded.ArtCrc), Hex(current.ArtCrc)));
	}
	if (recorded.AiCrc != current.AiCrc) {
		return(Difference("AI rules with checksum", Hex(recorded.AiCrc), Hex(current.AiCrc)));
	}
	if (recorded.Mods != current.Mods) {
		return(Difference("the mods", recorded.Mods, current.Mods));
	}
	return(std::string());
}


WriterClass::~WriterClass(void)
{
	Close();
}


/// <summary>
/// Creates the file and writes the header to it.
/// </summary>
/// <returns>bool; Is the file ready for records?</returns>
bool WriterClass::Open(char const * path, HeaderType const & header)
{
	Close();

	File = std::fopen(path, "wb");
	if (File == nullptr) {
		return(false);
	}

	std::vector<std::uint8_t> bytes;
	Put_Header(bytes, header);
	Put(bytes);
	if (File != nullptr) {
		std::fflush(File);
	}

	return(File != nullptr);
}


void WriterClass::Put(std::vector<std::uint8_t> const & bytes)
{
	if (File != nullptr && std::fwrite(bytes.data(), 1, bytes.size(), File) != bytes.size()) {
		std::fclose(File);
		File = nullptr;
	}
}


void WriterClass::Add_Event(EventType const & event)
{
	std::vector<std::uint8_t> bytes;
	Put_U8(bytes, TAG_EVENT);
	Put_U32(bytes, event.Frame);
	Put_U8(bytes, event.Type);
	Put_U32(bytes, static_cast<std::uint32_t>(event.EventFrame));
	Put_U32(bytes, static_cast<std::uint32_t>(event.House));
	Put_U32(bytes, static_cast<std::uint32_t>(event.Data.size()));
	bytes.insert(bytes.end(), event.Data.begin(), event.Data.end());
	Put(bytes);
}


/// <summary>
/// Appends a state hash and flushes the file, so that a crash loses at most the frames since the last hash.
/// </summary>
void WriterClass::Add_Hash(HashType const & hash)
{
	std::vector<std::uint8_t> bytes;
	Put_U8(bytes, TAG_HASH);
	Put_U32(bytes, hash.Frame);
	Put_U32(bytes, hash.State);
	Put_U32(bytes, hash.Targets);
	Put(bytes);
	if (File != nullptr) {
		std::fflush(File);
	}
}


/// <summary>
/// Writes the end record, which names the last frame played, and closes the file.
/// </summary>
void WriterClass::Finish(HashType const & end)
{
	std::vector<std::uint8_t> bytes;
	Put_U8(bytes, TAG_END);
	Put_U32(bytes, end.Frame);
	Put_U32(bytes, end.State);
	Put_U32(bytes, end.Targets);
	Put(bytes);
	Close();
}


void WriterClass::Close(void)
{
	if (File != nullptr) {
		std::fclose(File);
		File = nullptr;
	}
}

}
