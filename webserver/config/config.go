/*
 * File: config.go
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 6th June 2019 7:07:38 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 6th June 2019 7:26:10 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

package config

import (
	"fmt"
	"os"

	"github.com/BurntSushi/toml"
)

// _isDockerized is a flag that indicates if the process is running in a Docker container
// -1 = uninitialized
//  1 = running in docker container
//  0 = not running in docker container
var _isDockerized = -1

type tomlConfig struct {

	// HostingAddress - The IP or Domain where this service is running
	HostingAddress string `toml:"hosting_address"`

	// HostingPort - The port where this service is running
	HostingPort string `toml:"hosting_port"`

	// DevelopmentKey - Development host key. It's function is to restrict access from testing
	// handles from our service There is no righ or wront, it can be anything
	// we decide.
	DevelopmentKey string `toml:"development_key"`

	// SystemVersion - describes this software version
	SystemVersion string `toml:"system_version"`

	// UseMiddleware - Prototype function to use middleware
	UseMiddleware bool `toml:"use_middleware"`

	// path with executable for the domefy application binary
	Win32DomefyBinary    string `toml:"win32_domefy_binary"`
	GNULinuxDomefyBinary string `toml:"gnulinux_domefy_binary"`

	Databases map[string]Database
}

// Database - simple database structure
type Database struct {
	IP   string
	Port int
	Name string
	User string
	Pass string
}

// Config - global variable that holds the configuration of the web-server
var Config tomlConfig

func printMissingConfigFileMessage() {
	fmt.Println(`
	+-----------------------------------------------+
	|    NO .TOML FILE FOUND                        |
	|                                               |
	|    --------------------                       |
	|                                               |
	|    IF YOU ARE TESTING, MAKE SURE TO COPY      |
	|    THE CONFIG FILES TO EACH TEST FOLDER.      |
	|                                               |
	|    YOU CAN DO THIS AUTOMATICALLY BY RUNNING   |
	|    create_links.sh                            |
	|                                               |
	+-----------------------------------------------+

	`)
}

// HasConfigBeenLoaded - Informs if any configuration file has been loaded
func (c tomlConfig) HasConfigBeenLoaded() bool {
	if len(c.Databases) > 0 {
		return true
	}
	return false
}

// LoadConfigurations - loads the configuration for the web-server into memory
func LoadConfigurations() {
	if _, err := toml.DecodeFile("config.toml", &Config); err != nil {
		fmt.Println(err, " - Could not load TOML file.")
		printMissingConfigFileMessage()
		os.Exit(3)
	}

	fmt.Println(Config)

	// fmt.Printf("Title: %s\n", Config)
	// fmt.Printf("Title: %d\n", Config.Databases["postgres"].Port)
}

// IsDockerized - Determinates if process is running in a docker container.
// according to https://stackoverflow.com/questions/23513045/how-to-check-if-a-process-is-running-inside-docker-container
// Docker creates a file /.dockerenv
// therefor, we can simply check if it exists
func IsDockerized() bool {
	if _isDockerized == -1 {
		// Check if docker environment file exists
		if _, err := os.Stat("/.dockerenv"); os.IsNotExist(err) {
			_isDockerized = 0
		} else {
			_isDockerized = 1
		}
	}
	if _isDockerized == 1 {
		return true
	} // else
	return false
}
