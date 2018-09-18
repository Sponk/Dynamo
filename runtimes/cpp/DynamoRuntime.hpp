#pragma once

#include <typeinfo>
#include <memory>
#include <cassert>
#include <vector>
#include <functional>
#include <unordered_map>
#include <cmath>

#include <iostream>


#ifdef DYNAMO_GC
#include <gc/gc.h>
#include <gc/gc_cpp.h>
#include <gc/gc_allocator.h>
#endif

namespace DynamoRuntime
{

struct IFixedValue;
struct IFixedValueHash
{
	std::size_t operator() (const IFixedValue&) const;
};

#ifdef DYNAMO_GC
typedef std::unordered_map<IFixedValue, IFixedValue, IFixedValueHash, std::equal_to<IFixedValue>, gc_allocator<std::pair<IFixedValue, IFixedValue>>> Table;
typedef Table* TableRef;

typedef std::basic_string<char, std::char_traits<char>, gc_allocator<char>> String;
typedef String* StringRef;

#else
typedef std::unordered_map<IFixedValue, IFixedValue, IFixedValueHash> Table;
typedef std::shared_ptr<Table> TableRef;

typedef std::string String;
typedef std::shared_ptr<String> StringRef;

#endif

enum TYPES
{
	NIL = 0,
	NUMBER,
	INTEGER,
	BOOL,
	STRING,
	TABLE,
	UNDEFINED,
	MAX_TYPE
};

template<typename T>
TYPES typeOf()
{
	if constexpr (std::is_same<int, T>::value)
		return INTEGER;
	// TODO Use a flag to determine default floating point size!
	else if constexpr ( /*std::is_same<float, T>::value ||*/ std::is_same<double, T>::value)
		return NUMBER;
	else if constexpr (std::is_same<bool, T>::value)
		return BOOL;
	else if constexpr (std::is_same<Table, T>::value)
		return TABLE;
	else if constexpr (std::is_same<String, T>::value)
		return STRING;
	else if constexpr (std::is_same<TableRef, T>::value)
		return TABLE;
	else if constexpr (std::is_same<StringRef, T>::value)
		return STRING;
	else
		return UNDEFINED;
}

template<typename T>
TYPES typeOf(const T& value)
{
	return typeOf<T>();
}

extern IFixedValue calculateTableIndex(const IFixedValue& v);

struct IFixedValue
{
	IFixedValue() = default;
	IFixedValue(const IFixedValue& t) { *this = t; }
	
	template<typename T>
	static IFixedValue makeValue(const T& t)
	{
		IFixedValue v;
		v.set(t);
		return v;
	}
	
	template<typename T>
	static IFixedValue makeValue()
	{
		IFixedValue v;
		v.set(T());
		return v;
	}
	
	static const IFixedValue& makeValue(const IFixedValue& t)
	{
		return t;
	}
	
	static IFixedValue makeValue(Table&& init)
	{
		IFixedValue v;
#ifdef DYNAMO_GC
		TableRef t = new(GC_MALLOC(sizeof(Table))) Table(std::move(init));
		v.set(t);
#else
		TableRef t = std::make_shared<Table>(init);
		new (v.getData()) TableRef();
		v.set(t);
#endif
		return v;
	}
	
	static IFixedValue makeValue(const char* init)
	{
		IFixedValue v;
#ifdef DYNAMO_GC
		StringRef t = new(GC_MALLOC(sizeof(String))) String(init);
		v.set(t);
#else
		StringRef t = std::make_shared<String>(init);
		new (v.getData()) StringRef();
		v.set(t);
#endif
		return v;
	}
	
	static IFixedValue makeValue(String&& init)
	{
		IFixedValue v;
#ifdef DYNAMO_GC
		StringRef t = new(GC_MALLOC(sizeof(String))) String(std::move(init));
		v.set(t);
#else
		StringRef t = std::make_shared<String>(init);
		new (v.getData()) StringRef();
		v.set(t);
#endif
		return v;
	}
	
	template<typename... Args>
	static IFixedValue makeValue(const std::function<IFixedValue(Args...)>& init)
	{
		IFixedValue v;
#ifdef DYNAMO_GC
		std::function<IFixedValue(Args...)>* t = new(GC_MALLOC_ATOMIC(sizeof(std::function<IFixedValue(Args...)>))) std::function<IFixedValue(Args...)>(init);
		*reinterpret_cast<uintptr_t*>(v.data) = reinterpret_cast<uintptr_t>(t);
#else
		auto t = std::make_shared<std::function<IFixedValue(Args...)>>(init);
		*reinterpret_cast<std::shared_ptr<std::function<IFixedValue(Args...)>>*>(v.data) = t;
#endif
		// Add num of args to type
		static_assert(MAX_TYPE + sizeof...(Args) < 256, "Too many arguments to fit into the type tag!");
		v.type = MAX_TYPE + sizeof...(Args);
		return v;
	}
	
#ifdef DYNAMO_GC
	char data[8]; // So a pointer can fit!
#else
	char data[16]; // So a shared_ptr can fit!
#endif
	
