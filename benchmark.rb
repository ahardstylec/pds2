#!/usr/bin/env ruby
require 'fileutils'
# check if path to mpiexec is set, if not set  path and recompile binaries
if `echo $PATH | grep mpich` && ! $?.success?
  `export export PATH=/home/mpich/bin:$PATH && make clean && make`
end

FileUtils.mkdir_p("benchmarks")

puts "calc matrices parallel"
#For other matrix sizes
counter = 10
File.open("benchmarks/matrices_parallel.txt", 'w') do |f|
	10.times do |i|
		system("./matrix_generator #{counter} #{counter} > matrix#{counter}.txt")
		puts "Benchmark for #{counter}"
		(1..100).each do |k|
			start = Time.now
			success = system("mpiexec  -n #{k} ./matrix_parallel matrix#{counter}.txt matrix#{counter}.txt > /dev/null")
			unless success
				puts "Some Error occurd during mpiexec with #{k} processes on matrix: matrix#{counter}.txt"
				exit 1
			end
			ende = (Time.now - start)
			f.puts "#{k}\t #{ende}"
		end
		counter += 10
	end
end

puts "calc matrices sequential"
File.open("benchmarks/matrices_sequential.txt", 'w') do |f|
	(1..100).each do |i|
		start = Time.now
		system("./matrix_sequential matrix-A-10x10.txt matrix-B-10x10.txt > /dev/null")
		ende = (Time.now - start)
		f.puts "#{i}\t #{ende}"
	end
end