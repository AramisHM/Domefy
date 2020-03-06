package domefy

import (
	"bufio"
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
	"strconv"
	"strings"

	"github.com/AramisHM/Domefy/webserver/config"
	"github.com/AramisHM/Domefy/webserver/logger"
	"github.com/AramisHM/Domefy/webserver/rest"
	"github.com/mdp/qrterminal"

	"syscall"

	"github.com/gin-gonic/gin"

	"github.com/TheTitanrain/w32"
	"github.com/hnakamur/w32syscall"
)

// processes - slice of Domefy Processes
var processes []*exec.Cmd
var CEFprocesses []*exec.Cmd
var cefHibernate bool

// indicates if browser has been spawnd (might not be)
var browserSpawnd bool

// HideWindow - Hide a window process by name
func HideWindow(windowName string) {
	err := w32syscall.EnumWindows(func(hwnd syscall.Handle, lparam uintptr) bool {
		h := w32.HWND(hwnd)
		text := w32.GetWindowText(h)
		if strings.Contains(text, windowName) {
			// r := w32.GetWindowRect(h)
			//w32.MoveWindow(h, 0, 7000, 1, 1, true)
			w32.ShowWindow(h, 0)
		}
		return true
	}, 0)
	if err != nil {
		log.Fatalln(err)
	}
}

// ShowWindow - Hide a window process by name
func ShowWindow(windowName string) {
	err := w32syscall.EnumWindows(func(hwnd syscall.Handle, lparam uintptr) bool {
		h := w32.HWND(hwnd)
		text := w32.GetWindowText(h)
		if strings.Contains(text, windowName) {
			// r := w32.GetWindowRect(h)
			w32.ShowWindow(h, 1)
		}
		return true
	}, 0)
	if err != nil {
		log.Fatalln(err)
	}
}

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
	router.POST("/KillDomefy", func(c *gin.Context) { KillDomefy() })
	router.POST("/KillCEF", func(c *gin.Context) { KillCEF(c) })
	router.POST("/StartCEF", func(c *gin.Context) { StartCEF(c) })
	router.POST("/HideCEF", func(c *gin.Context) { HideCEF(c) })
	router.POST("/ShowCEF", func(c *gin.Context) { ShowCEF(c) })
}

// HideCEF -
func HideCEF(c *gin.Context) {
	HideWindow("DomefyCEF")
	cefHibernate = true
}

// ShowCEF -
func ShowCEF(c *gin.Context) {
	ShowWindow("DomefyCEF")
	cefHibernate = false
}

// KillCEF - Kills all runniing process of Domefy
func KillCEF(c *gin.Context) {
	for _, p := range CEFprocesses {
		if err := p.Process.Kill(); err != nil {
			fmt.Errorf("Failed to kill process: %v", err)
		}
	}
	KillProcByNameWindows("fpmed-with-cef.exe")
	CEFprocesses = nil
}

// StartCEF - Starts a Domefy process with CEF support
func StartCEF(c *gin.Context) {
	paramObj := rest.GetPostParameters(c)
	scriptName, gotScript := paramObj["script"].(string)
	useCef, gotCefFlag := paramObj["cef"].(bool)
	urlForCef, gotUrl := paramObj["url"].(string)

	if gotScript {
		// get configuration globals
		fmt.Println("GOOS: ", runtime.GOOS)
		fmt.Println(os.Getwd())

		if runtime.GOOS == "windows" {
			KillCEF(c) // Kill previous processes

			win32Bin := config.Config.Win32DomefyBinaryCef

			fmt.Println(win32Bin + " " + scriptName)
			// force a safe script // TODO: make this less HACKY
			scriptName = "Scripts/CEFOnly.as"
			s := []string{win32Bin, scriptName}

			if gotCefFlag && useCef && gotUrl && len(urlForCef) > 0 {
				s = append(s, urlForCef)
			} else {
				s = append(s, "file:///./Data/Textures/assets-march/pucpr-shadown.png") // default puc logo
			}

			cmd := exec.Command(s[0], s[1:]...)

			go StartDomefy(cmd)
		} else { // Linux or similar variant
			fmt.Println("CEF IS NOT SUPPORTED ON LINUX YET!")
		}
		// save the parameters in a local file
		c.JSON(http.StatusOK, "done")
		return
	}
	c.JSON(http.StatusBadRequest, "Something went wrong")
	return
}

// SetExampleTextMessage - Used to send commands to the CPP instance of Domefy. TODO: FIXME: change this name
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

	// for CEF only
	if !cefHibernate {
		conn2, err2 := net.Dial("udp", "127.0.0.1:42872")
		if err2 != nil {
			fmt.Printf("Some error %v", err2)
			return
		}
		fmt.Fprintf(conn2, cmdStr)
	}

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

func printIPs(ips []string) {
	for i, ip := range ips {
		fmt.Println(i+1, " ", ip)
	}
}

