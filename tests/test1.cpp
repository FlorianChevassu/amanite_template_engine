#include "amanite/template_engine/Compiler.h"
#include "amanite/template_engine/Renderer.h"
#include "amanite/template_engine/context/JsonContext.h"

int main(){
	using namespace amanite::template_engine;
	Compiler tec;

	//tec.registerContext<JsonContext>();
	CompiledTemplate res = tec.compile("FlorianChevassu/ATE/tests/test1.mustache");

	//assert(res.size() == 3);

	json11::Json data = json11::Json::object {
		{ "name", "Florian" },
		{ "city", "Lyon" },
		{"family", json11::Json::object {
						{"name", "Chevassu"},
						{"brothers", json11::Json::array{
										json11::Json::object { {"name", "Jo"} },
										json11::Json::object { {"name", "Cécile"} },
									}
						}
					}
				}
		};

	Renderer ter;

	ter.render<context::JsonContext, decltype(&context::JsonContext::hasParent), &context::JsonContext::hasParent>(context::JsonContext(data), std::cout, res);


	return 0;
}
