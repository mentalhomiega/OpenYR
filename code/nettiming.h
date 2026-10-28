/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#pragma once

#include "nettime.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>


namespace NetTiming
{
	constexpr Milliseconds MINIMUM_RTO = 100;
	// Above any round trip a private link is expected to carry, so a slow link's first retry
	// does not precede its acknowledgement.
	constexpr Milliseconds MAXIMUM_RTO = 4000;
	constexpr Milliseconds MINIMUM_CONNECTION_TIMEOUT = 2000;
	constexpr Milliseconds MAXIMUM_CONNECTION_TIMEOUT = 30000;
	constexpr Milliseconds MAXIMUM_PROCESS_MILLISECONDS = 1000;
	constexpr Milliseconds MAXIMUM_REPORTED_RTT = UINT16_MAX - 1u;

	constexpr unsigned int MAX_TIMING_PLAYERS = 8;
	constexpr unsigned int MINIMUM_TIMING_RUNG = 1;
	constexpr unsigned int MAXIMUM_TIMING_RUNG = 10;
	constexpr unsigned int INITIAL_TIMING_RUNG = 2;
	constexpr unsigned int BOOTSTRAP_FALLBACK_RUNG = 3;
	constexpr unsigned int MAXIMUM_MAX_AHEAD = 250;

	constexpr std::uint32_t BOOTSTRAP_REPORT_INTERVAL = 32;
	constexpr std::uint32_t BOOTSTRAP_FIRST_EVALUATION = 64;
	constexpr std::uint32_t BOOTSTRAP_FINAL_EVALUATION = 128;
	constexpr std::uint32_t REPORT_INTERVAL = 128;
	constexpr std::uint32_t EVALUATION_INTERVAL = 256;
	constexpr std::uint32_t CHANGE_COOLDOWN = 256;
	constexpr std::uint32_t REPORT_EXPIRY = 512;
	constexpr unsigned int GOOD_EVALUATIONS_REQUIRED = 3;
	constexpr unsigned int DESCENT_EVALUATIONS_REQUIRED = 1;
	// Longest single wait that still allows a step down.
	constexpr Milliseconds STALL_IMPROVE_MILLISECONDS = 100;

	struct RetryDecision
	{
		bool Send = false;
		bool TimedOut = false;
	};

	struct RetransmitState
	{
		Milliseconds FirstSend = 0;
		Milliseconds LastSend = 0;
		Milliseconds CapturedRto = MINIMUM_RTO;
		unsigned int TransmissionCount = 0;
	};

	class RttEstimator
	{
		public:
			void Reset(void);
			bool Add_Sample(Milliseconds round_trip, bool retransmitted = false);
			bool Acknowledge(Milliseconds sent_at, unsigned int transmission_count, MillisecondClock const & clock = Default_Clock());
			void Note_Retransmit(Milliseconds captured_rto);

			bool Has_Sample(void) const {return(Initialized);}
			bool Is_Provisional(void) const {return(Provisional);}
			Milliseconds Smoothed_Rtt(void) const {return(SmoothedRtt);}
			Milliseconds Rtt_Variation(void) const {return(RttVariation);}
			Milliseconds Retransmit_Timeout(void) const {return(RetransmitTimeout);}

		private:
			bool Initialized = false;
			Milliseconds SmoothedRtt = 0;
			Milliseconds RttVariation = 0;
			Milliseconds RetransmitTimeout = MINIMUM_RTO;
			// Set while the estimate comes from an ambiguous first acknowledgement.
			bool Provisional = false;
	};

	Milliseconds Connection_Timeout(Milliseconds smoothed_rtt, Milliseconds retransmit_timeout);
	Milliseconds Initial_Retry_Timeout(Milliseconds retransmit_timeout, Milliseconds connection_timeout);
	Milliseconds Retransmit_Delay(Milliseconds base_rto, unsigned int prior_retransmissions, Milliseconds maximum_delay = MAXIMUM_RTO);
	bool Retransmit_Is_Due(Milliseconds last_send, Milliseconds now, Milliseconds base_rto,
		unsigned int prior_retransmissions, Milliseconds maximum_delay = MAXIMUM_RTO);
	RetryDecision Evaluate_Retry(RetransmitState const & state, Milliseconds now, Milliseconds current_rto, Milliseconds connection_timeout,
		bool timeout_enabled, bool adaptive);

