#include "amanite/template_engine/Compiler.h"
#include "amanite/template_engine/Renderer.h"
#include "amanite/template_engine/contexts/JsonContextAdapter.h"

int main(){
	using namespace amanite::template_engine;

	Compiler tec;

	std::string templateContent = R"(Hello {{name}}
You have just won {{value}} dollars!
{{#in_ca}}
Well, {{= 10000 - (10000 * 0.4)}} dollars, after taxes.
{{/in_ca}})";

	//std::cout << templateContent << std::endl;

	istringstream iss(templateContent);

	CompiledTemplate res = tec.compile(iss);


	string err;

	json11::Json data = json11::Json::parse(R"({
								"name": "Chris",
								"value": 10000,
								"taxed_value": 6000,
								"in_ca": true
							})", err);

	context::JsonContextAdapter dataCtx = data;

	//std::cout << "dataCtx.get(\"name\") = " << dataCtx.get("name").getAsString() << std::endl;


	std::cout << err << std::endl;
	assert(err.empty());


	Renderer<context::JsonContextAdapter> ter;

	ter.render(context::JsonContextAdapter(data), std::cout, res);


	return 0;
}
