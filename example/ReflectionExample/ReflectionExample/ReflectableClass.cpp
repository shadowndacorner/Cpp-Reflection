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

int reflection_example::Human::GetAge()
{
	return 0;
}
