#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "json11.hpp"


namespace amanite {
	namespace template_engine {
		namespace context {

			struct JsonContext {
				const json11::Json* m_json;
				const JsonContext* m_parent;
				mutable std::map <std::string, std::unique_ptr<JsonContext>> m_children;
				mutable std::vector <JsonContext> m_array_items;

				JsonContext() : m_json(nullptr), m_parent(nullptr) { }

				JsonContext(const json11::Json& json) : m_json(&json), m_parent(nullptr) { }

				JsonContext(const json11::Json& json, const JsonContext& parent) : m_json(&json), m_parent(&parent) { }

				const JsonContext& operator[](const std::string& key) const {
					return get(key);
				}

				const JsonContext& get(const std::string& key) const {
					auto item = m_children.find(key);
					if(item == m_children.end()) {
						m_children[key] = std::unique_ptr<JsonContext>(new JsonContext((*m_json)[key], *this));
						return *(m_children[key].get());
					} else {
						return *(item->second.get());
					}
				}

				bool hasParent() const {
					return m_parent != nullptr;
				}

				const JsonContext& getParentContext() const {
					return *m_parent;
				}

				std::string getAsString() const {
					if(m_json != nullptr)
						return m_json->string_value();
					else
						return "";
				}

				std::string dump() const {
					if(m_json != nullptr)
						return m_json->dump();
					else
						return "";
				}

				bool isArray() const {
					if(m_json != nullptr)
						return m_json->is_array();
					else
						return false;
				}

				bool isObject() const {
					if(m_json != nullptr)
						return m_json->is_object();
					else
						return false;
				}

				const std::vector <JsonContext>& getAsArray() const {
					const json11::Json::array& ai = m_json->array_items();
					m_array_items.clear();//???
					m_array_items.reserve(ai.size());
					for(auto& item : ai)
						m_array_items.push_back(JsonContext(item, *this));
					return m_array_items;
				}
			};
		}
	}
}