package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"
)

func main() {

	//argsWithProg := os.Args
	argsWithoutProg := os.Args[1:]

	if len(argsWithoutProg) < 2 {
		fmt.Println(`
		      __                   ___ 
		|    /  \  |\/|  /\  |__/ |__  
		|___ \__/  |  | /~~\ |  \ |___                                  

USAGE

	param 1 - Project Name

	param 2 - Input directory

	param 3 - Output directory
	`)
		return
	}

	files, err := ioutil.ReadDir(argsWithoutProg[1])
	if err != nil {
		log.Fatal(err)
	}

	for _, f := range files {
		fileName := f.Name()
		name := strings.TrimSuffix(fileName, filepath.Ext(fileName))

		outputDir := "./"
		if len(argsWithoutProg) > 1 {
			outputDir = argsWithoutProg[2]
		}

		file, err := os.Create(outputDir + "/" + name + ".xml") // Truncates if file already exists, be careful!
		if err != nil {
			log.Fatalf("failed creating file: %s", err)
			return
		}

		file.WriteString(`
<material>
	<technique name="Techniques/DiffAlphaTranslucent.xml" />
	<texture unit="diffuse" name="Textures/` + argsWithoutProg[0] + "/" + fileName + `" />
	<parameter name="MatDiffColor" value="1 1 1 0.05" />
	<parameter name="name" value="` + name + `" />
</material>
		`)
		file.Close()
	}
}
