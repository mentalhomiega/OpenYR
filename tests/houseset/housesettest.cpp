/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// Holds HouseSet, CountrySet and HouseArray to their contract: every house has its own mark or
// value, set algebra works across all houses at once, a save carries every slot, including
// those past the thirty-second house, and a CountrySet bit is shared by the houses of one
// country.
//
// Needs no game data.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <type_traits>
#include <vector>


// Stands in for the engine's house, which would drag the whole game in behind it. The
// templates read its heap index and its country bit only.
class HouseClass
{
	public:
		int HeapID;
		int OwnCountry;

		// Mirrors HouseClass::Own_Mask: the bit of the country, modulo 32, or none without one.
		int Own_Mask(void) const {return(OwnCountry < 0 ? 0 : 1 << (OwnCountry & 31));}
};

#include "houseset.h"


namespace {

int Failures = 0;
int Checked = 0;


void Check(bool passed, char const * what)
{
	Checked++;
	if (!passed) {
		std::printf("FAILED %s\n", what);
		Failures++;
	}
}


HouseClass House[HOUSE_MAX];


// Writes or reads the raw bytes of whatever it is handed, the way the save stream treats
// numbers and arrays of numbers.
class ByteStream
{
	public:
		explicit ByteStream(bool saving) : Saving(saving), Cursor(0) {}

		bool Is_Saving(void) const {return(Saving);}
		bool Is_Loading(void) const {return(!Saving);}

		template<typename T>
		void Serialize(T & value)
		{
			static_assert(std::is_trivially_copyable_v<T>);
			unsigned char * bytes = reinterpret_cast<unsigned char *>(&value);
			if (Saving) {
				Bytes.insert(Bytes.end(), bytes, bytes + sizeof(T));
			} else {
				std::memcpy(bytes, Bytes.data() + Cursor, sizeof(T));
				Cursor += sizeof(T);
			}
		}

