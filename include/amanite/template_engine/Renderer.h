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
#include "stringutils.h"

namespace amanite {
	namespace template_engine {
		class CompiledTemplate;

		template<class Context,
				class HasParentFuncType = decltype(&Context::hasParent),
				HasParentFuncType hasParentFunc = &Context::hasParent,

				class GetParentContextFuncType = decltype(&Context::getParentContext),
				GetParentContextFuncType getParentContextFunc = &Context::getParentContext,

				class GetItemFuncType = decltype(&Context::operator[]),
				GetItemFuncType getItemFunc = &Context::operator[],

				class GetAsStringFuncType = decltype(&Context::getAsString),
				GetAsStringFuncType getAsStringFunc = &Context::getAsString,

				class IsArrayFuncType = decltype(&Context::isArray),
				IsArrayFuncType isArrayFunc = &Context::isArray,

				class IsObjectFuncType = decltype(&Context::isObject),
				IsObjectFuncType isObjectFunc = &Context::isObject,

				class GetAsArrayFuncType = decltype(&Context::getAsArray),
				GetAsArrayFuncType getAsArrayFunc = &Context::getAsArray
		>
		class Renderer {
			script::ScriptEngine m_scriptingEngine;

		public:
			void render(const Context& c, std::ostream& os, const CompiledTemplate& tmpl, const Context* parentContext = nullptr) {
				render(c, os, tmpl.getNodes(), tmpl.getDeps(), parentContext);
			}
		private:

			/*********************************/
			/* Proxies for the context class */
			/*********************************/

			/*---------------*/
			/*   hasParent   */
			/*---------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<HasParentFuncType, bool (Context::*)() const>::value, Dummy>::type>
			bool hasParentProxy_imp(const Context& c, int) const {
				return (c.*hasParentFunc)();
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<HasParentFuncType, bool (*)()>::value, Dummy>::type>
			bool hasParentProxy_imp(const Context& c, long) const{
				return hasParentFunc(c);
			}

			bool hasParentProxy(const Context& c) const {
				return hasParentProxy_imp(c, 0);//0 is used to prefer the member function (as int will be a better match)
			}


			/*----------------------*/
			/*   getParentContext   */
			/*----------------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetParentContextFuncType, const Context& (Context::*)() const>::value, Dummy>::type>
			const Context& getParentContextProxy_imp(const Context& c, int) const {
				return (c.*getParentContextFunc)();
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetParentContextFuncType, const Context& (*)()>::value, Dummy>::type>
			const Context& getParentContextProxy_imp(const Context& c, long) const{
				return getParentContextFunc(c);
			}

			const Context& getParentContextProxy(const Context& c) const {
				return getParentContextProxy_imp(c, 0);//0 is used to prefer the member function (as int will be a better match)
			}


			/*-------------*/
			/*   getItem   */
			/*-------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetItemFuncType, const Context& (Context::*)(const std::string&) const>::value, Dummy>::type>
			const Context& getItemProxy_imp(const Context& c, const std::string& val, int) const {
				return (c.*getItemFunc)(val);
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetItemFuncType, const Context& (*)(const std::string&)>::value, Dummy>::type>
			const Context& getItemProxy_imp(const Context& c, const std::string& val, long) const{
				return getItemFunc(c, val);
			}

			const Context& getItemProxy(const Context& c, const std::string& val) const {
				return getItemProxy_imp(c, val, 0);//0 is used to prefer the member function (as int will be a better match)
			}


			/*-----------------*/
			/*   getAsString   */
			/*-----------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetAsStringFuncType, std::string (Context::*)() const>::value, Dummy>::type>
			std::string getAsStringProxy_imp(const Context& c, int) const {
				return (c.*getAsStringFunc)();
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetAsStringFuncType, std::string (*)()>::value, Dummy>::type>
			std::string getAsStringProxyProxy_imp(const Context& c, long) const{
				return getAsStringFunc(c);
			}

			std::string getAsStringProxy(const Context& c) const {
				return getAsStringProxy_imp(c, 0);//0 is used to prefer the member function (as int will be a better match)
			}

			/*-------------*/
			/*   isArray   */
			/*-------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<IsArrayFuncType, bool (Context::*)() const>::value, Dummy>::type>
			bool isArrayProxy_imp(const Context& c, int) const {
				return (c.*isArrayFunc)();
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<HasParentFuncType, bool (*)()>::value, Dummy>::type>
			bool isArrayProxy_imp(const Context& c, long) const{
				return isArrayFunc(c);
			}

			bool isArrayProxy(const Context& c) const {
				return isArrayProxy_imp(c, 0);//0 is used to prefer the member function (as int will be a better match)
			}

			/*--------------*/
			/*   isObject   */
			/*--------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<IsObjectFuncType, bool (Context::*)() const>::value, Dummy>::type>
			bool isObjectProxy_imp(const Context& c, int) const {
				return (c.*isObjectFunc)();
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<HasParentFuncType, bool (*)()>::value, Dummy>::type>
			bool isObjectProxy_imp(const Context& c, long) const{
				return isObjectFunc(c);
			}

			bool isObjectProxy(const Context& c) const {
				return isObjectProxy_imp(c, 0);//0 is used to prefer the member function (as int will be a better match)
			}

			/*----------------*/
			/*   getAsArray   */
			/*----------------*/

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetAsArrayFuncType, const std::vector<Context>& (Context::*)() const>::value, Dummy>::type>
			const std::vector<Context>& getAsArrayProxy_imp(const Context& c, int) const {
				return (c.*getAsArrayFunc)();
			}

