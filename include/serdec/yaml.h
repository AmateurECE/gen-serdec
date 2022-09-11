///////////////////////////////////////////////////////////////////////////////
// NAME:            yaml.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Header-only library for ser/deser support of YAML.
//
// CREATED:         09/09/2022
//
// LAST EDITED:     09/11/2022
//
// Copyright 2022, Ethan D. Twardy
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
////

#ifndef SERDEC_YAML_H
#define SERDEC_YAML_H

#include <stdbool.h>
#include <stdlib.h>
#include <yaml.h>

typedef struct SerdecResult {
    bool ok;
    char* error;
} SerdecResult;

typedef struct SerdecYamlContext {
    yaml_parser_t* parser;
    yaml_event_t event;
    bool error;
    char* error_message;
} SerdecYamlContext;

typedef void SerdecYamlVisitor(SerdecYamlContext* context, void* data);

static inline const char* serdec_yaml_event_type_to_string(yaml_event_type_t event_type) {
    switch (event_type) {
    case YAML_NO_EVENT: return "YAML_NO_EVENT";
    case YAML_STREAM_START_EVENT: return "YAML_STREAM_START_EVENT";
    case YAML_STREAM_END_EVENT: return "YAML_STREAM_END_EVENT";
    case YAML_DOCUMENT_START_EVENT: return "YAML_DOCUMENT_START_EVENT";
    case YAML_DOCUMENT_END_EVENT: return "YAML_DOCUMENT_END_EVENT";
    case YAML_ALIAS_EVENT: return "YAML_ALIAS_EVENT";
    case YAML_SCALAR_EVENT: return "YAML_SCALAR_EVENT";
    case YAML_SEQUENCE_START_EVENT: return "YAML_SEQUENCE_START_EVENT";
    case YAML_SEQUENCE_END_EVENT: return "YAML_SEQUENCE_END_EVENT";
    case YAML_MAPPING_START_EVENT: return "YAML_MAPPING_START_EVENT";
    case YAML_MAPPING_END_EVENT: return "YAML_MAPPING_END_EVENT";
    default: return "Unknown Event";
    }
}

static inline void serdec_yaml_consume_event(SerdecYamlContext* context,
    yaml_event_type_t event_type, SerdecYamlVisitor* visitor, void* data)
{
    if (context->error) {
        return;
    }

    yaml_parser_parse(context->parser, &context->event);
    if (event_type != context->event.type) {
        char error_message[96] = {0};
        snprintf(error_message, sizeof(error_message),
            "expected %s, but found %s",
            serdec_yaml_event_type_to_string(event_type),
            serdec_yaml_event_type_to_string(context->event.type));

        context->error_message = strdup(error_message);
        context->error = true;

        yaml_event_delete(&context->event);
        return;
    }

    if (NULL != visitor) {
        visitor(context, data);
    }

    yaml_event_delete(&context->event);
}

///////////////////////////////////////////////////////////////////////////////
// Event Types
////

static inline void serdec_yaml_stream_start(SerdecYamlContext* context) {
    serdec_yaml_consume_event(context, YAML_STREAM_START_EVENT, NULL, NULL);
}

static inline void serdec_yaml_stream_end(SerdecYamlContext* context) {
    serdec_yaml_consume_event(context, YAML_STREAM_END_EVENT, NULL, NULL);
}

static inline void serdec_yaml_document_start(SerdecYamlContext* context) {
    serdec_yaml_consume_event(context, YAML_DOCUMENT_START_EVENT, NULL, NULL);
}

static inline void serdec_yaml_document_end(SerdecYamlContext* context) {
    serdec_yaml_consume_event(context, YAML_DOCUMENT_END_EVENT, NULL, NULL);
}

static inline void serdec_yaml_mapping_start(SerdecYamlContext* context) {
    serdec_yaml_consume_event(context, YAML_MAPPING_START_EVENT, NULL, NULL);
}

static inline void serdec_yaml_mapping_end(SerdecYamlContext* context) {
    serdec_yaml_consume_event(context, YAML_MAPPING_END_EVENT, NULL, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// Visitors
////

static inline void serdec_yaml_visit_field_name(SerdecYamlContext* context,
    void* data)
{
    const char* field_name = (const char*)data;
    if (strncmp(field_name, (const char*)context->event.data.scalar.value,
            context->event.data.scalar.length))
    {
        char error_message[64] = {0};
        snprintf(error_message, sizeof(error_message), "expected field '%s', "
            "got field '%s'", field_name, context->event.data.scalar.value);
        context->error_message = strdup(error_message);
        context->error = true;
    }
}

static inline void serdec_yaml_visit_string(SerdecYamlContext* context,
    void* data)
{
    char** value = (char**)data;
    *value = malloc(context->event.data.scalar.length + 1);
    if (NULL == *value) {
        abort();
    }
    memcpy(*value, context->event.data.scalar.value,
        context->event.data.scalar.length);
}

static inline void serdec_yaml_visit_double(SerdecYamlContext* context,
    void* data)
{
    double* value = (double*)data;
    char temp[64] = {0};
    if (sizeof(temp) < context->event.data.scalar.length) {
        snprintf(temp, sizeof(temp), "double field exceeds maximum length "
            "(%zu)", sizeof(temp));
        context->error_message = strdup(temp);
        context->error = true;
        return;
    }

    memcpy(temp, context->event.data.scalar.value,
        context->event.data.scalar.length);
    char* end_pointer = NULL;
    *value = strtod(temp, &end_pointer);
    if (NULL != end_pointer && '\0' != *end_pointer) {
        snprintf(temp, sizeof(temp), "error converting %s to double",
            context->event.data.scalar.value);
        context->error_message = strdup(temp);
        context->error = true;
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Deserialization Helpers
////

static inline void serdec_yaml_mapping_field_name(SerdecYamlContext* context,
    const char* field_name)
{
    serdec_yaml_consume_event(context, YAML_SCALAR_EVENT,
        serdec_yaml_visit_field_name, (void*)field_name);
}

static inline void serdec_yaml_deserialize_string(SerdecYamlContext* context,
    char** value)
{
    serdec_yaml_consume_event(context, YAML_SCALAR_EVENT,
        serdec_yaml_visit_string, value);
}

static inline void serdec_yaml_deserialize_float(SerdecYamlContext* context,
    double* value)
{
    serdec_yaml_consume_event(context, YAML_SCALAR_EVENT,
        serdec_yaml_visit_double, value);
}

#endif // SERDEC_YAML_H

///////////////////////////////////////////////////////////////////////////////
