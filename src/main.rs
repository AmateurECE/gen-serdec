///////////////////////////////////////////////////////////////////////////////
// NAME:            main.rs
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Application entrypoint.
//
// CREATED:         09/09/2022
//
// LAST EDITED:     09/10/2022
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

use clap::Parser;
use serde::Deserialize;
use std::collections::HashMap;
use std::fs::File;
use std::io;
use std::io::BufRead;
use std::path::Path;

///////////////////////////////////////////////////////////////////////////////
// Type Definitions
////

#[derive(Debug, Deserialize)]
struct SchemaProperties {
    #[serde(rename = "$ref")]
    pub schema_ref: Option<String>,
    #[serde(rename = "type")]
    pub data_type: Option<String>,
    pub description: Option<String>,
    pub default: Option<String>,
}

#[derive(Debug, Deserialize)]
struct Schema {
    #[serde(rename = "$schema")]
    pub schema: String,
    pub id: String,
    pub tag: String,
    pub title: String,
    pub description: String,

    #[serde(rename = "type")]
    pub schema_type: String,
    pub properties: HashMap<String, SchemaProperties>,
    pub required: Vec<String>,

    #[serde(rename = "additionalProperties")]
    pub additional_properties: bool,
}

#[derive(Debug)]
struct DataDefinition {
    pub identifier: String,
    pub prefix: String,
    pub schema: Schema,
}

#[derive(Parser, Debug)]
#[clap(author, version, about, long_about = None)]
struct Args {
    /// yaml-schema file to parse
    #[clap(value_parser)]
    name: String,
}

///////////////////////////////////////////////////////////////////////////////
// Code Generation
////

fn stem_to_prefix(stem: &str) -> String {
    stem.to_string().replace("-", "_")
}

fn stem_to_identifier(stem: &str) -> String {
    stem.to_string().split("-").map(|word| {
        let mut chars = word.chars().collect::<Vec<char>>();
        chars[0] = chars[0].to_uppercase().nth(0).unwrap();
        chars.into_iter().collect::<String>()
    }).collect::<String>()
}

fn comment_line<W: io::Write>(out: &mut W) -> io::Result<()> {
    out.write(b"//\n").map(|_| ())
}

fn blank_line<W: io::Write>(out: &mut W) -> io::Result<()> {
    out.write(b"\n").map(|_| ())
}

const MIT_LICENSE: &str = "\
Permission is hereby granted, free of charge, to any person obtaining a copy\n\
of this software and associated documentation files (the \"Software\"), to\n\
deal in the Software without restriction, including without limitation the\n\
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or\n\
sell copies of the Software, and to permit persons to whom the Software is\n\
furnished to do so, subject to the following conditions:\n\
\n\
The above copyright notice and this permission notice shall be included in\n\
all copies or substantial portions of the Software.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n\
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n\
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n\
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n\
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING\n\
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS\n\
IN THE SOFTWARE.\n\
";

fn header<W: io::Write>(out: &mut W, name: &str) -> io::Result<()> {
    comment_line(out)?;
    out.write(format!("// {}\n", name).as_bytes())?;
    comment_line(out)?;
    out.write(b"// Copyright 2022, Ethan D. Twardy\n")?;
    comment_line(out)?;
    let license_reader = io::BufReader::new(io::Cursor::new(MIT_LICENSE));
    for line in license_reader.lines() {
        out.write(format!("// {}\n", line?).as_bytes())?;
    }
    comment_line(out)?;
    Ok(())
}

fn preamble<W: io::Write>(out: &mut W) -> io::Result<()> {
    out.write(b"#include <yaml/yaml.h>\n").map(|_| ())
}

fn deserialize_definitions<W: io::Write>(
    out: &mut W,
    definition: &DataDefinition,
) -> io::Result<()> {
    out.write(format!(
        "int {}_{}_from_{}_{}(const char* string, {}* data) {{\n",
        &definition.prefix, "deserialize", "yaml", "string",
        &definition.identifier).as_bytes()
    )?;
    out.write(b"}\n");
    Ok(())
}

fn source_file<W: io::Write>(
    out: &mut W,
    name: &str,
    definition: &DataDefinition,
) -> io::Result<()> {
    header(out, name)?;
    blank_line(out)?;
    preamble(out)?;
    blank_line(out)?;
    deserialize_definitions(out, definition)
}

///////////////////////////////////////////////////////////////////////////////
// Main
////

fn main() -> anyhow::Result<()> {
    let args = Args::parse();
    let schema: Schema = serde_yaml::from_reader(File::open(&args.name)?)?;
    let stem = Path::new(&args.name).file_stem().unwrap().to_os_string()
        .into_string().unwrap();
    let data = DataDefinition {
        prefix: stem_to_prefix(&stem),
        identifier: stem_to_identifier(&stem),
        schema,
    };

    let source_name = stem + ".c";
    let mut stdout = io::stdout().lock();
    source_file(&mut stdout, &source_name, &data)?;
    Ok(())
}

///////////////////////////////////////////////////////////////////////////////
