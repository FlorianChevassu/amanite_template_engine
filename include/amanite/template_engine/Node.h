#pragma once

#include <string>
#include <list>
#include <map>
#include <deque>

namespace amanite {
	namespace template_engine {
		struct Node {
			enum Type {
				root,
				text,
				var,
				section,
				code,
				partial,
				startScope,
				endScope,
			};

			Node(Node::Type t, const std::string& v, const std::deque<std::string>& ts = {})
					: type(t), value(v), tags(ts) {
			}
			Type type;
			std::string value;
			std::list<Node> children;
			std::deque<std::string> tags;
		};
	}
}