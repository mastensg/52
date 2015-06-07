package main

import (
	"archive/zip"
	"encoding/base64"
	"encoding/xml"
	"io/ioutil"
	"log"
	"path/filepath"
	"strings"
)

/***************************************************************************/

type Element struct {
	Name string `xml:"name,attr"`
	Src  string `xml:"src,attr"`
}

type Elements struct {
	Elements []Element `xml:"element"`
}

type Setting struct {
	Name  string `xml:"name,attr"`
	Value string `xml:"value,attr"`
}

type Settings struct {
	Settings []Setting `xml:"setting"`
}

type Component struct {
	Name     string   `xml:"name,attr"`
	Settings Settings `xml:"settings"`
	Elements Elements `xml:"elements"`
}

type Components struct {
	Components []Component `xml:"component"`
}

type Skin struct {
	XMLName    xml.Name   `xml:"skin"`
	Author     string     `xml:"author,attr"`
	Name       string     `xml:"name,attr"`
	Target     string     `xml:"target,attr"`
	Version    string     `xml:"version,attr"`
	Components Components `xml:"components"`
}

/***************************************************************************/

func encodeImage(img []byte) string {
	img64 := base64.StdEncoding.EncodeToString(img)
	return "data:image/png;base64," + img64
}

func newSkin(b []byte) (skin Skin, err error) {
	err = xml.Unmarshal(b, &skin)
	return
}

func encodeSkin(skin Skin) ([]byte, error) {
	return xml.MarshalIndent(skin, "", "  ")
}

func zipReadFile(zr *zip.Reader, path string) []byte {
	for _, f := range zr.File {
		if f.Name != path {
			continue
		}

		rc, err := f.Open()
		if err != nil {
			log.Panicln(err)
		}

		b, err := ioutil.ReadAll(rc)
		if err != nil {
			log.Panicln(err)
		}

		return b
	}
	return nil
}

func PackZip(zr *zip.Reader) []byte {
	skinDir := ""
	skinFile := ""

	for _, f := range zr.File {
		path := f.Name
		base := filepath.Base(path)
		dir := filepath.Dir(path)

		if strings.HasPrefix(base, ".") {
			continue
		}

		if !strings.HasSuffix(path, ".xml") {
			continue
		}

		skinDir = dir
		skinFile = base
		break
	}

	skinDoc := zipReadFile(zr, filepath.Join(skinDir, skinFile))

	skin, err := newSkin(skinDoc)
	if err != nil {
		log.Panicln(err)
	}

	for ic, c := range skin.Components.Components {
		for ie, e := range c.Elements.Elements {
			imagePath := filepath.Join(skinDir, c.Name, e.Src)
			image := zipReadFile(zr, imagePath)
			imageUrl := encodeImage(image)

			skin.Components.Components[ic].Elements.Elements[ie].Src = imageUrl
		}
	}

	pack, err := encodeSkin(skin)
	if err != nil {
		log.Panicln(err)
	}

	return pack
}
