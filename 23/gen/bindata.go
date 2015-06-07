package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"os"
)

const bindataPrefix = "bindata/"

func main() {
	out, err := os.Create("gen_bindata.go")
	if err != nil {
		log.Fatalln(err)
	}

	files, err := ioutil.ReadDir(bindataPrefix)
	if err != nil {
		log.Fatalln(err)
	}

	fmt.Fprintf(out, "package main\n")
	fmt.Fprintf(out, "var binData = map[string]string{\n")

	for _, fi := range files {
		name := fi.Name()

		fmt.Fprintf(out, "\"%s\": ", name)

		file, err := os.Open(bindataPrefix + name)
		if err != nil {
			log.Fatalln(err)
		}

		bytes, err := ioutil.ReadAll(file)
		if err != nil {
			log.Fatalln(err)
		}

		fmt.Fprintf(out, "\"")

		for _, b := range bytes {
			fmt.Fprintf(out, "\\x%02x", b)
		}

		fmt.Fprintf(out, "\",\n")
	}

	fmt.Fprintf(out, "}\n")
}
