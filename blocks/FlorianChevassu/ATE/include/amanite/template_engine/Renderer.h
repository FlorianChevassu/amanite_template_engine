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
#include "Node.h"
#include "stringutils.h"

namespace amanite {
	namespace template_engine {
		class CompiledTemplate;

		class Renderer {
			script::ScriptEngine m_scriptingEngine;

		public:
			template<class Context>
			void render(const Context& c, std::ostream& os, const CompiledTemplate& tmpl, const Context* parentContext = nullptr) {
				render(c, os, tmpl.getNodes(), tmpl.getDeps(), parentContext);
			}
		private:
			template<class Context>
			void render(const Context& c, std::ostream& os, const std::list<Node>& tmpl, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext = nullptr) {
				m_scriptingEngine.registerVariable(os, "out");
				m_scriptingEngine.registerVariable(c, "context");
				if(parentContext != nullptr)
					m_scriptingEngine.registerVariable(*parentContext, "parentContext");


				std::for_each(tmpl.begin(), tmpl.end(), [&](const Node& item) {
					switch(item.type) {
						case Node::Type::text:
							if(!getState().skipText)
								os << item.value;
							break;
						case Node::Type::var: {
							pushState();
							applyTags(item.tags);
							const Context* currentContext = &c;
							for(int i = 0; i < getState().contextOffset; ++i) {
								if(currentContext->is_root()) {
									throw std::runtime_error("ROOT does not have parents");
								}
								currentContext = &currentContext->getParentContext();
							}
							const Context& currentContextRef = *currentContext;

							os << currentContextRef[item.value].getAsString();
							popState();
						}
							break;
						case Node::Type::section: {
							//reduce scope of variable secItems to the local case to avoid compiler error.
							pushState();
							applyTags(item.tags);
							const Context* currentContext = &c;
							for(int i = 0; i < getState().contextOffset; ++i) {
								if(currentContext->is_root()) {
									throw std::runtime_error("ROOT does not have parents");
								}
								currentContext = &currentContext->getParentContext();
							}

							const Context& currentContextRef = *currentContext;
							if(currentContextRef[item.value].isArray()) {
								auto& secItems = currentContextRef[item.value].getAsArray();
								std::for_each(secItems.begin(), secItems.end(), [&](const Context& secIt) {
									render<Context>(secIt, os, item.children, deps, &currentContextRef);
								});
							} else if(c[item.value].isObject()) {
								render<Context>(currentContextRef[item.value], os, item.children, deps, &currentContextRef);
							}
						}
							break;
						case Node::Type::partial:
							render<Context>(c, os, deps.find(item.value)->second, deps, parentContext);
							break;
						case Node::Type::code:
							m_scriptingEngine.eval(item.value);
							break;
						case Node::Type::endScope:
							popState();
							break;
						case Node::Type::startScope:
							pushState();
							applyTags(item.tags);
							break;
						default:
							//should never happen. The compilation step should detect problems
							assert(true);
					}
				});
			}

			script::ScriptEngine& getScriptEngine() {
				return m_scriptingEngine;
			}


			template<class Context>
			void registerContext() {
				m_scriptingEngine.registerClass<Context>("Context");
				m_scriptingEngine.registerFunction(&Context::get, "get");
				m_scriptingEngine.registerFunction(&Context::getParentContext, "getParentContext");
				m_scriptingEngine.registerFunction(&Context::operator[], "[]");
			}

			template<class C>
			void registerClass(const std::string& className) {
				m_scriptingEngine.registerClass<C>(className);
			}


			template<class T>
			void registerFunction(const T& f, const std::string& funcName) {
				m_scriptingEngine.registerFunction(f, funcName);
			}

			template<class T>
			void registerVariable(T& v, const std::string& varName) {
				registerVariable(v, varName);
			}


		private:

			/**
		* Engine state stack.
		* This class contains the information related to the state of the engine.
		*/
			class EngineState {
			public:
				//tags
				enum Tag {
					SKIP_TEXT,
					VERBATIM,
					CONTEXT_OFFSET
				};

				static Tag getTag(const std::string& tagStr) {
					Tag res;
					if(tagStr.compare("skipText") == 0)
						res = SKIP_TEXT;
					else if(tagStr.compare("verbatim") == 0)
						res = SKIP_TEXT;
					else if(tagStr.compare("contextOffset") == 0)
						res = CONTEXT_OFFSET;
					else
						throw std::runtime_error("BAD TAG" + tagStr);

					return res;
				}


				//flags
				bool skipText = false;
				bool verbatim = false;

				int contextOffset = 0;

				/**
			* Apply the boolean tag \var tag to the state. If negate is set to true, the tag will have the inverse behavior.
			*/
				void applyTag(Tag tag, bool negate = false) {
					switch(tag) {
						case SKIP_TEXT:
							skipText = !negate;
							break;
						case VERBATIM:
							verbatim = !negate;
							break;
					}
				}

				/**
			* Apply the value tag \var tag to the state with value \var value.
			*/
				void applyTag(Tag tag, int value) {
					switch(tag) {
						case CONTEXT_OFFSET:
							contextOffset = value;
							break;
					}
				}

				/**
			* Reset a particular tag.
			*/
				void resetTag(Tag tag) {
					switch(tag) {
						case CONTEXT_OFFSET:
							contextOffset = 0;
							break;
					}
				}
			};

			std::stack <EngineState> m_engineStateStack;

			/**
		* Add a state to the engine state stack, and reset tags that are not "heritable".//TODO : heritable ???
		*/
			void pushState() {
				m_engineStateStack.push(m_engineStateStack.top());

				//erase all tags that are not heritable
				m_engineStateStack.top().resetTag(EngineState::CONTEXT_OFFSET);
			}

			/**
		* Pop a state from the engine state stack.
		*/
			void popState() {
				m_engineStateStack.pop();
			}


			/**
		* Return the current state from the engine state stack. Construct a new EngineState if the stack is empty.
		*/
			EngineState& getState() {
				if(m_engineStateStack.size() == 0) {
					m_engineStateStack.push(EngineState());
				}
				return m_engineStateStack.top();
			}


			/**
		* Apply a set of tags to the current engine state.
		*/
			void applyTags(const std::deque <std::string>& tags) {
				std::for_each(std::begin(tags), std::end(tags), [this](const std::string& tag) {
					std::size_t pos = tag.find('=');
					if(pos != std::string::npos) {
						std::string tagSpelling = tag.substr(0, pos);
						int tagValue = std::stoi(tag.substr(pos + 1));
						getState().applyTag(EngineState::getTag(tagSpelling), tagValue);
					} else {
						if(tag[0] == '!') {
							getState().applyTag(EngineState::getTag(tag.substr(1)), true);
						} else {
							getState().applyTag(EngineState::getTag(tag));
						}
					}
				});
			}


		};
	}
}
