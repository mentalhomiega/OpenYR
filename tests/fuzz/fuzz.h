/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

// A deterministic mutation fuzzer for the parsers that read data from other players. Each
// harness describes one parser as a Target: synthetic seed inputs it writes itself, inputs
// that once crashed, and a function that feeds one input to the parser. The driver mutates
// the seeds with a fixed-seed generator and runs a bounded number of iterations, so a run
// repeats exactly. The harness needs no game files and no fuzzing library.
//
//   fuzz_<name>                      run the seeds, the regressions and the default iterations
//   fuzz_<name> --iterations N       run N mutated inputs instead (FUZZ_ITERATIONS does the same)
//   fuzz_<name> --seed S             start the generator from S (FUZZ_SEED does the same)
//   fuzz_<name> --file PATH          run the parser on the bytes of one file
//   fuzz_<name> --dump-iteration N   write mutated input N to standard output as hex
//
// A crash, a sanitizer report, a run of more than FUZZ_ITERATION_LIMIT_MS milliseconds on one
// input (default 10000), or a single allocation of more than 256 MiB ends the run with the
// hex of the input that caused it on standard error.

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


namespace Fuzz
{
	typedef std::vector<std::uint8_t> Bytes;


	// A fixed generator (xoshiro256**) so that a run does not depend on the standard library.
	class Random
	{
		public:
			explicit Random(std::uint64_t seed);

			std::uint64_t Next(void);

			// A value from 0 to limit - 1, or 0 when limit is 0.
			std::uint32_t Below(std::uint32_t limit);

			bool Chance(std::uint32_t one_in) {return(Below(one_in) == 0);}

		private:
			std::uint64_t State[4];
	};


	// A named input. Seeds and regressions are both lists of these.
	struct Input
	{
		std::string Name;
		Bytes Data;
	};


	// A parser failed an invariant the harness checks on top of not crashing.
	struct Violation
	{
		std::string Message;
	};


	struct Target
	{
		char const * Name;

		// Mutated inputs are cut to this many bytes.
		std::size_t MaxSize;

		// Mutated inputs per run, unless the command line or FUZZ_ITERATIONS says otherwise.
		int Iterations;

		// Well-formed inputs the mutator starts from, written by the harness.
		std::vector<Input> (*Seeds)(void);

		// Inputs that once crashed, hung or overflowed. They run unmutated before the others
		// and stay in the harness for good.
		std::vector<Input> (*Regressions)(void);

		// Strings the mutator inserts whole, such as keywords of a text format. May be null.
		std::vector<std::string> (*Dictionary)(void);

		// Feeds one input to the parser. A parser that accepts or rejects the input returns
		// normally; throw Violation when an invariant of the result does not hold.
		void (*Run)(Bytes const & input);
	};


	// The target of this executable, defined by the harness.
	Target const & Get_Target(void);

	// Reads a run of hex digit pairs, ignoring white space.
	Bytes From_Hex(char const * text);

	Bytes From_String(std::string const & text);

	// Appends little-endian integers.
	void Put_U16(Bytes & bytes, std::uint32_t value);
	void Put_U32(Bytes & bytes, std::uint32_t value);

	// Throws Violation with the message when the condition is false.
	void Require(bool condition, char const * message);

	// One mutation round: one to four edits of the input chosen by the generator, using the
	// corpus for splices and the dictionary for tokens.
	Bytes Mutate(Random & random, Bytes const & input, std::vector<Input> const & corpus,
		std::vector<std::string> const & dictionary, std::size_t max_size);
}
