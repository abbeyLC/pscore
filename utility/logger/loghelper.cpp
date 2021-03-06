/**************************************************************************************************
*                                            PSCORE                                               *
*                               Copyright (C) 2020 Pointon Software                               *
*                                                                                                 *
*           This program is free software: you can redistribute it and/or modify                  *
*           it under the terms of the GNU Affero General Public License as published              *
*           by the Free Software Foundation, either version 3 of the License, or                  *
*           (at your option) any later version.                                                   *
*                                                                                                 *
*           This program is distributed in the hope that it will be useful,                       *
*           but WITHOUT ANY WARRANTY; without even the implied warranty of                        *
*           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                         *
*           GNU Affero General Public License for more details.                                   *
*                                                                                                 *
*           You should have received a copy of the GNU Affero General Public License              *
*           along with this program.  If not, see <https://www.gnu.org/licenses/>.                *
*                                                                                                 *
*           Ben Ziv <pointonsoftware@gmail.com>                                                   *
*                                                                                                 *
**************************************************************************************************/
#include <cstdarg>
#include <string>
#include <vector>
#include "loghelper.hpp"
#include "consolelog.hpp"
#include "filelog.hpp"
#include "socketlog.hpp"
#include <cfg/config.hpp>

/**
* This code is sourced from StackOverflow
* Author profile: https://stackoverflow.com/users/3990012/serup
*
* Original question: https://stackoverflow.com/q/15106102
* Answer: https://stackoverflow.com/a/52184144
*/
#define EXTRACT_VAR(logFormat, logString) \
    do { \
        va_list args; \
        va_start(args, logFormat); \
        const size_t len = std::vsnprintf(NULL, 0, logFormat.c_str(), args); \
        va_end(args); \
        std::vector<char> vec(len + 1); \
        va_start(args, logFormat); \
        std::vsnprintf(&vec[0], len + 1, logFormat.c_str(), args); \
        va_end(args); \
        logString = &vec[0]; \
    } while (0)

namespace utility {

constexpr char LOG_CONFIG[] = "pslog.cfg";

/* todo (xxx) - this constructor is doing too much of work
 * might be a good idea to have a separate initialize() call
*/
LogHelper::LogHelper()
    : mConfig(std::make_unique<Config>(LOG_CONFIG)), mLogLevel(LogLevel::VERBOSE) {
    initializeLoggerType();
    initializeLogLevel();
}

void LogHelper::write(const std::string& logMode, const std::string& prettyFunction,
                      const std::string logFormat...) const {
    if (!mLogger) {
        // Dont proceed if mLogger is not initialized
        return;
    }
    if (isLogModeWritable(logMode)) {
        const std::string signature = getMethodName(prettyFunction);
        const std::string className = extractClassName(signature);
        const std::string methodName = extractMethodName(signature);
        std::string logString = logFormat;
        //! logFormat might have arguments
        if (logFormat.find("%") != std::string::npos) {
            EXTRACT_VAR(logFormat, logString);
        }
        mLogger->write(logMode, className, methodName, logString);
    }
}

bool LogHelper::isLogModeWritable(const std::string& logMode) const {
    /* We need to apply the following rules:
     * VERBOSE  => debug, info, warn, error
     * NORMAL   => info, warn, error
     * ALERT    => warn, error
     * CRITICAL => error
    */
    if (logMode.compare("debug") == 0) {
        return mLogLevel == LogLevel::VERBOSE;
    }
    if (logMode.compare("info") == 0) {
        return (mLogLevel == LogLevel::VERBOSE) ||
               (mLogLevel == LogLevel::NORMAL);
    }
    if (logMode.compare("warn") == 0) {
        return mLogLevel != LogLevel::CRITICAL;
    }
    return true;
}

const std::string LogHelper::getMethodName(const std::string& prettyFunction) const {
    size_t colons = prettyFunction.find("::");
    if ((colons < MIN_CHAR) || (colons > MAX_NAME)) {
        colons = MAX_NAME;
    }
    const size_t begin = prettyFunction.substr(0, colons).rfind(" ") + 1;
    const size_t end = prettyFunction.rfind("(") - begin;
    return prettyFunction.substr(begin, end);
}

const std::string LogHelper::extractClassName(const std::string& signature) const {
    const size_t colons = signature.find("::");
    if (signature.empty() || (colons < MIN_CHAR) || (colons > MAX_NAME)) {
        return "unknown";
    }
    return signature.substr(0, colons);
}

const std::string LogHelper::extractMethodName(const std::string& signature) const {
    if (signature.empty()) {
        return "unknown";
    }
    const size_t colons = signature.find("::");
    const size_t begin = [&colons] {
        return (colons < MIN_CHAR) || (colons > MAX_NAME) ? 0 : colons + 2;
    }();
    return signature.substr(begin);
}

void LogHelper::initializeLoggerType() {
    if (!mConfig) {
        // Dont proceed if mConfig is not initialized
        return;
    }
    // Get logger type from config
    const LoggerType loggerType = [this] {
        try {
            return static_cast<LoggerType>(std::stoi(mConfig->get("log_type", "0")));
        } catch (const std::invalid_argument&) {
            return LoggerType::CONSOLE;
        }
        catch (const std::out_of_range&) {
            return LoggerType::CONSOLE;
        }
    }();
    mLogger = [loggerType] {
        std::unique_ptr<LoggerInterface> temp;
        switch (loggerType) {
            case LoggerType::FILE:
                temp = std::make_unique<FileLogger>();
                break;
            case LoggerType::SOCKET:
                temp = std::make_unique<SocketLogger>();
                break;
            case LoggerType::CONSOLE:
                // Fallthrough
            default:
                // We default to console
                temp = std::make_unique<ConsoleLogger>();
                break;
        }
        return temp;
    }();
}

void LogHelper::initializeLogLevel() {
    if (!mConfig) {
        // Dont proceed if mConfig is not initialized
        return;
    }
    mLogLevel = [this] {
        try {
            return static_cast<LogLevel>(std::stoi(mConfig->get("log_level", "0")));
        } catch (const std::invalid_argument&) {
            return LogLevel::VERBOSE;
        }
        catch (const std::out_of_range&) {
            return LogLevel::VERBOSE;
        }
    }();
}

}  // namespace utility
