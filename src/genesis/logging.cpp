/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief Implementation of Logging functions.
 *
 * @file
 * @ingroup utils
 */

#include "logging.h"
#include "../global_defs.h"
#include "../thread/parallel_context.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

#ifdef MULTITHREAD
#include <mutex>
#endif

#include "string.h"
#include "date_time.h"

/* if LOG_ALL_PROCS = false, output is handled only for root process */
#define LOG_ALL_PROCS true

namespace genesis {
namespace utils {

static inline bool skip_print()
{
    return !( (LOG_ALL_PROCS || ROOT) ); 
}

// =============================================================================
//     Settings
// =============================================================================

#ifdef MULTITHREAD
    static std::mutex log_mutex;
#endif

// TODO use different init for log details depending on DEBUG

// init static members
LoggingDetails Logging::details = {
    false, // count
    false, // date
    false, // time
    false, // runtime
    false, // file
    false, // line
    false, // function
    false  // level
};
Logging::LoggingLevel      Logging::max_level_  = kDebug4;
long                       Logging::count_      = 0;
std::vector<std::ostream*> Logging::ostreams_;
std::vector<std::ostream*> Logging::estreams_;
int                        Logging::report_percentage_ = 5;
std::string                Logging::debug_indent       = "    ";

/**
 * @brief Set the highest log level that is reported.
 *
 * Invocations of log with higher levels will create no output.
 * It creates a warning if the set level is higher than the static compile time
 * level set by #LOG_LEVEL_MAX.
 */
void Logging::max_level (const LoggingLevel level)
{
    if ((LOG_ALL_PROCS || ROOT) && level > LOG_LEVEL_MAX) {
        LOG_WARN << "Logging max level set to " << level << ", but compile "
                 << "time max level is " << LOG_LEVEL_MAX << ", so that "
                 << "everything above that will not be logged.";
    }
    max_level_ = level;
}

/**
 * @brief set the percentage for reporting #LOG_PROG messages.
 */
void Logging::report_percentage (const int percentage)
{
    if (percentage <= 0) {
        LOG_WARN << "Logging report percentage less than 1% not possible.";
        report_percentage_ = 1;
        return;
    }
    if (percentage > 100) {
        LOG_WARN << "Logging report percentage greater than 100% not meaningful.";
        report_percentage_ = 100;
        return;
    }
    report_percentage_ = percentage;
}

/**
 * @brief Return a string representation of a log level.
 */
std::string Logging::level_to_string(const LoggingLevel level)
{
    /* NONE ERR WARN INFO PROG DBG DBG1 DBG2 DBG3 DBG4 */
    static const char* const buffer[] = {
        "NONE", "[ERR] ", "[WARN] ", "[INFO] ",
        "[PROG] ", "[DBG] ", "[DBG1] ", "[DBG2] ", "[DBG3] ", "[DBG4] "
    };
    return buffer[level];
}

/**
 * @brief Add stdout as output stream to which log messages are written.
 */
void Logging::log_to_stdout ()
{
  if (skip_print()) return;

  // check whether stdout was already added.
  for (std::ostream* os : ostreams_) {
      if (os == &std::cout) {
          return;
      }
  }

  // if not, add it as output stream.
  ostreams_.push_back (&std::cout);
}

/**
 * @brief Add stdout as output stream to which log messages are written.
 */
void Logging::log_to_stderr ()
{
  if (skip_print()) return;

  // check whether stdout was already added.
  for (std::ostream* os : ostreams_) {
      if (os == &std::cerr) {
          return;
      }
  }

  // if not, add it as output stream.
  ostreams_.push_back (&std::cout);
}

/**
 * @brief Add an output stream to which log messages are written.
 */
void Logging::log_to_stream (std::ostream& os)
{
  if (skip_print()) return;
  ostreams_.push_back (&os);
}

void Logging::disable_stdout ()
{
  std::vector<std::ostream*>::iterator it = ostreams_.begin();
  for( ; it != ostreams_.end(); it++)
  {
      ostreams_.erase(it);
      return;
  }
}

/**
 * @brief Add an output file to which log messages are written.
 *
 * This creates a stream to the file.
 */
void Logging::log_to_file (const std::string& fn)
{
  if (skip_print()) return;

  // TODO the log file stream is never deleted. this is not a big leak,
  // as commonly only one file is used for logging, but still is a smell.
  std::ofstream* file = new std::ofstream();
  file->open (fn, std::ios::out | std::ios::app);
  if (file->is_open()) {
      ostreams_.push_back (file);
  } else {
      throw std::runtime_error( "Cannot open logging file " + fn );
  }
}

/**
 * @brief Add stdout as output stream to which log messages are written.
 */
void Logging::err_to_stdout ()
{
  if (skip_print()) return;

  // check whether stdout was already added.
  for (std::ostream* os : ostreams_) {
      if (os == &std::cout) {
          return;
      }
  }

  // if not, add it as output stream.
  estreams_.push_back (&std::cout);
}

/**
 * @brief Add stdout as output stream to which log messages are written.
 */
void Logging::err_to_stderr ()
{
  if (skip_print()) return;

  // check whether stdout was already added.
  for (std::ostream* os : ostreams_) {
      if (os == &std::cerr) {
          return;
      }
  }

  // if not, add it as output stream.
  estreams_.push_back (&std::cout);
}

/**
 * @brief Add an output stream to which log messages are written.
 */
void Logging::err_to_stream (std::ostream& os)
{
  if (skip_print()) return;

  estreams_.push_back (&os);
}

/**
 * @brief Add an output file to which log messages are written.
 *
 * This creates a stream to the file.
 */
void Logging::err_to_file (const std::string& fn)
{
  if (skip_print()) return;

  // TODO the log file stream is never deleted. this is not a big leak,
  // as commonly only one file is used for logging, but still is a smell.
  std::ofstream* file = new std::ofstream();
  file->open (fn, std::ios::out | std::ios::app);
  if (file->is_open()) {
      estreams_.push_back (file);
  } else {
      throw std::runtime_error( "Cannot open logging file " + fn );
  }
}

// =============================================================================
//     Destructor (does the actual work)
// =============================================================================

/**
 * @brief Destructor that is invoked at the end of each log line and does the actual
 * output.
 */
Logging::~Logging()
{
    // build the details for the log message into a buffer
    std::ostringstream det_buff;
    det_buff.str("");
    if (details_.count) {
        det_buff.fill('0');
        det_buff.width(4);
        det_buff << count_ << " ";
    }
    if (details_.date) {
        det_buff << current_date() << " ";
    }
    if (details_.time) {
        det_buff << current_time() << " ";
    }
    if (details_.file) {
        det_buff << file_ << (details_.line ? "" : " ");
    }
    if (details_.line) {
        det_buff << ":" << line_ << " ";
    }
    if (details_.function) {
        det_buff << "(" << function_ << ") ";
    }
    if (details_.level) {
        det_buff << level_to_string(level_);
    }

    // make multi line log messages align to the length of the detail header,
    // and trim trailing whitespace, as we only want one newline at the end
    std::string msg = det_buff.str();
    if (msg.length() > 0) {
        msg += utils::replace_all(
            buff_.str(), "\n", "\n" + std::string(msg.length(), ' ')
        );
    } else {
        msg += buff_.str();
    }

    // output the message to every stream, thread safe!
#ifdef MULTITHREAD
    log_mutex.lock();
#endif

    if (level_ == kError)
    {
      for (std::ostream* out : estreams_) {
          //(*out) << msg << std::endl << std::flush;
          (*out) << msg << std::flush;
      }
    }
    else
    {
      for (std::ostream* out : ostreams_) {
          //(*out) << msg << std::endl << std::flush;
          (*out) << msg << std::flush;
      }
    }
#ifdef MULTITHREAD
    log_mutex.unlock();
#endif

    // inc log message counter
    count_++;
}

// =============================================================================
//     Singleton accessors
// =============================================================================

/**
 * @brief Getter for the singleton instance of log, is called by the standard macros.
 *
 * It returns the string stream buffer used to capture the log messages.
 */
std::ostringstream& Logging::get(
    const std::string& file, const int line, const std::string& function,
    const LoggingLevel level
)
{
    return get(file, line, function, level, details);
}

/**
 * @brief Getter for the singleton instance of log, is called by special macros
 * that change the details of the log message.
 *
 * It stores some relevant information and returns the string stream buffer
 * used to capture the log messages.
 */
std::ostringstream& Logging::get(
    const std::string& file, const int line, const std::string& function,
    const LoggingLevel level, const LoggingDetails dets
)
{
    // save the information given when called from the macros
    file_     = file;
    line_     = line;
    function_ = function;
    level_    = level;
    details_  = dets;
    buff_.str("");
    return buff_;
}

} // namespace utils
} // namespace genesis
