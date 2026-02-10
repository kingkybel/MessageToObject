/*
 * Repository:  https://github.com/kingkybel/MessageToObject
 * File Name:   include/msg_generated_object.h
 * Description: Interface for objects generated from messages.
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

#if not defined MSG_GENERATED_OBJECT_H_INCLUDED

    #define MSG_GENERATED_OBJECT_H_INCLUDED

namespace util
{
/**
 * @brief Base class for the objects created by generators
 */
struct MsgGeneratedObjectIfc
{
    virtual ~MsgGeneratedObjectIfc() = default;
};

};  // namespace util

#endif
