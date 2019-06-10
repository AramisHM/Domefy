package rest_test

import (
	"fmt"
	"testing"

	"github.com/AramisHM/Domefy/webserver/config"

	"github.com/AramisHM/Domefy/webserver/rest"
)

func TestSend(t *testing.T) {

	// Must be called first !!!
	config.LoadConfigurations()

	payload := make(map[string]interface{})
	payload["test"] = "test"

	retBody, gotRet := rest.SendPostWithParameters("https://ptsv2.com/t/a97q9-1539779894/post", payload, "", false)

	if gotRet {
		fmt.Println((string)(retBody))
		t.Log(fmt.Sprintf("Working"))
	} else {
		fmt.Println("No response from server")
		t.Error(fmt.Sprintf("No response from server"))
	}
}
