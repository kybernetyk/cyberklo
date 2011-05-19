package main

import (
	"irc"
	"fmt"
)

func main() {
	fmt.Println("klobot 01.09.11 starting ...")
	c := irc.New()
	c.Tls = true
	c.Nick = "cyberklo"
	c.Channels = append(c.Channels, "#fm", "#kraftfick", "#botafrika")
	c.Connect("irc.hackint.eu")

	for {
		select {
		}
	}

}
