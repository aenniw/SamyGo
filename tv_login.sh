#!/usr/bin/expect

set timeout 5
set name [lindex $argv 0]
set cmd [lindex $argv 1]
spawn telnet $name
expect "login:"
send "root\n"
send "$cmd\n"
interact