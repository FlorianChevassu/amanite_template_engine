#pragma once

namespace amanite{
	namespace tools {
		bool startsWith(const std::string& str, const std::string& substr) {
			return str.compare(0, substr.length(), substr) == 0;
		}

		std::vector<std::string> split(const std::string& str, char delim){
			std::vector<std::string> result;
			std::istringstream iss(str);
			std::string s;
			while (std::getline(iss, s, delim)) {
				result.push_back(s);
			}
			return result;
		}
	}
}