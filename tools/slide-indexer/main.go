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

		SLIDE-INDEXER

USAGE

	param 1 - Project Name

	param 2 - Input directory

	param 3 - Output directory
	`)
		return
	}

	// get files from directory
	inputDir := argsWithoutProg[1]
	files, err := ioutil.ReadDir(inputDir)
	if err != nil {
		log.Fatal(err)
	}
	outputDir := "./"
	if len(argsWithoutProg) > 1 {
		outputDir = argsWithoutProg[2]
	}

	var materialNames []string

	// Generate material XMLs
	for _, f := range files {
		fileName := f.Name()
		name := strings.TrimSuffix(fileName, filepath.Ext(fileName))

		outFileName := outputDir + name + ".xml"
		file, err := os.Create(outFileName) // Truncates if file already exists, be careful!
		if err != nil {
			log.Fatalf("failed creating file: %s", err)
			return
		}

		file.WriteString(`
		<material>
			<technique name="Techniques/DiffOverlay.xml" />
			<texture unit="diffuse" name="` + inputDir + fileName + `" />
			<parameter name="MatDiffColor" value="1 1 1 1" />
			<depthbias constant="-0.00001" slopescaled="0" />
		</material>
		`)

		file.Close()
		materialNames = append(materialNames, outFileName)
	}

	// Generate set.xml
	file, err := os.Create(outputDir + "/set.xml")
	if err != nil {
		log.Fatalf("failed creating file: %s", err)
		return
	} else {
		desc := "generated in slide-indexer"
		file.WriteString(`
		<FDS>
			<info name="` + desc + `" author="` + desc + `" description="` + desc + `" thumbnail="thumbnail.png"/>

			<SlideGroup description = "Things 1" position="1.0f, 1.0f, 1.0f"/>
		`)

		for _, mn := range materialNames {
			file.WriteString(`
				<Slide img_file ="` + mn + `"/>`)
		}

		file.WriteString(`
			</SlideGroup>
		</FDS>
		`)

	}
}
