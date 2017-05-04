// MAKE SURE TO INCLUDE THIS WITH CPP_REFLECTION_IMPLEMENTATION DEFINED IN SOME SOURCE FILE

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


#pragma once
#include <cassert>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <type_traits>

namespace reflection_internal {
	template <typename T>
	class ClassMetadataRegistration;
}

namespace reflection
{
	class Type
	{
	public:
		class Member
		{
		public:
			friend class Type;
			Member();
			Member(const std::string& name, size_t offset, const type_info& typehash, bool is_ptr);

		public:
			const Type& GetType() const;
			const std::string& GetName() const;
			std::string GetTypeName() const;
			size_t GetOffset() const;
			bool IsPtr() const;

			template <typename T>
			inline T& GetFromInstance(void* data) const
			{
				return *reinterpret_cast<T*>(((size_t)data) + m_offset);
			}

		private:
			std::string m_name;
			size_t m_offset;
			const type_info* m_typeinfo;
			Type* m_type;
			bool is_valid;
			bool m_isptr;
		};

		template <typename T>
		friend class reflection_internal::ClassMetadataRegistration;
	
	public:
		Type();
		Type(const std::string& name, const size_t& size, const type_info& id);

	public:
		const std::string& GetName() const;
		const size_t& GetSize() const;
		const Member& GetMember(const std::string& name) const;
		//const Member& GetInheritedMember(const std::string& name) const;
		bool ChildOf(const Type& rhs) const;
		bool ParentOf(const Type& rhs) const;

		const std::unordered_map<std::string, Member>& GetMembers() const;
//		const std::unordered_map<std::string, Member>& GetInheritedMembers() const;

		const std::unordered_set<const Type*>& GetParentClasses() const;
		const std::unordered_set<const Type*>& GetChildClasses() const;

	private:
		std::string m_name;
		std::unordered_map<std::string, Member> m_members;
		//std::unordered_map<std::string, Member> m_inherited_members;
		std::unordered_set<const Type*> m_parents;
		std::unordered_set<const Type*> m_children;
		size_t m_size;
		const type_info* m_typeid;
		bool is_valid;

		template <typename T>
		Member& AddMember(const std::string& name, T* mem)
		{
			return m_members[name] = Member(name, (size_t)mem, typeid(std::remove_pointer<T>::type), std::is_pointer<T>::value);
		}

		/*
		Member& AddInheritedMember(const Member& rhs, size_t offset)
		{
			return m_inherited_members[rhs.GetName()] = Member(rhs.m_name, offset, *rhs.m_typeinfo);
		}*/

		template <typename T>
		void AddParent(const std::string& name)
		{
			auto& parent = const_cast<Type&>(GetType<T>());
			parent.AddChild(this);
			m_parents.insert(&parent);

			/*
			// Get all inherited members
			for (auto it = parent.m_members.begin(); it != parent.m_members.end(); ++it)
			{

				auto& mem = it->second;
				AddInheritedMember(it->second, );
			}*/
		}

		void AddChild(Type* type)
		{
			m_children.insert(type);
		}
	};
}


namespace reflection_internal
{
	std::unordered_map<std::string, size_t>& get_reflection_name_table();
	std::unordered_map<size_t, reflection::Type>& get_reflection_table();
}

namespace reflection_internal
{
	template <typename T>
	class ClassMetadataRegistration
	{
	public:
		inline ClassMetadataRegistration(const std::string& name)
		{
			auto& id = typeid(T);
			using namespace reflection;
			auto& table = get_reflection_table();
			auto& names = get_reflection_name_table();
			assert(table.count(id.hash_code()) == 0);

			names[name] = id.hash_code();
			auto& type = table[id.hash_code()] = Type(name, sizeof(T), id);
			Initialize(type);
		}

		inline T* null() {return nullptr;}

		void Initialize(reflection::Type&);
	};
}

namespace reflection
{
	template <typename T>
	const Type& GetType()
	{
		return reflection_internal::get_reflection_table()[typeid(T).hash_code()];
	}

	template <typename T>
	const Type& GetObjectType(T& obj)
	{
		return reflection_internal::get_reflection_table()[typeid(obj).hash_code()];
	}

	const Type& GetTypeByName(const std::string& name);
};

#define MACRO_PASTE(__, ___) __##___
#define NAME_GENERATOR_INTERNAL( _ ) MACRO_PASTE( GENERATED_NAME, _ )
#define NAME_GENERATOR( ) NAME_GENERATOR_INTERNAL( __COUNTER__ )

#define DEFINE_TYPE(name) static reflection_internal::ClassMetadataRegistration<name> NAME_GENERATOR() = #name; \
void reflection_internal::ClassMetadataRegistration<name>::Initialize(reflection::Type& type)

#define DEFINE_MEMBER(name) type.AddMember(#name, &(null()->name))

