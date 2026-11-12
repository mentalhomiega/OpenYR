/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "ui/dev/uiperfoverlay.h"

#include "_ui.h"
#include "autotest.h"
#include "bgfxbackend.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "hud.h"
#include "monotonic.h"
#include "opents_build.h"
#include "options.h"
#include "perfstat.h"
#include "renderthread.h"
#include "shellmap.h"
#include "tab.h"
#include "ui/uishell.h"
#include "video.h"
#include "viewzoom.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <imgui.h>
#include <vector>


namespace
{

// The numbers cover the samples of the last WINDOW_MS, or as many as the history keeps.
constexpr double WINDOW_MS = 5000.0;

// Sizes at a font of 13 pixels; they grow with the font.
constexpr float BASE_FONT = 13.0f;
constexpr int GRAPH_COLUMNS = 300;
constexpr float GRAPH_HEIGHT = 48.0f;
constexpr float GRAPH_SCALE_MS = 50.0f;
constexpr float BAR_WIDTH = 80.0f;
constexpr float BAR_FULL_MS = 1000.0f / 60.0f;

// The marked frame times: 60 and 30 frames a second.
constexpr float GRAPH_LINES_MS[] = {1000.0f / 60.0f, 1000.0f / 30.0f};

// A row is left out while its phase took less than this on average and at worst.
constexpr double HIDDEN_AVERAGE_MS = 0.005;
constexpr double HIDDEN_WORST_MS = 0.05;

constexpr ImU32 BACKDROP = IM_COL32(0, 0, 0, 170);
constexpr ImU32 REST_BAR = IM_COL32(120, 120, 120, 255);
constexpr ImU32 PART_BAR = IM_COL32(240, 240, 240, 255);
constexpr ImU32 LINE_DARK = IM_COL32(0, 0, 0, 255);
constexpr ImU32 LINE_LIGHT = IM_COL32(255, 220, 90, 255);
constexpr ImU32 BAR_FRAME = IM_COL32(160, 160, 160, 255);

char const * const Level_Names[PERF_OVERLAY_LEVEL_COUNT] = {"Off", "Basic", "Detailed"};

double BuildMs = 0.0;
double DrawMs = 0.0;
int LoggedLevel = PERF_OVERLAY_OFF;
int ShownRows = 0;


struct RateNumbers
{
	double Fps;
	PerfWindowSummary Interval;
};


// What the overlay shows above its graphs.
struct OverlayNumbers
{
	RateNumbers Game;
	RateNumbers Drawn;
	PerfWindowSummary Logic;
	PerfWindowSummary DrawTotal;
	int Frames;
	int Draws;
	int Presents;
};


int Samples_In_Window(PerfDomain domain)
{
	return(Perf_Newest_Since(Perf_Domain_Times(domain), Monotonic_Nanoseconds() / 1e6 - WINDOW_MS));
}


double Per_Second_Fps(double milliseconds)
{
	return(milliseconds > 0.0 ? 1000.0 / milliseconds : 0.0);
}


RateNumbers Rate_Of(PerfRing<float> const & intervals, int count)
{
	RateNumbers rate;
	rate.Interval = Perf_Summarise(intervals, count);
	rate.Fps = Per_Second_Fps(rate.Interval.Average);
	return(rate);
}


OverlayNumbers Measure(void)
{
	OverlayNumbers numbers;
	numbers.Frames = Samples_In_Window(PERF_DOMAIN_FRAME);
	numbers.Draws = Samples_In_Window(PERF_DOMAIN_DRAW);
	numbers.Presents = Samples_In_Window(PERF_DOMAIN_PRESENT);
	numbers.Game = Rate_Of(Perf_Frame_Intervals(), numbers.Frames);
	numbers.Drawn = Rate_Of(Perf_Present_Gaps(), numbers.Presents);
	numbers.Logic = Perf_Average_Worst(Perf_Phase_History(PERF_FRAME_LOGIC), numbers.Frames);
	numbers.DrawTotal = Perf_Average_Worst(Perf_Phase_History(PERF_DRAW_TOTAL), numbers.Draws);
	return(numbers);
}


// Two lines: the rate with its average frame time and 1% low, then the newest and worst frame time and the work.
void Rate_Lines(char const * name, RateNumbers const & rate, char const * work, PerfWindowSummary const & work_time)
{
	ImGui::Text("%-5s %5.1f fps | avg %5.1f ms | 1%% low %5.1f fps", name, rate.Fps, rate.Interval.Average, Per_Second_Fps(rate.Interval.P99));
	ImGui::Text("      now %5.1f ms | worst %5.1f ms | %s %.2f avg, %.1f max", rate.Interval.Latest, rate.Interval.Worst, work, work_time.Average, work_time.Worst);
}


/// <summary>
/// Draws the newest values of a ring as columns, newest at the right, on a fixed scale of
/// GRAPH_SCALE_MS with dashed, labelled lines at 16.7 and 33.3 ms. A column past the scale is
/// cut at the top and marked with a triangle. With a part, each column's newest-aligned part
/// value is drawn bright over the grey column.
/// </summary>
void Draw_Graph(PerfRing<float> const & total, PerfRing<float> const * part)
{
	float const unit = ImGui::GetFontSize() / BASE_FONT;
	float const width = GRAPH_COLUMNS * unit;
	float const height = GRAPH_HEIGHT * unit;
	ImVec2 const origin = ImGui::GetCursorScreenPos();
	float const bottom = origin.y + height;
	ImDrawList * draw = ImGui::GetWindowDrawList();

	draw->AddRectFilled(origin, ImVec2(origin.x + width, bottom), BACKDROP);

	auto bar_height = [&](float value) {
		return(std::min(value / GRAPH_SCALE_MS, 1.0f) * height);
	};

	int const columns = std::min(GRAPH_COLUMNS, total.Count());
	for (int index = 0; index < columns; index++) {
		float const value = total.Newest(index);
		float const right = origin.x + width - index * unit;
		float const left = right - unit;
		draw->AddRectFilled(ImVec2(left, bottom - bar_height(value)), ImVec2(right, bottom), REST_BAR);
		if (part != nullptr && index < part->Count()) {
			draw->AddRectFilled(ImVec2(left, bottom - bar_height(part->Newest(index))), ImVec2(right, bottom), PART_BAR);
		}
		if (value > GRAPH_SCALE_MS) {
			draw->AddTriangleFilled(ImVec2(left - 2 * unit, origin.y + 4 * unit), ImVec2(right + 2 * unit, origin.y + 4 * unit), ImVec2((left + right) / 2, origin.y), PART_BAR);
		}
	}

	float const label_x = origin.x + width + 4 * unit;
	for (float line_ms : GRAPH_LINES_MS) {
		float const y = bottom - bar_height(line_ms);
		draw->AddLine(ImVec2(origin.x, y), ImVec2(origin.x + width, y), LINE_DARK, 2 * unit);
		for (float x = origin.x; x < origin.x + width; x += 6 * unit) {
			draw->AddLine(ImVec2(x, y), ImVec2(std::min(x + 3 * unit, origin.x + width), y), LINE_LIGHT, unit);
		}
		char label[16];
		std::snprintf(label, sizeof(label), "%.1f ms", line_ms);
		draw->AddText(ImVec2(label_x, y - ImGui::GetFontSize() / 2), IM_COL32_WHITE, label);
	}

	ImGui::Dummy(ImVec2(width + ImGui::CalcTextSize("33.3 ms").x + 6 * unit, height));
}


void Draw_Bar(double milliseconds)
{
	float const unit = ImGui::GetFontSize() / BASE_FONT;
	float const width = BAR_WIDTH * unit;
	float const height = ImGui::GetTextLineHeight() - 2 * unit;
	ImVec2 const origin = ImGui::GetCursorScreenPos();
	ImDrawList * draw = ImGui::GetWindowDrawList();
	float const filled = (float)std::min(milliseconds / BAR_FULL_MS, 1.0) * width;
	draw->AddRect(ImVec2(origin.x, origin.y + unit), ImVec2(origin.x + width, origin.y + unit + height), BAR_FRAME);
	draw->AddRectFilled(ImVec2(origin.x, origin.y + unit), ImVec2(origin.x + filled, origin.y + unit + height), PART_BAR);
	ImGui::Dummy(ImVec2(width, ImGui::GetTextLineHeight()));
}


// One domain's breakdown as a table of name, average, worst and a bar of the average. Returns the rows shown.
int Draw_Breakdown(char const * id, char const * title, PerfDomain domain, int count)
{
	std::vector<PerfPhaseRow> const rows = Perf_Phase_Rows(domain);
	size_t name_chars = std::strlen("phase");
	for (PerfPhaseRow const & row : rows) {
		name_chars = std::max(name_chars, row.Depth * 2 + 1 + std::strlen(Perf_Phase_Info(row.Phase).Name));
	}
	float const char_width = ImGui::CalcTextSize("M").x;

	int shown = 0;
	ImGui::TextUnformatted(title);
	if (!ImGui::BeginTable(id, 4, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX)) {
		return(shown);
	}
	ImGui::TableSetupColumn("phase", ImGuiTableColumnFlags_WidthFixed, name_chars * char_width);
	ImGui::TableSetupColumn("avg ms", ImGuiTableColumnFlags_WidthFixed, 6 * char_width);
	ImGui::TableSetupColumn("max ms", ImGuiTableColumnFlags_WidthFixed, 6 * char_width);
	ImGui::TableSetupColumn("0-16.7 ms", ImGuiTableColumnFlags_WidthFixed, BAR_WIDTH * ImGui::GetFontSize() / BASE_FONT);
	ImGui::TableHeadersRow();

	for (PerfPhaseRow const & row : rows) {
		PerfWindowSummary const time = Perf_Average_Worst(Perf_Phase_History(row.Phase), count);
		if (time.Average < HIDDEN_AVERAGE_MS && time.Worst < HIDDEN_WORST_MS) {
			continue;
		}
		PerfPhaseInfo const & info = Perf_Phase_Info(row.Phase);
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text("%*s%s%s", row.Depth * 2, "", info.Across ? "~" : "", info.Name);
		ImGui::TableNextColumn();
		ImGui::Text("%6.2f", time.Average);
		ImGui::TableNextColumn();
		ImGui::Text("%6.1f", time.Worst);
		ImGui::TableNextColumn();
		Draw_Bar(time.Average);
		shown++;
	}
	ImGui::EndTable();
	return(shown);
}


void Draw_Counts(OverlayNumbers const & numbers)
{
	auto per_second = [](PerfCounter counter) {
		return((unsigned long long)Perf_Last_Second_Count(counter));
	};
	ImGui::Text("Objects %d, units and structures %d", Objects.Count(), Technos.Count());
	ImGui::Text("%.0f s: %d frames, %d draws, %d presents", WINDOW_MS / 1000.0, numbers.Frames, numbers.Draws, numbers.Presents);
	ImGui::TextUnformatted("Last second:");
	ImGui::Text("  path searches %llu, target scans %llu", per_second(PERF_COUNT_PATHS), per_second(PERF_COUNT_THREATS));
	ImGui::Text("  object AI %llu, voxels %llu", per_second(PERF_COUNT_OBJECT_AI), per_second(PERF_COUNT_VOXELS));
	ImGui::Text("  particles %llu, uploads %.1f MB", per_second(PERF_COUNT_PARTICLES), per_second(PERF_COUNT_UPLOAD_BYTES) / 1048576.0);
	ImGui::Text("  draws skipped %llu, presents put off %llu", per_second(PERF_COUNT_DRAWS_SKIPPED), per_second(PERF_COUNT_PRESENTS_BUSY));
	ImGui::Text("Perf overlay: %.2f ms build, %.2f ms draw", BuildMs, DrawMs);
}


char const * Gpu_Drawing_Text(void)
{
	if (!Options.GpuRender) {
		return("off");
	}
	return(Modern_Interface() && Video_Map_Layer_Supported() ? "on" : "on, not in use");
}


void Draw_Setup_Lines(void)
{
	VideoScaleInfo const & scale = Video_Get_Scale_Info();
	ImGui::Text("Window %dx%d, frame %dx%d", scale.DestWidth, scale.DestHeight, scale.GameWidth, scale.GameHeight);
	ImGui::Text("Zoom %.2f, %s", DisplayZoom, Backend_Renderer_Name());
	ImGui::Text("GPU drawing %s", Gpu_Drawing_Text());
	ImGui::Text("%s interface, render thread %s", Modern_Interface() ? "Modern" : "Classic", RenderThread_Running() ? "on" : "off");
}


// The part of the screen the overlay may cover, in frame pixels: the map view's top left, clear of the
// modern build panel and the command bar.
Rect Overlay_Area(void)
{
	int const left = ScreenTacticalRect.X + HUD_MARGIN;
	int const top = ScreenTacticalRect.Y + HUD_MARGIN;
	int right = ScreenTacticalRect.X + ScreenTacticalRect.Width - HUD_MARGIN;
	int bottom = ScreenTacticalRect.Y + ScreenTacticalRect.Height - HUD_MARGIN;
	if (Modern_Interface()) {
		right = std::min(right, Hud_Layout().Panel.X - HUD_MARGIN);
	}
	Rect const bar = TabClass::Command_Bar_Frame();
	if (bar.Height > 0) {
		bottom = std::min(bottom, bar.Y - HUD_MARGIN);
	}
	return(Rect(left, top, std::max(0, right - left), std::max(0, bottom - top)));
}


void Draw_Contents(bool detailed)
{
	OverlayNumbers const numbers = Measure();

	Rate_Lines("Game", numbers.Game, "logic", numbers.Logic);
	Rate_Lines("Drawn", numbers.Drawn, "draw", numbers.DrawTotal);

	ImGui::TextUnformatted("Game frames, 0-50 ms: grey = time between, white = logic");
	Draw_Graph(Perf_Frame_Intervals(), &Perf_Phase_History(PERF_FRAME_LOGIC));
	ImGui::TextUnformatted("Drawn frames, 0-50 ms: time between presents");
	Draw_Graph(Perf_Present_Gaps(), nullptr);

	ShownRows = 0;
	if (detailed) {
		ImGui::Separator();
		ImGui::BeginGroup();
		ShownRows += Draw_Breakdown("##frame", "Game frame", PERF_DOMAIN_FRAME, numbers.Frames);
		ImGui::TextUnformatted("Indented: part of the row above");
		ImGui::TextUnformatted("~: part of whichever row ran it");
		Draw_Counts(numbers);
		ImGui::EndGroup();
		ImGui::SameLine(0.0f, ImGui::CalcTextSize("MM").x);
		ImGui::BeginGroup();
		ShownRows += Draw_Breakdown("##draw", "Draw", PERF_DOMAIN_DRAW, numbers.Draws);
		ShownRows += Draw_Breakdown("##present", "Present", PERF_DOMAIN_PRESENT, numbers.Presents);
		Draw_Setup_Lines();
		ImGui::EndGroup();
	}

	ImGui::Separator();
	ImGui::Text("Build %s (%s)", OPENTS_BUILD_DESCRIPTION, OPENTS_COMMIT_DATE);
	if (!detailed) {
		ImGui::Text("Perf overlay: %.2f ms build, %.2f ms draw", BuildMs, DrawMs);
	}
}

}


