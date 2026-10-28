/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/


#include "nettiming.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <numeric>


namespace NetTiming
{
	namespace
	{
		constexpr std::uint64_t Divide_Round_Up(std::uint64_t numerator, std::uint64_t denominator)
		{
			return((numerator + denominator - 1) / denominator);
		}


		constexpr Milliseconds Clamp_Rto(std::uint64_t value)
		{
			return(static_cast<Milliseconds>(std::clamp<std::uint64_t>(value, MINIMUM_RTO, MAXIMUM_RTO)));
		}


		TimingSettings Desired_Settings(TimingCensus const & census, unsigned int target_fps, bool require_headroom)
		{
			if (census.RequiresConservativeTiming) {
				return(TimingSettings{MAXIMUM_TIMING_RUNG, MAXIMUM_MAX_AHEAD});
			}
			if (census.ActivePlayers == 0) {
				return(Settings_For_Rung(INITIAL_TIMING_RUNG));
			}
			return(Select_Timing_Settings(census.WorstRoundTrip, target_fps, require_headroom));
		}


		/// <summary>Checks whether settings increase the scheduling horizon.</summary>
		bool Timing_Is_Worse(TimingSettings candidate, TimingSettings current)
		{
			return(candidate.FrameSendRate > current.FrameSendRate
				|| (candidate.FrameSendRate == current.FrameSendRate && candidate.MaxAhead > current.MaxAhead));
		}


		/// <summary>Checks whether settings reduce the scheduling horizon.</summary>
		bool Timing_Is_Better(TimingSettings candidate, TimingSettings current)
		{
			return(candidate.FrameSendRate < current.FrameSendRate
				|| (candidate.FrameSendRate == current.FrameSendRate && candidate.MaxAhead < current.MaxAhead));
		}
	}


	void RttEstimator::Reset(void)
	{
		Initialized = false;
		SmoothedRtt = 0;
		RttVariation = 0;
		RetransmitTimeout = MINIMUM_RTO;
		Provisional = false;
	}


	/// <summary>Updates SRTT, RTTVAR, and RTO from an eligible sample.</summary>
	bool RttEstimator::Add_Sample(Milliseconds round_trip, bool retransmitted)
	{
		// Karn's rule excludes ambiguous acknowledgements after retransmission.
		if (retransmitted) {
			return(false);
		}

		if (!Initialized) {
			Initialized = true;
			SmoothedRtt = round_trip;
			RttVariation = (round_trip + 1) / 2;
		} else {
			Milliseconds const error = SmoothedRtt > round_trip ? SmoothedRtt - round_trip : round_trip - SmoothedRtt;
			RttVariation = static_cast<Milliseconds>((3ull * RttVariation + error + 2) / 4);
			SmoothedRtt = static_cast<Milliseconds>((7ull * SmoothedRtt + round_trip + 4) / 8);
		}

		std::uint64_t const variation = std::max<std::uint64_t>(1, 4ull * RttVariation);
		RetransmitTimeout = Clamp_Rto(static_cast<std::uint64_t>(SmoothedRtt) + variation);
		return(true);
	}


	/// <summary>Samples an acknowledgement; an unmeasured link takes an ambiguous one as a provisional upper bound.</summary>
	bool RttEstimator::Acknowledge(Milliseconds sent_at, unsigned int transmission_count, MillisecondClock const & clock)
	{
		if (transmission_count == 0) {
			return(false);
		}

		Milliseconds const elapsed = Elapsed_Milliseconds(sent_at, clock.Now());
		if (transmission_count != 1) {
			if (Initialized) {
				return(false);
			}
			Provisional = Add_Sample(elapsed);
			return(Provisional);
		}

		// The first clean sample replaces a provisional seed instead of blending with it.
		if (Provisional) {
			Initialized = false;
			Provisional = false;
		}
		return(Add_Sample(elapsed));
	}


	/// <summary>Doubles the timeout once for each round of retransmissions, so a slower link stays measurable.</summary>
	void RttEstimator::Note_Retransmit(Milliseconds captured_rto)
	{
		if (!Initialized) {
			return;
		}

		// Only a packet sent under the current timeout proves that timeout too short.
		if (captured_rto >= RetransmitTimeout) {
			RetransmitTimeout = Clamp_Rto(2ull * RetransmitTimeout);
		}
	}


	/// <summary>Derives a timeout that covers measured latency and three transmissions at the current RTO.</summary>
	Milliseconds Connection_Timeout(Milliseconds smoothed_rtt, Milliseconds retransmit_timeout)
	{
		std::uint64_t const timeout = std::max(8ull * smoothed_rtt + 250, 4ull * retransmit_timeout);
		return(static_cast<Milliseconds>(std::clamp<std::uint64_t>(timeout, MINIMUM_CONNECTION_TIMEOUT, MAXIMUM_CONNECTION_TIMEOUT)));
	}


	/// <summary>Bounds a packet's first retry so the connection timeout allows at least three transmissions.</summary>
	Milliseconds Initial_Retry_Timeout(Milliseconds retransmit_timeout, Milliseconds connection_timeout)
	{
		return(std::max(MINIMUM_RTO, std::min(retransmit_timeout, connection_timeout / 4)));
	}


