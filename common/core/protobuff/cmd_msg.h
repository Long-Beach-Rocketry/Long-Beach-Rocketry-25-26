/**
* @file cmd_msg.h
* @author Bex Saw
* @brief Interface for serialized structs
*/

#pragma once

#include <cstddef>
#include <cstdint>

namespace LBR
{
class CmdMessage
{
public:
    /**
     * @brief Constructor initializing the buffer size for serialization.
     * @param buffer_size Size of the buffer to hold serialized data.
     */
    explicit CmdMessage(size_t buffer_size) : buffer_size(buffer_size)
    {
    }

    /**
    * @brief Get the buffer containing the serialized data
    * @param buffer The buffer to hold the serialized data
    * @param size The size of the buffer
    * @return true if the buffer was successfully filled, false otherwise
    */
    virtual bool get_buf(uint8_t* buffer, size_t size) = 0;

    /**
    * @brief Get the size of the serialized data
    * @return The size of the serialized data
    */
    virtual size_t get_size() const = 0;

    /**
    * @brief Parse the serialized data from a buffer
    * @param buffer The buffer containing the serialized data
    * @param size The size of the buffer
    * @return true if the data was successfully parsed, false otherwise
    */
    virtual bool parse(uint8_t* buffer, size_t size) = 0;
};
}  // namespace LBR