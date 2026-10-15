/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "fuzz.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <new>
#include <thread>

#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define FUZZ_SANITIZED 1
#endif
#endif
#if defined(__SANITIZE_ADDRESS__)
#define FUZZ_SANITIZED 1
#endif

#if defined(FUZZ_SANITIZED) && (defined(__GNUC__) || defined(__clang__))
extern "C" void __sanitizer_set_death_callback(void (*callback)(void)) __attribute__((weak));
#endif


#if defined(__GNUC__) || defined(__clang__)
// The sanitizers read these when they start. A harness wants the first undefined behavior to
// end the run with an abort that Install_Reporters sees, and has no use for leak reports from
// parsers that leave their own state allocated.
extern "C" __attribute__((used)) char const * __ubsan_default_options(void)
{
	return("print_stacktrace=1:abort_on_error=1");
}

extern "C" __attribute__((used)) char const * __asan_default_options(void)
{
	return("detect_leaks=0");
}
#endif


namespace
{

// The largest single allocation a parser may ask for. A header field that decides an
// allocation size is the usual way a damaged file exhausts memory.
constexpr std::size_t ALLOCATION_CAP = std::size_t(256) << 20;

std::atomic<std::size_t> OversizeRequest(0);


void * Allocate(std::size_t size)
{
	if (size > ALLOCATION_CAP) {
		OversizeRequest = size;
		return(nullptr);
	}
	void * block = std::malloc(size != 0 ? size : 1);
	return(block);
}

}


void * operator new(std::size_t size)
{
	void * block = Allocate(size);
	if (block == nullptr) {
		throw std::bad_alloc();
	}
	return(block);
}


void * operator new[](std::size_t size)
{
	void * block = Allocate(size);
	if (block == nullptr) {
		throw std::bad_alloc();
	}
	return(block);
}


void * operator new(std::size_t size, std::nothrow_t const &) noexcept
{
	return(Allocate(size));
}


void * operator new[](std::size_t size, std::nothrow_t const &) noexcept
{
	return(Allocate(size));
}


void operator delete(void * block) noexcept {std::free(block);}
void operator delete[](void * block) noexcept {std::free(block);}
void operator delete(void * block, std::size_t) noexcept {std::free(block);}
void operator delete[](void * block, std::size_t) noexcept {std::free(block);}
void operator delete(void * block, std::nothrow_t const &) noexcept {std::free(block);}
void operator delete[](void * block, std::nothrow_t const &) noexcept {std::free(block);}


namespace Fuzz
{

namespace
{

constexpr std::uint64_t DEFAULT_SEED = 0x4F70656E54530001ull;
constexpr std::size_t MAX_DUMP_BYTES = 65536;

// What the crash reporters print. They read these after the parser has stopped the process,
// so they are plain globals rather than state passed down.
Bytes const * CurrentInput = nullptr;
std::string CurrentName;
char const * CurrentTarget = "";
std::atomic<bool> Reported(false);

std::atomic<bool> Running(false);
std::atomic<long long> StartedAt(0);
long long LimitMilliseconds = 10000;


long long Now_Milliseconds(void)
{
	return(std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now().time_since_epoch()).count());
}


void Print_Hex(Bytes const & bytes)
{
	std::size_t const shown = std::min(bytes.size(), MAX_DUMP_BYTES);
	std::fputs("  hex:", stderr);
	for (std::size_t index = 0; index < shown; index++) {
		std::fprintf(stderr, "%s%02x", (index % 32 == 0) ? "\n    " : " ", bytes[index]);
	}
	if (shown < bytes.size()) {
		std::fprintf(stderr, "\n    (%zu more bytes not shown)", bytes.size() - shown);
	}
	std::fputc('\n', stderr);
}


void Report_Current(char const * why)
{
	if (Reported.exchange(true)) {
		return;
	}
	std::fprintf(stderr, "fuzz %s: %s\n  input: %s (%zu bytes)\n", CurrentTarget, why,
		CurrentName.c_str(), CurrentInput != nullptr ? CurrentInput->size() : std::size_t(0));
	if (CurrentInput != nullptr) {
		Print_Hex(*CurrentInput);
	}
	std::fflush(stderr);
}


void On_Sanitizer_Death(void)
{
	Report_Current("a sanitizer stopped the run");
}


extern "C" void On_Signal(int signal_number)
{
	Report_Current(signal_number == SIGABRT ? "the run aborted" : "the run crashed");
	std::_Exit(2);
}


void On_Terminate(void)
{
	Report_Current("an exception left the parser unhandled");
	std::_Exit(2);
}


void Watchdog(void)
{
	for (;;) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		if (Running && Now_Milliseconds() - StartedAt > LimitMilliseconds) {
			Report_Current("one input ran past the time limit");
			std::_Exit(3);
		}
	}
}


