/*
Copyright (c) 2017 Ian Diaz

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <cassert>

#define CPP_REFLECTION_IMPLEMENTATION
#include <reflection.hpp>
#include "ReflectableClass.h"

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

	// Ensure that polymorphism works
	BaseClass* child = new ChildClass;
	
	// We are derived
	ChildClass& child_cast = dynamic_cast<ChildClass&>(*child);
	assert(&GetObjectType(*child) == &GetType<ChildClass>());
	delete child;

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

	std::cout << std::endl << "Press return to exit" << std::endl;
	// Wait for user input to close
	char buf[512];
	std::cin.getline(buf, 512);
}