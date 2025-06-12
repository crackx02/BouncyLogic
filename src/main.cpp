
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "MinHook.h"

#include "Visualization.hpp"

constexpr uint8_t LineSegmentCount = 5;
constexpr float LineArcHeight = 1.0f;

// Do not change this limit - the game crashes if there are more than ~10-20k lines.
constexpr uint64_t LineCountLimit = 10000;
constexpr uintptr_t Offset_GenerateGrabbedConnectionLine = 0x02bcb70;

SM::Visualization** g_ppVisualization = nullptr;
static std::vector<SM::Visualization::ConnectionLine> g_vecGeneratedLines;

static void GenerateArc(const SM::Visualization::ConnectionLine& line, uint8_t segCount, float height) {
	Vec3 directionLength = line.end - line.start;
	
	if ( directionLength.length() <= 0.1f )
		return;

	Vec3 previousPoint = line.start;
	for ( uint8_t i = 1; i < segCount; ++i ) {
		float t = float(i) / float(segCount);
		
		Vec3 newPoint = line.start + (directionLength * t);
		newPoint.z += height * (t * (1.0f - t));

		g_vecGeneratedLines.emplace_back(previousPoint, newPoint, line.color);
		previousPoint = newPoint;
	}
	g_vecGeneratedLines.emplace_back(previousPoint, line.end, line.color);
}

static void(*O_GenerateGrabbedConnectionLine)(void*, void*, void*, void*) = nullptr;
static void H_GenerateGrabbedConnectionLine(void* self, void* p2, void* p3, void* p4) {
	O_GenerateGrabbedConnectionLine(self, p2, p3, p4);

	SM::Visualization* pVisualization = *g_ppVisualization;
	if ( pVisualization != nullptr ) {
		auto& vecConnectionLines = pVisualization->getConnectionLines();
		
		g_vecGeneratedLines.clear();
		uint64_t estimatedLineCount = vecConnectionLines.size() * LineSegmentCount;
		g_vecGeneratedLines.reserve(estimatedLineCount < LineCountLimit ? estimatedLineCount : LineCountLimit);
		
		for (auto& line : vecConnectionLines) {
			// Enforce a limit to prevent a game crash
			if ( (LineCountLimit - g_vecGeneratedLines.size()) < LineSegmentCount )
				break;
			GenerateArc(line, LineSegmentCount, LineArcHeight);
		}

		vecConnectionLines.clear();
		vecConnectionLines.swap(g_vecGeneratedLines);
	}
}

static bool Attach() {
	uintptr_t base = (uintptr_t)GetModuleHandle(0);

	g_ppVisualization = (SM::Visualization**)(base + SM::Visualization::Offset);

	if ( MH_Initialize() != MH_OK ) {
		MessageBoxA(nullptr, "MinHook Init failed", "BouncyLogic ERROR", MB_OK);
		return false;
	}

	if (MH_CreateHook((void*)(base + Offset_GenerateGrabbedConnectionLine), H_GenerateGrabbedConnectionLine, (LPVOID*)&O_GenerateGrabbedConnectionLine) != MH_OK) {
		MessageBoxA(nullptr, "MinHook CreateHook failed", "BouncyLogic ERROR", MB_OK);
		return false;
	}

	if ( MH_EnableHook((void*)(base + Offset_GenerateGrabbedConnectionLine)) != MH_OK ) {
		MessageBoxA(nullptr, "MinHook EnableHook failed", "BouncyLogic ERROR", MB_OK);
		return false;
	}

	return true;
}

static void Detach() {
	MH_Uninitialize();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		return Attach();
		break;
	}
	case DLL_PROCESS_DETACH:
		Detach();
	}
	return TRUE;
}
