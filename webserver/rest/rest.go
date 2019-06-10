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
	"bytes"
	"crypto/tls"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"net/url"
	"reflect"
	"strings"

	"github.com/AramisHM/Domefy/webserver/logger"

	"github.com/gin-gonic/gin"
)

// ToFloat64Slice - converts a slice of interface{} to a slice of float64
func ToFloat64Slice(is interface{}) []float64 {
	s := reflect.ValueOf(is)
	if s.Kind() != reflect.Slice {
		fmt.Println("InterfaceSlice() given a non-slice type")
	} else {
		var retSlice []float64
		for i := 0; i < s.Len(); i++ {
			retSlice = append(retSlice, s.Index(i).Interface().(float64))
		}
		return retSlice
	}
	return nil
}

// GetPostParameters - get the parameters from the gin context
// note: works with gin package only
func GetPostParameters(c *gin.Context) map[string]interface{} {
	body := c.Request.Body
	byteStream, _ := ioutil.ReadAll(body)

	// get JSON as a object on Go
	var obj map[string]interface{}
	json.Unmarshal([]byte(byteStream), &obj)

	return obj
}

// GenerateGetParameters - Creates the serialized string that goes in the GET URL
func GenerateGetParameters(obj map[string]interface{}, questionmark bool) (string, bool) {
	var buffer bytes.Buffer
	flagFirst := questionmark // should it begin with questionmark?

	//buffer.WriteString("?") // Add manually

	for k := range obj {
		// try assert the value to string
		// if fails, we dont add the given
		// parameter to the final URL
		value, asserted := obj[k].(string)

		if asserted {
			if flagFirst {
				buffer.WriteString("?")
				flagFirst = false
			} else {
				buffer.WriteString("&")
			}
			buffer.WriteString((string)(k))
			buffer.WriteString("=")
			buffer.WriteString(value)
		} else {
			return "", false
		}
	}
	return buffer.String(), true
}

// ResponseBytesToObject - Convert the responde from bytes to an object
func ResponseBytesToObject(body []byte) (map[string]interface{}, bool) {
	response := make(map[string]interface{})
	err := json.Unmarshal(body, &response)
	if err != nil {
		fmt.Println(err)
		fmt.Println(response)
	}
	if len(string(body)) == 0 {
		return nil, false
	}

	return response, true
}

// ResponseBytesToObjectSlice - Convert the responde from bytes to an object slice
func ResponseBytesToObjectSlice(body []byte) ([]map[string]interface{}, bool) {
	var response []map[string]interface{}
	err := json.Unmarshal(body, &response)
	if err != nil {
		fmt.Println(err)
		//fmt.Println(response)
		return nil, false
	}
	if len(string(body)) == 0 {
		return nil, false
	}

	return response, true
}

// createPostRequet - create a request
func createX3WFormPostRequet(urlString string, obj map[string]interface{}, bearer string) (*http.Request, error) {

	var paramComposition bytes.Buffer

	// convert payload object to URL type, to generate the proper form
	for k, v := range obj {
		paramComposition.WriteString(url.QueryEscape(k) + "=" + url.QueryEscape(v.(string)) + "&")
	}
	fmt.Println(paramComposition.String())
	req, err2 := http.NewRequest("POST", urlString, strings.NewReader(paramComposition.String()))
	logger.CheckErr(err2)
	if bearer != "" {
		req.Header.Add("Authorization", bearer)
	}
	req.Header.Add("content-type", "application/x-www-form-urlencoded")
	req.Header.Add("cache-control", "no-cache")
	return req, err2
}

// createPostRequet - create a request
func createPostRequet(url string, obj map[string]interface{}, bearer string) (*http.Request, error) {
	requestByte, err1 := json.Marshal(obj)
	logger.CheckErr(err1)
	payload := bytes.NewReader(requestByte)
	req, err2 := http.NewRequest("POST", url, payload)
	logger.CheckErr(err2)
	if bearer != "" {
		req.Header.Add("Authorization", bearer)
	}
	req.Header.Add("content-type", "application/json")
	return req, err2
}

// createGetRequet - create a request
func createGetRequet(url string, bearer string) (*http.Request, error) {
	// generate the URL
	//urlV := "http://localhost:11111/route?point=-23.530222%2C-46.641866&point=-23.523287%2C-46.638422&locale=pt-BR&vehicle=car&weighting=fastest&elevation=false&use_miles=false&points_encoded=false"
	req, err := http.NewRequest("GET", url, nil)
	req.Header.Add("Authorization", bearer)

	return req, err
}

