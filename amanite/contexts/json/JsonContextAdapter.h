#pragma once

#include <string>
#include <map>
#include <list>
#include <memory>

#include "json11.hpp"


namespace amanite {
	namespace template_engine {
		namespace context {

			struct JsonContextAdapter {
				const json11::Json* m_json = nullptr;
				const JsonContextAdapter* m_parent = nullptr;
				mutable std::map <std::string, std::unique_ptr<JsonContextAdapter>> m_children;
				mutable std::list<JsonContextAdapter> m_array_items;

				JsonContextAdapter(const json11::Json& json) : m_json(&json)/*, m_parent(nullptr)*/ { }

				JsonContextAdapter(const json11::Json& json, const JsonContextAdapter& parent) : m_json(&json), m_parent(&parent) { }

				const JsonContextAdapter& operator[](const std::string& key) const {
					return get(key);
				}

				const JsonContextAdapter& get(const std::string& key) const {
					auto& item = m_children.find(key);
					if(item == m_children.end()) {
						m_children.emplace(key, std::make_unique<JsonContextAdapter>((*m_json)[key], *this));
						return *(m_children[key].get());
					} else {
						return *(item->second.get());
					}
				}

				bool hasParent() const {
					return m_parent != nullptr;
				}

				const JsonContextAdapter& getParentContext() const {
					assert(m_parent != nullptr);
					return *m_parent;
				}

				/*std::string dump() const {
					if(m_json != nullptr)
						return m_json->dump();
					else
						return "";
				}*/

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

				const std::list<JsonContextAdapter>& getAsArray() const {
					//Ne compile pas car reserve requiert un ctr par copie au 
					//cas où il faudrait copier des élements existants... c'est 
					//exactement ce qu'on voulait éviter en utilisant reserve d'ailleurs.

					//Revoir le contrat d'un adapteur pour que n'importe quel conteneur puisse etre utilisé... Utiliser les concepts serait cool !

					const json11::Json::array& ai = m_json->array_items();
					m_array_items.clear();//???
					//m_array_items.reserve(ai.size());
					for (const json11::Json& item : ai)
						m_array_items.emplace_back(item, *this);
					return m_array_items;
				}



				bool isString() const{
					return m_json->is_string();
				}

				std::string getAsString() const {
					std::string res;
					if(m_json == nullptr) {
						res = "";
					}else if(m_json->is_bool()) {
						res = m_json->bool_value() ? "true" : "false";
					} else if(m_json->is_null()) {
						res = "null";
					} else if(m_json->is_number()) {
						res = std::to_string(m_json->number_value());
					} else if(m_json->is_string()) {
						res = m_json->string_value();
					}else{
						res = m_json->dump();
					}
					return res;
				}

				bool isDouble() const{
					return m_json->is_number();
				}

				double getAsDouble() const{
					return m_json->number_value();
				}

				bool isBoolean() const{
					return m_json->is_bool();
				}

				double getAsBoolean() const{
					return m_json->bool_value();
				}

				bool isNull() const{
					return m_json->is_null();
				}
			};
		}
	}
}