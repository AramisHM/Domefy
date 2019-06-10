package stress

import (
	"fmt"
	"net/http"

	"github.com/AramisHM/Domefy/webserver/rest"

	"github.com/gin-gonic/gin"
)

func RestStressTest() {

	// Very rigorous stress test to send data via REST protocol
	url := "http://localhost:9090/stress"

	header := make(map[string]interface{})
	header["Content-Type"] = "application/json"
	// header["Authorization"] = Access.accessToken

	bodyBytes, gotData := rest.SendManualREST(url, nil, header, "GET")
	if gotData {
		// ret, _ := rest.ResponseBytesToObject(bodyBytes)
		fmt.Println(string(bodyBytes))
	}
}

// RegisterStress -
func RegisterStress(router *gin.Engine) {
	router.GET("/stress", func(c *gin.Context) { AnswerStress(c) })
}

func AnswerStress(c *gin.Context) {
	c.JSON(http.StatusOK, "howdy howdy schlimy bowdy")
}
