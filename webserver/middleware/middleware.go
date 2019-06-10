/*
 * File: middleware.go
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 6th June 2019 7:10:12 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 6th June 2019 7:10:55 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

package middleware

import (
	"log"
	"time"

	"github.com/gin-gonic/gin"
)

// Feuerwand - Is our multipurpose, generic middleware (still a prototype)
func Feuerwand() gin.HandlerFunc {
	return func(c *gin.Context) {
		t := time.Now()

		// req := c.Request
		// radd := req.RemoteAddr
		// url := req.URL.Path
		// fmt.Println(req, radd, url)

		c.Next()

		// after request

		latency := time.Since(t)
		log.Print(latency)

		// access the status we are sending
		status := c.Writer.Status()
		log.Println(status)
	}
}
