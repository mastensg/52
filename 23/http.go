package main

import (
	"archive/zip"
	"bytes"
	"html/template"
	"io/ioutil"
	"log"
	"net/http"
	"runtime/debug"
)

/***************************************************************************/

var templates map[string]*template.Template

/***************************************************************************/

func handleNew(w http.ResponseWriter, r *http.Request) {
	defer func() {
		if p := recover(); p != nil {
			http.Error(w, "Internal Server Error", http.StatusInternalServerError)
			debug.PrintStack()
		}
	}()

	if r.Method != "POST" {
		http.Error(w, "Method not allowed", http.StatusMethodNotAllowed)
		return
	}

	ff, _, err := r.FormFile("file")
	if err != nil {
		log.Panicln(err)
	}

	fb, err := ioutil.ReadAll(ff)
	if err != nil {
		log.Panicln(err)
	}

	zr, err := zip.NewReader(bytes.NewReader(fb), int64(len(fb)))
	if err != nil {
		log.Println(err)
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	pb := PackZip(zr)

	w.Header().Set("Content-Type", "text/xml; charset=utf-8")
	w.Write(pb)
}

func handleRoot(w http.ResponseWriter, r *http.Request) {
	defer func() {
		if p := recover(); p != nil {
			http.Error(w, "Internal Server Error", http.StatusInternalServerError)
			debug.PrintStack()
		}
	}()

	var buf bytes.Buffer

	err := templates["index"].Execute(&buf, nil)
	if err != nil {
		log.Panicln(err)
	}

	w.Header().Set("Content-Type", "text/html; charset=utf-8")

	w.Write(buf.Bytes())
}

/***********************************/

func initTemplates() {
	it := func(filenames ...string) *template.Template {
		t := template.New("")

		for _, f := range filenames {
			template.Must(t.Parse(binData[f]))
		}
		return t
	}

	templates = make(map[string]*template.Template)

	templates["index"] = it("base.html", "index.html")
}

func initHandlers() {
	ih := func(pattern string, f func(http.ResponseWriter, *http.Request)) {
		h := http.HandlerFunc(f)

		http.Handle(pattern, http.StripPrefix(pattern, h))
	}

	ih("/", handleRoot)
	ih("/new", handleNew)
}

func listenAndServe(addr string) {
	log.Printf("Listening on http://%v/", addr)

	log.Fatal(http.ListenAndServe(addr, nil))
}
