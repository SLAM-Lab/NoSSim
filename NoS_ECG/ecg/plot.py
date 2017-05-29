import matplotlib.pyplot as plt
import numpy as np

Y=[]

with open("./data_360/data_101.txt") as f:
    for line in f:
        Y.append(float(line.split("\n")[0]))
	if len(Y)>10000:
		break;
X=range(len(Y));
#t = np.arange(0.0, 2.0, 0.01)
#s = np.sin(2*np.pi*t)
plt.plot(X, Y)

plt.xlabel('Time')
plt.ylabel('Wave')
plt.title('101 Data Set')
plt.grid(True)
plt.savefig("test.png")
plt.show()
