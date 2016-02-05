#ifndef LOGGABLE_H
#define LOGGABLE_H

#include <iostream>

/**
 * @brief Loggable classes interface
 *
 * A loggable class must implement output_log and input_log.
 * The output format is arbitrary as long as the class knows how to load itself
 * from the same string. All attributes must be logged.
 */
class Loggable
{
public:
    virtual ~Loggable() {}

    /**
     * @brief Prints out the log
     * @param[in] out the output stream to print log to
     */
    virtual void output_log(std::ostream  &out) = 0;

    /**
     * @brief Loads a logfile from an input stream
     * @param[in] in the input stream to read log from
     */
    virtual void input_log(std::istream  &in) = 0;
};

#endif // LOGGABLE_H
