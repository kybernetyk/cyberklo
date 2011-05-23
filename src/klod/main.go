package main

import (
	"irc"
	"fmt"
	"bencode"
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
		case str := <-to_world_chan:
			dec := bencode.NewDecoder([]byte(str))
			o, err := dec.Decode()
			if err != nil {
				fmt.Println(err.String())
			}

			dict := o.(map[string]interface{})
			name := dict["name"].(string)
			req := dict["req"].(string)

			if req == "data" {
				data := dict["data"].(map[string]interface{})

				str = fmt.Sprintf("[Sensor: %s State: %d]",
					name,
					data["state"].(int64))
			} else {
				str = fmt.Sprintf("[Sensor: %s Req: %s]",
					name,
					req)
			}
			//propagate to IRC
			line := "PRIVMSG #polska :" + str
			c.Send(line)

			//propagate to twitter ...
		}
	}

}
