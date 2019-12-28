#pragma once
#include <vector>
#include <fstream>
#include <string>
#include "assert.h"

namespace shared
{
	class GameSettings
	{
	public:
		class FileSetting
		{
		public:
			// Float constructor. Internally sets the type of the setting
			FileSetting(const char* psGroup, const char* psName, float fValue);

			// String constructor.Internally sets the type of the setting
			FileSetting(const char* psGroup, const char* psName, const char* sValue);

			bool IsFloat() const { return m_Type == kSettingType_Float; }
			bool IsString() const { return m_Type == kSettingType_String; }

			// Gets the section the setting was stored under
			const char*         GetGroup()  const { return m_Group.c_str(); }
			const char*         GetName()   const { return m_Name.c_str(); }
			float               GetFloat()  const { assert(m_Type == kSettingType_Float, "Underlying data type not supported."); return m_ValueFloat; }
			const char*         GetString() const { assert(m_Type == kSettingType_String, "Underlying data type not supported."); return m_ValueString.c_str(); }

			template <typename T>
			T GetNumericAs() const
			{
				assert(m_Type == kSettingType_Float, "Underlying data type not supported.");
				return static_cast<T>(m_ValueFloat);
			}

			void SetFloat(float fData)
			{
				assert(m_Type == kSettingType_Float, "Underlying data type not supported.");
				m_ValueFloat = fData;
			}

			void SetString(const std::string& strData)
			{
				assert(m_Type == kSettingType_String, "Underlying data type not supported.");
				m_ValueString = strData;
			}

		protected:
			enum SettingType
			{
				kSettingType_Float,
				kSettingType_String
			};

			std::string m_Group;
			std::string m_Name;

			SettingType m_Type;
			float       m_ValueFloat;
			std::string m_ValueString;
		};

		typedef std::vector<FileSetting>  List;

		GameSettings(const char* psFilename = nullptr, List* defaultSettings = nullptr);
		~GameSettings();

		void Save();
		void Load(List* settingTable);
		void Set(const char* group, const char* name, const char* value);

		void ClearGroup(const char* group);

		const FileSetting*  Get(const char* group, const char* name) const;
		FileSetting*        Get(const char* group, const char* name);

	protected:
		void LoadFile(const char* filename, bool bCreateIfMissing = true);

		const char*  m_psFilename;
		std::fstream m_File;
		std::vector<FileSetting> m_Settings;
	};
}