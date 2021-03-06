/**
* Created from source code found in DdrawCompat v2.0
* https://github.com/narzoul/DDrawCompat/
*
* Updated 2017 by Elisha Riedlinger
*
*/

#pragma once

#define CINTERFACE

//********** Begin Edit *************
#define DDRAWLOG_H
#define APP_DDRAWNAME			DDrawCompat
#define APP_DDRAWVERSION		v0.2.0b
//********** End Edit ***************

#include <ddraw.h>
#include <fstream>
#include <type_traits>

#define LOG_ONCE(msg) \
	static bool isAlreadyLogged##__LINE__ = false; \
	if (!isAlreadyLogged##__LINE__) \
	{ \
		Compat::Log() << msg; \
		isAlreadyLogged##__LINE__ = true; \
	}

std::ostream& operator<<(std::ostream& os, const RECT& rect);
std::ostream& operator<<(std::ostream& os, const DDSCAPS& caps);
std::ostream& operator<<(std::ostream& os, const DDSCAPS2& caps);
std::ostream& operator<<(std::ostream& os, const DDPIXELFORMAT& pf);
std::ostream& operator<<(std::ostream& os, const DDSURFACEDESC& sd);
std::ostream& operator<<(std::ostream& os, const DDSURFACEDESC2& sd);

template <typename T>
typename std::enable_if<std::is_class<T>::value && !std::is_same<T, std::string>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& t)
{
	return os << static_cast<const void*>(&t);
}

template <typename T>
typename std::enable_if<std::is_class<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T* t)
{
	return t ? (os << *t) : (os << "null");
}

template <typename T>
std::ostream& operator<<(std::ostream& os, T** t)
{
	return t ? (os << reinterpret_cast<void*>(t) << '=' << *t) : (os << "null");
}

namespace Compat
{
	class Log
	{
	public:
		Log();
		~Log();

		template <typename T>
		Log& operator<<(const T& t)
		{
			s_logFile << t;
			return *this;
		}

	protected:
		template <typename... Params>
		Log(const char* prefix, const char* funcName, Params... params) : Log()
		{
			s_logFile << prefix << ' ' << funcName << '(';
			toList(params...);
			s_logFile << ')';
		}

	private:
		void toList()
		{
		}

		template <typename Param>
		void toList(Param param)
		{
			s_logFile << param;
		}

		template <typename Param, typename... Params>
		void toList(Param firstParam, Params... remainingParams)
		{
			s_logFile << firstParam << ", ";
			toList(remainingParams...);
		}

		//********** Begin Edit *************
		// Get wrapper file name
		static char * Init()
		{
			static char wrappername[MAX_PATH];

			// Get module name
			HMODULE hModule = NULL;
			GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)Init, &hModule);
			GetModuleFileName(hModule, wrappername, MAX_PATH);

			// Get process name
			char processname[MAX_PATH];
			GetModuleFileName(nullptr, processname, MAX_PATH);

			// Check if module name is the same as process name
			if (_strcmpi(strrchr(wrappername, '\\') + 1, strrchr(processname, '\\') + 1) == 0)
			{
				strcpy_s(strrchr(wrappername, '\\') + 1, MAX_PATH - strlen(wrappername), "dxwrapper.dll");
			}

			// Remove extension and add dash (-)
			strcpy_s(strrchr(wrappername, '.'), MAX_PATH - strlen(wrappername), "-");

			// Add process name
			strcat_s(wrappername, MAX_PATH, strrchr(processname, '\\') + 1);

			// Change extension to .log
			strcpy_s(strrchr(wrappername, '.'), MAX_PATH - strlen(wrappername), ".log");

			// Set lower case
			for (int z = 0; z < MAX_PATH && wrappername[z] != '\0'; z++) { wrappername[z] = (char)tolower(wrappername[z]); }

			return wrappername;
		}
		//********** End Edit ***************

		static std::ofstream s_logFile;
	};

//********** Begin Edit *************
#include "Logging\Logging.h"
#ifdef DDRAWCOMPATLOG
//********** End Edit ***************
	class LogEnter : private Log
	{
	public:
		template <typename... Params>
		LogEnter(const char* funcName, Params... params) : Log("-->", funcName, params...)
		{
		}
	};

	class LogLeave : private Log
	{
	public:
		template <typename... Params>
		LogLeave(const char* funcName, Params... params) : Log("<--", funcName, params...)
		{
		}

		template <typename Result>
		void operator<<(const Result& result)
		{
			static_cast<Log&>(*this) << " = " << std::hex << result << std::dec;
		}
	};
#else
	class LogEnter
	{
	public:
		template <typename... Params> LogEnter(const char*, Params...) {}
		template <typename Result> void operator<<(const Result&) {}
	};

	typedef LogEnter LogLeave;
#endif
}
