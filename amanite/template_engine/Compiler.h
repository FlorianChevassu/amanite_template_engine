#pragma once

#include <string>
#include <list>
#include <stack>
#include <map>
#include <istream>
#include <fstream>
#include <sstream>


#include <boost/filesystem.hpp>
#include <chaiscript/utility/utility.hpp>
#include "scriptEngine.h"
#include "amanite/tools/StringUtils.h"

#include "CompiledTemplate.h"
#include "Node.h"

namespace amanite {
	namespace template_engine {


		class Compiler {
		private:
			/**
		* Template engine configuration class.
		*/
			struct Configuration {
				Configuration() :
						nodeStartTag("{{"),
						nodeEndTag("}}"),
						sectionNodeStartTag("#"),
						scopeNodeStartTag("/"),
						scopeNodeEndTag("\\"),
						partialNodeStartTag(">"),
						localPartialNodeStartTag("<"),
						scriptNodeStartTag("="),
						commentNodeStartTag("!") {

				}

				boost::filesystem::path templatePath;
				std::string nodeStartTag;
				std::string nodeEndTag;
				std::string sectionNodeStartTag;
				std::string scopeNodeStartTag;
				std::string scopeNodeEndTag;
				std::string partialNodeStartTag;
				std::string localPartialNodeStartTag;
				std::string scriptNodeStartTag;
				std::string commentNodeStartTag;
			};

			Configuration m_configuration;

		public:
			Configuration& getConfiguration() {
				return m_configuration;
			}

			const Configuration& getConfiguration() const {
				return m_configuration;
			}

		private:

			std::map<std::string, std::list<Node>> m_compiledTemplates;
			std::set<std::string> m_compilingTemplates;


			script::ScriptEngine m_scriptingEngine;

		public:

			CompiledTemplate compile(const std::string& fileName) {
				CompiledTemplate res;
				res.getNodes() = internalCompile(fileName);
				res.getDeps() = m_compiledTemplates;
				return res;
			}

			CompiledTemplate compile(std::istream& is) {
				CompiledTemplate res;
				res.getNodes() = internalCompile(is);
				res.getDeps() = m_compiledTemplates;
				return res;
			}

		private:
			/**
			* Compilation of a file
			*/
			std::list<Node> internalCompile(const std::string& fileName) {
				using namespace boost::filesystem;
				path p = getConfiguration().templatePath;
				p.append(fileName.begin(), fileName.end());
				if(!exists(p)) {
					std::stringstream ss;
					ss << "The file " << fileName << " does not exist.";
					throw std::invalid_argument(ss.str());
				}
				std::ifstream ifs;
				ifs.open(p.string());
				if(m_compilingTemplates.find(fileName) == m_compilingTemplates.end()) {
					m_compilingTemplates.insert(fileName);
					m_compiledTemplates[fileName] = internalCompile(ifs);
					m_compilingTemplates.erase(m_compilingTemplates.find(fileName));
				}
				return m_compiledTemplates[fileName];
			}

			std::list<Node> internalCompile(std::istream& is, const std::string& expectedEndTag = "") {
				std::list<Node> res;

				while(is.good()) {
					res.emplace_back(Node::Type::text, extractText(is));

					std::string node = extractNode(is);
					//the size of the string "node" may be equal to zero if the last node was a text node.
					if(node.size() > 0) {
						//detect the node type
						const Configuration& config = getConfiguration();
						if(tools::startsWith(node, config.sectionNodeStartTag)) {
							res.splice(res.end(), compileSectionNode(node, is));
						} else if(tools::startsWith(node, config.scopeNodeStartTag)) {
							res.push_back(compileStartScopeNode(node, is));
							return res;
						} else if(tools::startsWith(node, config.scopeNodeEndTag)) {
							res.push_back(compileEndScopeNode(node, is));
							return res;
						} else if(tools::startsWith(node, config.partialNodeStartTag)) {
							res.push_back(compilePartialNode(node, is));
						} else if(tools::startsWith(node, config.localPartialNodeStartTag)) {
							compileLocalPartial(node, is);
						} else if(tools::startsWith(node, config.scriptNodeStartTag)) {
							res.push_back(compileScriptNode(node, is));
						} else if(tools::startsWith(node, config.commentNodeStartTag)) {
							//comment. Nothing to do
						} else {
							res.splice(res.end(), compileVariableNode(node, is));
						}
					}
				}
				return res;
			}

			std::string extractText(std::istream& is) {
				std::string::size_type startIndex;
				std::stringstream ss;
				std::string s;

				std::getline(is, s);
				while((startIndex = s.find("{{")) == std::string::npos && !is.eof()) {
					ss << s << std::endl;
					std::getline(is, s);
				}

				//if we are at the end of the file
				if(startIndex == std::string::npos && is.eof()) {
					ss << s;
				} else {
					//we found the characters "{{".
					ss << s.substr(0, startIndex);

					//"unget" the characters that that we do not need, so that it will be handled by the next iteration.
					for(std::size_t i = 0; i < s.substr(startIndex).size() + 1; ++i)
						is.unget();
				}

				return ss.str();
			}

