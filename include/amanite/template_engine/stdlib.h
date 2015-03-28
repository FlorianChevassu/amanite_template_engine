#pragma once

#include <chaiscript/chaiscript.hpp>
#include <chaiscript/dispatchkit/dispatchkit.hpp>

namespace amanite {
	namespace script {
		namespace stdlib {

			/******************/
			/*    ostreams    */
			/******************/
			template<class T>
			std::ostream& print_ostream(std::ostream& os, const T& s) {
				return os << s;
			}

			template<>
			std::ostream& print_ostream(std::ostream& os, const std::time_t& s) {
				//ctime always print \n at the end of the string...
				std::string time = std::string(std::ctime(&s));
				return os << time.substr(0, time.length() - 1);
			}

			/*******************/
			/*    date time    */
			/*******************/
			std::time_t now() {
				return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			}

			chaiscript::ModulePtr create() {
				chaiscript::ModulePtr lib = chaiscript::ModulePtr(new chaiscript::Module());

				/**************************/
				/**    OUTPUT STREAMS    **/
				/**************************/
				lib->add(chaiscript::fun(&print_ostream<std::string>), "<<");
				lib->add(chaiscript::fun(&print_ostream<std::time_t>), "<<");
				lib->add(chaiscript::fun(&print_ostream<std::size_t>), "<<");


				lib->add(chaiscript::fun(&now), "now");

				return lib;
			}
		}
	}
}
