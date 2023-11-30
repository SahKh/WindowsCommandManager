#include "system_command_handler.h"
#include <windows.h>
#include <powrprof.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <cassert>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "PowrProf.lib")
#pragma comment(lib, "Uuid.lib")

//MessageLogger

void logToFile(const std::string& message) {
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	auto now_tm = *std::localtime(&now_time_t);

	// Open log file
	std::ofstream logFile("handlerLog.txt", std::ios::app); 
	if (logFile.is_open()) {
		logFile << "[" << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
		logFile.close();
	}
}

//VolumeControl
void VolumeControl::adjustVolume(int newVolumeLevel) {
	if (newVolumeLevel >= 0 && newVolumeLevel <= 100) {
		float ScalarVolume = static_cast<float>(newVolumeLevel) / 100.00f;
		//Com Init
		CoInitialize(nullptr);

		//Default audio endpoint
		IMMDeviceEnumerator* enumer = nullptr;
		CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&enumer);
		IMMDevice* device = nullptr;
		enumer->GetDefaultAudioEndpoint(eRender, eConsole, &device);

		IAudioEndpointVolume* endpointVolume = nullptr;
		device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&endpointVolume);
		endpointVolume->SetMasterVolumeLevelScalar(ScalarVolume, nullptr);

		endpointVolume->Release();
		device->Release();
		enumer->Release();
		CoUninitialize();
	}
}
bool VolumeControl::handleCommand(const std::string& command) {
	logToFile("VolumeControl received command: " + command);
	//extract volume level
	std::istringstream iss(command);
	std::string volumeLevelStr;
	iss >> volumeLevelStr;

	int volumeLevel;
	try {
		volumeLevel = std::stoi(volumeLevelStr);
	}
	catch (std::invalid_argument& e) { 
		std::string errorMsg = "VolumeControl error: " + std::string(e.what());
		logToFile(errorMsg);
		return false;
	}

	adjustVolume(volumeLevel);
	
	return true;
}

std::string VolumeControl::getName() const { return "VolumeControl";}

//MediaControl

void MediaControl::play_pause() {
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;

	// Keypress for Play/Pause
	input.ki.wVk = VK_MEDIA_PLAY_PAUSE; 

	//keydown 
	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));

	//keyup
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}



void MediaControl::skipForward() {
	INPUT input = { 0 };
	input.type = INPUT_KEYBOARD;

	// Keypress for skipForward
	input.ki.wVk = VK_MEDIA_NEXT_TRACK;

	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));

	input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &input, sizeof(INPUT));
}

void MediaControl::skipBackward() {
	INPUT input{ 0 };
	input.type = INPUT_KEYBOARD;

	// Keypress for skipBackward
	input.ki.wVk = VK_MEDIA_PREV_TRACK;

	input.ki.dwFlags = 0;
	SendInput(1, &input, sizeof(INPUT));
}

bool MediaControl::handleCommand(const std::string& command) {
	logToFile("MediaControl received command: " + command);

	if (command == "play" || command == "pause") {
		play_pause();
	}
	else if (command == "next") {
		skipForward();
	}
	else if (command == "prev") {
		skipBackward();
	}
	else {
		logToFile("MediaControl received unknown command.");
		return false;
	}
	logToFile("MediaControl successfully executed command: " + command);
	return true;
}
std::string MediaControl::getName() const { return "MediaControl"; }

//ShutdownProt
bool ShutdownProtocol::shutdownInit() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	// get process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		logToFile("OpenProcessToken failed");
		return false;
	}

	// get LUID
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS) {
		logToFile("AdjustTokenPrivileges failed");
		return false;
	}

	// shutdown the system and force all applications to close. 
	if (!ExitWindowsEx(EWX_SHUTDOWN, 0)) {
		logToFile("Failed to initiate shutdown: " + std::to_string(GetLastError()));
		return false;
	}

	return true;
}
bool ShutdownProtocol::handleCommand(const std::string& command) {
	if (command == "shutdown") {
		return shutdownInit();
	}
	else {
		return false;
	}
}
std::string ShutdownProtocol::getName() const { return "ShutdownProtocol"; }

//SleepProt
bool SleepProtocol::sleepInit() {
	try {
		if (SetSuspendState(FALSE, TRUE, FALSE) != 0) {
			return true;
		}
		else {
			logToFile("Failed to initiate sleep");
			return false;
		}
	}
	catch (const std::exception& e) {
		logToFile("Error in SleepProtocol::sleepInit: " + std::string(e.what()));
		return false;
	}
}
bool SleepProtocol::handleCommand(const std::string& command) {
	if (command == "sleep") {
		return sleepInit();
	}
	else {
		return false;
	}
}
std::string SleepProtocol::getName() const { return "SleepProtocol"; }

//RestartProt
bool RestartProtocol::restartInit() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	// get process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		logToFile("OpenProcessToken failed");
		return false;
	}

	// get LUID
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS) {
		logToFile("AdjustTokenPrivileges failed");
		return false;
	}

	// Restart the system and force all applications to close. 
	if (!ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0)) {
		logToFile("Failed to initiate restart: " + std::to_string(GetLastError()));
		return false;
	}

	return true;
}

bool RestartProtocol::handleCommand(const std::string& command) {
	if (command == "restart") {
		return restartInit();
	}
	else {
		return false;
	}
}
std::string RestartProtocol::getName() const { return "RestartProtocol"; }

//InitFactory
SystemCommandHandlerPtr SystemCommandHandler_initializer::createCommandHandler(const std::string& commandType) const {
	if (commandType == "VolumeControl") {
		return std::make_shared<VolumeControl>();
	}
	else if (commandType == "MediaControl") {
		return std::make_shared<MediaControl>();
	}
	else if (commandType == "Shutdown") {
		return std::make_shared<ShutdownProtocol>();
	}
	else if (commandType == "Sleep") {
		return std::make_shared<SleepProtocol>();
	}
	else if (commandType == "Restart") {
		return std::make_shared<RestartProtocol>();
	}
	else {
		return nullptr;
	}
}                                                                                              