			template <typename Dummy = int, typename = typename std::enable_if<std::is_same<GetAsArrayFuncType, const std::vector<Context>& (*)()>::value, Dummy>::type>
			const std::vector<Context>& getAsArrayProxy_imp(const Context& c, long) const{
				return getAsStringFunc(c);
			}

			const std::vector<Context>& getAsArrayProxy(const Context& c) const {
				return getAsArrayProxy_imp(c, 0);//0 is used to prefer the member function (as int will be a better match)
			}



			/***********************/
			/* Main rendering code */
			/***********************/

			void render(const Context& c, std::ostream& os, const std::list<Node>& tmpl, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext = nullptr) {
				m_scriptingEngine.registerVariable(os, "out");
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
					if(!hasParentProxy(*currentContext)) {
						throw std::runtime_error("Context does not have parents");
					}
					currentContext = &getParentContextProxy(*currentContext);
				}
				os << getAsStringProxy(getItemProxy(*currentContext, node.value));
				m_engineStateStack.popState();
			}

			void renderSection(const Context& c, const Node& node, std::ostream& os, const std::map<std::string, std::list<Node>>& deps, const Context* parentContext){
				//reduce scope of variable secItems to the local case to avoid compiler error.
				m_engineStateStack.pushState();
				m_engineStateStack.applyTags(node.tags);
				const Context* currentContext = &c;
				for(int i = 0; i < m_engineStateStack.getCurrentState().contextOffset; ++i) {
					if(!hasParentProxy(*currentContext)) {
						throw std::runtime_error("Context does not have parents");
					}
					currentContext = &getParentContextProxy(*currentContext);
				}

				if(isArrayProxy(getItemProxy(*currentContext, node.value))) {
					auto& secItems = getAsArrayProxy(getItemProxy(*currentContext, node.value));
					std::for_each(secItems.begin(), secItems.end(), [&](const Context& secIt) {
						render(secIt, os, node.children, deps, currentContext);
					});
				} else if(isObjectProxy(getItemProxy(*currentContext, node.value))) {
					render(getItemProxy(*currentContext, node.value), os, node.children, deps, currentContext);
				}
			}

























			script::ScriptEngine& getScriptEngine() {
				return m_scriptingEngine;
			}


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
			EngineStateStack m_engineStateStack;
		};
	}
}