			std::string extractNode(std::istream& is) {
				std::string::size_type startIndex;
				std::string::size_type endIndex;
				std::stringstream ss;
				std::string s;

				std::getline(is, s);
				while((startIndex = s.find("{{")) == std::string::npos && !is.eof()) {
					std::getline(is, s);
				}

				//if we are at the end of the file
				if(startIndex == std::string::npos && is.eof()) {
					return "";
				} else {
					//we found the characters "{{".
					//check if "}}" are on the same line
					endIndex = s.find("}}");
					if(endIndex == std::string::npos && !is.eof()) {
						ss << s.substr(startIndex + 2);
						while(std::getline(is, s), (endIndex = s.find("}}")) == std::string::npos && !is.eof()) {
							ss << s << std::endl;
						}

						ss << s.substr(0, endIndex) << std::endl;
						//"unget" the characters that that we do not need, so that it will be handled by the next iteration.
						for (std::size_t i = 0; i < s.substr(endIndex + 2).size() + 1; ++i)
							is.unget();
						return ss.str();
					} else {
						int numCharToUnGet = s.substr(endIndex + 2).size();
						if(!is.eof()){
							++numCharToUnGet;
						}

						//"unget" the characters that that we do not need, so that it will be handled by the next iteration.
						for(int i = 0; i < numCharToUnGet; ++i)
							is.unget();

						return s.substr(startIndex + 2, endIndex - startIndex - 2);
					}
				}
			}


			std::list<Node> compileSectionNode(const std::string node, std::istream& is) {
				std::istringstream iss(node);

				std::deque<std::string> tags{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
				std::string key = tags[0].substr(1);
				tags.pop_front();

				//if key contains points ('.'), split it into multiple sections
				std::vector<std::string> sections = tools::split(key, '.');

				//Node res(Node::Type::section, sections[0], tags);
				std::list<Node> res;
				std::list<Node>* currentNodeList = &res;
				int currentContextOffset = 0;
				for(auto sec : sections) {
					if(sec.compare("parent") == 0) {
						if(currentContextOffset > 0)
							tags.pop_back();
						tags.push_back("contextOffset=" + std::to_string(++currentContextOffset));
					} else {
						currentNodeList->push_back({Node::Type::section, sec, tags});
						currentNodeList->push_back({Node::Type::endScope, sec});
						currentNodeList = &(currentNodeList->front().children);
					}
				}

				//compile the stream until the end of section tag has been found
				*currentNodeList = internalCompile(is, sections.back());

				//The last node is the "endScope" node. It should be at the same level as the "section" node.
				currentNodeList->pop_back();
				return res;
			}

			Node compileStartScopeNode(const std::string node, std::istream& is) {
				std::istringstream iss(node);

				std::deque<std::string> tags{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
				std::string key = tags[0].substr(1);
				tags.pop_front();

				return {Node::Type::startScope, key, tags};
			}

			Node compileEndScopeNode(const std::string node, std::istream& is) {
				std::istringstream iss(node);

				std::deque<std::string> tags{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
				std::string key = tags[0].substr(1);
				tags.pop_front();

				return {Node::Type::endScope, key};
			}

			Node compilePartialNode(const std::string node, std::istream& is) {
				std::istringstream iss(node);

				std::deque<std::string> tags{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
				std::string key = tags[0].substr(1);
				tags.pop_front();


				//partial defined in the file named as "key"
				if(m_compiledTemplates.find(key) == m_compiledTemplates.end()
						&& m_compilingTemplates.find(key) == m_compilingTemplates.end()) {
					compile(key);
				}

				return {Node::Type::partial, key, tags};
			}

			void compileLocalPartial(const std::string nodeContext, std::istream& is) {
				std::istringstream iss(nodeContext);

				std::deque<std::string> tags{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
				std::string key = tags[0].substr(1);
				tags.pop_front();

				//definition of a local partial
				//if the name already exists, we omit this declaration
				if(m_compiledTemplates.find(key) == m_compiledTemplates.end()) {
					m_compilingTemplates.insert(key);
					m_compiledTemplates[key].push_back({Node::Type::startScope, key, tags});
					m_compiledTemplates[key].splice(std::end(m_compiledTemplates[key]), internalCompile(is, key));
					//endScope is useless here because it will be handled by the end tag of the local partial
					//m_compiledTemplates[key].push_back({Node::Type::endScope, "", tags});
				} else {
					//TODO : WARNING
				}
			}

			Node compileScriptNode(const std::string node, std::istream& is) {
				return {Node::Type::code, node.substr(1)};
			}


			std::list<Node> compileVariableNode(const std::string node, std::istream& is) {
				std::istringstream iss(node);

				std::deque<std::string> tags{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
				std::string key = tags[0];
				tags.pop_front();

				//if key contains points ('.'), split it into multiple sections
				std::vector<std::string> sections = tools::split(key, '.');
				key = sections.back();
				sections.pop_back();

				std::list<Node> res;
				std::list<Node>* currentNodeList = &res;
				int currentContextOffset = 0;
				for(auto sec : sections) {
					if(sec.compare("parent") == 0) {
						if(currentContextOffset > 0)
							tags.pop_back();
						tags.push_back("contextOffset=" + std::to_string(++currentContextOffset));
					} else {
						throw std::runtime_error("Variable tag must be of form \"parent.[...].variableName\"");
					}
				}

				res.emplace_back(Node::Type::var, key, tags);
				return res;
			}


		};
	}
}
