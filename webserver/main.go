/*
 * File: main.go
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 6th June 2019 6:30:21 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 6th June 2019 6:30:31 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 *
 * This software is the backend server for Domefy application
 */

package main

import (
	"fmt"

	"github.com/AramisHM/Domefy/webserver/config"
	"github.com/AramisHM/Domefy/webserver/services/domefy"
	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
)

func main() {
	config.LoadConfigurations()
	router := gin.Default()
	router.Use(cors.Default())
	domefy.RegisterDomefy(router)
	fmt.Println("Copyright (c) 2019, Pteronura.com, all rights reserverd")
	fmt.Println("Domefy Backend Server [v", config.Config.SystemVersion, "]")
	domefy.GetThisMachineIpAddresses()

	router.Run(":9090")

}
