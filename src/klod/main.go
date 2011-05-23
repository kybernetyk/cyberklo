package main

import (
	"irc"
	"fmt"
)

func main() {
	fmt.Println("Starting ...")
	c := irc.New()
	c.Tls = true
	c.Nick = "karkowka"
	c.Channels = append(c.Channels, "#polska")
	c.Connect("irc.hackint.eu")

	go udpServe()

	for {
		select {
		}
	}

}
