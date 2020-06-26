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
	SelectedIndexHostIP  int    `toml:"selected_index_host_ip"`
	HostingAddress       string `toml:"hosting_address"`
	HostingPort          string `toml:"hosting_port"`
	SystemVersion        string `toml:"system_version"`
	Win32DomefyBinary    string `toml:"win32_domefy_binary"`
	Win32DomefyBinaryCef string `toml:"win32_domefy_binary_cef"`
	GNULinuxDomefyBinary string `toml:"gnulinux_domefy_binary"`
}

var Config tomlConfig

func printMissingConfigFileMessage() {
	fmt.Println(`NO .TOML FILE FOUND`)
}

// LoadConfigurations
func LoadConfigurations() {
	if _, err := toml.DecodeFile("config.toml", &Config); err != nil {
		fmt.Println(err, " - Could not load TOML file.")
		printMissingConfigFileMessage()
		os.Exit(3)
	}
	fmt.Println(Config)
}
