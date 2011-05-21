package main

import (
	"irc"
	"fmt"
)

func main() {
	c := irc.New()
	c.Tls = true
	c.Nick = "cyberklo"
	c.Channels = append(c.Channels, "#fm") 
	c.Connect("irc.hackint.eu")

	for {
		select {
		}
	}

}
