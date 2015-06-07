//go:generate go run gen/bindata.go
package main

import (
	"flag"
)

func main() {
	listen := flag.String("l", "127.0.0.1:8080", "address to listen on")

	flag.Parse()

	initTemplates()
	initHandlers()

	listenAndServe(*listen)
}
