#include "gamesettings.h"
#include <unordered_map>

shared::GameSettings::FileSetting::FileSetting(
	const char* psGroup, const char* psName, float fValue
) :
	m_Group(psGroup),
	m_Name(psName)
{
	m_Type = kSettingType_Float;
	m_ValueFloat = fValue;
}

shared::GameSettings::FileSetting::FileSetting(
	const char* psGroup, const char* psName, const char* sValue
) :
	m_Group(psGroup),
	m_Name(psName),
	m_ValueFloat(0.0f)
{
	m_Type = kSettingType_String;
	m_ValueString = sValue;
}

void shared::GameSettings::Load(List* settingTable)
{
	for (auto setting = settingTable->begin(); setting != settingTable->end(); ++setting)
	{
		m_Settings.push_back(*setting);
	}
}

void shared::GameSettings::ClearGroup(const char* group)
{
	for (auto it = m_Settings.begin(); it != m_Settings.end();)
	{
		if (it->GetGroup() == group)
		{
			it = m_Settings.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void shared::GameSettings::Set(const char* group, const char* name, const char* value)
{
	FileSetting* pExisting = Get(group, name);

	if (pExisting == nullptr)
	{
		try {
			float fValueFloat = std::stof(value);
			m_Settings.push_back({ group, name, fValueFloat });
		}
		catch (std::exception&)
		{
			m_Settings.push_back({ group, name, value });
		}
		return;
	}

	try {
		float fValueFloat = std::stof(value);
		pExisting->SetFloat(fValueFloat);
	}
	catch (std::exception&)
	{
		pExisting->SetString(value);
	}
}

void shared::GameSettings::Save()
{
	// Truncate the file for new data.
	m_File.close();
	m_File.open(m_psFilename, std::ios::out | std::ios::trunc);
	m_File.imbue(std::locale("C"));

	std::unordered_map<std::string, std::vector<FileSetting*>> m_SettingList;

	for (auto it = m_Settings.begin(); it != m_Settings.end(); ++it)
	{
		m_SettingList[it->GetGroup()].push_back(&(*it));
	}

	for (auto group = m_SettingList.begin(); group != m_SettingList.end(); ++group)
	{
		m_File << "[" << group->first << "]" << std::endl;

		for (auto it = group->second.begin(); it != group->second.end(); ++it)
		{
			FileSetting* setting = (*it);
			m_File << setting->GetName() << "=";

			if (setting->IsFloat())
			{
				m_File << setting->GetFloat();
			}
			else
			{
				m_File << setting->GetString();
			}

			m_File << std::endl;
		}

		m_File << std::endl;
	}
	m_File.close();
	m_File.open(m_psFilename, std::ios::in | std::ios::out);
}

enum ETokenResponse
{
	kTokenResponse_Append,
	kTokenResponse_SectionStart,
	kTokenResponse_SectionFinish,
	kTokenResponse_ValueStart
};

ETokenResponse ParseToken(char cToken)
{
	ETokenResponse result;

	switch (cToken)
	{
	case '[':
		result = kTokenResponse_SectionStart;
		break;
	case ']':
		result = kTokenResponse_SectionFinish;
		break;
	case '=':
		result = kTokenResponse_ValueStart;
		break;
	default:
		result = kTokenResponse_Append;
	}
	return result;
}

shared::GameSettings::FileSetting* shared::GameSettings::Get(const char* group, const char* name)
{
	for (auto it = m_Settings.begin(); it != m_Settings.end(); ++it)
	{
		if (strcmp(it->GetGroup(), group) == 0 && strcmp(it->GetName(), name) == 0)
		{
			return &(*it);
		}
	}

	return nullptr;
}

const shared::GameSettings::FileSetting* shared::GameSettings::Get(const char* group, const char* name) const
{
	for (auto it = m_Settings.begin(); it != m_Settings.end(); ++it)
	{
		if (strcmp(it->GetGroup(), group) == 0 && strcmp(it->GetName(), name) == 0)
		{
			return &(*it);
		}
	}

	return nullptr;
}

void shared::GameSettings::LoadFile(const char* filename, bool bCreateIfMissing)
{
	m_File.open(filename, std::ios::in | std::ios::out);

	if (!m_File.is_open() && bCreateIfMissing)
	{
		m_File.open(filename, std::fstream::binary | std::fstream::trunc | std::fstream::out);
		Save();
		return;
	}

	if (!m_File.is_open())
	{
		throw std::runtime_error("Settings file could not be opened.");
	}

	// Process file entries.
	std::string line;
	std::string active_section;

	while (std::getline(m_File, line))
	{
		bool bIsSection = false;

		if (line.empty())
			continue;

		std::string name;
		std::string value;

		bool bReadingValue = false;

		for (size_t i = 0; i < line.size(); ++i)
		{
			const char ch = line[i];

			ETokenResponse response = ParseToken(ch);

			if (response == kTokenResponse_SectionStart)
			{
				bIsSection = true;
				continue;
			}
			else if (response == kTokenResponse_SectionFinish)
			{
				active_section = name;
				break;
			}
			else if (response == kTokenResponse_ValueStart && !bIsSection && !bReadingValue)
			{
				bReadingValue = true;
			}
			else
			{
				if (bIsSection || !bReadingValue)
				{
					name += ch;
				}
				else if (bReadingValue)
				{
					value += ch;

					if (i == line.size() - 1)
					{
						// End of line, so try to insert the new setting into
						// the current settings list.
						// Try to cast string into float for float storage.
						FileSetting* pExisting;

						if ((pExisting = Get(active_section.c_str(), name.c_str())) == nullptr)
						{
							try {
								float fValueFloat = std::stof(value);
								m_Settings.push_back({ active_section.c_str(), name.c_str(), fValueFloat });
							}
							catch (std::exception&)
							{
								m_Settings.push_back({ active_section.c_str(), name.c_str(), value.c_str() });
							}
						}
						else
						{
							if (pExisting->IsFloat())
							{
								try {
									pExisting->SetFloat(std::stof(value));
								}
								catch (std::exception&)
								{
									pExisting->SetString(value);
								}
							}
							else
							{
								pExisting->SetString(value);
							}
						}

					}
				}
			}
		}
	}
}

shared::GameSettings::~GameSettings()
{
	Save();
}

shared::GameSettings::GameSettings(const char* psFilename, List* defaultSettings)
	: m_psFilename(psFilename)
{
	if (defaultSettings != nullptr)
	{
		Load(defaultSettings);
	}

	LoadFile(psFilename);

	for (auto it = m_Settings.begin(); it != m_Settings.end(); ++it)
	{
		if (it->IsFloat())
		{
			DbgLog("%s::%s = %d", it->GetGroup(), it->GetName(), it->GetNumericAs<std::uint32_t>());
		}
		else
		{
			DbgLog("%s::%s = %s", it->GetGroup(), it->GetName(), it->GetString());
		}
	}
}