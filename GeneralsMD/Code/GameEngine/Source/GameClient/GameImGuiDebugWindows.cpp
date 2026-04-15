/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2026 TheSuperHackers
**	Authors: zhp47
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// FILE: GameImGuiDebugWindows.cpp ////////////////////////////////////////////////////////////////
// TheSuperHackers @feature zhp47 15/04/2026
// ImGui debug windows exposing the engine's META DEMO debug commands.
// Each function mirrors the handler logic in CommandXlat.cpp so the same
// gameplay effect is achieved when toggling from the ImGui panel.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PreRTS.h"

#ifdef RTS_HAS_IMGUI

#include "GameClient/GameImGuiDebugWindows.h"
#include "ImGuiDebugMenu.h"
#include <imgui.h>

#include "Common/GlobalData.h"
#include "Common/Player.h"
#include "Common/PlayerList.h"

#include "GameClient/GameClient.h"
#include "GameClient/InGameUI.h"
#include "GameClient/View.h"

#include "GameLogic/GameLogic.h"

#if defined(RTS_DEBUG)
#include "Common/MessageStream.h"
#include "Common/GameAudio.h"
#include "GameClient/Drawable.h"
#endif


// Helper: iterate all human players and apply a function.
// Mirrors the pattern used by CommandXlat for build toggles.
template <typename Func>
static void ForEachHumanPlayer(Func fn)
{
	if (!ThePlayerList)
		return;
	for (Int n = 0; n < ThePlayerList->getPlayerCount(); ++n)
	{
		Player *player = ThePlayerList->getNthPlayer(n);
		if (player && player->getPlayerType() == PLAYER_HUMAN)
			fn(player);
	}
}


