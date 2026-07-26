#pragma once

#include <string>
#include <unordered_map>

class ConfigManager {
public:
	static ConfigManager& Instance();

	void load();
	void Save();

	bool Get(const std::string& name, bool defaultValue = true);
	void Set(const std::string& name, bool value);

private:
	std::unordered_map<std::string, bool> m_data;
};