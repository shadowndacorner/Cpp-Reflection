#include "ReflectableClass.h"

int reflection_example::Human::GetAge()
{
	return 0;
}


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

DEFINE_TYPE(int3)
{
	DEFINE_MEMBER(x);
	DEFINE_MEMBER(y);
	DEFINE_MEMBER(z);
}