#!/usr/bin/env ruby
require 'fileutils'
# check if path to mpiexec is set, if not set  path and recompile binaries
if `echo $PATH | grep mpich` && ! $?.success?
  `export export PATH=/home/mpich/bin:$PATH && make clean && make`
end

FileUtils.mkdir_p("benchmarks")

puts "calc matrices sequential"
counter = 10
File.open("benchmarks/matrices_sequential.txt", 'w') do |f|
	(1..10).each do |i|
		system("./matrix_generator #{counter} #{counter} > matrix#{counter}.txt")
		start = Time.now
		system("./matrix_sequential matrix#{counter}.txt matrix#{counter}.txt > /dev/null")
		ende = (Time.now - start)
		f.puts "#{i}\t #{ende}"
		counter += 10
	end
end