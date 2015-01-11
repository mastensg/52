package main

import (
	"fmt"
	"github.com/fhs/gompd/mpd"
	"html/template"
	"log"
	"net/http"
	"os"
	"path"
	"sort"
	"strconv"
	"strings"
	"time"
)

type Album struct {
	Name      string
	Directory string
	Folder    string
}

type mbdConfiguration struct {
	Listen         string
	MpdServer      string
	MusicDirectory string
}

var (
	MPD           *mpd.Client
	Albums        map[string]Album
	AlbumList     []Album
	configuration mbdConfiguration
)

func keepAlive() {
	for {
		err := MPD.Ping()
		if err != nil {
			log.Fatalln(err)
		}
		time.Sleep(1 * time.Second)
	}
}

func log_access(r *http.Request) {
	log.Printf("%v %v %v\n", r.RemoteAddr, r.Method, r.URL.Path)
}

func playHandler(w http.ResponseWriter, r *http.Request) {
	log_access(r)

	albumName := strings.TrimPrefix(r.URL.Path, "/play/")

	album, ok := Albums[albumName]
	if !ok {
		http.NotFound(w, r)
		return
	}

	MPD.Clear()
	MPD.Add(album.Directory)
	MPD.Play(0)

	http.Redirect(w, r, "/", http.StatusSeeOther)
}

func skipHandler(w http.ResponseWriter, r *http.Request) {
	log_access(r)

	track := strings.TrimPrefix(r.URL.Path, "/skip/")

	track_number, err := strconv.Atoi(track)
	if err != nil {
		http.Error(w, "not a number", http.StatusBadRequest)
		log.Println(err)
	}

	track_number -= 1

	if err := MPD.Play(track_number); err != nil {
		http.NotFound(w, r)
		log.Println(err)
		return
	}

	http.Redirect(w, r, "/", http.StatusSeeOther)
}

func rootHandler(w http.ResponseWriter, r *http.Request) {
	log_access(r)

	if r.Method == "POST" {
		action := r.FormValue("action")

		switch action {
		case "":
			http.Error(w, "Missing parameter: action", http.StatusBadRequest)
			return
		case "play":
			MPD.Pause(false)
			http.Redirect(w, r, "/", http.StatusSeeOther)
			return
		case "pause":
			MPD.Pause(true)
			http.Redirect(w, r, "/", http.StatusSeeOther)
			return
		case "previous":
			MPD.Previous()
			http.Redirect(w, r, "/", http.StatusSeeOther)
			return
		case "next":
			MPD.Next()
			http.Redirect(w, r, "/", http.StatusSeeOther)
			return
		default:
			http.Error(w, fmt.Sprintf("Unknown action: %q", action), http.StatusBadRequest)
			return
		}
	}
	var p struct {
		CurrentSong mpd.Attrs
		Playlist    []mpd.Attrs
		Album       Album
		Folder      string
		Albums      []Album
	}

	{
		song, err := MPD.CurrentSong()
		if err != nil {
			log.Fatalln(err)
		}
		p.CurrentSong = song

		songAlbum, ok := song["Album"]
		if ok {
			album, ok := Albums[songAlbum]
			if ok {
				p.Album = album
			}
		}
	}

	{
		playlist, err := MPD.PlaylistInfo(-1, -1)
		if err != nil {
			log.Fatalln(err)
		}

		for _, v := range playlist {
			if v["Track"] == p.CurrentSong["Track"] {
				v["Class"] = "track current"
			} else {
				v["Class"] = "track"
			}
		}
		p.Playlist = playlist
	}

	p.Albums = AlbumList

	t := template.Must(template.ParseFiles("html/base.html", "html/index.html"))
	t.Execute(w, p)
}

func robotsTxtHandler(w http.ResponseWriter, r *http.Request) {
	log_access(r)

	fmt.Fprintf(w, "User-agent: *\nDisallow: /\n")
}

func mustGetenv(name string) string {
	value := os.Getenv(name)

	if "" == value {
		log.Fatalf("%s not set\n", name)
	}

	return value
}

func main() {
	configuration.Listen = mustGetenv("MBD_LISTEN")
	configuration.MpdServer = mustGetenv("MBD_MPD_SERVER")
	configuration.MusicDirectory = mustGetenv("MBD_MUSIC_DIRECTORY")

	{
		conn, err := mpd.Dial("tcp", configuration.MpdServer)
		if err != nil {
			log.Fatalln(err)
		}
		MPD = conn
		defer conn.Close()
	}

	Albums = make(map[string]Album)

	{
		all, err := MPD.ListAllInfo("")
		if err != nil {
			log.Fatalln(err)
		}
		for _, song := range all {
			album := song["Album"]
			dir := path.Dir(song["file"])

			if album == "" {
				continue
			}
			if dir == "" {
				continue
			}
			if _, ok := Albums[album]; ok {
				continue
			}

			folder := fmt.Sprintf("/images/%s", path.Dir(song["file"]))

			a := Album{album, dir, folder}

			Albums[album] = a
		}
	}

	AlbumList = make([]Album, len(Albums))
	{
		var keys []string
		for k := range Albums {
			keys = append(keys, k)
		}
		sort.Strings(keys)
		for i, k := range keys {
			AlbumList[i] = Albums[k]
		}
	}

	go keepAlive()

	image_server := http.FileServer(http.Dir(configuration.MusicDirectory))
	http.Handle("/images/", http.StripPrefix("/images", image_server))

	http.HandleFunc("/", rootHandler)
	http.HandleFunc("/play/", playHandler)
	http.HandleFunc("/skip/", skipHandler)
	http.HandleFunc("/robots.txt", robotsTxtHandler)

	log.Fatal(http.ListenAndServe(configuration.Listen, nil))
}
