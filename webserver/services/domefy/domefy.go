package domefy

import (
	"fmt"
	"io/ioutil"
	"net"
	"net/http"

	"github.com/AramisHM/Domefy/webserver/rest"

	"github.com/gin-gonic/gin"
)

func SendExample() {

	// Very rigorous stress test to send data via REST protocol
	url := "http://localhost:9090/stress"

	header := make(map[string]interface{})
	header["Content-Type"] = "application/json"
	// header["Authorization"] = Access.accessToken

	bodyBytes, gotData := rest.SendManualREST(url, nil, header, "POST")
	if gotData {
		// ret, _ := rest.ResponseBytesToObject(bodyBytes)
		fmt.Println(string(bodyBytes))
	}
}

// RegisterDomefy -
func RegisterDomefy(router *gin.Engine) {
	router.POST("/setExampleTextMessage", func(c *gin.Context) { SetExampleTextMessage(c) })
	router.POST("/getConfigJSON", func(c *gin.Context) { GetConfigJSON(c) })
}

// SetExampleTextMessage - Sets the debug text auxiliar message
func SetExampleTextMessage(c *gin.Context) {
	paramObj := rest.GetPostParameters(c)
	cmdStr := paramObj["command"].(string)

	conn, err := net.Dial("udp", "127.0.0.1:42871")
	if err != nil {
		fmt.Printf("Some error %v", err)
		return
	}
	fmt.Fprintf(conn, cmdStr)

	c.JSON(http.StatusOK, "done")

	// actually send package to domefy c++ application
}

// GetConfigJSON - Returns the JSON with the configuration parameters
func GetConfigJSON(c *gin.Context) {
	dat, _ := ioutil.ReadFile("../bin/config.json")
	fmt.Print(string(dat))

	c.JSON(http.StatusOK, string(dat))

	// actually send package to domefy c++ application
}