// Tooltip helper — shows (?) with hover text.
static void HelpMarker(const char *desc)
{
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TAB: Camera (available in all builds — merged GenTool-style settings + debug camera controls)
///////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawCameraTab()
{
	if (!TheGlobalData)
		return;

	// --- Height / Zoom ---
	if (ImGui::CollapsingHeader("Height / Zoom", ImGuiTreeNodeFlags_DefaultOpen))
	{
		Real maxHeight = TheGlobalData->m_maxCameraHeight;
		if (ImGui::SliderFloat("Max Height", &maxHeight, 100.0f, 1000.0f, "%.0f"))
			TheWritableGlobalData->m_maxCameraHeight = maxHeight;
		HelpMarker("Maximum camera zoom-out distance. Default: 300.");

		Real minHeight = TheGlobalData->m_minCameraHeight;
		if (ImGui::SliderFloat("Min Height", &minHeight, 10.0f, 500.0f, "%.0f"))
			TheWritableGlobalData->m_minCameraHeight = minHeight;
		HelpMarker("Minimum camera zoom-in distance. Default: 100.");

		if (TheTacticalView)
		{
			Real currentHeight = TheTacticalView->getHeightAboveGround();
			ImGui::Text("Current Height: %.0f", currentHeight);

			bool zoomLimited = TheTacticalView->isZoomLimited();
			if (ImGui::Checkbox("Enforce Zoom Limits", &zoomLimited))
				TheTacticalView->setZoomLimited(zoomLimited);
			HelpMarker("When unchecked, camera can zoom past min/max limits freely.");
		}
	}

	// --- Pitch / Angle ---
	if (ImGui::CollapsingHeader("Pitch / Rotation", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (TheTacticalView)
		{
			Real pitchDeg = RAD_TO_DEGF(TheTacticalView->getPitch());
			if (ImGui::SliderFloat("Pitch", &pitchDeg, -90.0f, 0.0f, "%.1f deg"))
				TheTacticalView->setPitch(DEG_TO_RADF(pitchDeg));
			HelpMarker("Camera tilt angle. -90 = top-down, 0 = horizontal. Default: ~37.5.");

			Real angleDeg = RAD_TO_DEGF(TheTacticalView->getAngle());
			if (ImGui::SliderFloat("Rotation", &angleDeg, -180.0f, 180.0f, "%.1f deg"))
				TheTacticalView->setAngle(DEG_TO_RADF(angleDeg));
			HelpMarker("Camera yaw rotation around the map. Default: 0.");
		}
	}

	// --- Field of View ---
	if (ImGui::CollapsingHeader("Field of View"))
	{
		if (TheTacticalView)
		{
			Real fovDeg = RAD_TO_DEGF(TheTacticalView->getFieldOfView());
			if (ImGui::SliderFloat("FOV", &fovDeg, 20.0f, 120.0f, "%.1f deg"))
				TheTacticalView->setFieldOfView(DEG_TO_RADF(fovDeg));
			HelpMarker("Perspective field of view. Lower = zoomed in, higher = fish-eye. Default: 50.");
		}
	}

	// --- Scroll Speed ---
	if (ImGui::CollapsingHeader("Scroll Speed"))
	{
		Real hScroll = TheGlobalData->m_horizontalScrollSpeedFactor;
		if (ImGui::SliderFloat("Horizontal", &hScroll, 0.1f, 5.0f, "%.2f"))
			TheWritableGlobalData->m_horizontalScrollSpeedFactor = hScroll;
		HelpMarker("Mouse edge-scroll speed multiplier (horizontal). Default: 1.0.");

		Real vScroll = TheGlobalData->m_verticalScrollSpeedFactor;
		if (ImGui::SliderFloat("Vertical", &vScroll, 0.1f, 5.0f, "%.2f"))
			TheWritableGlobalData->m_verticalScrollSpeedFactor = vScroll;
		HelpMarker("Mouse edge-scroll speed multiplier (vertical). Default: 1.0.");

		Real kbScroll = TheGlobalData->m_keyboardScrollFactor;
		if (ImGui::SliderFloat("Keyboard", &kbScroll, 0.1f, 3.0f, "%.2f"))
			TheWritableGlobalData->m_keyboardScrollFactor = kbScroll;
		HelpMarker("Arrow key scroll speed multiplier. Default: 0.5.");
	}

#if defined(RTS_DEBUG)
	// --- Debug Camera Controls (RTS_DEBUG only) ---
	if (ImGui::CollapsingHeader("Debug Camera"))
	{
		if (TheGlobalData)
		{
			bool debugCamera = TheGlobalData->m_debugCamera;
			if (ImGui::Checkbox("Debug Camera Mode", &debugCamera))
				TheWritableGlobalData->m_debugCamera = debugCamera;
			HelpMarker("Freeform debug camera with no constraints.");
		}

		if (ImGui::Button("Lock to Selection"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_LOCK_CAMERA_TO_SELECTION);
		}
		HelpMarker("Camera follows the currently selected unit.");

		if (ImGui::Button("Lock to Planes"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_LOCK_CAMERA_TO_PLANES);
		}
		HelpMarker("Camera tracks aircraft in flight.");

		if (ImGui::Button("Toggle Zoom Lock"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_ZOOM_LOCK);
		}
		HelpMarker("Locks the camera zoom level, preventing scroll-wheel changes.");

		if (ImGui::Button("Cycle Time of Day"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TIME_OF_DAY);
		}
		HelpMarker("Cycles through morning, afternoon, evening, and night lighting.");
	}

	// --- Map / Shroud ---
	if (ImGui::CollapsingHeader("Map / Shroud"))
	{
		if (TheGlobalData)
		{
			bool fogOfWar = TheGlobalData->m_fogOfWarOn;
			if (ImGui::Checkbox("Fog of War", &fogOfWar))
				TheWritableGlobalData->m_fogOfWarOn = fogOfWar;
			HelpMarker("Toggles fog of war. When off, entire map is visible.");
		}

		if (ImGui::Button("Reveal Map"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_DESHROUD);
		}
		HelpMarker("Permanently removes shroud from the entire map.");

		ImGui::SameLine();
		if (ImGui::Button("Re-shroud Map"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_ENSHROUD);
		}
		HelpMarker("Reapplies shroud to the entire map.");
	}
#endif // RTS_DEBUG

	// --- Reset ---
	ImGui::Spacing();
	if (ImGui::Button("Reset Camera to Defaults"))
	{
		TheWritableGlobalData->m_maxCameraHeight = 300.0f;
		TheWritableGlobalData->m_minCameraHeight = 100.0f;
		TheWritableGlobalData->m_horizontalScrollSpeedFactor = 1.0f;
		TheWritableGlobalData->m_verticalScrollSpeedFactor = 1.0f;
		TheWritableGlobalData->m_keyboardScrollFactor = TheGlobalData->m_keyboardDefaultScrollFactor;

		if (TheTacticalView)
		{
			TheTacticalView->setPitch(DEG_TO_RADF(TheGlobalData->m_cameraPitch));
			TheTacticalView->setAngle(DEG_TO_RADF(TheGlobalData->m_cameraYaw));
			TheTacticalView->setFieldOfView(DEG_TO_RADF(50.0f));
			TheTacticalView->setZoomLimited(TRUE);
		}
	}
	HelpMarker("Resets all camera settings on this tab to their original values.");
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TAB: Rendering (available in all builds)
///////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawRenderingTab()
{
	if (!TheGlobalData)
		return;

	if (ImGui::CollapsingHeader("Shadows & Effects", ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool shadows = TheGlobalData->m_useShadowVolumes;
		if (ImGui::Checkbox("Shadow Volumes", &shadows))
		{
			TheWritableGlobalData->m_useShadowVolumes = shadows;
			TheWritableGlobalData->m_useShadowDecals = shadows;
		}
		HelpMarker("Toggles shadow rendering for all objects (volumes + decals).");

#if defined(RTS_DEBUG)
		if (TheGameLogic)
		{
			bool behindBuildings = TheGameLogic->getShowBehindBuildingMarkers();
			if (ImGui::Checkbox("Behind-Building Markers", &behindBuildings))
				TheGameLogic->setShowBehindBuildingMarkers(behindBuildings);
			HelpMarker("Shows unit outlines when obscured by buildings.");
		}
#endif
	}

#if defined(RTS_DEBUG)
	if (ImGui::CollapsingHeader("Terrain & Water"))
	{
		if (ImGui::Button("Toggle Trackmarks"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_TRACKMARKS);
		}
		HelpMarker("Vehicle tire/tread marks on terrain.");

		if (ImGui::Button("Toggle Water Plane"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_WATERPLANE);
		}
		HelpMarker("Toggles water surface rendering.");

		if (ImGui::Button("Toggle Feather Water"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_TOGGLE_FEATHER_WATER);
		}
		HelpMarker("Toggles water edge feathering effect.");
	}

	if (ImGui::CollapsingHeader("Color Filters"))
	{
		if (ImGui::Button("BW / Wireframe"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_BW_VIEW);
		}
		HelpMarker("Cycles through black & white and wireframe view modes.");

		ImGui::SameLine();
		if (ImGui::Button("Red"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_RED_VIEW);
		}
		ImGui::SameLine();
		if (ImGui::Button("Green"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_GREEN_VIEW);
		}
		HelpMarker("Applies red or green color filter overlays.");

		if (ImGui::Button("Motion Blur Zoom"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_MOTION_BLUR_ZOOM);
		}
		HelpMarker("Toggles motion blur effect when zooming.");
	}

	if (ImGui::CollapsingHeader("Level of Detail"))
	{
		if (ImGui::Button("LOD -"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_LOD_DECREASE);
		}
		ImGui::SameLine();
		if (ImGui::Button("LOD +"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_LOD_INCREASE);
		}
		ImGui::SameLine();
		if (ImGui::Button("Cycle LOD"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_CYCLE_LOD_LEVEL);
		}
		HelpMarker("Adjust model detail level. Lower = less detail, higher = more polygons.");
	}

	if (ImGui::CollapsingHeader("Letterbox & UI"))
	{
		if (ImGui::Button("Toggle Letterbox"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_LETTERBOX);
		}
		HelpMarker("Shows cinematic black bars at top and bottom of screen.");

		if (ImGui::Button("Toggle Message Text"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_MESSAGE_TEXT);
		}
		HelpMarker("Toggles in-game message text overlay.");
	}
#endif // RTS_DEBUG
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// The remaining tabs require RTS_DEBUG for message types and GlobalData members.
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(RTS_DEBUG)

// Returns true if the game is in a multiplayer (non-skirmish) match.
static Bool IsMultiplayerGame()
{
	return TheGameLogic && TheGameLogic->isInMultiplayerGame();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TAB: Cheats & Gameplay
///////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawCheatsTab()
{
	if (IsMultiplayerGame())
	{
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Most cheats disabled in multiplayer");
		ImGui::Separator();
	}

	Player *localPlayer = ThePlayerList ? ThePlayerList->getLocalPlayer() : nullptr;

	if (ImGui::CollapsingHeader("Build", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (localPlayer)
		{
			bool instantBuild = localPlayer->buildsInstantly();
			if (ImGui::Checkbox("Instant Build", &instantBuild))
			{
				ForEachHumanPlayer([instantBuild](Player *p) { p->enableInstantBuild(instantBuild); });
			}
			HelpMarker("All structures and units are built instantly with no timer.");

			bool freeBuild = localPlayer->buildsForFree();
			if (ImGui::Checkbox("Free Build", &freeBuild))
			{
				ForEachHumanPlayer([freeBuild](Player *p) { p->enableFreeBuild(freeBuild); });
			}
			HelpMarker("All production costs $0. No money is deducted.");

			bool ignorePrereqs = localPlayer->ignoresPrereqs();
			if (ImGui::Checkbox("Ignore Prerequisites", &ignorePrereqs))
			{
				ForEachHumanPlayer([ignorePrereqs](Player *p) { p->enableIgnorePrereqs(ignorePrereqs); });
			}
			HelpMarker("Build any unit or structure without needing prerequisite buildings.");
		}

		bool specialPowerDelay = TheGlobalData && TheGlobalData->m_specialPowerUsesDelay != 0;
		if (ImGui::Checkbox("Special Power Delays", &specialPowerDelay))
		{
			if (TheGlobalData)
				TheWritableGlobalData->m_specialPowerUsesDelay = specialPowerDelay ? 1 : 0;
		}
		HelpMarker("When unchecked, general powers have no cooldown timer.");
	}

	if (ImGui::CollapsingHeader("Economy"))
	{
		if (localPlayer && !IsMultiplayerGame())
		{
			if (ImGui::Button("Add $10,000"))
			{
				Money *money = localPlayer->getMoney();
				if (money)
					money->deposit(10000);
			}
			HelpMarker("Adds 10,000 credits to your current funds.");
		}
	}

	if (ImGui::CollapsingHeader("Sciences & Rank"))
	{
		if (localPlayer)
		{
			if (ImGui::Button("Give All Sciences"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_GIVE_ALL_SCIENCES);
			}
			HelpMarker("Unlocks every general power in the science tree.");

			ImGui::SameLine();
			if (ImGui::Button("Give Science Points"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_GIVE_SCIENCEPURCHASEPOINTS);
			}
			HelpMarker("Adds spendable science purchase points.");

			Int rankLevel = localPlayer->getRankLevel();
			ImGui::Text("Rank Level: %d", rankLevel);
			ImGui::SameLine();
			if (ImGui::Button("+##rank"))
				localPlayer->setRankLevel(rankLevel + 1);
			ImGui::SameLine();
			if (ImGui::Button("-##rank") && rankLevel > 0)
				localPlayer->setRankLevel(rankLevel - 1);
			HelpMarker("Manually adjust your general's rank level.");
		}
	}

	if (ImGui::CollapsingHeader("Combat"))
	{
		if (!IsMultiplayerGame())
		{
			if (ImGui::Button("Toggle Hand of God"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_HAND_OF_GOD_MODE);
			}
			HelpMarker("Selected units deal 100% instant kill damage per hit.");

			if (ImGui::Button("Toggle Hurt Me"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_HURT_ME_MODE);
			}
			HelpMarker("Selected units take 10% of max HP damage per hit received.");

			if (ImGui::Button("Kill Selected"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_KILL_SELECTION);
			}
			HelpMarker("Instantly destroys all currently selected units.");

			ImGui::SameLine();
			if (ImGui::Button("Kill All Enemies"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_KILL_ALL_ENEMIES);
			}
			HelpMarker("Instantly destroys all enemy units and structures.");

			if (ImGui::Button("Veterancy +"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_GIVE_VETERANCY);
			}
			ImGui::SameLine();
			if (ImGui::Button("Veterancy -"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_TAKE_VETERANCY);
			}
			HelpMarker("Promotes or demotes selected units by one veterancy rank.");
		}
	}

	if (ImGui::CollapsingHeader("Teams & Win"))
	{
		if (!IsMultiplayerGame())
		{
			if (ImGui::Button("Switch Teams"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_SWITCH_TEAMS);
			}
			HelpMarker("Swaps your control to the next player/team.");

			ImGui::SameLine();
			if (ImGui::Button("Switch China/USA"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_SWITCH_TEAMS_BETWEEN_CHINA_USA);
			}
			HelpMarker("Swaps control between China and USA factions.");

			if (ImGui::Button("Instant Win"))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_WIN);
			}
			HelpMarker("Immediately wins the current match.");
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TAB: Debug Displays
///////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawDebugDisplaysTab()
{
	if (!TheGlobalData)
		return;

	if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Toggle Framerate Bar"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_GRAPHICALFRAMERATEBAR);
		}
		HelpMarker("Shows a graphical bar visualizing frame time distribution.");

		if (ImGui::Button("Toggle Debug Stats"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_DEBUG_STATS);
		}
		HelpMarker("Shows engine debug statistics (draw calls, object counts, etc).");
	}

	if (ImGui::CollapsingHeader("Game Systems"))
	{
		if (ImGui::Button("Particle Debug"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_PARTICLEDEBUG);
		}
		HelpMarker("Shows particle system emitter count and performance info.");

		if (ImGui::Button("Threat Debug"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_THREATDEBUG);
		}
		HelpMarker("Visualizes AI threat map values across the terrain.");

		if (ImGui::Button("Vision Debug"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_VISIONDEBUG);
		}
		HelpMarker("Shows line-of-sight and vision range debug info.");

		if (ImGui::Button("Projectile Debug"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_PROJECTILEDEBUG);
		}
		HelpMarker("Shows projectile trajectories and collision data.");

		if (ImGui::Button("Cash Map Debug"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_CASHMAPDEBUG);
		}
		HelpMarker("Visualizes resource value distribution on the map.");

		if (ImGui::Button("Supply Center Placement"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_SUPPLY_CENTER_PLACEMENT);
		}
		HelpMarker("Shows optimal supply center placement zones.");
	}

	if (ImGui::CollapsingHeader("AI"))
	{
		ImGui::Text("AI Debug Level: %d", (Int)TheGlobalData->m_debugAI);
		if (ImGui::Button("Cycle AI Debug Level"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_AI_DEBUG);
		}
		HelpMarker("Cycles through AI debug verbosity levels (0 = off).");
	}

	if (ImGui::CollapsingHeader("Selection & Objects"))
	{
		if (ImGui::Button("Debug Selection"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_DEBUG_SELECTION);
		}
		HelpMarker("Prints detailed info about selected objects to debug output.");

		if (ImGui::Button("Show Extents"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_SHOW_EXTENTS);
		}
		HelpMarker("Draws bounding boxes around all objects.");

		if (ImGui::Button("Show Health"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_SHOW_HEALTH);
		}
		HelpMarker("Shows numeric health values above all units.");

		if (ImGui::Button("Show Audio Locations"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_SHOW_AUDIO_LOCATIONS);
		}
		HelpMarker("Shows 3D positions of active audio emitters.");
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TAB: Audio
///////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawAudioTab()
{
	if (ImGui::CollapsingHeader("Playback", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Toggle All Sound"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_SOUND);
		}
		HelpMarker("Master sound on/off. Mutes all audio when toggled off.");

		if (ImGui::Button("Toggle Music"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_MUSIC);
		}
		HelpMarker("Enables or disables background music playback.");

		if (ImGui::Button("Next Track"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_MUSIC_NEXT_TRACK);
		}
		ImGui::SameLine();
		if (ImGui::Button("Prev Track"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_MUSIC_PREV_TRACK);
		}
		HelpMarker("Skip to the next or previous music track.");
	}

	if (ImGui::CollapsingHeader("Options"))
	{
		if (ImGui::Button("Toggle Military Subtitles"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_MILITARY_SUBTITLES);
		}
		HelpMarker("Shows text captions for unit voice responses.");

		if (ImGui::Button("Toggle Audio Debug"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_AUDIODEBUG);
		}
		HelpMarker("Shows debug info for active audio events and channels.");
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// TAB: Scripts & Diagnostics
///////////////////////////////////////////////////////////////////////////////////////////////////
static void DrawScriptsDiagnosticsTab()
{
	if (ImGui::CollapsingHeader("Run Scripts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextDisabled("Execute map scripts by slot number (F1-F9)");

		static const GameMessage::Type scriptMsgs[] = {
			GameMessage::MSG_META_DEMO_RUNSCRIPT1,
			GameMessage::MSG_META_DEMO_RUNSCRIPT2,
			GameMessage::MSG_META_DEMO_RUNSCRIPT3,
			GameMessage::MSG_META_DEMO_RUNSCRIPT4,
			GameMessage::MSG_META_DEMO_RUNSCRIPT5,
			GameMessage::MSG_META_DEMO_RUNSCRIPT6,
			GameMessage::MSG_META_DEMO_RUNSCRIPT7,
			GameMessage::MSG_META_DEMO_RUNSCRIPT8,
			GameMessage::MSG_META_DEMO_RUNSCRIPT9,
		};

		for (int i = 0; i < 9; ++i)
		{
			if (i > 0 && (i % 3) != 0)
				ImGui::SameLine();
			char label[16];
			snprintf(label, sizeof(label), "Script %d", i + 1);
			if (ImGui::Button(label, ImVec2(80, 0)))
			{
				if (TheMessageStream)
					TheMessageStream->appendMessage(scriptMsgs[i]);
			}
		}
	}

	if (ImGui::CollapsingHeader("Diagnostics"))
	{
		if (ImGui::Button("Dump Assets"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_DUMP_ASSETS);
		}
		HelpMarker("Writes all loaded asset names to the debug log.");

		if (ImGui::Button("Dump Player Objects"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_DUMP_PLAYER_OBJECTS);
		}
		HelpMarker("Writes local player's object list to the debug log.");

		if (ImGui::Button("Dump All Player Objects"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEBUG_DUMP_ALL_PLAYER_OBJECTS);
		}
		HelpMarker("Writes all players' object lists to the debug log.");

#ifdef DUMP_PERF_STATS
		if (ImGui::Button("Statistical Dump"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_PERFORM_STATISTICAL_DUMP);
		}
		HelpMarker("Dumps performance statistics to the debug log.");
#endif
	}

	if (ImGui::CollapsingHeader("Movies"))
	{
		if (ImGui::Button("Next Objective Movie"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_NEXT_OBJECTIVE_MOVIE);
		}
		HelpMarker("Plays the next briefing/objective movie in sequence.");

		if (ImGui::Button("Play Cameo Movie"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_PLAY_CAMEO_MOVIE);
		}
		HelpMarker("Plays the cameo portrait movie for the selected unit.");

		if (ImGui::Button("Toggle AVI Capture"))
		{
			if (TheMessageStream)
				TheMessageStream->appendMessage(GameMessage::MSG_META_DEMO_TOGGLE_AVI);
		}
		HelpMarker("Starts or stops recording the game view to an AVI file.");
	}
}

#endif // RTS_DEBUG


///////////////////////////////////////////////////////////////////////////////////////////////////
// Main Draw — tabbed window with all categories.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameImGuiDebugWindows::Draw()
{
	ImGui::SetNextWindowPos(ImVec2(300, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);

	if (!ImGui::Begin("Game Debug"))
	{
		ImGui::End();
		return;
	}

	if (ImGui::BeginTabBar("GameDebugTabs"))
	{
		// Camera and Rendering tabs available in all builds.
		if (ImGui::BeginTabItem("Camera"))
		{
			DrawCameraTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Rendering"))
		{
			DrawRenderingTab();
			ImGui::EndTabItem();
		}

#if defined(RTS_DEBUG)
		if (ImGui::BeginTabItem("Cheats"))
		{
			DrawCheatsTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Debug"))
		{
			DrawDebugDisplaysTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Audio"))
		{
			DrawAudioTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Scripts"))
		{
			DrawScriptsDiagnosticsTab();
			ImGui::EndTabItem();
		}
#else
		// Non-debug build: show a hint about enabling the full feature set.
		ImGui::Spacing();
		ImGui::TextWrapped(
			"Build with RTS_BUILD_OPTION_DEBUG=ON (win32-debug preset) "
			"to enable Cheats, Debug Displays, Audio, and Scripts tabs.");
#endif

		ImGui::EndTabBar();
	}

	ImGui::End();
}


void GameImGuiDebugWindows::Register()
{
	rts::ImGui::DebugMenu::SetGameDrawCallback(&GameImGuiDebugWindows::Draw);
}

#endif // RTS_HAS_IMGUI