void Perf_Overlay_Set_Level(int level)
{
	Options.PerfOverlay = std::clamp(level, (int)PERF_OVERLAY_OFF, (int)PERF_OVERLAY_DETAILED);
	Perf_Set_User(PERF_USER_OVERLAY, Options.PerfOverlay != PERF_OVERLAY_OFF);
	LoggedLevel = PERF_OVERLAY_OFF;
	DebugString("PerfOverlay: %s\n", Level_Names[Options.PerfOverlay]);
}


void Perf_Overlay_Cycle(void)
{
	Perf_Overlay_Set_Level((Options.PerfOverlay + 1) % PERF_OVERLAY_LEVEL_COUNT);
}


bool UIPerf_Overlay_Wanted(void)
{
	return(Options.PerfOverlay != PERF_OVERLAY_OFF && GameActive && !Shellmap_Running() && !UIShell.Screen_Shown());
}


bool UIPerf_Overlay_Build(void)
{
	if (!UIPerf_Overlay_Wanted()) {
		return(false);
	}

	std::int64_t const start = Monotonic_Nanoseconds();
	VideoScaleInfo const & scale = Video_Get_Scale_Info();
	Rect const area = Overlay_Area();
	float const unit = ImGui::GetFontSize() / BASE_FONT;

	ImGui::SetNextWindowPos(ImVec2(area.X * scale.ScaleX, area.Y * scale.ScaleY));
	ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(area.Width * scale.ScaleX, area.Height * scale.ScaleY));
	ImGui::SetNextWindowBgAlpha(0.75f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6 * unit, 4 * unit));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6 * unit, 2 * unit));
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4 * unit, 0.0f));
	ImGuiWindowFlags const flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (ImGui::Begin("##perfoverlay", nullptr, flags)) {
		Draw_Contents(Options.PerfOverlay == PERF_OVERLAY_DETAILED);
	}
	ImGui::End();
	ImGui::PopStyleVar(3);

	BuildMs = (Monotonic_Nanoseconds() - start) / 1e6;
	return(true);
}


