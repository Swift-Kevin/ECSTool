#ifndef COMP_H_
#define COMP_H_

#include <unordered_map>
#include <typeindex>
#include <string>

inline std::unordered_map<std::string, std::type_index>& RegisteredComponents()
{
	static std::unordered_map<std::string, std::type_index> map;
	return map;
}

struct ComponentRegistry
{
	template<typename T>
	static void RegisterComponent(const std::string& name)
	{
		auto& map = RegisteredComponents();
		auto [tupl, inserted] = map.try_emplace(name, typeid(T));
		if (inserted)
		{
			std::cout << "Registered component: " << name << "\n";
		}
	}
};

#endif // !COMP_H_

// Macro that auto-registers the component
#define COMPONENT(name) \
struct name; \
struct name##_registrar { \
    name##_registrar() { ComponentRegistry::RegisterComponent<name>(#name); } \
}; \
static name##_registrar _##name##_registrar_instance; \
struct name