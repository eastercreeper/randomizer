#include "ConfigManager.h"

#include <fstream>

#include <filesystem>

#include <windows.h>

static std::string GetConfigPath()
{
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	return std::filesystem::path(path).parent_path().string() + "\\config.txt";
}

ConfigManager& ConfigManager::Instance() {
	static ConfigManager inst;
	return inst;
}

void ConfigManager::load() {

	m_data.clear();

	std::ifstream file(GetConfigPath());
	if (!file.is_open()) return;

	std::string name;
	bool value;

	while (file >> name >> value) {
		m_data[name] = value;
	}
}

void ConfigManager::Save() {
	std::ofstream file(GetConfigPath());

	for (const auto& [name, value] : m_data) {
		file << name << " " << value << "\n";
	}
}
bool ConfigManager::Get(const std::string & name, bool defaultValue) {
	auto it = m_data.find(name);
	if (it == m_data.end()) return defaultValue;

	return it->second;
}

void ConfigManager::Set(const std::string& name, bool value) {
	m_data[name] = value;
}