	/// <summary>Applies bounded exponential backoff to a packet's RTO.</summary>
	Milliseconds Retransmit_Delay(Milliseconds base_rto, unsigned int prior_retransmissions, Milliseconds maximum_delay)
	{
		maximum_delay = std::max(maximum_delay, MINIMUM_RTO);
		std::uint64_t delay = std::clamp(base_rto, MINIMUM_RTO, maximum_delay);
		while (prior_retransmissions-- > 0 && delay < maximum_delay) {
			delay = std::min<std::uint64_t>(delay * 2, maximum_delay);
		}
		return(static_cast<Milliseconds>(delay));
	}


	/// <summary>Checks whether a packet's current backoff interval has elapsed.</summary>
	bool Retransmit_Is_Due(Milliseconds last_send, Milliseconds now, Milliseconds base_rto, unsigned int prior_retransmissions, Milliseconds maximum_delay)
	{
		return(Milliseconds_Have_Elapsed(last_send, now, Retransmit_Delay(base_rto, prior_retransmissions, maximum_delay)));
	}


	/// <summary>Chooses the next action for one queued packet; a timed-out packet still retries at its capped backoff.</summary>
	RetryDecision Evaluate_Retry(RetransmitState const & state, Milliseconds now, Milliseconds current_rto, Milliseconds connection_timeout,
		bool timeout_enabled, bool adaptive)
	{
		if (state.TransmissionCount == 0) {
			return(RetryDecision{true, false});
		}

		RetryDecision decision;
		decision.TimedOut = timeout_enabled && Milliseconds_Have_Elapsed(state.FirstSend, now, connection_timeout);
		decision.Send = adaptive
			? Retransmit_Is_Due(state.LastSend, now, state.CapturedRto, state.TransmissionCount - 1, connection_timeout)
			: Milliseconds_Have_Elapsed(state.LastSend, now, current_rto);
		return(decision);
	}


	TimingSettings Settings_For_Rung(unsigned int rung)
	{
		rung = std::clamp(rung, MINIMUM_TIMING_RUNG, MAXIMUM_TIMING_RUNG);
		return(TimingSettings{rung, rung == 1 ? 4u : 3u * rung});
	}


	ConnectionQuality Connection_Quality_For_Settings(TimingSettings settings)
	{
		if (!Timing_Settings_Are_Valid(settings) || settings.MaxAhead > Settings_For_Rung(settings.FrameSendRate).MaxAhead) {
			return(ConnectionQuality::Bad);
		}
		if (settings.FrameSendRate <= 2) {
			return(ConnectionQuality::Fast);
		}
		if (settings.FrameSendRate <= 5) {
			return(ConnectionQuality::Normal);
		}
		if (settings.FrameSendRate <= 8) {
			return(ConnectionQuality::Poor);
		}
		return(ConnectionQuality::Bad);
	}


	/// <summary>Checks timing bounds and send-period alignment.</summary>
	bool Timing_Settings_Are_Valid(TimingSettings settings)
	{
		TimingSettings const minimum = Settings_For_Rung(settings.FrameSendRate);
		return(settings.FrameSendRate >= MINIMUM_TIMING_RUNG && settings.FrameSendRate <= MAXIMUM_TIMING_RUNG
			&& settings.MaxAhead >= minimum.MaxAhead && settings.MaxAhead <= MAXIMUM_MAX_AHEAD && settings.MaxAhead % settings.FrameSendRate == 0);
	}


	/// <summary>Accepts a legacy aligned horizon as the source of a safe transition.</summary>
	bool Timing_Transition_Source_Is_Valid(TimingSettings settings)
	{
		return(settings.FrameSendRate >= MINIMUM_TIMING_RUNG && settings.FrameSendRate <= MAXIMUM_TIMING_RUNG
			&& settings.MaxAhead >= 2 * settings.FrameSendRate && settings.MaxAhead <= MAXIMUM_MAX_AHEAD
			&& settings.MaxAhead % settings.FrameSendRate == 0);
	}


	/// <summary>Rounds a scheduling horizon up to a complete send period.</summary>
	std::optional<unsigned int> Align_Max_Ahead(unsigned int required, unsigned int frame_send_rate)
	{
		if (frame_send_rate == 0) {
			return(std::nullopt);
		}

		std::uint64_t const aligned = Divide_Round_Up(required, frame_send_rate) * frame_send_rate;
		if (aligned > MAXIMUM_MAX_AHEAD) {
			return(std::nullopt);
		}
		return(static_cast<unsigned int>(aligned));
	}