	void* getData() { return data; }
	const void* getData() const { return data; }
	
	// Contains the type tag for this value.
	// Functions have the number of arguments encoded into this as well
	// as "MAX_TYPE + <num args>" to at least do basic type checking at compile time!
	unsigned char type = NIL;
	
	template<typename T>
	void set(const T& t)
	{
		type = typeOf<T>();
		get<T>() = t;
	}
	
	template<typename T>
	T& get()
	{
		assert(isa<T>() && "Types for a cast must match!");
		return *reinterpret_cast<T*>(getData());
	}
	
	template<typename T>
	const T& get() const
	{
		assert(isa<T>() && "Types for a cast must match!");
		return *reinterpret_cast<const T*>(getData());
	}
	
	template<typename T>
	bool isa() const
	{
		return typeOf<T>() == type;
	}
	
	bool isa(const IFixedValue& a) const
	{
		return a.type == type;
	}
	
	void matchTypes(const IFixedValue& a) const
	{
		if(!isa(a))
			throw std::runtime_error("Types do not match!");
	}
	
	template<typename T>
	void matchTypes(const T& a) const
	{
		if(!isa<T>())
			throw std::runtime_error("Types do not match!");
	}
	
	template<typename Result = void, typename Function, typename... Args>
	static Result doTyped(const IFixedValue& self, Function&& F, Args... args)
	{
		if constexpr(std::is_void<Result>::value)
		{
			if(self.isa<int>())
				F(self.get<int>(), args...);
			else if(self.isa<float>())
				F(self.get<float>(), args...);
			else if(self.isa<double>())
				F(self.get<double>(), args...);
			else if(self.isa<bool>())
				F(self.get<bool>(), args...);
			else if(self.type == NIL)
				throw std::runtime_error("Invalid access to nil!");
			else throw std::runtime_error("Unknown type!");
		}
		else
		{
			if(self.isa<int>())
				return F(self.get<int>(), args...);
			else if(self.isa<float>())
				return F(self.get<float>(), args...);
			else if(self.isa<double>())
				return F(self.get<double>(), args...);
			else if(self.isa<bool>())
				return F(self.get<bool>(), args...);
			else if(self.type == NIL)
				throw std::runtime_error("Invalid access to nil!");
			else throw std::runtime_error("Unknown type!");
		}
	}
	
#undef FunctionResult
	
#define MAKE_OPERATOR(op) \
	template<typename T> \
	IFixedValue operator op(const T& ref) \
	{ \
		matchTypes(ref); \
		return makeValue(get<T>() op ref); \
	}
	
#define MAKE_OPERATOR_CONST(op) \
	template<typename T> \
	IFixedValue operator op(const T& ref) const \
	{ \
		matchTypes(ref); \
		return makeValue(get<T>() op ref); \
	}
	
#define MAKE_OPERATOR_BOOL(op) \
	template<typename T> \
	bool operator op(const T& ref) const \
	{ \
		matchTypes(ref); \
		return get<T>() op ref; \
	}
	
	MAKE_OPERATOR_BOOL(==)
	MAKE_OPERATOR_BOOL(!=)
	MAKE_OPERATOR_BOOL(>)
	MAKE_OPERATOR_BOOL(<)
	MAKE_OPERATOR_BOOL(>=)
	MAKE_OPERATOR_BOOL(<=)
	
	MAKE_OPERATOR_CONST(+)
	MAKE_OPERATOR_CONST(-)
	MAKE_OPERATOR_CONST(*)
	MAKE_OPERATOR_CONST(/)
	
	MAKE_OPERATOR(+=)
	MAKE_OPERATOR(-=)
	MAKE_OPERATOR(*=)
	MAKE_OPERATOR(/=)
	
#undef MAKE_OPERATOR
#undef MAKE_OPERATOR_BOOL
#undef MAKE_OPERATOR_BOOL_TYPE
#undef MAKE_OPERATOR_CONST
	
	template<typename T>
	IFixedValue& operator=(const T& ref)
	{
		set(ref);
		return *this;
	}
	
	bool operator!() const
	{
		return !get<bool>();
	}
	
	template<typename... Args>
	IFixedValue operator()(Args&&... args)
	{
		if(type < MAX_TYPE)
			throw std::runtime_error("Object is not a function!");
		else if(type - MAX_TYPE != sizeof...(args))
			throw std::runtime_error("Function called with wrong number of arguments!");
		
#ifdef DYNAMO_GC
		auto callable = *reinterpret_cast<std::function<IFixedValue(Args...)>**>(&data);
		
		assert(callable);
		return (*callable)(args...);
#else
		auto callable = reinterpret_cast<std::shared_ptr<std::function<IFixedValue(Args...)>>*>(data);
		
		assert(callable);
		return (*callable->get())(args...);
#endif
	}
	
	IFixedValue& operator[](const IFixedValue& key)
	{
		if(!isa<TableRef>())
			throw std::runtime_error("Trying to access non-table value as a table!");
		
		
		return (*get<TableRef>())[calculateTableIndex(key)];
	}
	
