/*
 * File: logger.go
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Thursday, 6th June 2019 7:14:23 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Thursday, 6th June 2019 7:14:40 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */

package logger

import (
	"fmt"
	"os"
	"runtime"
)

// MyCaller returns the caller of the function that called it
func MyCaller() string {
	// we get the callers as uintptrs - but we just need 1
	fpcs := make([]uintptr, 1)

	// skip 3 levels to get to the caller of whoever called Caller()
	n := runtime.Callers(3, fpcs)
	if n == 0 {
		return "n/a" // proper error her would be better
	}

	// get the info of the actual function that's in the pointer
	fun := runtime.FuncForPC(fpcs[0] - 1)
	if fun == nil {
		return "n/a"
	}

	// return its name
	return fun.Name()
}

// CheckErr - Centralized function to log the errors, if error occurs, the second parameter
// allows you to give a more human description of the error
// Added Sentry.io monitoring server support
func CheckErr(err error, additionInfo ...string) error {
	if err != nil {

		additInfo := "(null)"
		if len(additionInfo) > 0 {
			additInfo = additionInfo[0]
		}
		fmt.Println(MyCaller() + " ERROR -> " + err.Error() + "\nMore info: " + additInfo)

		f, err := os.OpenFile("./BACKENED.LOG", os.O_APPEND|os.O_WRONLY, 0644)
		_, err = f.WriteString(err.Error() + "\n")

		f.Close()

	}
	return err
}
