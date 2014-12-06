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
	10.times do |i|
		proc_counter_add=2
		system("./matrix_generator #{counter} #{counter} > matrix#{counter}.txt")
		puts "Benchmark for #{counter}"
		(1..20).each do |k|
			out = `mpiexec  -n #{proc_counter_add} ./matrix_parallel matrix#{counter}.txt matrix#{counter}.txt`
			unless $?
				puts "Some Error occurd during mpiexec with #{k} processes on matrix: matrix#{counter}.txt"
				exit 1
			end
			f.write out
			proc_counter_add=5*k
		end
		counter += 100
	end
end