	struct TimingSettings {
		unsigned int FrameSendRate = INITIAL_TIMING_RUNG;
		unsigned int MaxAhead = 3 * INITIAL_TIMING_RUNG;

		bool operator==(TimingSettings const &) const = default;
	};

	enum class ConnectionQuality : unsigned char {
		Bad,
		Poor,
		Normal,
		Fast,
	};

	TimingSettings Settings_For_Rung(unsigned int rung);
	ConnectionQuality Connection_Quality_For_Settings(TimingSettings settings);
	bool Timing_Settings_Are_Valid(TimingSettings settings);
	bool Timing_Transition_Source_Is_Valid(TimingSettings settings);
	std::optional<unsigned int> Align_Max_Ahead(unsigned int required, unsigned int frame_send_rate);
	TimingSettings Select_Timing_Settings(Milliseconds worst_round_trip, unsigned int target_fps, bool require_headroom = false);
	bool Report_Is_Due(std::uint32_t elapsed_frames);
	bool Evaluation_Is_Due(std::uint32_t elapsed_frames);

	struct TimingCensus {
		unsigned int ActivePlayers = 0;
		unsigned int FreshProcessReports = 0;
		unsigned int FreshRoundTripReports = 0;
		Milliseconds WorstProcessMilliseconds = 0;
		Milliseconds WorstRoundTrip = 0;
		Milliseconds WorstStallMilliseconds = 0;
		bool ProcessComplete = true;
		bool RoundTripComplete = true;
		bool RequiresConservativeTiming = false;
	};

	class TimingReportCensus
	{
		public:
			void Reset(void);
			bool Set_Player_Active(unsigned int player, bool active, std::uint32_t frame);
			bool Is_Player_Active(unsigned int player) const;
			bool Record_Report(unsigned int player, Milliseconds process_milliseconds, std::optional<Milliseconds> round_trip, std::uint32_t frame,
				Milliseconds stall_milliseconds = 0);
			TimingCensus Inspect(std::uint32_t frame) const;

		private:
			struct PlayerReport {
				bool Active = false;
				bool HasReport = false;
				bool HasRoundTrip = false;
				bool EverHadRoundTrip = false;
				Milliseconds ProcessMilliseconds = 0;
				Milliseconds RoundTrip = 0;
				Milliseconds StallMilliseconds = 0;
				std::uint32_t ActiveSinceFrame = 0;
				std::uint32_t ReportFrame = 0;
			};

			std::array<PlayerReport, MAX_TIMING_PLAYERS> Reports = {};
	};

	unsigned int Game_Speed_Frame_Rate(int game_speed);
	unsigned int Solo_Game_Speed_Frame_Rate(int game_speed);

	class FramePacer
	{
		public:
			Milliseconds Next_Wait(unsigned int rate);

		private:
			unsigned int Rate = 0;
			// The part of a millisecond each frame is owed, counted in 1/Rate of a millisecond.
			unsigned int Leftover = 0;
	};
	unsigned int Select_Desired_Frame_Rate(TimingCensus const & census, unsigned int synchronized_fps, unsigned int game_speed_fps);

	// gamemd's online timing, which the default policy follows. Response times are in ticks of
	// 1/60 second, as gamemd's IPX response time reports them.
	constexpr unsigned int NETWORK_TICKS_PER_SECOND = 60;
	constexpr unsigned int GAMEMD_EVALUATION_INTERVAL = 256;
	constexpr unsigned int GAMEMD_LATENCY_STAGE = 1;
	constexpr unsigned int GAMEMD_PACING_MIN_MAX_AHEAD = 31;
	constexpr Milliseconds GAMEMD_PACING_STEP = 10;

