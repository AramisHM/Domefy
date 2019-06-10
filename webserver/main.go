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
	"log"
	"net/http"
	"time"

	"github.com/AramisHM/Domefy/webserver/services/domefy"
	"github.com/AramisHM/Domefy/webserver/services/stress"

	"github.com/AramisHM/Domefy/webserver/middleware"

	"github.com/AramisHM/Domefy/webserver/config"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
)

func main() {
	// Must be called first
	config.LoadConfigurations()

	// add CORS policy on the router
	router := gin.Default()
	router.Use(cors.Default())

	if config.IsDockerized() {
		fmt.Println("Running in Docker container")
		// Wait for slow containers to boot up
		time.Sleep(10 * time.Second)
	} else {
		fmt.Println("Running in regular system")
	}

	// use middleware ?
	if config.Config.UseMiddleware {
		router.Use(middleware.Feuerwand()) // book the middleware so gin may use it
	}

	// Hookup services
	stress.RegisterStress(router)
	domefy.RegisterDomefy(router)

	// middleware example with gorulp
	// testg := router.Group("/test-middleware")
	// // at this group, we use the middleware\
	// testg.Use(middleware.Feuerwand())
	// {
	// 	testg.GET("/login", func(c *gin.Context) {
	// 		example := c.MustGet("example").(string)

	// 		// it would print: "12345"
	// 		log.Println(example)
	// 	})
	// 	testg.GET("/submit", func(c *gin.Context) {
	// 		example := c.MustGet("example").(string)

	// 		// it would print: "12345"
	// 		log.Println(example)
	// 	})
	// 	testg.GET("/read", func(c *gin.Context) {
	// 		example := c.MustGet("example").(string)

	// 		// it would print: "12345"
	// 		log.Println(example)
	// 	})

	// 	// nested group
	// 	testing := testg.Group("testing")
	// 	testing.GET("/analytics", func(c *gin.Context) {
	// 		example := c.MustGet("example").(string)

	// 		// it would print: "12345"
	// 		log.Println(example)
	// 	})
	// }

	fmt.Println("Copyright (c) 2019, Pteronura.com, all rights reserverd")
	fmt.Println("Domefy Backend Server [v", config.Config.SystemVersion, "]")

	fs := http.FileServer(http.Dir("static"))
	http.Handle("/", fs)

	log.Println("Listening...")
	http.ListenAndServe(":3000", nil)

	if config.IsDockerized() {
		router.RunTLS(":9090", "certificate.crt", "private.key")
	} else {
		router.Run(":9090")
	}
}
