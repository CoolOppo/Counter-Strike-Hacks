
// Preprocessor header guard to stop multiple includes
#ifndef _LOGGER_H_
#define _LOGGER_H_

// C++ Standard Library
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

// Boost C++ Libraries
#pragma warning(push, 1)
#pragma warning(disable: 6328)
#pragma warning(disable: 4996)
#pragma warning(disable: 4702)
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#pragma warning(pop)

// Logging class
template <typename _charT>
class Logger
{
public:
	// Sink information
	typedef _charT char_type;
	typedef boost::iostreams::sink_tag category;

	// STL typedefs
	typedef std::basic_ofstream<char_type> tofstream;
	typedef std::basic_string<char_type> tstring;

	// Boost typedefs
	typedef boost::basic_format<char_type> tformat;

	// Stream typedef
	typedef boost::iostreams::stream< Logger<_charT> > Stream;

	// Constructor
	Logger() : m_LogPath(GeneratePath())
	{ }

	// Generate path to log file.
	tstring GeneratePath()
	{
		return g_Directory + "/NVDumper.log";
	}

	// Writes n characters from s
	std::streamsize write(const char_type* s, std::streamsize n)
	{
		// For boost
		using namespace boost::posix_time;
		// Get time
		ptime Time(second_clock::local_time());
		tstring TimeStr = to_simple_string_type<char_type>(Time);
		// Open file
		tofstream Out(m_LogPath.c_str(), tofstream::out | tofstream::app);

		if(Out.is_open() && Out.good())
		{
			// Write time as string
			Out << '[' << TimeStr << "]: ";
			// Write data
			Out.write(s, n);
		}

		// Return size
		return n;
	}

private:
	// Path to log file
	tstring m_LogPath;
};

#endif // _LOGGER_H_