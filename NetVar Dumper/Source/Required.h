
#ifndef Required_h__
#define Required_h__

//--
#define WIN32_LEAN_AND_MEAN
//--
#include <windows.h>
//--
#include <string>
#include <iomanip>
#include <fstream>
#include <iostream>
//--
#include <boost/format.hpp>
//--
#include "Logger.h"
#include "client_class.h"
//--
#include "Hook.h"
#include "NetVars.h"
//--
extern std::string			g_Directory;
extern Logger<char>::Stream	logstream;
#endif // Required_h__