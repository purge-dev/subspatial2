<p align="center"><b>Subspatial</b></p>
<p align="center"><img src="https://cdn.discordapp.com/attachments/447292498226970644/795233305795887114/unknown.png"></p>

## About
Subspatial is a lightweight MERVBot plugin written in C++ that provides bot functionality between Subspace Continuum and Discord. It is currently in operation at **SSCJ Devastation** and is hosted on an AWS t2.micro EC2 instance.

Subspatial's current build only supports a single Discord server use-case as no other SSC zones have demonstrated interest in the project; however, it may easily be edited to support multiple servers on one build. Please fork the repo and contact me for more information.

## Usage
* Simply clone the branch of your choice and edit the include directories to match those of your local environment.
* Make sure the working language is set to CPP 17.
* Build the plugin typically as you would a MERVBot DLL.

## Dependencies
* [aegiscpp](https://github.com/zeroxs/aegis.cpp)
* [cURL](https://curl.se/libcurl/)

## Aegis (C++ Discord Library)
[![Build Status](https://travis-ci.org/zeroxs/aegis.cpp.svg?branch=master)](https://travis-ci.org/zeroxs/aegis.cpp) [![Discord](https://discordapp.com/api/guilds/287048029524066334/widget.png)](https://discord.gg/w7Y3Bb8) [![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/zeroxs/aegis.cpp/blob/master/LICENSE)

This MERVBot plugin was written using Aegis library (header only config). Please check [here](https://github.com/zeroxs/aegis.cpp) for information on how to build it.
