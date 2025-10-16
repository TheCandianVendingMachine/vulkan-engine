#include <iostream>
#include <engine/engine.h>
#include <engine/reflection/type.h>


struct Example {
	int a;
	int b;
	float c;
	double d;
	unsigned int e;

	REFLECT_START(Example)
		REFLECT_MEMBER(a),
		REFLECT_MEMBER(b),
		REFLECT_MEMBER(c),
		REFLECT_MEMBER(d),
		REFLECT_MEMBER(e)
	REFLECT_END;
};

int main() {
	auto a = Example{
		5,
		-7,
		3.14159f,
		2.71828182846,
		static_cast<unsigned int>(-1)
	};

	auto meta = Example::Meta(a);
	std::cout << "Object " << meta.name << "\n";
	for (auto& member : meta.members()) {
		std::cout << "\t" << member.meta.type_info->name() << " " << member.meta.name << " = " << member.to_human_string() << "\n";
	}
	std::cout << std::endl;

    return 0;
}