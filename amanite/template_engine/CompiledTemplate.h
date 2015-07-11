#pragma once

#include "Node.h"

#include <string>
#include <list>
#include <map>
#include <deque>

namespace amanite {
	namespace template_engine {

		class CompiledTemplate {
			std::list <Node> m_nodes;
			std::map <std::string, std::list<Node>> m_deps;

		public:

			std::list <Node>& getNodes(){
				return m_nodes;
			}
			const std::list <Node>& getNodes() const{
				return m_nodes;
			}


			std::map <std::string, std::list<Node>>& getDeps(){
				return m_deps;
			}
			const std::map <std::string, std::list<Node>>& getDeps() const{
				return m_deps;
			}


		};
	}
}