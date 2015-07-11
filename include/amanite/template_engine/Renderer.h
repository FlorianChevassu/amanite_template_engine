#pragma once

#include <string>
#include <list>
#include <stack>
#include <map>
#include <istream>
#include <fstream>
#include <sstream>
#include <type_traits>

#include "EngineStateStack.h"

#include <boost/filesystem.hpp>
#include <chaiscript/utility/utility.hpp>
#include "scriptEngine.h"
#include "Node.h"

namespace amanite {
	namespace template_engine {
		class CompiledTemplate;

		template <class Context>
		class Renderer {
			script::ScriptEngine m_scriptingEngine;

			/***********************/
			/* Main rendering code */
			/***********************/

		public:
			void render(const Context& c, std::ostream& os, const CompiledTemplate& tmpl, const Context* parentContext = nullptr) {
				render(c, os, tmpl.getNodes(), tmpl.getDeps(), parentContext);
			}


		private:
			void render(const Context& c, std::ostream& os, const std::list<Node>& tmpl, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext = nullptr) {
				m_scriptingEngine.add(chaiscript::var(&os), "out");
				m_scriptingEngine.registerVariable(c, "context");

				if(parentContext != nullptr)
					m_scriptingEngine.registerVariable(*parentContext, "parentContext");


				std::for_each(tmpl.begin(), tmpl.end(), [&](const Node& item) {
					switch(item.type) {
						case Node::Type::text:
							renderText(c, item, os, deps, parentContext);
							break;
						case Node::Type::var:
							renderVariable(c, item, os, deps, parentContext);
							break;
						case Node::Type::section:
							renderSection(c, item, os, deps, parentContext);
							break;
						case Node::Type::partial:
							render(c, os, deps.find(item.value)->second, deps, parentContext);
							break;
						case Node::Type::code:
							m_scriptingEngine.eval(item.value);
							break;
						case Node::Type::endScope:
							m_engineStateStack.popState();
							break;
						case Node::Type::startScope:
							m_engineStateStack.pushState();
							m_engineStateStack.applyTags(item.tags);
							break;
						default:
							//should never happen. The compilation step should detect problems
							assert(true);
					}
				});
			}

			void renderText(const Context& c, const Node& node, std::ostream& os, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext){
				if(!m_engineStateStack.getCurrentState().skipText)
					os << node.value;
			}

			void renderVariable(const Context& c, const Node& node, std::ostream& os, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext){
				m_engineStateStack.pushState();
				m_engineStateStack.applyTags(node.tags);
				const Context* currentContext = &c;
				for(int i = 0; i < m_engineStateStack.getCurrentState().contextOffset; ++i) {
					if(!currentContext->hasParent()) {
						throw std::runtime_error("Context does not have parents");
					}
					currentContext = &currentContext->getParentContext();
				}

				//TODO : escape characters if m_engineStateStack.getCurrentState().escape is set to true.
				os << currentContext->get(node.value).getAsString();
				m_engineStateStack.popState();
			}

			void renderSection(const Context& c, const Node& node, std::ostream& os, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext){
				//reduce scope of variable secItems to the local case to avoid compiler error.
				m_engineStateStack.pushState();
				m_engineStateStack.applyTags(node.tags);
				const Context* currentContext = &c;
				for(int i = 0; i < m_engineStateStack.getCurrentState().contextOffset; ++i) {
					if(!currentContext->hasParent()) {
						throw std::runtime_error("Context does not have parents");
					}
					currentContext = &currentContext->getParentContext();
				}

				if(currentContext->get(node.value).isArray()) {
					auto& secItems = currentContext->get(node.value).getAsArray();
					std::for_each(std::begin(secItems), std::end(secItems), [&](const Context& secIt) {
						render(secIt, os, node.children, deps, currentContext);
					});
				} else if(currentContext->get(node.value).isObject()) {
					render(currentContext->get(node.value), os, node.children, deps, currentContext);
				} else {
					const Context& ctx = currentContext->get(node.value);
					bool needRendering = false;
					if(ctx.isDouble()){
						//TODO : >0 or !=0 ?? The problem with !=0 is that itcannot be done rigorously for doubles...
						needRendering = ctx.getAsDouble() > 0;
					}else if(ctx.isBoolean()){
						needRendering = ctx.getAsBoolean();
					}else if(ctx.isString()){
						const std::string& s = ctx.getAsString();
						//todo : add "true", "oui", etc...
						if(s.compare("yes") == 0){
							needRendering = true;
						}
					}
					if(needRendering) {
						render(*currentContext, os, node.children, deps, currentContext);
					}
				}
			}





			/******************/
			/* Scripting code */
			/******************/
		public:
			script::ScriptEngine& getScriptEngine() {
				return m_scriptingEngine;
			}

		private:
			void registerContext() {
				m_scriptingEngine.registerClass<Context>("Context");
				m_scriptingEngine.registerFunction(&Context::get, "get");
				m_scriptingEngine.registerFunction(&Context::operator[], "[]");
				m_scriptingEngine.registerFunction(&Context::hasParent, "hasParent");
				m_scriptingEngine.registerFunction(&Context::getParentContext, "getParentContext");
				m_scriptingEngine.registerFunction(&Context::isArray, "isArray");
				m_scriptingEngine.registerFunction(&Context::getAsArray, "getAsArray");
				m_scriptingEngine.registerFunction(&Context::isObject, "isObject");
				m_scriptingEngine.registerFunction(&Context::getAsObject, "getAsObject");
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
			EngineStateStack m_engineStateStack;
		};
	}
}
