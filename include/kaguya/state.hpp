#pragma once

#include <string>

#include "kaguya/config.hpp"

#include "kaguya/utils.hpp"
#include "kaguya/metatable.hpp"
#include "kaguya/error_handler.hpp"

#include "kaguya/lua_ref_table.hpp"

namespace kaguya
{
	class State
	{
		lua_State *state_;
		bool created_;
		
		//non copyable
		State(const State&);
		State& operator =(const State&);

		static void stderror_out(int status, const char* message)
		{
			std::cerr << message << std::endl;
		}
		void init()
		{
			setErrorHandler(&stderror_out);
			nativefunction::reg_functor_destructor(state_);
		}
	public:
		State() :state_(luaL_newstate()), created_(true)
		{
			init();
		}
		State(lua_State* lua) :state_(lua), created_(false)
		{
			init();
		}
		~State()
		{
			if (created_)
			{
				lua_close(state_);
			}
		}


		static int error_handler_cleanner(lua_State *state)
		{
			ErrorHandler::instance().unregisterHandler(state);
			return 0;
		}

		void setErrorHandler(standard::function<void(int statuscode,const char*message)> errorfunction)
		{
			utils::ScopedSavedStack save(state_);
			ErrorHandler::instance().registerHandler(state_, errorfunction);

			luaL_getmetatable(state_, KAGUYA_ERROR_HANDLER_METATABLE);
			int result = lua_type(state_, -1);
			lua_pop(state_, 1);
			if (result != LUA_TTABLE)//not registered cleaner
			{
				luaL_newmetatable(state_, KAGUYA_ERROR_HANDLER_METATABLE);
				lua_pushcclosure(state_, &error_handler_cleanner, 0);
				lua_setfield(state_, -2, "__gc");
				lua_setfield(state_, -1, "__index");
				lua_newtable(state_);
				luaL_setmetatable(state_, KAGUYA_ERROR_HANDLER_METATABLE);
				luaL_ref(state_, LUA_REGISTRYINDEX);
			}
		}

		void openlibs()
		{
			utils::ScopedSavedStack save(state_);
			luaL_openlibs(state_);
		}

		bool dofile(const std::string& str)
		{
			return dofile(str.c_str());
		}
		bool dofile(const char* file)
		{
			utils::ScopedSavedStack save(state_);

			int status = luaL_loadfile(state_, file);

			if (status)
			{
				ErrorHandler::instance().handle(status, state_);
				return false;
			}
			status = lua_pcall(state_, 0, LUA_MULTRET, 0);
			if (status)
			{
				ErrorHandler::instance().handle(status, state_);
				return false;
			}
			return true;
		}

		bool dostring(const char* str)
		{
			utils::ScopedSavedStack save(state_);

			int status = luaL_loadstring(state_, str);

			status = lua_pcall(state_, 0, LUA_MULTRET, 0);
			if (status)
			{
				ErrorHandler::instance().handle(status, state_);
				return false;
			}
			status = luaL_dostring(state_, str);
			return true;
		}
		bool dostring(const std::string& str)
		{
			return dostring(str.c_str());
		}

		bool operator()(const std::string& str)
		{
			return dostring(str);
		}
		bool operator()(const char* str)
		{
			return dostring(str);
		}
		TableKeyReference operator[](const std::string& str)
		{
			return TableKeyReference(globalTable(), LuaRef(state_, str));
		}

		TableKeyReference operator[](const char* str)
		{
			return TableKeyReference(globalTable(),LuaRef(state_, str));
		}

		LuaRef globalTable()
		{
			return LuaRef(state_, GlobalTable());
		}

		void garbageCollect()
		{
			lua_gc(state_, LUA_GCCOLLECT, 0);
		}
		lua_State *state() { return state_; };
	};
};