void UIPerf_Overlay_Note_Drawn(double milliseconds)
{
	DrawMs = milliseconds;
	if (LoggedLevel != Options.PerfOverlay) {
		LoggedLevel = Options.PerfOverlay;
		AUTOTEST_LOG("perfoverlay drew level %s\n", Level_Names[LoggedLevel]);
	}
}


void UIPerf_Overlay_Report(void)
{
	OverlayNumbers const numbers = Measure();
	DebugString("AUTOTEST perfoverlay report frame %d level %s: game %.1f fps interval avg/p99/max %.2f/%.2f/%.2f ms, logic avg/max %.2f/%.2f ms, %d frames;"
		" drawn %.1f fps gap avg/p99/max %.2f/%.2f/%.2f ms, %d presents; draw total avg/max %.2f/%.2f ms, %d draws; %d breakdown rows; overlay build %.3f ms, draw %.3f ms\n",
		Frame, Level_Names[Options.PerfOverlay], numbers.Game.Fps, numbers.Game.Interval.Average, numbers.Game.Interval.P99, numbers.Game.Interval.Worst,
		numbers.Logic.Average, numbers.Logic.Worst, numbers.Frames,
		numbers.Drawn.Fps, numbers.Drawn.Interval.Average, numbers.Drawn.Interval.P99, numbers.Drawn.Interval.Worst, numbers.Presents,
		numbers.DrawTotal.Average, numbers.DrawTotal.Worst, numbers.Draws, ShownRows, BuildMs, DrawMs);
}