void Install_Reporters(void)
{
	std::set_terminate(On_Terminate);

	// UndefinedBehaviorSanitizer ends the run with an abort, which this handler reports. The
	// sanitizers own the other signals, so those are handled here only without them.
	std::signal(SIGABRT, On_Signal);
#if defined(FUZZ_SANITIZED) && (defined(__GNUC__) || defined(__clang__))
	if (__sanitizer_set_death_callback != nullptr) {
		__sanitizer_set_death_callback(On_Sanitizer_Death);
	}
#else
	std::signal(SIGSEGV, On_Signal);
	std::signal(SIGFPE, On_Signal);
	std::signal(SIGILL, On_Signal);
#endif

	std::thread(Watchdog).detach();
}


std::uint64_t Splitmix(std::uint64_t & state)
{
	std::uint64_t value = (state += 0x9E3779B97F4A7C15ull);
	value = (value ^ (value >> 30)) * 0xBF58476D1CE4E5B9ull;
	value = (value ^ (value >> 27)) * 0x94D049BB133111EBull;
	return(value ^ (value >> 31));
}


std::uint64_t Rotate_Left(std::uint64_t value, int bits)
{
	return((value << bits) | (value >> (64 - bits)));
}


std::uint64_t Hash_Name(char const * text)
{
	std::uint64_t hash = 0xCBF29CE484222325ull;
	for (; *text != '\0'; text++) {
		hash = (hash ^ (unsigned char)*text) * 0x100000001B3ull;
	}
	return(hash);
}


std::uint32_t const Interesting_32[] = {
	0, 1, 2, 3, 4, 7, 8, 15, 16, 31, 32, 63, 64, 100, 127, 128, 129, 255, 256, 257, 511, 512,
	1000, 1023, 1024, 4095, 4096, 4097, 8191, 8192, 16383, 16384, 32767, 32768, 32769, 65535,
	65536, 65537, 0x00FFFFFFu, 0x01000000u, 0x7FFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u,
	0xFFFFFF00u, 0xFFFFFFFEu, 0xFFFFFFFFu
};

std::size_t const INTERESTING_COUNT = sizeof(Interesting_32) / sizeof(Interesting_32[0]);


void Store_Little(Bytes & bytes, std::size_t at, std::uint32_t value, std::size_t width)
{
	for (std::size_t index = 0; index < width && at + index < bytes.size(); index++) {
		bytes[at + index] = (std::uint8_t)(value >> (8 * index));
	}
}


std::uint32_t Load_Little(Bytes const & bytes, std::size_t at, std::size_t width)
{
	std::uint32_t value = 0;
	for (std::size_t index = 0; index < width && at + index < bytes.size(); index++) {
		value |= (std::uint32_t)bytes[at + index] << (8 * index);
	}
	return(value);
}

}


Random::Random(std::uint64_t seed)
{
	std::uint64_t mixer = seed;
	for (int index = 0; index < 4; index++) {
		State[index] = Splitmix(mixer);
	}
}


std::uint64_t Random::Next(void)
{
	std::uint64_t const result = Rotate_Left(State[1] * 5, 7) * 9;
	std::uint64_t const t = State[1] << 17;
	State[2] ^= State[0];
	State[3] ^= State[1];
	State[1] ^= State[2];
	State[0] ^= State[3];
	State[2] ^= t;
	State[3] = Rotate_Left(State[3], 45);
	return(result);
}


std::uint32_t Random::Below(std::uint32_t limit)
{
	if (limit == 0) {
		return(0);
	}
	return((std::uint32_t)((Next() >> 32) % limit));
}


