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
	f.sync=true
	100.times do |i|
		system("./matrix_generator #{counter} #{counter} > matrix#{counter}.txt")
		puts "Benchmark for #{counter}"
		(1..50).each do |k|
			out = `mpiexec -f hostFile.txt -n #{k} -wdir ~/pds2/ ./matrix_parallel matrix#{counter}.txt matrix#{counter}.txt`
			unless $?
				puts "Some Error occurd during mpiexec with #{k} processes on matrix: matrix#{counter}.txt"
				exit 1
			end
			f.write out
		end
		counter += 10
	end
end