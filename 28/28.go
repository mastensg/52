package main

import (
	"bufio"
	"fmt"
	"log"
	"math/rand"
	"os"
	"os/exec"
	"strconv"
	"strings"
	"time"
)

func termSize() (cols, rows int, err error) {
	c := exec.Command("stty", "size")

	c.Stdin = os.Stdin

	out, err := c.Output()
	if err != nil {
		return
	}

	_, err = fmt.Sscanf(string(out), "%d %d", &rows, &cols)

	return
}

func printUsage(cols int, usage *int) error {
	line := make([]byte, 5*cols)

	for i := 0; i < cols; i++ {
		b := []byte("\033[0m ")
		for j := 0; j < len(b); j++ {
			line[5*i+j] = b[j]
		}
	}

	bg := []byte("0")[0]
	fg := []byte("7")[0]

	for {
		p := float64(*usage) / 100

		for i := 0; i < cols; i++ {
			line[5*i+2] = bg
		}

		for i := 0; i < cols; i++ {
			if rand.Float64() < p {
				line[5*i+2] = fg
			}
		}

		os.Stdout.Write(line)

		time.Sleep(100 * time.Millisecond)
	}
}

func updateUsage(usage *int) error {
	c := exec.Command("vmstat", "-n", "-w", "1")

	stdout, err := c.StdoutPipe()
	if err != nil {
		return err
	}

	if err := c.Start(); err != nil {
		return err
	}

	lines := bufio.NewScanner(stdout)
	for lines.Scan() {
		line := lines.Text()

		words := bufio.NewScanner(strings.NewReader(line))
		words.Split(bufio.ScanWords)

		w := 0
		for words.Scan() {
			w++
			if w == 15 {
				break
			}
		}
		if w != 15 {
			continue
		}

		word := words.Text()
		idle, err := strconv.ParseUint(word, 10, 8)
		if err != nil {
			continue
		}

		if 100 < idle {
			return fmt.Errorf("idle out of bounds: %v", idle)
		}

		*usage = 100 - int(idle)
	}

	return nil
}

func main() {
	cols, _, err := termSize()
	if err != nil {
		log.Fatalln(err)
	}

	usage := 0

	go updateUsage(&usage)

	log.Fatalln(printUsage(cols, &usage))
}
