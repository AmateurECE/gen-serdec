///////////////////////////////////////////////////////////////////////////////
// NAME:            foo.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Deserialization code for Foo.
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
#include <stdlib.h>
#include <serdec/yaml.h>

SerdecResult foo_deserialize_from_yaml_object(yaml_parser_t* parser,
    Foo* data)
{
    SerdecYamlContext context = {0};
    context.parser = parser;

    serdec_yaml_mapping_start(&context);
    serdec_yaml_mapping_field_name(&context, "investigator");
    serdec_yaml_deserialize_string(&context, &data->investigator);
    serdec_yaml_mapping_field_name(&context, "exposure_time");
    serdec_yaml_deserialize_float(&context, &data->exposure_time);
    serdec_yaml_mapping_end(&context);

    return (SerdecResult){
        .ok = !context.error,
        .error = context.error_message
    };
}

SerdecResult foo_deserialize_from_yaml_string(const char* string, Foo* data) {
    yaml_parser_t parser = {0};
    yaml_parser_initialize(&parser);
    yaml_parser_set_input_string(&parser,
        (const yaml_char_t*)string, strlen(string));

    SerdecYamlContext context = {0};
    context.parser = &parser;

    serdec_yaml_stream_start(&context);
    serdec_yaml_document_start(&context);

    SerdecResult result = foo_deserialize_from_yaml_object(&parser, data);
    if (!result.ok) {
        return result;
    }

    serdec_yaml_document_end(&context);
    serdec_yaml_stream_end(&context);

    yaml_parser_delete(&parser);

    return (SerdecResult){
        .ok = !context.error,
        .error = context.error_message
    };
}

void foo_free(Foo* foo) {
    if (NULL != foo->investigator) {
        free(foo->investigator);
        foo->investigator = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