#define DEFINE_PARENT(name) type.AddParent<name>(#name)

#define ALLOW_PRIVATE_REFLECTION(name) friend class reflection_internal::ClassMetadataRegistration<name>

// Define this in order to implement many of the functions
#ifdef CPP_REFLECTION_IMPLEMENTATION
#include <typeinfo>

using namespace reflection;
using namespace reflection_internal;

static Type InvalidType;
static Type::Member InvalidMember;

static std::unordered_map<size_t, reflection::Type>* types;
static std::unordered_map<std::string, size_t>* names;

std::unordered_map<size_t, reflection::Type>& reflection_internal::get_reflection_table()
{
	if (!types)
		types = new std::unordered_map<size_t, reflection::Type>;

	return *types;
}

std::unordered_map<std::string, size_t>& reflection_internal::get_reflection_name_table()
{
	if (!names)
		names = new std::unordered_map<std::string, size_t>;
	return *names;
}

reflection::Type::Type() : is_valid(false), m_name("INVALID TYPE"){}
inline reflection::Type::Type(const std::string & name, const size_t & size, const type_info& id) : is_valid(true), m_name(name), m_size(size), m_typeid(&id){}

inline const std::string & reflection::Type::GetName() const
{
	return m_name;
}

inline const size_t & reflection::Type::GetSize() const
{
	return m_size;
}

inline const Type::Member& reflection::Type::GetMember(const std::string & name) const
{
	if (!is_valid)
		return InvalidMember;

	return const_cast<std::unordered_map<std::string, Type::Member>&>(m_members)[name];
}

inline bool reflection::Type::ChildOf(const Type & rhs) const
{
	return m_parents.count(&const_cast<Type&>(rhs));
}

inline bool reflection::Type::ParentOf(const Type & rhs) const
{
	return m_children.count(&const_cast<Type&>(rhs)) > 0;
}

inline const std::unordered_map<std::string, Type::Member>& reflection::Type::GetMembers() const
{
	return m_members;
}


/*
inline const Type::Member & reflection::Type::GetInheritedMember(const std::string & name) const
{
if (!is_valid)
return InvalidMember;

return const_cast<std::unordered_map<std::string, Type::Member>&>(m_inherited_members)[name];
}

inline const std::unordered_map<std::string, Type::Member>& reflection::Type::GetInheritedMembers() const
{
	return m_inherited_members;
}*/

inline const std::unordered_set<const Type*>& reflection::Type::GetParentClasses() const
{
	return m_parents;
}

inline const std::unordered_set<const Type*>& reflection::Type::GetChildClasses() const
{
	return m_children;
}

const Type& reflection::GetTypeByName(const std::string & name)
{
	if (names->count(name) > 0)
		return (*types)[(*names)[name]];

	throw std::invalid_argument("Invalid typename");
}

inline reflection::Type::Member::Member() : m_name("INVALID MEMBER"), is_valid(false), m_type(nullptr), m_typeinfo(&typeid(void)) {}
reflection::Type::Member::Member(const std::string & name, size_t offset, const type_info& typeinfo, bool is_ptr) : is_valid(false), m_typeinfo(&typeinfo), m_name(name), m_offset(offset), m_type(nullptr), m_isptr(is_ptr){}

inline const Type& reflection::Type::Member::GetType() const
{
	// Lazy instantiation of type object
	if (!m_type)
	{
		if (reflection_internal::get_reflection_table().count(m_typeinfo->hash_code()) > 0)
			const_cast<Type*>(m_type) = &reflection_internal::get_reflection_table()[m_typeinfo->hash_code()];
		else
			const_cast<Type*>(m_type) = &InvalidType;
	}
	return *m_type;
}

inline const std::string & reflection::Type::Member::GetName() const
{
	return m_name;
}

inline std::string reflection::Type::Member::GetTypeName() const
{
	if (GetType().is_valid)
		return m_type->GetName();
	return m_typeinfo->name();
}

inline size_t reflection::Type::Member::GetOffset() const
{
	if (!is_valid)
		return 0;

	return m_offset;
}

inline bool reflection::Type::Member::IsPtr() const
{
	return m_isptr;
}

#include <inttypes.h>
// Defining builtin pure data types
DEFINE_TYPE(char) {}

DEFINE_TYPE(uint8_t) {}
DEFINE_TYPE(uint16_t) {}
DEFINE_TYPE(uint32_t) {}
DEFINE_TYPE(uint64_t) {}

DEFINE_TYPE(int8_t) {}
DEFINE_TYPE(int16_t) {}
DEFINE_TYPE(int32_t) {}
DEFINE_TYPE(int64_t) {}

DEFINE_TYPE(bool) {}
DEFINE_TYPE(std::string) {}

DEFINE_TYPE(reflection::Type) {}
DEFINE_TYPE(reflection::Type::Member) {}
#endif