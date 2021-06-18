#!/bin/bash

nohup ./vpaxos_server --me=127.0.0.1:38000 --peers=127.0.0.1:38001,127.0.0.1:38002 --path=/tmp/vpaxos/127.0.0.1:38000 --ping 1>38000.msg 2>38000.err &
nohup ./vpaxos_server --me=127.0.0.1:38001 --peers=127.0.0.1:38000,127.0.0.1:38002 --path=/tmp/vpaxos/127.0.0.1:38001 --ping 1>38001.msg 2>38001.err &
nohup ./vpaxos_server --me=127.0.0.1:38002 --peers=127.0.0.1:38000,127.0.0.1:38001 --path=/tmp/vpaxos/127.0.0.1:38002 --ping 1>38002.msg 2>38002.err &


