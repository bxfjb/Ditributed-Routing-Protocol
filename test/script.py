import numpy as np
import matplotlib.pyplot as plt

# Make some fake data.
a = b = np.arange(0, 3, .02)
c = np.exp(a)
d = c[::-1]
_,ax1=plt.subplots()
ax2 = ax1.twinx()  
# Create plots with pre-defined labels.


ax1.plot(a, c, 'k--',label='Model length')
ax1.plot(a, d, 'r', label='Data length')
# 这里先把地一个曲线标志显示了
ax1.legend(loc='upper left', shadow=True, fontsize='large')

ax2.plot(a, c + d, 'g', label='Total message length')
#接着再显示第二个
ax2.legend(loc='upper right', shadow=True, fontsize='x-large')

plt.show()