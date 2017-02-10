import matplotlib.pyplot as plt

#lines = [line.rstrip('\n') for line in open('test_performance_results.txt')]
with open("test_performance_results.txt", "r") as f:
	lines = []
	for line in f:
		lines.append(line.rstrip('\n'))	
		
	lbb          = lines[0].split(', ')
	glibc        = lines[1].split(', ')
	lbb_memset   = lines[2].split(', ')
	glibc_memset = lines[3].split(', ')

	size = [32,64,128,256,512,1024,2048,4096,8192,16384]

	plt.subplot(2, 1, 1)
	plt.plot(size, lbb, marker='+', label='lbb_alloc_free')
	plt.plot(size, glibc,  marker='.', label='glibc_malloc_free')
	plt.ylabel('ns(avg) spent on alloc+free per block')
	plt.xlabel('block size')

	size1=[32,512,1024,2048,4096,8192,16384]
	plt.xticks(size1)
	plt.legend()

	plt.subplot(2, 1, 2)
	plt.plot(size, lbb_memset, marker='+', label='lbb_alloc_free')
	plt.plot(size, glibc_memset,  marker='.', label='glibc_malloc_free')
	plt.ylabel('ns(avg) spent on alloc+memset+free per block')
	plt.xlabel('block size')

	plt.xticks(size1)
	plt.legend()

	fig = plt.gcf()
	fig.set_size_inches(16, 9)
	fig.savefig('performance.png', dpi=200)