	const IFixedValue& operator[](const IFixedValue& key) const
	{
		if(!isa<TableRef>())
			throw std::runtime_error("Trying to access non-table value as a table!");
		
		return (*get<TableRef>()).at(calculateTableIndex(key));
	}
	
	IFixedValue operator*() const
	{
		if(!isa<TableRef>())
			throw std::runtime_error("Trying to access non-table value as a table!");
		
		// FIXME Make numbers castable!
		return makeValue((double) (*get<TableRef>()).size());
	}
	
	IFixedValue operator-() const
	{
		return doTyped<IFixedValue>(*this, [this](auto that) {
			return makeValue(-that);
		});
	}
	
	IFixedValue operator~() const
	{
		if(isa<int>())
			return makeValue(~get<int>());
		else if(isa<float>())
			return makeValue(~static_cast<int>(get<float>()));
		else if(isa<double>())
			return makeValue(~static_cast<int>(get<double>()));
	}
	
	String toString() const
	{
		if(type == NIL)
			return "nil";
		if(type >= MAX_TYPE)
			return "function";
		else if(isa<StringRef>())
			return *get<StringRef>();
		
		return doTyped<String>(*this, [this](auto that) {
			if constexpr(std::is_same<bool, decltype(that)>::value)
				return String(that ? "true" : "false");
			
			return String(std::to_string(that).c_str());
		});
	}
};

// START OPERATOR SPECIALIZATION

template<>
IFixedValue& IFixedValue::operator=(const IFixedValue& ref)
{
	type = ref.type;
	doTyped(ref, [this](auto that) {
		get<decltype(that)>() = that;
	});
	
	return *this;
}

#define MAKE_OPERATOR(op) \
	template<> \
	IFixedValue IFixedValue::operator op (const IFixedValue& ref) \
	{ \
		return doTyped<IFixedValue>(ref, [this](auto that) { \
			return makeValue(get<decltype(that)>() op that); \
		}); \
	}
	
#define MAKE_OPERATOR_CONST(op) \
	template<> \
	IFixedValue IFixedValue::operator op(const IFixedValue& ref) const \
	{ \
		return doTyped<IFixedValue>(ref, [this](auto that) { \
			return makeValue(get<decltype(that)>() op that);\
		}); \
	}
	
#define MAKE_OPERATOR_BOOL(op) \
	template<> \
	bool IFixedValue::operator op(const IFixedValue& ref) const \
	{ \
		if (isa<StringRef>() && ref.isa<StringRef>())\
			return *get<StringRef>() op *ref.get<StringRef>(); \
		else if (ref.type == NIL)\
			return type == NIL; \
			\
		return doTyped<bool>(ref, [this](auto that) { \
				return get<decltype(that)>() op that;\
		}); \
	}
	
MAKE_OPERATOR_BOOL(==)
MAKE_OPERATOR_BOOL(!=)
MAKE_OPERATOR_BOOL(>)
MAKE_OPERATOR_BOOL(<)
MAKE_OPERATOR_BOOL(>=)
MAKE_OPERATOR_BOOL(<=)

template<>
IFixedValue IFixedValue::operator+(const IFixedValue& ref) const
{
	if (isa<StringRef>())
		return makeValue((*get<StringRef>()) + ref.toString());
	else if (ref.isa<StringRef>())
		return makeValue(toString() + *ref.get<StringRef>());
	
	return doTyped<IFixedValue>(ref, [this](auto that) {
		return makeValue(get<decltype(that)>() + that);
	});
}

MAKE_OPERATOR_CONST(-)
MAKE_OPERATOR_CONST(*)
MAKE_OPERATOR_CONST(/)

MAKE_OPERATOR(+=)
MAKE_OPERATOR(-=)
MAKE_OPERATOR(*=)
MAKE_OPERATOR(/=)

#undef MAKE_OPERATOR
#undef MAKE_OPERATOR_BOOL
#undef MAKE_OPERATOR_BOOL_TYPE
#undef MAKE_OPERATOR_CONST
// END OPERATOR SPECIALIZATION

std::size_t IFixedValueHash::operator()(const IFixedValue& k) const
{
	if(k.isa<StringRef>())
	{
		auto ref = k.get<StringRef>();
		const char* str = ref->data();
		
		// SDBM hashing
		unsigned long hash = 0;
		int c;

		while(c = *str++)
			hash = c + (hash << 6) + (hash << 16) - hash;

		return hash;
	}
	else if(k.type == NIL)
	{
		return 0;
	}
	
	return IFixedValue::doTyped<std::size_t>(k, [&k](auto v) {
		const uint64_t* hash = reinterpret_cast<const uint64_t*>(k.getData());
		return *hash << k.type;
	});
}

#ifdef DYNAMO_GC

void dynamoInit()
{
	GC_INIT();
	// GC_enable_incremental();
	GC_set_dont_expand(0);
	//GC_set_full_freq(100);
	
	GC_expand_hp(4096*1024);
	GC_disable();
}

#else

void dynamoInit()
{}

#endif

}