Bytes From_Hex(char const * text)
{
	Bytes bytes;
	int pending = -1;
	for (; *text != '\0'; text++) {
		int digit;
		if (*text >= '0' && *text <= '9') {
			digit = *text - '0';
		} else if (*text >= 'a' && *text <= 'f') {
			digit = *text - 'a' + 10;
		} else if (*text >= 'A' && *text <= 'F') {
			digit = *text - 'A' + 10;
		} else {
			continue;
		}
		if (pending < 0) {
			pending = digit;
		} else {
			bytes.push_back((std::uint8_t)(pending * 16 + digit));
			pending = -1;
		}
	}
	return(bytes);
}


Bytes From_String(std::string const & text)
{
	return(Bytes(text.begin(), text.end()));
}


void Put_U16(Bytes & bytes, std::uint32_t value)
{
	bytes.push_back((std::uint8_t)value);
	bytes.push_back((std::uint8_t)(value >> 8));
}


void Put_U32(Bytes & bytes, std::uint32_t value)
{
	Put_U16(bytes, value);
	Put_U16(bytes, value >> 16);
}


void Require(bool condition, char const * message)
{
	if (!condition) {
		throw Violation{message};
	}
}


Bytes Mutate(Random & random, Bytes const & input, std::vector<Input> const & corpus,
	std::vector<std::string> const & dictionary, std::size_t max_size)
{
	Bytes bytes = input;
	int const edits = 1 + (int)random.Below(4);

	for (int edit = 0; edit < edits; edit++) {
		std::size_t const size = bytes.size();
		std::uint32_t const kind = random.Below(15);

		switch (kind) {
			case 0:
				if (size > 0) {
					bytes[random.Below((std::uint32_t)size)] ^= (std::uint8_t)(1u << random.Below(8));
				}
				break;

			case 1:
				if (size > 0) {
					bytes[random.Below((std::uint32_t)size)] = (std::uint8_t)random.Below(256);
				}
				break;

			case 2:
				if (size > 0) {
					bytes[random.Below((std::uint32_t)size)] = (std::uint8_t)Interesting_32[random.Below(INTERESTING_COUNT)];
				}
				break;

			case 3: {
				// Overwrite two or four bytes with a value that sits on a boundary.
				std::size_t const width = random.Chance(2) ? 2 : 4;
				if (size >= width) {
					std::size_t const at = random.Below((std::uint32_t)(size - width + 1));
					Store_Little(bytes, at, Interesting_32[random.Below(INTERESTING_COUNT)], width);
				}
				break;
			}

			case 4: {
				// Add or subtract a small amount from a 1, 2 or 4 byte value.
				std::size_t const width = std::size_t(1) << random.Below(3);
				if (size >= width) {
					std::size_t const at = random.Below((std::uint32_t)(size - width + 1));
					std::uint32_t const delta = 1 + random.Below(35);
					std::uint32_t const value = Load_Little(bytes, at, width);
					Store_Little(bytes, at, random.Chance(2) ? value + delta : value - delta, width);
				}
				break;
			}

			case 5: {
				// Insert random bytes.
				std::size_t const at = random.Below((std::uint32_t)(size + 1));
				std::size_t const count = 1 + random.Below(8);
				Bytes fill(count);
				for (std::size_t index = 0; index < count; index++) {
					fill[index] = (std::uint8_t)random.Below(256);
				}
				bytes.insert(bytes.begin() + (std::ptrdiff_t)at, fill.begin(), fill.end());
				break;
			}

			case 6:
				// Delete a range.
				if (size > 0) {
					std::size_t const at = random.Below((std::uint32_t)size);
					std::size_t const count = 1 + random.Below((std::uint32_t)std::min<std::size_t>(size - at, 16));
					bytes.erase(bytes.begin() + (std::ptrdiff_t)at, bytes.begin() + (std::ptrdiff_t)(at + count));
				}
				break;

			case 7:
				// Repeat a range in place.
				if (size > 0) {
					std::size_t const at = random.Below((std::uint32_t)size);
					std::size_t const count = 1 + random.Below((std::uint32_t)std::min<std::size_t>(size - at, 32));
					Bytes copy(bytes.begin() + (std::ptrdiff_t)at, bytes.begin() + (std::ptrdiff_t)(at + count));
					bytes.insert(bytes.begin() + (std::ptrdiff_t)at, copy.begin(), copy.end());
				}
				break;

			case 8:
				// Cut the input short.
				if (size > 0) {
					bytes.resize(random.Below((std::uint32_t)size));
				}
				break;

			case 9:
				// Copy a range of another corpus input over this one.
				if (!corpus.empty() && size > 0) {
					Bytes const & other = corpus[random.Below((std::uint32_t)corpus.size())].Data;
					if (!other.empty()) {
						std::size_t const from = random.Below((std::uint32_t)other.size());
						std::size_t const count = 1 + random.Below((std::uint32_t)std::min<std::size_t>(other.size() - from, 64));
						std::size_t const at = random.Below((std::uint32_t)size);
						for (std::size_t index = 0; index < count && at + index < bytes.size(); index++) {
							bytes[at + index] = other[from + index];
						}
					}
				}
				break;

			case 10:
				// Fill a range with one byte.
				if (size > 0) {
					std::size_t const at = random.Below((std::uint32_t)size);
					std::size_t const count = 1 + random.Below((std::uint32_t)std::min<std::size_t>(size - at, 64));
					std::uint8_t const value = random.Chance(2) ? (std::uint8_t)random.Below(256)
						: (std::uint8_t)Interesting_32[random.Below(INTERESTING_COUNT)];
					std::fill(bytes.begin() + (std::ptrdiff_t)at, bytes.begin() + (std::ptrdiff_t)(at + count), value);
				}
				break;

			case 11:
				// Swap two bytes.
				if (size > 1) {
					std::swap(bytes[random.Below((std::uint32_t)size)], bytes[random.Below((std::uint32_t)size)]);
				}
				break;

			case 12:
			case 13:
				// Insert a dictionary token, or overwrite the bytes at a position with one.
				if (!dictionary.empty()) {
					std::string const & token = dictionary[random.Below((std::uint32_t)dictionary.size())];
					std::size_t const at = random.Below((std::uint32_t)(size + 1));
					if (kind == 12 || at >= size) {
						bytes.insert(bytes.begin() + (std::ptrdiff_t)at, token.begin(), token.end());
					} else {
						for (std::size_t index = 0; index < token.size() && at + index < bytes.size(); index++) {
							bytes[at + index] = (std::uint8_t)token[index];
						}
					}
				}
				break;

			default:
				// Grow the input with zero bytes at the end.
				bytes.resize(size + 1 + random.Below(64));
				break;
		}
	}

	if (bytes.size() > max_size) {
		bytes.resize(max_size);
	}
	return(bytes);
}


