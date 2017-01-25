import matplotlib.pyplot as plt

## todo with the test_performance.c 
size = [32,64,128,256,512,1024,2048,4096,8192,16384]

lbb  = [26,21,24,60,20,21,64,68,21,58]
glibc= [129,101,151,147,137,142,1465,2676,3469,3789]

lbb_memset  =[26, 26, 33, 89, 139, 178, 314, 749, 1671, 3344]
glibc_memset=[123,140,155,182,232,306,1524,3742,7707,17273]

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

plt.show()