	// Frame-send period for a frame rate: 10 at 31 frames per second and up, otherwise 5.
	unsigned int Gamemd_Frame_Send_Rate(unsigned int frame_rate);
	// Frame-send period and look-ahead at game start, from the game speed. A tunnelled match adds 20.
	TimingSettings Gamemd_Start_Settings(int game_speed, bool through_tunnel);
	// Look-ahead for a response time in ticks; the current look-ahead is the rise limit.
	unsigned int Gamemd_Max_Ahead(unsigned int response_ticks, unsigned int frame_rate, unsigned int frame_send_rate,
		unsigned int current_max_ahead, unsigned int latency_stage = GAMEMD_LATENCY_STAGE);
	// The settings a master sends for the worst round trip at the desired frame rate.
	TimingSettings Gamemd_Timing_Settings(Milliseconds worst_round_trip, unsigned int frame_rate, unsigned int current_max_ahead);
	// What a host asked for with the lobby's send period and look-ahead commands (or a launch file's
	// FrameSendRate and MaxAhead keys). Zero leaves that value to the timing policy. The host's choice
	// holds for the whole match: it sets the start values and every later evaluation keeps to it.
	struct HostTimingChoice {
		unsigned int FrameSendRate = 0;
		unsigned int MaxAhead = 0;

		bool Is_Set(void) const {return(FrameSendRate != 0 || MaxAhead != 0);}
		bool operator==(HostTimingChoice const &) const = default;
	};

	// The send periods the CnCNet client writes when its host never chose one: 2 is the client's forced option and 7 is
	// the default its host commands show. A launch file with one of these and no MaxAhead is read as no choice, so a
	// stock client's games keep gamemd's timing. A launch file that sets HostTiming=yes is taken literally.
	constexpr bool Is_CnCNet_Default_Frame_Send_Rate(unsigned int frame_send_rate)
	{
		return(frame_send_rate == 2 || frame_send_rate == 7);
	}

	// A chosen send period is 1 to 10 frames. A chosen look-ahead is 0 (left to the policy) or 4 to 250 frames.
	constexpr bool Host_Frame_Send_Rate_Is_Valid(unsigned int frame_send_rate)
	{
		return(frame_send_rate >= MINIMUM_TIMING_RUNG && frame_send_rate <= MAXIMUM_TIMING_RUNG);
	}
	constexpr bool Host_Max_Ahead_Is_Valid(unsigned int max_ahead)
	{
		return(max_ahead == 0 || (max_ahead >= 4 && max_ahead <= MAXIMUM_MAX_AHEAD));
	}
	// The look-ahead a send period can carry: a whole number of send periods, at least the smallest the game accepts
	// and at most 250.
	unsigned int Host_Max_Ahead_For(unsigned int max_ahead, unsigned int frame_send_rate);
	// Puts the host's choice over settings a policy produced. Returns settings when nothing is chosen.
	TimingSettings Apply_Host_Choice(TimingSettings settings, HostTimingChoice choice);
	// Everything a host chooses about the network of a match: the timing above and the protocol. Protocol 2 sends
	// orders in batches every send period and lets the master adjust the timing; protocol 0 sends every frame and
	// keeps the look-ahead it starts with. 2 is the default.
	struct HostNetworkOptions {
		HostTimingChoice Timing;
		int Protocol = 2;

		bool Is_Default(void) const {return(!Timing.Is_Set() && Protocol == 2);}
		bool operator==(HostNetworkOptions const &) const = default;
	};

	constexpr bool Host_Protocol_Is_Valid(int protocol)
	{
		return(protocol == 0 || protocol == 2);
	}

	struct HostCommandResult {
		bool Recognised = false;    // The line was one of the host's network commands.
		bool Changed = false;       // A value changed.
		std::string Message;        // What to tell the host, in plain words.
	};