	/// <summary>Chooses the lowest rung that covers the adjusted RTT.</summary>
	TimingSettings Select_Timing_Settings(Milliseconds worst_round_trip, unsigned int target_fps, bool require_headroom)
	{
		target_fps = std::clamp(target_fps, 1u, 60u);

		std::uint64_t adjusted = worst_round_trip;
		if (require_headroom) {
			adjusted = Divide_Round_Up(adjusted * 5, 4);
		}

		std::uint64_t const one_way_frames = Divide_Round_Up(adjusted * target_fps, 2000);
		// A rung must cover one-way flight time plus a complete send period.
		for (unsigned int rung = MINIMUM_TIMING_RUNG; rung < MAXIMUM_TIMING_RUNG; rung++) {
			TimingSettings const settings = Settings_For_Rung(rung);
			std::uint64_t const floor = 3ull * settings.FrameSendRate;
			std::uint64_t const needed = std::max(floor, one_way_frames + settings.FrameSendRate);
			if (needed > std::numeric_limits<unsigned int>::max()) {
				continue;
			}

			std::optional<unsigned int> const aligned = Align_Max_Ahead(static_cast<unsigned int>(needed), settings.FrameSendRate);
			if (aligned && *aligned <= settings.MaxAhead) {
				return(settings);
			}
		}

		TimingSettings settings = Settings_For_Rung(MAXIMUM_TIMING_RUNG);
		std::uint64_t const needed = std::max<std::uint64_t>(settings.MaxAhead, one_way_frames + settings.FrameSendRate);
		if (needed >= MAXIMUM_MAX_AHEAD) {
			settings.MaxAhead = MAXIMUM_MAX_AHEAD - (MAXIMUM_MAX_AHEAD % settings.FrameSendRate);
		} else {
			settings.MaxAhead = *Align_Max_Ahead(static_cast<unsigned int>(needed), settings.FrameSendRate);
		}
		return(settings);
	}


	/// <summary>Uses two early reports before settling on the normal cadence.</summary>
	bool Report_Is_Due(std::uint32_t elapsed_frames)
	{
		return(elapsed_frames > 0 && ((elapsed_frames <= BOOTSTRAP_FIRST_EVALUATION && elapsed_frames % BOOTSTRAP_REPORT_INTERVAL == 0)
			|| elapsed_frames % REPORT_INTERVAL == 0));
	}


	/// <summary>Schedules two bootstrap evaluations and the steady-state cadence.</summary>
	bool Evaluation_Is_Due(std::uint32_t elapsed_frames)
	{
		return(elapsed_frames == BOOTSTRAP_FIRST_EVALUATION || elapsed_frames == BOOTSTRAP_FINAL_EVALUATION
			|| (elapsed_frames > 0 && elapsed_frames % EVALUATION_INTERVAL == 0));
	}


	void TimingReportCensus::Reset(void)
	{
		Reports = {};
	}


	/// <summary>Adds or removes a player, discarding any report the slot held.</summary>
	bool TimingReportCensus::Set_Player_Active(unsigned int player, bool active, std::uint32_t frame)
	{
		if (player >= Reports.size()) {
			return(false);
		}

		PlayerReport & report = Reports[player];
		if (report.Active != active) {
			report = {};
			report.Active = active;
			report.ActiveSinceFrame = frame;
		}
		return(true);
	}


	bool TimingReportCensus::Is_Player_Active(unsigned int player) const
	{
		return(player < Reports.size() && Reports[player].Active);
	}


	/// <summary>Records process time, optional RTT, and longest wait as one report.</summary>
	bool TimingReportCensus::Record_Report(unsigned int player, Milliseconds process_milliseconds, std::optional<Milliseconds> round_trip, std::uint32_t frame,
		Milliseconds stall_milliseconds)
	{
		if (player >= Reports.size() || !Reports[player].Active || process_milliseconds > MAXIMUM_PROCESS_MILLISECONDS
			|| (round_trip && *round_trip > MAXIMUM_REPORTED_RTT)) {
			return(false);
		}

		PlayerReport & report = Reports[player];
		report.HasReport = true;
		report.HasRoundTrip = round_trip.has_value();
		report.EverHadRoundTrip |= round_trip.has_value();
		report.ProcessMilliseconds = process_milliseconds;
		report.RoundTrip = round_trip.value_or(0);
		report.StallMilliseconds = stall_milliseconds;
		report.ReportFrame = frame;
		return(true);
	}


	/// <summary>Summarizes fresh reports for a simulation frame.</summary>
	TimingCensus TimingReportCensus::Inspect(std::uint32_t frame) const
	{
		TimingCensus result;
		for (PlayerReport const & report : Reports) {
			if (!report.Active) {
				continue;
			}

			result.ActivePlayers++;
			bool const fresh = report.HasReport && frame - report.ReportFrame < REPORT_EXPIRY;
			if (fresh) {
				result.FreshProcessReports++;
				result.WorstProcessMilliseconds = std::max(result.WorstProcessMilliseconds, report.ProcessMilliseconds);
				result.WorstStallMilliseconds = std::max(result.WorstStallMilliseconds, report.StallMilliseconds);
			} else {
				result.ProcessComplete = false;
			}

			if (fresh && report.HasRoundTrip) {
				result.FreshRoundTripReports++;
				result.WorstRoundTrip = std::max(result.WorstRoundTrip, report.RoundTrip);
			} else {
				result.RoundTripComplete = false;
				// Only a link that has never been measured forces conservative timing; a measured
				// link holds the current timing until its next report.
				if (!report.EverHadRoundTrip && frame - report.ActiveSinceFrame >= REPORT_EXPIRY) {
					result.RequiresConservativeTiming = true;
				}
			}
		}
		return(result);
	}


	/// <summary>The network frame rate for a game speed, 0 fastest to 6 slowest; others get 60.</summary>
	unsigned int Game_Speed_Frame_Rate(int game_speed)
	{
		switch (game_speed) {
			case 0: return(60);
			case 1: return(45);
			case 2: return(30);
			case 3: return(20);
			case 4: return(15);
			case 5: return(12);
			case 6: return(10);
			default: return(60);
		}
	}