namespace
{

int Failures = 0;
bool KeepGoing = false;


// Runs one input and returns whether the parser survived it and kept its invariants.
bool Run_One(Target const & target, std::string const & name, Bytes const & input)
{
	CurrentInput = &input;
	CurrentName = name;
	OversizeRequest = 0;

	char const * failure = nullptr;
	std::string detail;

	StartedAt = Now_Milliseconds();
	Running = true;
	try {
		target.Run(input);
	} catch (Violation const & violation) {
		failure = "an invariant failed";
		detail = violation.Message;
	} catch (std::bad_alloc const &) {
		failure = "an allocation failed";
		detail = OversizeRequest != 0
			? "the parser asked for " + std::to_string(OversizeRequest.load()) + " bytes at once"
			: "the parser ran out of memory";
	} catch (std::exception const & error) {
		failure = "an exception left the parser";
		detail = error.what();
	} catch (...) {
		failure = "an exception left the parser";
		detail = "unknown exception type";
	}
	Running = false;

	if (failure == nullptr) {
		return(true);
	}

	Failures++;
	std::fprintf(stderr, "fuzz %s: %s: %s\n  input: %s (%zu bytes)\n", target.Name, failure,
		detail.c_str(), name.c_str(), input.size());
	Print_Hex(input);
	return(false);
}


bool Read_File(char const * path, Bytes & bytes)
{
	std::FILE * file = std::fopen(path, "rb");
	if (file == nullptr) {
		return(false);
	}
	std::uint8_t chunk[4096];
	std::size_t got;
	while ((got = std::fread(chunk, 1, sizeof(chunk), file)) > 0) {
		bytes.insert(bytes.end(), chunk, chunk + got);
	}
	std::fclose(file);
	return(true);
}

}

}


