#!/bin/bash

while true
do
	stillRunning=$(ps -ef | grep "a.out" | grep -v "grep")
	echo "$stillRunning"
	if [ "$stillRunning" ];then
	echo "watch process is running!"
	else
	echo "watch process has been restarted!"
	sudo ./server_a9
	fi

sleep 5
done