// GetThisMachineIpAddresses - Returns the string with this machine IP address
func GetThisMachineIpAddresses() string {
	ifaces, err := net.Interfaces()

	if err != nil {
		fmt.Println(err)
		return ""
	}

	ips := make([]string, 1)

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
			// fmt.Println(ip)
			ips = append(ips, ip.String())
		}
	}
	fmt.Println("The available Network interface IPs are:")
	printIPs(ips)
	fmt.Printf("Please, select one of these with a number from 1 to %d according to the order printed above.\n", len(ips))

	if len(ips) == 0 {
		fmt.Println("Your computer seems to have no Network Interface, this application is of no use without Network Interfaces. Exiting")
		os.Exit(-142) // Some random number just to aid debugging
	}

	selected := 0
	scanner := bufio.NewScanner(os.Stdin)
	nTriesToScan := 3 // this is to prevent infinite loop...

	if config.Config.SelectedIndexHostIP > 0 && config.Config.SelectedIndexHostIP <= len(ips) {
		selected = config.Config.SelectedIndexHostIP - 1
	} else {
		for nTriesToScan > 0 {

			if scanner.Scan() {
				// fmt.Println(scanner.Text())
				selected, _ = strconv.Atoi(scanner.Text())
			}

			if selected < 1 || selected > len(ips) {
				nTriesToScan--
				fmt.Println("Invalid IP selected... ", nTriesToScan, " tries left... please try again... Here is the list of IPs:")
				printIPs(ips)
			} else {
				selected-- // 1 indexed to 0 indexed
				break
			}
		}
		if nTriesToScan <= 0 {
			selected = 1 // default 127.0.0.1
		}
	}

	fmt.Println("Selected Network Interface with IP", ips[selected])

	adminURL := fmt.Sprintf("http://%s:%d/%s", ips[selected], 3000, "custom_pages/admin/admin.html")
	qrterminal.Generate(adminURL, qrterminal.L, os.Stdout)
	fmt.Printf(`
	%s
	Use the QR code above to open
	the administration page on the browser.
	Note that if the IP is local,
	you should be connected to the
	respective Network aswell.
	`, adminURL)

	// TODO: Parametrizate all the variables

	// write the configuration file with the selected ip address
	dataBytes, _ := ioutil.ReadFile("./frontwebapp/static/config.js.template")
	newData := strings.Replace((string)(dataBytes), "{{.thisMachineIP}}", ips[selected], -1)
	d1 := []byte(newData)
	ioerr := ioutil.WriteFile("./frontwebapp/static/config.js", d1, 0644)
	logger.CheckErr(ioerr)
	fmt.Println("WriteFile err: ", ioerr)

	return ips[selected]
}

// StartDomefy - Runs a Domefy instance
func StartDomefy(cmd *exec.Cmd) {

	cmd.Stdout = os.Stdout
	fmt.Println("Starting Domefy subprocess\n")
	err := cmd.Run()
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println("Domefy Finished\n")
}

// KillProcByNameWindows - kills a process on windows by its name
func KillProcByNameWindows(name string) {
	// FORCE KILL
	// Taskkill /F /IM fpmed.exe
	// force kill remaining fpmed with browser instances
	s := []string{"Taskkill", "/F", "/T", "/IM", name}
	cmd := exec.Command(s[0], s[1:]...)
	cmd.Stdout = os.Stdout
	err := cmd.Run()
	if err != nil {
		fmt.Println("Failed to kill FPMED.", err)
	} else {
		fmt.Println("KILLED fpmed process\n")
	}
}

// KillDomefy - Kills all runniing process of Domefy
func KillDomefy() {
	for _, p := range processes {

		if runtime.GOOS == "windows" {

			if err := p.Process.Kill(); err != nil {
				fmt.Errorf("Failed to kill process: %v", err)
			}
		} else { // unix variant
			if err := p.Process.Kill(); err != nil {
				fmt.Errorf("Failed to kill process: %v", err)
			}
		}
	}
	if runtime.GOOS == "windows" {
		KillProcByNameWindows("fpmed.exe")
	}

	processes = nil
}

// StartScriptApplication - Starts a Domefy process and make it run a specific script
func StartScriptApplication(c *gin.Context) {
	paramObj := rest.GetPostParameters(c)
	scriptName, gotScript := paramObj["script"].(string)
	configName, gotConfig := paramObj["viewport_config"].(string)
	// useCef, gotCefFlag := paramObj["cef"].(bool)
	// urlForCef, gotUrl := paramObj["url"].(string)

	if gotScript {
		// get configuration globals
		fmt.Println("GOOS: ", runtime.GOOS)
		fmt.Println(os.Getwd())
		var cmd *exec.Cmd
		KillDomefy() // kill and remove all previous processes
		if runtime.GOOS == "windows" {
			win32Bin := config.Config.Win32DomefyBinary

			fmt.Println(win32Bin + " " + scriptName)
			s := []string{win32Bin, scriptName}

			if gotConfig {
				s = append(s, configName)
			} else {
				s = append(s, "./presets/default.json")
			}

			cmd := exec.Command(s[0], s[1:]...)

			go StartDomefy(cmd)
		} else { // Linux or similar variant
			fmt.Println(config.Config.GNULinuxDomefyBinary + " " + scriptName)
			cmd = exec.Command(config.Config.GNULinuxDomefyBinary, scriptName)
			go StartDomefy(cmd)
		}

		// save the parameters in a local file
		c.JSON(http.StatusOK, "done")
		return
	}
	c.JSON(http.StatusBadRequest, "No script supplied.")
	return
}
