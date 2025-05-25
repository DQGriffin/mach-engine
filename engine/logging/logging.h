//
// Created by Dquavius Griffin on 5/21/25.
//

#ifndef LOGGING_H
#define LOGGING_H
#include <string>

#ifdef ENABLE_LOGGING
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG_DEBUG(msg) LOG("DEBUG", msg)
#define LOG_INFO(msg) LOG("INFO", msg)
#define LOG_WARNING(msg) LOG("WARNING", msg)
#define LOG_ERROR(msg) LOG("ERROR", msg)
#define LOG_FATAL(msg) LOG("FATAL", msg)

#define LOG(level, line) \
std::cout << "[" << level << "] " << __FILENAME__ << ":" << __LINE__ << " " << __FUNCTION__ << "(): " << line << std::endl;
#else
#define LOG(level, line)
#endif

#endif //LOGGING_H
