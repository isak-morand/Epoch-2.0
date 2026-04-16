#pragma once
#include <string>
#include <unordered_map>

namespace Epoch
{
	class Args
	{
	public:
		Args(int aArgc, char** aArgv)
		{
			Parse(aArgc, aArgv);
		}

		bool Has(const std::string& aKey) const
		{
			return m_Args.contains(aKey);
		}

		std::string GetString(const std::string& aKey, const std::string& aDefaultValue = "") const
		{
			if (!Has(aKey)) return aDefaultValue;
			return m_Args.at(aKey);
		}

		int GetInt(const std::string& aKey, int aDefaultValue = 0) const
		{
			if (!Has(aKey)) return aDefaultValue;
			return std::stoi(m_Args.at(aKey));
		}

		bool GetBool(const std::string& aKey, bool aDefaultValue = false) const
		{
			if (!Has(aKey)) return aDefaultValue;

			const std::string& v = m_Args.at(aKey);
			return (v == "true" || v == "1" || v.empty());
		}

	private:
		void Parse(int aArgc, char** aArgv)
		{
			for (int i = 1; i < aArgc; ++i)
			{
				std::string arg = aArgv[i];

				if (arg.rfind("--", 0) == 0)
				{
					arg = arg.substr(2);

					size_t eq = arg.find('=');
					if (eq != std::string::npos)
					{
						m_Args[arg.substr(0, eq)] = arg.substr(eq + 1);
					}
					else
					{
						if (i + 1 < aArgc && std::string(aArgv[i + 1]).rfind("--", 0) != 0)
						{
							m_Args[arg] = aArgv[++i];
						}
						else
						{
							m_Args[arg] = "";
						}
					}
				}
			}
		}

	private:
		std::unordered_map<std::string, std::string> m_Args;
	};
}