	/// <summary>The solo frame rate for a game speed, 0 fastest to 6 slowest, or zero for no limit.</summary>
	unsigned int Solo_Game_Speed_Frame_Rate(int game_speed)
	{
		switch (game_speed) {
			case 1: return(60);
			case 2: return(45);
			case 3: return(30);
			case 4: return(20);
			case 5: return(15);
			case 6: return(10);
			default: return(0);
		}
	}


	/// <summary>The whole milliseconds the next frame lasts so frames average the rate exactly.</summary>
	Milliseconds FramePacer::Next_Wait(unsigned int rate)
	{
		if (rate != Rate) {
			Rate = rate;
			Leftover = 0;
		}
		if (rate == 0) {
			return(0);
		}

		Milliseconds wait = 1000 / rate;
		Leftover += 1000 % rate;
		if (Leftover >= rate) {
			Leftover -= rate;
			wait++;
		}
		return(wait);
	}


	/// <summary>Uses fresh process reports without discarding the synchronized frame rate.</summary>
	unsigned int Select_Desired_Frame_Rate(TimingCensus const & census, unsigned int synchronized_fps, unsigned int game_speed_fps)
	{
		synchronized_fps = std::clamp(synchronized_fps, 1u, 60u);
		game_speed_fps = std::clamp(game_speed_fps, 1u, 60u);
		if (!census.ProcessComplete) {
			return(synchronized_fps);
		}

		unsigned int const process_fps = census.WorstProcessMilliseconds == 0 ? 60u
			: static_cast<unsigned int>(std::max<Milliseconds>(1, 1000 / census.WorstProcessMilliseconds));
		return(std::min(process_fps, game_speed_fps));
	}


	/// <summary>The frame-send period gamemd uses for a frame rate: 10 at 31 frames per second and up, else 5.</summary>
	unsigned int Gamemd_Frame_Send_Rate(unsigned int frame_rate)
	{
		return(frame_rate >= GAMEMD_PACING_MIN_MAX_AHEAD ? 10u : 5u);
	}


	/// <summary>Returns gamemd's send period and look-ahead for a game speed, from its game-start table.</summary>
	TimingSettings Gamemd_Start_Settings(int game_speed, bool through_tunnel)
	{
		unsigned int frame_send_rate = 5;
		unsigned int max_ahead = 20;
		switch (game_speed) {
			case 0:
			case 1:
				frame_send_rate = 10;
				max_ahead = 40;
				break;
			case 2:
				max_ahead = 30;
				break;
			case 3:
			case 4:
			case 5:
				break;
			default:
				max_ahead = 10;
				break;
		}
		if (through_tunnel) {
			max_ahead += 20;
		}
		return(TimingSettings{frame_send_rate, max_ahead});
	}


	/// <summary>Computes gamemd's look-ahead: a one-way flight in frames plus a stage add-on, rounded to send periods.</summary>
	unsigned int Gamemd_Max_Ahead(unsigned int response_ticks, unsigned int frame_rate, unsigned int frame_send_rate,
		unsigned int current_max_ahead, unsigned int latency_stage)
	{
		frame_send_rate = std::max(frame_send_rate, 1u);
		std::uint64_t response = response_ticks;
		std::uint64_t add_on = 0;
		// The stage scales the response time before the frame count and adds frames after it.
		if (response != 0) {
			switch (latency_stage) {
				case 1: response += response / 2; add_on = 10; break;
				case 2: response *= 2; add_on = 20; break;
				case 3: response *= 3; add_on = 30; break;
				default: break;
			}
		}

		std::uint64_t target = response * frame_rate / (2 * NETWORK_TICKS_PER_SECOND) + add_on;
		// A rise is limited to one send period per evaluation; a fall applies at once.
		if (current_max_ahead < target && current_max_ahead + frame_send_rate <= target) {
			target = current_max_ahead + frame_send_rate;
		}

		std::uint64_t aligned = Divide_Round_Up(target, frame_send_rate) * frame_send_rate;
		aligned = std::max<std::uint64_t>(aligned, 3ull * frame_send_rate);
		std::uint64_t const ceiling = Divide_Round_Up(MAXIMUM_MAX_AHEAD, frame_send_rate) * frame_send_rate;
		if (ceiling <= aligned) {
			aligned = ceiling;
		}
		return(static_cast<unsigned int>(aligned));
	}


	/// <summary>Settings for the worst round trip at the desired frame rate, under gamemd's rules.</summary>
	TimingSettings Gamemd_Timing_Settings(Milliseconds worst_round_trip, unsigned int frame_rate, unsigned int current_max_ahead)
	{
		unsigned int const frame_send_rate = Gamemd_Frame_Send_Rate(frame_rate);
		unsigned int const max_ahead = Gamemd_Max_Ahead(Round_Trip_Ticks(worst_round_trip), frame_rate, frame_send_rate,
			current_max_ahead, GAMEMD_LATENCY_STAGE);
		return(TimingSettings{frame_send_rate, max_ahead});
	}