		bool Saving;
		std::size_t Cursor;
		std::vector<unsigned char> Bytes;
};


class CRCRecorder
{
	public:
		void operator()(int value) {Values.push_back(value);}
		std::vector<int> Values;
};


void Check_Set(void)
{
	HouseSet set;
	Check(!set.Any(), "a fresh set holds no house");

	set.Set(&House[0]);
	set.Set(&House[40]);
	set.Set(&House[HOUSE_MAX - 1]);
	Check(set[&House[0]], "the first house is marked");
	Check(set[&House[40]], "a house past the thirty-second is marked");
	Check(set[&House[HOUSE_MAX - 1]], "the last house is marked");
	Check(!set[&House[1]], "an unmarked house reads clear");
	Check(!set[&House[8]], "a house one byte along reads clear");
	Check(!set[static_cast<HouseClass const *>(nullptr)], "no house reads clear");

	set.Clear(&House[40]);
	Check(!set[&House[40]], "clearing a house removes only its mark");
	Check(set[&House[0]] && set[&House[HOUSE_MAX - 1]], "clearing a house leaves the others");

	set.Clear();
	Check(!set.Any(), "clearing the set removes every mark");
}


void Check_Algebra(void)
{
	HouseSet mapped;
	HouseSet visible;
	mapped.Set(&House[2]);
	mapped.Set(&House[33]);
	mapped.Set(&House[50]);
	visible.Set(&House[33]);

	HouseSet stale = mapped & ~visible;
	Check(stale[&House[2]] && stale[&House[50]], "mapped and not visible keeps the unseen houses");
	Check(!stale[&House[33]], "mapped and not visible drops the house still seeing");

	HouseSet both = stale | visible;
	Check(both == mapped, "the parts join back into the whole");
	Check(both != stale, "sets with different houses differ");

	HouseSet copy = mapped;
	copy &= visible;
	Check(copy == visible, "&= keeps only the houses in both");
	copy |= stale;
	Check(copy == mapped, "|= adds the other houses");
}


void Check_Country(void)
{
	CountrySet spied;
	Check(!spied.Any(), "a fresh country set holds no country");

	spied.Set(&House[1]);
	Check(spied[&House[1]], "the spy's own house reads its country as spied");
	Check(spied[&House[33]], "a house of the same country reads the spied bit");
	Check(!spied[&House[2]], "a house of another country reads clear");
	Check(!spied[static_cast<HouseClass const *>(nullptr)], "no house reads clear in a country set");

	HouseClass countryless;
	countryless.HeapID = 60;
	countryless.OwnCountry = -1;
	spied.Set(&countryless);
	Check(!spied[&countryless], "a house with no country is in no country set");
	Check(spied[&House[1]], "setting a countryless house leaves the others");

	spied.Set(&House[9]);
	spied.Clear(&House[33]);
	Check(!spied[&House[1]] && !spied[&House[33]], "clearing a house clears its country for all of its houses");
	Check(spied[&House[9]], "clearing one country leaves the others");

	spied.Clear();
	Check(!spied.Any(), "clearing the country set removes every country");

	CountrySet americans;
	americans.Set(&House[0]);
	CountrySet other;
	other.Set(&House[2]);
	Check((americans | other)[&House[32]], "a union keeps every country");
	Check(((americans | other) & americans) == americans, "an intersection keeps the common country");
	Check(americans != other, "countries that differ compare unequal");
}


void Check_Array(void)
{
	HouseArray<std::uint16_t> counts;
	Check(counts[&House[0]] == 0 && counts[&House[63]] == 0, "a fresh array holds zero for every house");

	counts[&House[5]] += 2;
	counts[&House[60]] = 300;
	Check(counts[&House[5]] == 2, "a house keeps its own value");
	Check(counts[&House[60]] == 300, "a value past what a byte holds survives");
	Check(counts[&House[6]] == 0, "a neighbor's value is untouched");

	HouseArray<std::uint16_t> const & view = counts;
	Check(view[&House[60]] == 300, "a const array reads the same value");

	counts.Fill(7);
	Check(counts[&House[0]] == 7 && counts[&House[63]] == 7, "filling sets every house");
}


void Check_Save(void)
{
	HouseSet set;
	set.Set(&House[1]);
	set.Set(&House[62]);
	HouseArray<std::uint16_t> counts;
	counts[&House[3]] = 9;
	counts[&House[63]] = 1234;

	ByteStream out(true);
	set.Serialize(out);
	counts.Serialize(out);

	HouseSet set_back;
	HouseArray<std::uint16_t> counts_back;
	ByteStream in(false);
	in.Bytes = out.Bytes;
	set_back.Serialize(in);
	counts_back.Serialize(in);

	Check(set_back == set, "a set comes back from a save with every house");
	Check(counts_back[&House[3]] == 9 && counts_back[&House[63]] == 1234, "an array comes back from a save with every value");
	Check(in.Cursor == out.Bytes.size(), "a load reads exactly what the save wrote");
	Check(out.Bytes.size() == sizeof(std::uint64_t) + sizeof(std::uint64_t) + 2 * sizeof(std::uint16_t), "an array saves only the houses that hold a value");

	HouseArray<std::uint16_t> stale;
	stale[&House[7]] = 5;
	ByteStream again(false);
	again.Bytes = out.Bytes;
	again.Cursor = sizeof(std::uint64_t);
	stale.Serialize(again);
	Check(stale[&House[7]] == 0 && stale[&House[63]] == 1234, "a load clears the houses the save left out");
}


void Check_Country_Save(void)
{
	CountrySet spied;
	spied.Set(&House[4]);
	spied.Set(&House[30]);

	ByteStream out(true);
	spied.Serialize(out);
	Check(out.Bytes.size() == sizeof(std::uint32_t), "a country set saves one 32-bit word");

	CountrySet back;
	ByteStream in(false);
	in.Bytes = out.Bytes;
	back.Serialize(in);
	Check(back == spied, "a country set comes back from a save with every country");
	Check(in.Cursor == out.Bytes.size(), "a country set load reads exactly what the save wrote");
}


void Check_CRC(void)
{
	HouseSet low;
	HouseSet high;
	low.Set(&House[0]);
	high.Set(&House[32]);

	CRCRecorder a;
	CRCRecorder b;
	low.Compute_CRC(a);
	high.Compute_CRC(b);
	Check(a.Values.size() == 2 && b.Values.size() == 2, "a set feeds the checksum both halves");
	Check(a.Values != b.Values, "houses either side of the thirty-second reach the checksum apart");
}


void Check_Layout(void)
{
	Check(std::is_trivially_copyable_v<HouseSet>, "a set survives the raw copy cells are moved with");
	Check(std::is_trivially_copyable_v<CountrySet>, "a country set survives the raw copy cells are moved with");
	Check(std::is_trivially_copyable_v<HouseArray<std::uint16_t>>, "an array survives the raw copy cells are moved with");
	Check(sizeof(HouseSet) == 8, "a set is one 64-bit word");
	Check(sizeof(CountrySet) == 4, "a country set is one 32-bit word");
}

} // namespace


int main(void)
{
	for (int index = 0; index < HOUSE_MAX; index++) {
		House[index].HeapID = index;
		House[index].OwnCountry = index % 32;
	}

	Check_Set();
	Check_Algebra();
	Check_Country();
	Check_Array();
	Check_Save();
	Check_Country_Save();
	Check_CRC();
	Check_Layout();

	std::printf("%-52s %s\n", "House sets and arrays",
		Failures == 0 ? "ok" : "FAILED");
	std::printf("checked %d cases, %d mismatches\n", Checked, Failures);

	return(Failures == 0 ? 0 : 1);
}
