///////////////////////////////////////////////////////////////////////////////
// NAME:            main.rs
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Application entrypoint.
//
// CREATED:         09/09/2022
//
// LAST EDITED:     09/09/2022
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

use std::error::Error;
use std::fs::File;
use std::collections::HashMap;
use serde::Deserialize;
use serde_yaml;
use clap::Parser;

#[derive(Debug, Deserialize)]
struct SchemaProperties {
    #[serde(rename = "$ref")]
    pub schema_ref: Option<String>,
    #[serde(rename = "type")]
    pub property_type: Option<String>,
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

#[derive(Parser, Debug)]
#[clap(author, version, about, long_about = None)]
struct Args {
    #[clap(value_parser)]
    name: String,
}

fn main() -> Result<(), Box<dyn Error>> {
    let args = Args::parse();
    let schema: Schema = serde_yaml::from_reader(File::open(&args.name)?)?;
    println!("{:?}", schema);
    Ok(())
}

///////////////////////////////////////////////////////////////////////////////