	// Runs a host's lobby chat line such as "/framesendrate 6", "/maxahead 40" or "/protocolversion 0" on the options.
	// A command with no value shows the current one. "auto" (or 0) leaves a timing value to the policy.
	HostCommandResult Apply_Host_Command(std::string_view line, HostNetworkOptions & options);
	// One line that tells a player the options, as "send period 6 frames, look-ahead automatic, protocol 2".
	std::string Describe_Host_Options(HostNetworkOptions const & options);
	// The options in one word for a packet, 0 when they are the default. Unpack returns nothing for a word that
	// holds a value out of range.
	std::uint32_t Pack_Host_Options(HostNetworkOptions const & options);
	std::optional<HostNetworkOptions> Unpack_Host_Options(std::uint32_t packed);
	// gamemd's settings for the worst round trip, with the host's choice kept.
	TimingSettings Gamemd_Timing_Settings(Milliseconds worst_round_trip, unsigned int frame_rate, unsigned int current_max_ahead,
		HostTimingChoice choice);
	// Whole milliseconds a frame is armed for. gamemd truncates 1000 / rate and does not spread the remainder.
	Milliseconds Gamemd_Frame_Wait(unsigned int rate);
	// Extra milliseconds a frame waits for a peer that lags by lag_frames, at look-ahead 31 and up.
	Milliseconds Gamemd_Local_Pacing(unsigned int max_ahead, unsigned int lag_frames);
	// Round trip in whole ticks, as gamemd's response time counts it.
	unsigned int Round_Trip_Ticks(Milliseconds round_trip);

	struct TimingEvaluation {
		TimingSettings Settings;
		unsigned int Rung = INITIAL_TIMING_RUNG;
		bool Evaluated = false;
		bool Changed = false;
	};

	class BalancedTimingPolicy
	{
		public:
			void Reset(std::uint32_t frame = 0);
			void Reset_From(TimingSettings settings, std::uint32_t frame);
			TimingEvaluation Evaluate(TimingCensus const & census, unsigned int target_fps, std::uint32_t frame);

			unsigned int Current_Rung(void) const {return(CurrentRung);}
			TimingSettings Current_Settings(void) const {return(CurrentSettings);}
			unsigned int Good_Evaluations(void) const {return(GoodEvaluations);}
			bool Is_Bootstrapping(void) const {return(Bootstrapping);}
			std::uint32_t Cadence_Origin(void) const {return(BootstrapStartFrame);}

		private:
			void Change_To(TimingSettings settings, std::uint32_t frame);
			void Finish_Bootstrap(void);

			unsigned int CurrentRung = INITIAL_TIMING_RUNG;
			TimingSettings CurrentSettings = {INITIAL_TIMING_RUNG, 3 * INITIAL_TIMING_RUNG};
			unsigned int GoodEvaluations = 0;
			std::uint32_t BootstrapStartFrame = 0;
			std::uint32_t LastEvaluationFrame = 0;
			std::uint32_t LastChangeFrame = 0;
			bool HasEvaluated = false;
			bool HasChanged = false;
			bool Bootstrapping = true;
			// Set by an improvement; while it holds, each evaluation with headroom steps one more rung.
			bool ImprovementStreak = false;
	};

	struct StagedTimingUpdate {
		TimingSettings Settings;
		unsigned int InitialMaxAhead = 0;
		std::uint32_t ActivationFrame = 0;
		bool Deferred = false;
	};

	struct TimingTransitionState {
		StagedTimingUpdate Plan;
		std::uint32_t LastStepFrame = 0;
		bool Activated = false;
	};

	struct TimingTransitionAdvance {
		TimingSettings Settings;
		bool Changed = false;
		bool Complete = false;
	};

	enum class ScheduleResult
	{
		Rejected,
		Applied,
		Staged,
	};

	std::optional<StagedTimingUpdate> Stage_Timing_Update(TimingSettings current, TimingSettings requested, std::uint32_t event_frame);
	std::optional<std::uint32_t> Next_Send_Boundary(std::uint32_t frame, unsigned int frame_send_rate);
	std::optional<unsigned int> Next_Transition_Max_Ahead(TimingSettings current, TimingSettings requested);
	std::optional<TimingTransitionAdvance> Advance_Timing_Transition(TimingTransitionState & transition, TimingSettings current, std::uint32_t frame);
	bool Timing_Update_Is_Due(std::uint32_t frame, std::uint32_t activation_frame);
	bool Event_Is_Due(int event_frame, bool is_executed, int frame);
}
