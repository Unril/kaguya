#pragma once

#include<string>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}


#ifndef KAGUYA_USE_CPP11
#if defined(__cpp_decltype) || __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1700)
#define KAGUYA_USE_CPP11 1
#else
#define KAGUYA_USE_CPP11 0
#endif
#endif


#ifndef KAGUYA_USE_BOOST
#if KAGUYA_USE_CPP11
#define KAGUYA_USE_BOOST 0
#else
#define KAGUYA_USE_BOOST 1
#endif
#endif



#if KAGUYA_USE_BOOST
#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#if BOOST_VERSION >= 104800
#include <boost/move/move.hpp>
#endif
#else
#include <functional>
#include <tuple>
#include <memory>
#include <utility>
#include <type_traits>
#endif


#ifndef KAGUYA_ERROR_NO_THROW
#define KAGUYA_ERROR_NO_THROW 1
#endif

#ifndef KAGUYA_USE_RVALUE_REFERENCE
#if KAGUYA_USE_CPP11
#define KAGUYA_USE_RVALUE_REFERENCE 1
#else
#define KAGUYA_USE_RVALUE_REFERENCE 0
#endif
#endif

#ifndef KAGUYA_USE_VARIADIC_TEMPLATE
#if KAGUYA_USE_CPP11 && (!defined(_MSC_VER) || _MSC_VER >= 1800)
#define KAGUYA_USE_VARIADIC_TEMPLATE 1
#else
#define KAGUYA_USE_VARIADIC_TEMPLATE 0
#endif
#endif


namespace kaguya
{
	namespace standard
	{
#if KAGUYA_USE_BOOST
		using namespace boost;
#if BOOST_VERSION < 104800
		template<typename T>T forward(T v) { return v; }
#endif

		inline std::string to_string(int v)
		{
			char buffer[128] = { 0 };
			snprintf(buffer, sizeof(buffer), "%d", v);
			return buffer;
		}
#else
		using namespace std;
#endif
	}

	//compatibility for Lua5.1
#if LUA_VERSION_NUM < 502
	inline void luaL_setmetatable(lua_State *L, const char *tname) {
		luaL_getmetatable(L, tname);
		lua_setmetatable(L, -2);
	}
	inline void *luaL_testudata(lua_State *L, int index, const char *tname) {
		void *p = lua_touserdata(L, index);
		if (p == NULL)
		{
			return 0;
		}
		if (lua_getmetatable(L, index))
		{
			luaL_getmetatable(L, tname);
			if (!lua_rawequal(L, -1, -2))
			{
				p = 0;
			}
			lua_pop(L, 2);//pop metatables
			return p;
		}
		return 0;
	}
	inline void luaL_requiref(lua_State *L, const char *modname,
		lua_CFunction openf, int glb) {

		lua_pushcfunction(L, openf);
		lua_pushstring(L, modname);
		lua_call(L, 1, 1);

		if (glb) {
			lua_pushvalue(L, -1);
			lua_setglobal(L, modname);
		}
	}
#endif
}
