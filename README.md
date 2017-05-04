# C++ Reflection System

This is a lightweight, header-only reflection system for C++.  It was mostly made as a learning exercise for myself, but seemed like it could potentially be useful for others.

## Installation:
Simply drop reflection.hpp into your include directory and do the following in some source file:

```c++
#define CPP_REFLECTION_IMPLEMENTATION
#include <reflection.hpp>
#undef CPP_REFLECTION_IMPLEMENTATION
```

## Usage:
For a full example, refer to the example folder.

### Defining classes:
```c++
// mammals.h
class Mammal
{
public:
	int age;
};

class Human : Mammal
{
	// Declare that the reflection system can access
	// private members
	ALLOW_PRIVATE_REFLECTION(Human);
	
public:
	std::string name;
	std::vector<Human> children;

	int GetAge()
	{
		return age;
	}
};
```

```c++
// mammals.cpp
// Define the Mammal class for reflection
DEFINE_TYPE(Mammal)
{
	// Define its age member
	DEFINE_MEMBER(age);
}

DEFINE_TYPE(Human)
{
	// Declare that we have a parent class
	// Note that this also declares Human a child of Mammal
	DEFINE_PARENT(Mammal);
	
	// Declare Human's members
	DEFINE_MEMBER(name);
	DEFINE_MEMBER(children);

	// Since the class definition allows private reflection, we can get Mammal::Age
	DEFINE_MEMBER(age);
}
```

### Using type data:
```c++

void main()
{
	Human p;

	// Get type info for object p
	auto& human_type = GetObjectType(p);

	// Get member "name"
	auto& name_mem = human_type.GetMember("name");

	// Get instance of name from p
	auto& val = name_mem.GetFromInstance<std::string>(&p);
	assert(&val == &p.name);

	// Get the age member, inherited privately from Mammal
	auto& age_mem = human_type.GetMember("age");
	auto& age_val = age_mem.GetFromInstance<int>(&p);

	// This allows us to modify the private member, which would
	// otherwise be impossible
	age_val = 10;
	
	assert(age_val == p.GetAge());

	// Ensure that both references point to the same object

	auto& mammal_type = GetType<Mammal>();

	// Check if human is a child of mammal
	assert(human_type.ChildOf(mammal_type));
	
	// And vice versa...
	assert(mammal_type.ParentOf(human_type));

}
```