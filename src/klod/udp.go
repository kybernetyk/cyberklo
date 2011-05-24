package main

import (
	"net"
	"log"
)

func propagate(s string) {
	to_world_chan <- s
}

func udpServe() {
	b := make([]byte, 1000)
	for {
		conn, err := net.ListenUDP("udp4", &net.UDPAddr{
			IP:   net.IPv4(0, 0, 0, 0),
			Port: 1234,
		})
		if err != nil {
			continue
		}

		i, addr, _ := conn.ReadFrom(b)
		s := string(b[0:i])
		go propagate(s)
		log.Printf("%#v: %s", addr, s)
		conn.WriteTo([]byte("K THX BRO!"), addr)
		conn.Close()
	}
}

/*func broadcast(data []byte) {
	BROADCAST_IPv4 := net.IPv4(255, 255, 255, 255)
	socket, err := net.DialUDP("udp4", nil, &net.UDPAddr{
		IP:   BROADCAST_IPv4,
		Port: 6502,
	})
	if err != nil {
		fmt.Println(err.String())
		return

	}
	socket.Write(data)
}*/


/*func main() {
	go serve()
	lol := make(chan string)
	for {
		select {
		case <-lol:
			break
		}
	}
}*/
