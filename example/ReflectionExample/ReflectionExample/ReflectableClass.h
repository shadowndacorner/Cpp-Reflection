#pragma once
#include <reflection.hpp>
#include <string>
#include <vector>

namespace reflection_example
{
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
}