int main(int argc, char ** argv)
{
	using namespace Fuzz;

	Target const & target = Get_Target();
	CurrentTarget = target.Name;

	long iterations = target.Iterations;
	std::uint64_t seed = DEFAULT_SEED;
	char const * file_argument = nullptr;
	long dump_iteration = -1;

	if (char const * text = std::getenv("FUZZ_ITERATIONS")) {
		iterations = std::strtol(text, nullptr, 10);
	}
	if (char const * text = std::getenv("FUZZ_SEED")) {
		seed = std::strtoull(text, nullptr, 0);
	}
	if (char const * text = std::getenv("FUZZ_ITERATION_LIMIT_MS")) {
		LimitMilliseconds = std::strtoll(text, nullptr, 10);
	}

	for (int index = 1; index < argc; index++) {
		std::string const option = argv[index];
		bool const has_value = index + 1 < argc;
		if (option == "--iterations" && has_value) {
			iterations = std::strtol(argv[++index], nullptr, 10);
		} else if (option == "--seed" && has_value) {
			seed = std::strtoull(argv[++index], nullptr, 0);
		} else if (option == "--file" && has_value) {
			file_argument = argv[++index];
		} else if (option == "--dump-iteration" && has_value) {
			dump_iteration = std::strtol(argv[++index], nullptr, 10);
		} else if (option == "--keep-going") {
			KeepGoing = true;
		} else {
			std::fprintf(stderr, "usage: %s [--iterations N] [--seed S] [--file PATH] [--dump-iteration N] [--keep-going]\n", argv[0]);
			return(64);
		}
	}

	Install_Reporters();

	if (file_argument != nullptr) {
		Bytes input;
		if (!Read_File(file_argument, input)) {
			std::fprintf(stderr, "fuzz %s: cannot read %s\n", target.Name, file_argument);
			return(66);
		}
		bool const survived = Run_One(target, file_argument, input);
		std::printf("fuzz %s: %s\n", target.Name, survived ? "the input ran cleanly" : "the input failed");
		return(survived ? 0 : 1);
	}

	std::vector<Input> const seeds = target.Seeds != nullptr ? target.Seeds() : std::vector<Input>();
	std::vector<Input> const regressions = target.Regressions != nullptr ? target.Regressions() : std::vector<Input>();
	std::vector<std::string> const dictionary = target.Dictionary != nullptr ? target.Dictionary() : std::vector<std::string>();

	std::vector<Input> corpus = seeds;
	corpus.insert(corpus.end(), regressions.begin(), regressions.end());
	if (corpus.empty()) {
		std::fprintf(stderr, "fuzz %s: the target has no seeds\n", target.Name);
		return(70);
	}

	Random random(seed ^ Hash_Name(target.Name));

	if (dump_iteration >= 0) {
		for (long iteration = 0; iteration <= dump_iteration; iteration++) {
			Bytes const & base = corpus[random.Below((std::uint32_t)corpus.size())].Data;
			Bytes const input = Mutate(random, base, corpus, dictionary, target.MaxSize);
			if (iteration == dump_iteration) {
				Print_Hex(input);
			}
		}
		return(0);
	}

	auto const started = std::chrono::steady_clock::now();

	for (Input const & regression : regressions) {
		if (!Run_One(target, "regression " + regression.Name, regression.Data) && !KeepGoing) {
			return(1);
		}
	}
	for (Input const & item : seeds) {
		if (!Run_One(target, "seed " + item.Name, item.Data) && !KeepGoing) {
			return(1);
		}
	}

	for (long iteration = 0; iteration < iterations; iteration++) {
		Input const & base = corpus[random.Below((std::uint32_t)corpus.size())];
		Bytes const input = Mutate(random, base.Data, corpus, dictionary, target.MaxSize);
		if (!Run_One(target, "iteration " + std::to_string(iteration) + " from " + base.Name, input)) {
			if (!KeepGoing) {
				return(1);
			}
			if (Failures >= 20) {
				break;
			}
		}
	}

	double const seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - started).count();
	std::printf("fuzz %s: %zu seeds, %zu regressions, %ld mutated inputs, seed 0x%llx: %s (%.1f s)\n",
		target.Name, seeds.size(), regressions.size(), iterations, (unsigned long long)seed,
		Failures == 0 ? "ok" : "FAILED", seconds);
	return(Failures == 0 ? 0 : 1);
}
