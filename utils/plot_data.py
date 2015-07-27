import matplotlib.pyplot as plt
import numpy as np
import glob

insert_files = glob.glob("../build/*_insert.csv")

print insert_files

insert_data = []

for f in insert_files:
  insert_data.append( np.genfromtxt (f, delimiter=";"))

fig = plt.figure()
ax = fig.add_subplot(111)
i=0;
for data in insert_data:
  ax.plot( data[:,0], data[:,1], label=insert_files[i] )
  i += 1
plt.title('Insert - Size of Sequence')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Series')
# plt.savefig("insert.png")
plt.show()


#border acces
fig = plt.figure()
ax = fig.add_subplot(111)
i=0;
for data in insert_data:
  ax.plot( data[:,0], data[:,2], label=insert_files[i] )
  i += 1
plt.title('Border Access - Size of Sequence')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Series')
# plt.savefig("border_access.png")
plt.show()



# complet access
access_files = glob.glob("../build/*_access.csv")

print access_files

access_data = []

for f in access_files:
  access_data.append( np.genfromtxt (f, delimiter=";"))

fig = plt.figure()
ax = fig.add_subplot(111)
i=0;
for data in access_data:
  ax.plot( data[:,0], data[:,1], label=access_files[i] )
  i += 1
plt.title('Access - Size of Sequence')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Series')
# plt.savefig("access.png")
plt.show()