#!/usr/bin/ruby

#For other matrix sizes
counter = 10
10.times do |i|
	counter += 10
	system("./matrix_generator #{counter} #{counter} > matrix#{counter}.txt")
	puts "Benchmark for #{counter}"
	100.times do |k|
		start = Time.now
		success = system("mpiexec  -n #{k} ./matrix_parallel matrix#{counter}.txt matrix#{counter}.txt > /dev/null")
		if success == false || success.nil?
			puts "Some Error occurd during mpiexec with #{k} processes on matrix: matrix#{counter}.txt"
			return 1
		end
		ende = (Time.now - start)
		puts "#{k}\t #{ende}"
	end
end

100.times do |i|
	start = Time.now
	system("mpiexec  -n #{i} ./matrix_parallel matrix-A-10x10.txt matrix-B-10x10.txt > /dev/null")
	ende = (Time.now - start)
	puts "#{i}\t #{ende}"
end
