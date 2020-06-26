/*
 * File: rest.go
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 6th June 2019 7:07:09 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 6th June 2019 7:12:02 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

package rest

import (
	"encoding/json"
	"io/ioutil"

	"github.com/gin-gonic/gin"
)

// GPPAr - get the parameters from the gin context
// note: works with gin package only
func GPPAr(c *gin.Context) map[string]interface{} {
	body := c.Request.Body
	byteStream, _ := ioutil.ReadAll(body)

	// get JSON as a object on Go
	var obj map[string]interface{}
	json.Unmarshal([]byte(byteStream), &obj)

	return obj
}
