/*
 * Repository:  https://github.com/kingkybel/MessageToObject
 * File Name:   include/generator_map.h
 * Description: Static map for registering generators by hashed message keys.
 *
 * Copyright (C) 2026 Dieter J Kybelksties <github@kybelksties.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @date: 2026-02-10
 * @author: Dieter J Kybelksties
 */

#ifndef MESSAGETOOBJECTS_GENERATOR_MAP_H_INCLUDED
#define MESSAGETOOBJECTS_GENERATOR_MAP_H_INCLUDED

#include "msg_generated_object.h"
#include "string_to_hash.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace util
{

/**
 * A static map that uses a key generator to create a hash from a string and returns a generator function
 * object to create an object described by the string.
 * @tparam N number of char to use for the key (for default KeyGenerator)
 * @tparam KeyGenerator A type that can generate a hash from a string
 *  @code std::shared_ptr<MsgGeneratedObjectIfc> generate_from_msg(const std::string &message) @endcode
 */
template<uint8_t N, typename KeyGenerator = util::prefix_key<N>>
class generator_map
{
    static_assert(is_valid_key_generator<KeyGenerator>::value,
                  "KeyGenerator must be constructible from a std::string and have an integral 'hash' member.");

    public:
    using GeneratorFunc = std::function<std::shared_ptr<MsgGeneratedObjectIfc>(const std::string &message)>;

    private:
    /**
     * Retrieve the one instance (per `KeyGenerator` type) of the map.
     * @return  singleton map
     */
    static std::unordered_map<size_t, GeneratorFunc> &generatorMap_()
    {
        static std::unordered_map<size_t, GeneratorFunc> generatorMap;
        return generatorMap;
    }

    public:
    /**
     * @brief Clear the map.
     */
    static void clear()
    {
        generatorMap_().clear();
    }

    /**
     * @brief Register a generator for a message.
     * @param msg message from which to generate a key.
     * @param gen a generator function
     */
    static void registerGenerator(const std::string &msg, GeneratorFunc gen)
    {
        KeyGenerator key(msg);
        generatorMap_()[key.hash()] = std::move(gen);
    };

    /**
     * @brief Get the configured generator for a message, if possible.
     * @param message the message we need a generator for
     * @return shared pointer to a generated object, if possible, an empty pointer otherwise
     */
    static std::shared_ptr<MsgGeneratedObjectIfc> get(const std::string &message)
    {
        KeyGenerator key{message};
        auto         it = generatorMap_().find(key.hash());
        if(it != generatorMap_().end())
        {
            return it->second(message);  // Call the generator
        }
        return {};  // return empty shared_ptr
    }
};

}  // namespace util

#endif  // MESSAGETOOBJECTS_GENERATOR_MAP_H_INCLUDED