	/// <summary>Rounds a look-ahead up to whole send periods, within the smallest and largest the game accepts.</summary>
	unsigned int Host_Max_Ahead_For(unsigned int max_ahead, unsigned int frame_send_rate)
	{
		frame_send_rate = std::clamp(frame_send_rate, MINIMUM_TIMING_RUNG, MAXIMUM_TIMING_RUNG);
		unsigned int const minimum = Settings_For_Rung(frame_send_rate).MaxAhead;
		unsigned int const ceiling = MAXIMUM_MAX_AHEAD - (MAXIMUM_MAX_AHEAD % frame_send_rate);
		std::uint64_t const aligned = Divide_Round_Up(std::max(max_ahead, minimum), frame_send_rate) * frame_send_rate;
		return(static_cast<unsigned int>(std::min<std::uint64_t>(aligned, ceiling)));
	}


	TimingSettings Apply_Host_Choice(TimingSettings settings, HostTimingChoice choice)
	{
		if (!choice.Is_Set()) {
			return(settings);
		}
		if (Host_Frame_Send_Rate_Is_Valid(choice.FrameSendRate)) {
			settings.FrameSendRate = choice.FrameSendRate;
		}
		// A chosen look-ahead stands; otherwise the policy's look-ahead is kept whole under the chosen send period.
		unsigned int const wanted = Host_Max_Ahead_Is_Valid(choice.MaxAhead) && choice.MaxAhead != 0 ? choice.MaxAhead : settings.MaxAhead;
		settings.MaxAhead = Host_Max_Ahead_For(wanted, settings.FrameSendRate);
		return(settings);
	}


	TimingSettings Gamemd_Timing_Settings(Milliseconds worst_round_trip, unsigned int frame_rate, unsigned int current_max_ahead,
		HostTimingChoice choice)
	{
		unsigned int const frame_send_rate = Host_Frame_Send_Rate_Is_Valid(choice.FrameSendRate)
			? choice.FrameSendRate : Gamemd_Frame_Send_Rate(frame_rate);
		unsigned int const max_ahead = Gamemd_Max_Ahead(Round_Trip_Ticks(worst_round_trip), frame_rate, frame_send_rate,
			current_max_ahead, GAMEMD_LATENCY_STAGE);
		return(Apply_Host_Choice(TimingSettings{frame_send_rate, max_ahead}, choice));
	}


	namespace
	{
		std::string Lower(std::string_view text)
		{
			std::string lower(text);
			for (char & ch : lower) {
				ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
			}
			return(lower);
		}

		std::string_view Trim(std::string_view text)
		{
			while (!text.empty() && (text.front() == ' ' || text.front() == '\t')) {
				text.remove_prefix(1);
			}
			while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\r' || text.back() == '\n')) {
				text.remove_suffix(1);
			}
			return(text);
		}

		// A whole number of up to five digits.
		bool Parse_Number(std::string_view text, unsigned int & value)
		{
			if (text.empty() || text.size() > 5) {
				return(false);
			}
			unsigned int number = 0;
			for (char ch : text) {
				if (ch < '0' || ch > '9') {
					return(false);
				}
				number = number * 10 + static_cast<unsigned int>(ch - '0');
			}
			value = number;
			return(true);
		}

