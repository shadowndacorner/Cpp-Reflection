#include <iostream>
#include <cassert>

#define CPP_REFLECTION_IMPLEMENTATION
#include <reflection.hpp>
#undef CPP_REFLECTION_IMPLEMENTATION

#include "ReflectableClass.h"

// Define the Mammal class for reflection
DEFINE_TYPE(reflection_example::Mammal)
{
	// Define its age member
	DEFINE_MEMBER(age);
}

DEFINE_TYPE(reflection_example::Human)
{
	// Declare that we have a parent class
	// Note that this also declares Human a child of Mammal
	DEFINE_PARENT(reflection_example::Mammal);
	
	// Declare Human's members
	DEFINE_MEMBER(name);
	DEFINE_MEMBER(children);

	// Since the class definition allows private reflection, we can get Mammal::Age
	DEFINE_MEMBER(age);
}

void print_type_info(const Type& type)
{
	auto human = reflection_example::Human();

	std::cout << "Type Info: " << type.GetName() << "\n";
	auto& members = type.GetMembers();
	std::cout << "Members:\n";
	for (auto it = members.begin(); it != members.end(); ++it)
	{
		auto& mem = it->second;
		std::cout << "\tMember " << mem.GetName() << " of type " << mem.GetTypeName() << " has offset " << mem.GetOffset() << std::endl;
	}
}

void main()
{
	using namespace reflection_example;
	Human p;

	// Get type info for object p
	auto& human_type = GetObjectType(p);

	// Get member "name"
	auto& name_mem = human_type.GetMember("name");

	// Get instance of name from p
	auto& val = name_mem.GetFromInstance<std::string>(&p);

	// Ensure that both references point to the same object
	assert(&val == &p.name);

	// Get the age member, inherited privately from Mammal
	auto& age_mem = human_type.GetMember("age");
	auto& age_val = age_mem.GetFromInstance<int>(&p);

	// This allows us to modify the private member, which would
	// otherwise be impossible
	age_val = 10;
	
	assert(age_val == p.GetAge());

	auto& mammal_type = GetType<Mammal>();

	// Check if human is a child of mammal
	assert(human_type.ChildOf(mammal_type));

	// And vice versa...
	assert(mammal_type.ParentOf(human_type));
	
	print_type_info(mammal_type);
	print_type_info(human_type);

	char buf[512];
	std::cin.getline(buf, 512);
}