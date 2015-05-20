#!/usr/bin/env ruby
#########################################
# Author:Deepak Luitel			#
# TCP Server				#
# Ruby Shell  				#
# Note: Server Runtime 2 sec ~ 5 sec   	#
#	Client Runtime 2 sec ~ 5 sec	#
#########################################
require 'timeout'

pid = Process.fork do
	exec 'make'
end

serverFile = File.new(ARGV[0],"r")
clientFile = File.new(ARGV[1],"r")

while line = serverFile.gets
	line=~(/([0-9]+),([A-Za-z0-9]+)/)
	pid = Process.fork do
		exec "./server #{$1} > #{$2}.out"
	end
	begin
        	Timeout.timeout(2) do #wait for 2 seconds
            		Process.wait
        	end
	rescue Timeout::Error
        Process.kill 9, pid
        	# collect status so it doesn't stick around as zombie process
        Process.wait pid
    	end
end
while line = clientFile.gets
	line=~(/([a-z]+),([0-9]+),([A-Za-z0-9]+),([A-Za-z]+),([A-Za-z0-9]+)/)

	pid = Process.fork do
		exec "./client #{$1} #{$2} #{$3} #{$4} > #{$5}.out"
	end
	begin
        	Timeout.timeout(2) do #wait for 2 seconds
            		Process.wait
        	end
	rescue Timeout::Error
        Process.kill 9, pid
        	# collect status so it doesn't stick around as zombie process
        Process.wait pid
    	end
end
serverFile.close
clientFile.close
