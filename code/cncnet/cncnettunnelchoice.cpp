/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "cncnet/cncnettunnelchoice.h"

#include "cncnet/cncnethttp.h"

#include <fstream>
#include <iterator>
#include <mutex>
#include <thread>


struct CnCNetTunnelChoiceClass::SharedType
{
	std::mutex Lock;
	bool Done = false;
	std::vector<CnCNetTunnelEntryType> Tunnels;
	std::string Note;           // Where the list came from, or why it did not.
};


CnCNetTunnelChoiceClass::CnCNetTunnelChoiceClass(void) :
	Shared(std::make_shared<SharedType>())
{
}


void CnCNetTunnelChoiceClass::Start(std::string const & url, std::string const & cache_file)
{
	std::shared_ptr<SharedType> shared = Shared;
	std::thread([shared, url, cache_file](void) {
		std::string text;
		std::string error;
		std::string note;
		if (CnCNet_Http_Get(url, 10000, 1 << 20, text, error) && !CnCNet_Parse_Tunnel_List(text).empty()) {
			if (!cache_file.empty()) {
				std::ofstream file(cache_file, std::ios::binary | std::ios::trunc);
				file << text;
			}
		} else {
			note = error.empty() ? "The tunnel list from " + url + " had no usable tunnel." : error;
			text.clear();
			std::ifstream file(cache_file, std::ios::binary);
			if (!cache_file.empty() && file) {
				text.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
				note += " Using the list kept from the last good download.";
			}
		}

		std::vector<CnCNetTunnelEntryType> tunnels = CnCNet_Parse_Tunnel_List(text);
		CnCNet_Ping_Tunnels(tunnels, 2, 1000, 12);

		std::lock_guard<std::mutex> guard(shared->Lock);
		shared->Tunnels = std::move(tunnels);
		shared->Note = note;
		shared->Done = true;
	}).detach();
}


CnCNetTunnelChoiceType CnCNetTunnelChoiceClass::Poll(int players) const
{
	CnCNetTunnelChoiceType choice;
	std::lock_guard<std::mutex> guard(Shared->Lock);
	choice.Done = Shared->Done;
	if (!Shared->Done) {
		choice.Text = "Looking for the nearest CnCNet tunnel...";
		return(choice);
	}
	std::optional<std::size_t> const best = CnCNet_Pick_Tunnel(Shared->Tunnels, players);
	if (!best.has_value()) {
		choice.Text = "No CnCNet tunnel answered. " + Shared->Note;
		return(choice);
	}
	CnCNetTunnelEntryType const & tunnel = Shared->Tunnels[*best];
	choice.Found = true;
	choice.Host = tunnel.Host;
	choice.Port = tunnel.Port;
	choice.Name = tunnel.Name;
	choice.PingMS = tunnel.PingMS;
	choice.Text = "Tunnel " + tunnel.Name + " (" + tunnel.Host + ":" + std::to_string(tunnel.Port) + "), " + std::to_string(tunnel.PingMS) + " ms.";
	return(choice);
}
