///////////////////////////////////////////////////////////////////////////////
// NAME:            foo.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Serialization for foo
//
// CREATED:         09/11/2022
//
// LAST EDITED:     09/11/2022
//
// Copyright 2022, Ethan D. Twardy
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////

#ifndef FOO_H
#define FOO_H

#include <serdec/yaml.h>

typedef struct Foo {
    char* investigator;
    double exposure_time;
} Foo;

SerdecResult foo_deserialize_from_yaml_object(yaml_parser_t* parser,
    Foo* data);
SerdecResult foo_deserialize_from_yaml_string(const char* string, Foo* data);
void foo_free(Foo* foo);

#endif // FOO_H

///////////////////////////////////////////////////////////////////////////////