// -------------- Send REST protocols --------------

// createTLSRenegotiationClienet - Creates the special, renegotiation capable
// http client that Azure's servers need
// see reference: https://github.com/golang/go/issues/5742#issuecomment-273989107
func createTLSRenegotiationClienet() *http.Client {

	// Setup custom HTTPS client, for the azure Server
	tlsConfig := &tls.Config{
		Renegotiation: tls.RenegotiateFreelyAsClient,
	}
	tlsConfig.BuildNameToCertificate()
	transport := &http.Transport{TLSClientConfig: tlsConfig}
	client := &http.Client{Transport: transport}
	return client
}

// SendPostWithParameters - sends a SIMPLE post payload
func SendPostWithParameters(url string, obj map[string]interface{}, bearer string, renegotiation bool) ([]byte, bool) {

	req, reqErr := createPostRequet(url, obj, bearer)
	if logger.CheckErr(reqErr, "Problem while trying to create the request") != nil {
		return nil, false
	}

	var response *http.Response
	var resErr error

	if renegotiation == true {
		response, resErr = createTLSRenegotiationClienet().Do(req)
	} else {
		response, resErr = http.DefaultClient.Do(req)
	}
	if logger.CheckErr(resErr) != nil {
		return []byte{}, false // return empty byte array, and false.
	}

	defer response.Body.Close()
	body, readErr := ioutil.ReadAll(response.Body)

	if readErr != nil {
		logger.CheckErr(readErr)
		return nil, false
	}
	return body, true
}

// SendGetWithParameters - sends a SIMPLE post payload
func SendGetWithParameters(url string, bearer string, renegotiation bool) ([]byte, bool) {

	req, reqErr := createGetRequet(url, bearer)
	if logger.CheckErr(reqErr, "Problem while trying to create the request") != nil {
		return nil, false
	}

	var response *http.Response
	var resErr error

	if renegotiation == true {
		response, resErr = createTLSRenegotiationClienet().Do(req)
	} else {
		response, resErr = http.DefaultClient.Do(req)
	}
	if logger.CheckErr(resErr) != nil {
		return []byte{}, false // return empty byte array, and false.
	}

	defer response.Body.Close()
	body, readErr := ioutil.ReadAll(response.Body)

	if readErr != nil {
		logger.CheckErr(readErr)
		return nil, false
	}

	return body, true
}

// SendXMLBasedPostWithParameters - sends a SIMPLE post payload
func SendXMLBasedPostWithParameters(url string, obj map[string]interface{}, bearer string, renegotiation bool) ([]byte, bool) {

	req, reqErr := createX3WFormPostRequet(url, obj, bearer)
	if logger.CheckErr(reqErr, "Problem while trying to create the request") != nil {
		return nil, false
	}

	var response *http.Response
	var resErr error

	if renegotiation == true {
		response, resErr = createTLSRenegotiationClienet().Do(req)
	} else {
		response, resErr = http.DefaultClient.Do(req)
	}
	if logger.CheckErr(resErr) != nil {
		return []byte{}, false // return empty byte array, and false.
	}

	defer response.Body.Close()
	body, readErr := ioutil.ReadAll(response.Body)

	if readErr != nil {
		logger.CheckErr(readErr)
		return nil, false
	}
	return body, true
}

// SendManualREST - All configurable
// TODO: Make return false case
func SendManualREST(url string, payload map[string]interface{}, header map[string]interface{}, restType string) ([]byte, bool) {

	var req *http.Request
	var payloadReader *strings.Reader
	if restType == "POST" {
		payloadBytes, err := json.Marshal(payload)
		if logger.CheckErr(err) != nil {
			return nil, false
		}
		payloadReader = strings.NewReader(string(payloadBytes))
		req, _ = http.NewRequest(restType, url, payloadReader)
	} else if restType == "GET" {
		//fmt.Println(url)
		req, _ = http.NewRequest(restType, url, nil)
	}

	for k, v := range header {
		req.Header.Add(k, v.(string))
	}

	res, errCliDo := http.DefaultClient.Do(req)

	if logger.CheckErr(errCliDo) != nil {
		fmt.Println(errCliDo)
		return nil, false
	}

	body, _ := ioutil.ReadAll(res.Body)
	res.Body.Close()
	//fmt.Println(res)
	//fmt.Println(string(body))

	return body, true
}
