package main

import (
	"encoding/json"
	"fmt"
	"html/template"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"strings"
	"time"
)

type sample struct {
	Time    time.Time
	Address string
}

type host struct {
	Name    string
	Samples []sample
}

var (
	hosts     []host
	hostIndex map[string]*host
)

func hostsLoad() {
	bytes, err := ioutil.ReadFile("hosts.json")
	if err != nil {
		log.Fatalln(err)
	}

	hs := make([]host, 0)

	json.Unmarshal(bytes, &hs)

	hosts = make([]host, 0)
	hostIndex = make(map[string]*host)

	for _, h := range hs {
		for i := range h.Samples {
			j := len(h.Samples) - i - 1
			s := h.Samples[j]
			hostsUpdate(h.Name, s)
		}
	}
}

func hostsSave() {
	bytes, err := json.Marshal(hosts)
	if err != nil {
		log.Fatalln(err)
	}

	if err := ioutil.WriteFile("hosts.json", bytes, 0600); err != nil {
		log.Fatalln(err)
	}
}

func hostsUpdate(name string, s sample) {
	h, ok := hostIndex[name]
	if !ok {
		hosts = append(hosts, host{Name: name})
		h = &hosts[len(hosts)-1]
		hostIndex[name] = h
	}

	h.Samples = append([]sample{s}, h.Samples...)
}

func log_access(r *http.Request) {
	log.Printf("%v %v %v\n", r.RemoteAddr, r.Method, r.URL.Path)
}

func rootHandler(w http.ResponseWriter, r *http.Request) {
	log_access(r)

	hostname := ""

	{
		fh := r.FormValue("hostname")

		uh := ""
		fmt.Sscanf(r.URL.Path, "/%s", &uh)

		if fh == "" {
			hostname = uh
		} else {
			hostname = fh
		}
	}

	if r.Method == "POST" {
		if hostname == "" {
			http.Error(w, "Missing parameter: hostname", http.StatusBadRequest)
			return
		}

		{
			l := strings.Split(r.RemoteAddr, ":")
			h := strings.Join(l[:len(l)-1], ":")
			s := sample{time.Now(), h}

			hostsUpdate(hostname, s)
			hostsSave()
		}

		http.Redirect(w, r, r.URL.Path, http.StatusSeeOther)
		return
	}

	if hostname == "" {
		t := template.Must(template.ParseFiles("html/base.html", "html/index.html"))
		t.Execute(w, hosts)
		return
	}

	host, ok := hostIndex[hostname]
	if !ok {
		http.NotFound(w, r)
		return

	}

	plain := r.FormValue("plain")

	if plain != "" {
		w.Header().Set("Content-Type", "text/plain")
		io.WriteString(w, host.Samples[0].Address)
		return
	}

	t := template.Must(template.ParseFiles("html/base.html", "html/host.html"))
	t.Execute(w, host)
}

func robotsTxtHandler(w http.ResponseWriter, r *http.Request) {
	log_access(r)

	fmt.Fprintf(w, "User-agent: *\nDisallow: /\n")
}

func main() {
	hostsLoad()

	http.HandleFunc("/", rootHandler)
	http.HandleFunc("/robots.txt", robotsTxtHandler)

	log.Println("Listening on 0.0.0.0:8080...")
	log.Fatal(http.ListenAndServe("0.0.0.0:8080", nil))
}
