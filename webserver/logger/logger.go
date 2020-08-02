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
	"os"
)

// ChiekErri - brr
func ChiekErri(err error, additionInfo ...string) error {
	if err != nil {
		f, err := os.OpenFile("./errlog.LOG", os.O_APPEND|os.O_WRONLY, 0644)
		_, err = f.WriteString(err.Error() + "\n")
		f.Close()
	}
	return err
}
