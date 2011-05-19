package irc

//quick and dirty IRC package - nothing too fancy, just enough to spam

import (
	"os"
	"crypto/tls"
	"net"
	"strings"
	"bufio"
	"fmt"
)


type Conn struct {
	Connected bool
	Nick      string
	Channels  []string
	Server    string
	Tls       bool

	io   *bufio.ReadWriter
	sock net.Conn

	in  chan string
	out chan string
}

func New() *Conn {
	c := &Conn{}
	c.in = make(chan string, 32)
	c.out = make(chan string, 32)
	return c
}

func hasPort(s string) bool {
	return strings.LastIndex(s, ":") > strings.LastIndex(s, "]")
}

func (conn *Conn) Connect(server string) os.Error {
	tmpsrv := server
	if conn.Tls {
		if !hasPort(server) {
			server += ":6697"
		}

		s, err := tls.Dial("tcp", server, nil)
		if err == nil {
			conn.sock = s
		} else {
			return err
		}
	} else {
		if !hasPort(server) {
			server += ":6667"
		}

		s, err := net.Dial("tcp", server)
		if err == nil {
			conn.sock = s
		} else {
			return err
		}
	}

	rdr := bufio.NewReader(conn.sock)
	wrtr := bufio.NewWriter(conn.sock)

	conn.Server = server
	conn.io = bufio.NewReadWriter(rdr, wrtr)
	go conn.sender()
	go conn.receiver()
	go conn.dispatcher()
	conn.out <- "NICK " + conn.Nick
	conn.out <- "USER klobotirc klobot " + tmpsrv + " :klo bot"

	return nil
}

//send string to the IRC
func (conn *Conn) Send(line string) {
	go func() {
		conn.out <- line
	}()
}

//go routine
func (conn *Conn) sender() {
	for {
		select {
		case line := <-conn.out:
			fmt.Println("sending: " + line)
			_, err := conn.io.WriteString(line + "\r\n")
			if err != nil {
				panic("sender() died!")
			}
			conn.io.Flush()
		}
	}
}

//go routine
func (conn *Conn) receiver() {
	for {
		s, err := conn.io.ReadString('\n')
		if err != nil {
			panic("receiver error!")
		}
		s = strings.Trim(s, "\r\n")
		conn.in <- s
	}
}

//go routine
func (conn *Conn) dispatcher() {
	for {
		select {
		case line := <-conn.in:
			fmt.Println(line)
			conn.dispatch(line)
		}
	}
}

//dispatch server line to event handlers, etc, blah
func (conn *Conn) dispatch(line string) {
	args := strings.Split(line, " :", 2)
	if len(args) > 1 {
		args = append(strings.Fields(args[0]), args[1])
	} else {
		args = strings.Fields(args[0])
	}
	//fmt.Printf("args: %#v\n", args)

	if args[0] == "PING" {
		conn.Send("PONG :" + args[1])
	}

	//001 - connection succeeded, we're online, HACK THE PLANET
	if args[1] == "001" {
		for _, c := range conn.Channels {
			conn.Send("JOIN " + c)
		}
	}

	//if we receive !panic we should panic() :]]]]
	if len(args) == 4  && args[1] == "PRIVMSG" {
		if args[3] == "!panic" {
			panic("OMG REMOTE PANIC!")
		}
	}
}
