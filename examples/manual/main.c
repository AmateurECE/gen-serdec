///////////////////////////////////////////////////////////////////////////////
// NAME:            main.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Entrypoint using serialization code.
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

#include "foo.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <serdec/yaml.h>

static const char* FOO_OBJECT = "\
investigator: 'Ethan D. Twardy'\n\
exposure_time: 0.001\n\
";

int main() {
    Foo foo = {0};
    SerdecResult result = foo_deserialize_from_yaml_string(FOO_OBJECT, &foo);
    if (!result.ok) {
        fprintf(stderr, "%s\n", result.error);
        free(result.error);
        assert(true == result.ok && "Deserialization failed");
    }

    assert(!strcmp("Ethan D. Twardy", foo.investigator));
    assert(0.001 == foo.exposure_time);
    foo_free(&foo);
}

///////////////////////////////////////////////////////////////////////////////
