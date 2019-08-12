package domefy

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"

	"github.com/AramisHM/Domefy/webserver/config"
	"github.com/AramisHM/Domefy/webserver/rest"

	"github.com/gin-gonic/gin"
)

// processes - slice of Domefy Processes
var processes []*exec.Cmd

// SendExample - Sends a single dummy POST to own API.
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
	router.POST("/saveCalibrationParameters", func(c *gin.Context) { SaveCalibrationParameters(c) })
	router.POST("/StartScriptApplication", func(c *gin.Context) { StartScriptApplication(c) })
	router.POST("/KillAllApplicationProcesses", func(c *gin.Context) { KillAllApplicationProcesses() })
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
	// send package to domefy c++ application
	fmt.Fprintf(conn, cmdStr)

	c.JSON(http.StatusOK, "done")
}

// GetConfigJSON - Returns the JSON with the configuration parameters
func GetConfigJSON(c *gin.Context) {
	dat, _ := ioutil.ReadFile("../bin/config.json")
	//fmt.Print(string(dat))
	var obj map[string]interface{}

	json.Unmarshal(dat, &obj)

	presetFilePath := obj["presets_file"].(string)
	presetsDat, _ := ioutil.ReadFile("../bin/" + presetFilePath)
	c.JSON(http.StatusOK, string(presetsDat))
}

// SaveCalibrationParameters - Receives a JSON parametrization of the calibration
func SaveCalibrationParameters(c *gin.Context) {
	paramObj := rest.GetPostParameters(c)
	// calibrationName := paramObj["name"].(string)

	// fmt.Println(paramObj) // debug

	// convert go-map to json
	jsonByteArray, _ := json.Marshal(paramObj)
	fmt.Println(string(jsonByteArray))
	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}
	f, err := os.Create(dir + "/presets/" + paramObj["calibration_name"].(string) + ".json")

	if err != nil {
		fmt.Println(err)
	}

	defer f.Close()
	f.Write(jsonByteArray)

	// save the parameters in a local file
	c.JSON(http.StatusOK, "done")
}

// GetThisMAchineIpAddres - Returns the string with this machine IP address
func GetThisMAchineIpAddres() string {
	ifaces, err := net.Interfaces()

	if err != nil {
		fmt.Println(err)
		return ""
	}

	// handle err
	for _, i := range ifaces {
		addrs, _ := i.Addrs()

		// handle err
		for _, addr := range addrs {
			var ip net.IP
			switch v := addr.(type) {
			case *net.IPNet:
				ip = v.IP
			case *net.IPAddr:
				ip = v.IP
			}
			// process IP address
			fmt.Println(ip)
		}
	}
	return ""
}

// RunDomefy - Runs a Domefy instance
func RunDomefy(cmd *exec.Cmd) {
	KillAllApplicationProcesses() // kill and remove all previous processes
	cmd.Stdout = os.Stdout
	err := cmd.Start()
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println("Started Domefy subprocess %d\n", cmd.Process.Pid)
	processes = append(processes, cmd)
}

func KillAllApplicationProcesses() {
	for _, p := range processes {
		if err := p.Process.Kill(); err != nil {
			fmt.Errorf("Failed to kill process: %v", err)
		}
	}
	processes = nil
}

// StartScriptApplication - Starts a Domefy process and make it run a specific script
func StartScriptApplication(c *gin.Context) {
	paramObj := rest.GetPostParameters(c)
	scriptName, gotScript := paramObj["script"].(string)

	if gotScript {
		// get configuration globals
		fmt.Println("GOOS: ", runtime.GOOS)
		fmt.Println(os.Getwd())
		var cmd *exec.Cmd
		if runtime.GOOS == "windows" {
			fmt.Println(config.Config.Win32DomefyBinary + " " + scriptName)
			cmd = exec.Command(config.Config.Win32DomefyBinary, scriptName)
		} else {
			fmt.Println(config.Config.GNULinuxDomefyBinary + " " + scriptName)
			cmd = exec.Command(config.Config.GNULinuxDomefyBinary, scriptName)
		}
		go RunDomefy(cmd)

		// save the parameters in a local file
		c.JSON(http.StatusOK, "done")
		return
	}
	c.JSON(http.StatusBadRequest, "Something went wrong")
	return
}

// StopScriptApplication - Stops the Domefy processes
func StopScriptApplication(c *gin.Context) {
}