		std::string Frames(unsigned int value, char const * automatic)
		{
			return(value == 0 ? std::string(automatic) : std::to_string(value) + (value == 1 ? " frame" : " frames"));
		}
	}


	std::string Describe_Host_Options(HostNetworkOptions const & options)
	{
		std::string text = "send period " + Frames(options.Timing.FrameSendRate, "automatic");
		unsigned int look_ahead = options.Timing.MaxAhead;
		// A look-ahead is a whole number of send periods, so with both chosen the game plays the rounded one.
		if (look_ahead != 0 && options.Timing.FrameSendRate != 0) {
			look_ahead = Host_Max_Ahead_For(look_ahead, options.Timing.FrameSendRate);
		}
		text += ", look-ahead " + Frames(look_ahead, "automatic");
		text += ", protocol " + std::to_string(options.Protocol);
		return(text);
	}


	HostCommandResult Apply_Host_Command(std::string_view line, HostNetworkOptions & options)
	{
		HostCommandResult result;
		line = Trim(line);
		if (line.empty() || line.front() != '/') {
			return(result);
		}

		std::size_t const space = line.find_first_of(" \t");
		std::string const name = Lower(line.substr(1, space == std::string_view::npos ? std::string_view::npos : space - 1));
		std::string const argument = space == std::string_view::npos ? std::string() : Lower(Trim(line.substr(space)));

		enum class Command { SendPeriod, LookAhead, Protocol, Other };
		Command command = Command::Other;
		if (name == "framesendrate") {
			command = Command::SendPeriod;
		} else if (name == "maxahead") {
			command = Command::LookAhead;
		} else if (name == "protocolversion" || name == "protocol") {
			command = Command::Protocol;
		}
		if (command == Command::Other) {
			return(result);
		}
		result.Recognised = true;

		if (argument.empty()) {
			switch (command) {
				case Command::SendPeriod: result.Message = "Syntax: /FrameSendRate <1 to 10, or auto>. Now: send period "
					+ Frames(options.Timing.FrameSendRate, "automatic") + "."; break;
				case Command::LookAhead: result.Message = "Syntax: /MaxAhead <4 to 250, or auto>. Now: look-ahead "
					+ Frames(options.Timing.MaxAhead, "automatic") + "."; break;
				default: result.Message = "Syntax: /ProtocolVersion <0 or 2>. Now: protocol " + std::to_string(options.Protocol) + "."; break;
			}
			return(result);
		}

		bool const automatic = argument == "auto" || argument == "default";
		unsigned int value = 0;
		bool const parsed = automatic || Parse_Number(argument, value);
		HostNetworkOptions const before = options;
		switch (command) {
			case Command::SendPeriod:
				if (!parsed || (value != 0 && !Host_Frame_Send_Rate_Is_Valid(value))) {
					result.Message = "FrameSendRate must be 1 to 10 frames, or auto.";
					return(result);
				}
				options.Timing.FrameSendRate = value;
				break;
			case Command::LookAhead:
				if (!parsed || !Host_Max_Ahead_Is_Valid(value)) {
					result.Message = "MaxAhead must be 4 to 250 frames, or auto.";
					return(result);
				}
				options.Timing.MaxAhead = value;
				break;
			default:
				if (!parsed || (!automatic && !Host_Protocol_Is_Valid(static_cast<int>(value)))) {
					result.Message = "ProtocolVersion only allows the values 0 and 2.";
					return(result);
				}
				options.Protocol = automatic ? 2 : static_cast<int>(value);
				break;
		}
		result.Changed = !(options == before);
		result.Message = std::string(result.Changed ? "Set: " : "Unchanged: ") + Describe_Host_Options(options) + ".";
		return(result);
	}


	// Bit 31 marks a word that holds options; bits 0 to 7 hold the send period, 8 to 15 the look-ahead, and bit 16 is set for protocol 0.
	std::uint32_t Pack_Host_Options(HostNetworkOptions const & options)
	{
		if (options.Is_Default()) {
			return(0);
		}
		return(0x80000000u | (options.Timing.FrameSendRate & 0xFFu) | ((options.Timing.MaxAhead & 0xFFu) << 8) | (options.Protocol == 0 ? 0x10000u : 0u));
	}


	std::optional<HostNetworkOptions> Unpack_Host_Options(std::uint32_t packed)
	{
		HostNetworkOptions options;
		if (packed == 0) {
			return(options);
		}
		if ((packed & 0x80000000u) == 0 || (packed & 0x7FFE0000u) != 0) {
			return(std::nullopt);
		}
		options.Timing.FrameSendRate = packed & 0xFFu;
		options.Timing.MaxAhead = (packed >> 8) & 0xFFu;
		options.Protocol = (packed & 0x10000u) != 0 ? 0 : 2;
		if ((options.Timing.FrameSendRate != 0 && !Host_Frame_Send_Rate_Is_Valid(options.Timing.FrameSendRate))
			|| !Host_Max_Ahead_Is_Valid(options.Timing.MaxAhead)) {
			return(std::nullopt);
		}
		return(options);
	}


	/// <summary>Whole milliseconds per frame, truncated as gamemd does; zero means no limit.</summary>
	Milliseconds Gamemd_Frame_Wait(unsigned int rate)
	{
		return(rate == 0 ? 0 : static_cast<Milliseconds>(1000 / rate));
	}


	/// <summary>Adds 10 ms for each of look-ahead / 4, / 2 and 3 / 4 that a peer's lag reaches; a zero lag adds nothing.</summary>
	Milliseconds Gamemd_Local_Pacing(unsigned int max_ahead, unsigned int lag_frames)
	{
		if (lag_frames == 0 || max_ahead < GAMEMD_PACING_MIN_MAX_AHEAD) {
			return(0);
		}

		Milliseconds extra = 0;
		if (max_ahead / 4 <= lag_frames) {
			extra += GAMEMD_PACING_STEP;
		}
		if (max_ahead / 2 <= lag_frames) {
			extra += GAMEMD_PACING_STEP;
		}
		if ((3 * max_ahead) / 4 <= lag_frames) {
			extra += GAMEMD_PACING_STEP;
		}
		return(extra);
	}


	/// <summary>Converts a round trip in milliseconds to whole ticks of 1/60 second, rounding down.</summary>
	unsigned int Round_Trip_Ticks(Milliseconds round_trip)
	{
		return(static_cast<unsigned int>((static_cast<std::uint64_t>(round_trip) * NETWORK_TICKS_PER_SECOND) / 1000));
	}


	/// <summary>Restores the initial rung and anchors the evaluation cadence to a frame.</summary>
	void BalancedTimingPolicy::Reset(std::uint32_t frame)
	{
		CurrentRung = INITIAL_TIMING_RUNG;
		CurrentSettings = Settings_For_Rung(INITIAL_TIMING_RUNG);
		GoodEvaluations = 0;
		BootstrapStartFrame = frame;
		LastEvaluationFrame = frame;
		LastChangeFrame = 0;
		HasEvaluated = false;
		HasChanged = false;
		Bootstrapping = true;
		ImprovementStreak = false;
	}


	/// <summary>Restores synchronized policy state after a master handoff.</summary>
	void BalancedTimingPolicy::Reset_From(TimingSettings settings, std::uint32_t frame)
	{
		CurrentRung = std::clamp(settings.FrameSendRate, MINIMUM_TIMING_RUNG, MAXIMUM_TIMING_RUNG);
		CurrentSettings = settings;
		GoodEvaluations = 0;
		LastEvaluationFrame = frame;
		LastChangeFrame = frame;
		HasEvaluated = true;
		HasChanged = true;
		Bootstrapping = false;
		ImprovementStreak = false;
	}


	/// <summary>Commits a policy change and resets hysteresis.</summary>
	void BalancedTimingPolicy::Change_To(TimingSettings settings, std::uint32_t frame)
	{
		ImprovementStreak = Timing_Is_Better(settings, CurrentSettings);
		CurrentRung = std::clamp(settings.FrameSendRate, MINIMUM_TIMING_RUNG, MAXIMUM_TIMING_RUNG);
		CurrentSettings = settings;
		GoodEvaluations = 0;
		LastChangeFrame = frame;
		HasChanged = true;
	}


	/// <summary>Anchors steady-state evaluations to 256 frames after reset.</summary>
	void BalancedTimingPolicy::Finish_Bootstrap(void)
	{
		Bootstrapping = false;
		GoodEvaluations = 0;
		ImprovementStreak = false;
		LastEvaluationFrame = BootstrapStartFrame;
		HasEvaluated = true;
	}


	/// <summary>Applies cadence, hysteresis, and improvement headroom.</summary>
	TimingEvaluation BalancedTimingPolicy::Evaluate(TimingCensus const & census, unsigned int target_fps, std::uint32_t frame)
	{
		TimingEvaluation result{Current_Settings(), CurrentRung, false, false};
		if (Bootstrapping) {
			std::uint32_t const elapsed_frames = frame - BootstrapStartFrame;
			if (elapsed_frames < BOOTSTRAP_FIRST_EVALUATION || (HasEvaluated && elapsed_frames < BOOTSTRAP_FINAL_EVALUATION)) {
				return(result);
			}

			HasEvaluated = true;
			LastEvaluationFrame = frame;
			result.Evaluated = true;
			bool const complete = census.ProcessComplete && census.RoundTripComplete;
			if (census.RequiresConservativeTiming || complete || elapsed_frames >= BOOTSTRAP_FINAL_EVALUATION) {
				TimingSettings const selected = census.RequiresConservativeTiming ? Desired_Settings(census, target_fps, false)
					: complete ? Desired_Settings(census, target_fps, true) : Settings_For_Rung(BOOTSTRAP_FALLBACK_RUNG);
				if (selected != CurrentSettings) {
					Change_To(selected, frame);
					result.Changed = true;
				}
				Finish_Bootstrap();
				result.Settings = Current_Settings();
				result.Rung = CurrentRung;
			}
			return(result);
		}

		if (HasEvaluated && frame - LastEvaluationFrame < EVALUATION_INTERVAL) {
			return(result);
		}

		HasEvaluated = true;
		LastEvaluationFrame = frame;
		result.Evaluated = true;
		if (!census.RequiresConservativeTiming && census.ActivePlayers > 0 && !census.RoundTripComplete) {
			// A lapsed report holds the current timing, but the reports that did arrive can still worsen it.
			GoodEvaluations = 0;
			ImprovementStreak = false;
			if (census.FreshRoundTripReports > 0) {
				TimingSettings const desired_settings = Desired_Settings(census, target_fps, false);
				if (Timing_Is_Worse(desired_settings, CurrentSettings)) {
					Change_To(desired_settings, frame);
					result.Changed = true;
					result.Settings = Current_Settings();
					result.Rung = CurrentRung;
				}
			}
			return(result);
		}

		// Worsening is immediate; the first improvement must clear the headroom, waiting, cadence, and
		// cooldown gates, and a descent then continues one rung per evaluation while the headroom holds.
		TimingSettings const desired_settings = Desired_Settings(census, target_fps, false);
		if (Timing_Is_Worse(desired_settings, CurrentSettings)) {
			Change_To(desired_settings, frame);
			result.Changed = true;
		} else if (Timing_Is_Better(desired_settings, CurrentSettings) && census.WorstStallMilliseconds < STALL_IMPROVE_MILLISECONDS
			&& (!HasChanged || ImprovementStreak || frame - LastChangeFrame >= CHANGE_COOLDOWN)) {
			TimingSettings const headroom = Desired_Settings(census, target_fps, true);
			if (Timing_Is_Better(headroom, CurrentSettings)) {
				GoodEvaluations++;
				if (GoodEvaluations >= (ImprovementStreak ? DESCENT_EVALUATIONS_REQUIRED : GOOD_EVALUATIONS_REQUIRED)) {
					TimingSettings const next = desired_settings.FrameSendRate < CurrentRung
						? Settings_For_Rung(CurrentRung - 1) : desired_settings;
					Change_To(next, frame);
					result.Changed = true;
				}
			} else {
				GoodEvaluations = 0;
				ImprovementStreak = false;
			}
		} else {
			GoodEvaluations = 0;
			ImprovementStreak = false;
		}

		result.Settings = Current_Settings();
		result.Rung = CurrentRung;
		return(result);
	}


	/// <summary>Delays decreases until the old scheduling horizon drains.</summary>
	std::optional<StagedTimingUpdate> Stage_Timing_Update(TimingSettings current, TimingSettings requested, std::uint32_t event_frame)
	{
		if (!Timing_Transition_Source_Is_Valid(current) || !Timing_Settings_Are_Valid(requested)) {
			return(std::nullopt);
		}

		if (requested.FrameSendRate > current.FrameSendRate && requested.MaxAhead < current.MaxAhead) {
			std::optional<unsigned int> const immediate_horizon = Align_Max_Ahead(current.MaxAhead, requested.FrameSendRate);
			if (immediate_horizon) {
				return(StagedTimingUpdate{requested, *immediate_horizon, event_frame, true});
			}
		}

		bool const decrease = requested.FrameSendRate < current.FrameSendRate || requested.MaxAhead < current.MaxAhead;
		if (!decrease) {
			return(StagedTimingUpdate{requested, requested.MaxAhead, event_frame, false});
		}

		std::uint64_t const period = std::lcm(current.FrameSendRate, requested.FrameSendRate);
		std::uint64_t const old_horizon = static_cast<std::uint64_t>(event_frame) + current.MaxAhead;
		std::uint64_t const activation = Divide_Round_Up(old_horizon, period) * period;
		if (activation > std::numeric_limits<std::uint32_t>::max()) {
			return(std::nullopt);
		}

		unsigned int const minimum_horizon = std::max(requested.MaxAhead, current.MaxAhead - current.FrameSendRate);
		std::optional<unsigned int> const initial_max_ahead = Align_Max_Ahead(minimum_horizon, requested.FrameSendRate);
		if (!initial_max_ahead) {
			return(std::nullopt);
		}

		return(StagedTimingUpdate{requested, *initial_max_ahead, static_cast<std::uint32_t>(activation), true});
	}


	/// <summary>Returns the first send boundary strictly after an event frame.</summary>
	std::optional<std::uint32_t> Next_Send_Boundary(std::uint32_t frame, unsigned int frame_send_rate)
	{
		if (frame_send_rate == 0) {
			return(std::nullopt);
		}

		std::uint64_t const boundary = (static_cast<std::uint64_t>(frame) / frame_send_rate + 1) * frame_send_rate;
		if (boundary > std::numeric_limits<std::uint32_t>::max()) {
			return(std::nullopt);
		}
		return(static_cast<std::uint32_t>(boundary));
	}


	/// <summary>Advances one catch-up step without dropping below the target horizon.</summary>
	std::optional<unsigned int> Next_Transition_Max_Ahead(TimingSettings current, TimingSettings requested)
	{
		if (!Timing_Settings_Are_Valid(current) || !Timing_Settings_Are_Valid(requested) || current.FrameSendRate != requested.FrameSendRate) {
			return(std::nullopt);
		}

		if (current.MaxAhead <= requested.MaxAhead) {
			return(requested.MaxAhead);
		}
		return(std::max(requested.MaxAhead, current.MaxAhead - requested.FrameSendRate));
	}


	/// <summary>Advances one deterministic drain or catch-up boundary.</summary>
	std::optional<TimingTransitionAdvance> Advance_Timing_Transition(TimingTransitionState & transition, TimingSettings current, std::uint32_t frame)
	{
		bool const current_is_valid = transition.Activated ? Timing_Settings_Are_Valid(current) : Timing_Transition_Source_Is_Valid(current);
		if (!transition.Plan.Deferred || !current_is_valid || !Timing_Settings_Are_Valid(transition.Plan.Settings)
			|| !Timing_Settings_Are_Valid({transition.Plan.Settings.FrameSendRate, transition.Plan.InitialMaxAhead})) {
			return(std::nullopt);
		}

		TimingTransitionAdvance result{current};
		if (!transition.Activated) {
			if (!Timing_Update_Is_Due(frame, transition.Plan.ActivationFrame)) {
				return(result);
			}
			result.Settings = {transition.Plan.Settings.FrameSendRate, transition.Plan.InitialMaxAhead};
			result.Changed = result.Settings != current;
			transition.LastStepFrame = frame;
			transition.Activated = true;
		} else if (current.MaxAhead > transition.Plan.Settings.MaxAhead && frame > transition.LastStepFrame
			&& frame % transition.Plan.Settings.FrameSendRate == 0) {
			std::optional<unsigned int> const next = Next_Transition_Max_Ahead(current, transition.Plan.Settings);
			if (!next) {
				return(std::nullopt);
			}
			result.Settings.MaxAhead = *next;
			result.Changed = result.Settings != current;
			transition.LastStepFrame = frame;
		}

		result.Complete = transition.Activated && result.Settings == transition.Plan.Settings;
		return(result);
	}


	/// <summary>Checks a staged activation frame with wraparound semantics.</summary>
	bool Timing_Update_Is_Due(std::uint32_t frame, std::uint32_t activation_frame)
	{
		return(static_cast<std::int32_t>(frame - activation_frame) >= 0);
	}


	/// <summary>Includes unexecuted events whose scheduled frame was skipped by a send-period change.</summary>
	bool Event_Is_Due(int event_frame, bool is_executed, int frame)
	{
		return(!is_executed && event_frame <= frame);
	}
}
