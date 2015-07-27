
import matplotlib.pyplot as plt
import numpy as np
import glob


def calc_matrix(files):
  data = []
  for f in files:
    data.append( np.genfromtxt (f, delimiter=";"))

  data_count = len(data)
  matrix = data[0];

  for i in range(1, data_count ):
    matrix += data[i]
  matrix /= data_count
  return matrix;



fig = plt.figure()
ax = fig.add_subplot(111)

arr_seq_files  = glob.glob("../build/*array_seq_insert.csv")
arr_seq_matrix = calc_matrix(arr_seq_files)
ax.plot( arr_seq_matrix[:,0], arr_seq_matrix[:,1], label="array_seq" )

gap_seq_files  = glob.glob("../build/*gap_seq_insert.csv")
gap_seq_matrix = calc_matrix(gap_seq_files)
ax.plot( gap_seq_matrix[:,0], gap_seq_matrix[:,1], label="gap_seq" )

list_seq_files  = glob.glob("../build/*list_seq_insert.csv")
list_seq_matrix = calc_matrix(list_seq_files)
ax.plot( list_seq_matrix[:,0], list_seq_matrix[:,1], label="list_seq" )

pc_seq_files  = glob.glob("../build/*piece_chain_seq_insert.csv")
pc_seq_matrix = calc_matrix(pc_seq_files)
ax.plot( pc_seq_matrix[:,0], pc_seq_matrix[:,1], label="piece_chain_seq" )

plt.title('Insert - Size of Sequence')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Structures')
# plt.savefig("insert.png")
plt.show()

#border access
fig = plt.figure()
ax = fig.add_subplot(111)

ax.plot( arr_seq_matrix[:,0], arr_seq_matrix[:,2], label="array_seq" )
ax.plot( gap_seq_matrix[:,0], gap_seq_matrix[:,2], label="gap_seq" )
ax.plot( list_seq_matrix[:,0], list_seq_matrix[:,2], label="list_seq" )
ax.plot( pc_seq_matrix[:,0], pc_seq_matrix[:,2], label="piece_chain_seq" )

plt.title('Border Access - Size of Sequence')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Structures')
# plt.savefig("border_access.png")
plt.show()

# complet access
fig = plt.figure()
ax = fig.add_subplot(111)

arr_seq_files  = glob.glob("../build/*array_seq_access.csv")
arr_seq_matrix = calc_matrix(arr_seq_files)
ax.plot( arr_seq_matrix[:,0], arr_seq_matrix[:,1], label="array_seq" )

gap_seq_files  = glob.glob("../build/*gap_seq_access.csv")
gap_seq_matrix = calc_matrix(gap_seq_files)
ax.plot( gap_seq_matrix[:,0], gap_seq_matrix[:,1], label="gap_seq" )

list_seq_files  = glob.glob("../build/*list_seq_access.csv")
list_seq_matrix = calc_matrix(list_seq_files)
ax.plot( list_seq_matrix[:,0], list_seq_matrix[:,1], label="list_seq" )

pc_seq_files  = glob.glob("../build/*piece_chain_seq_access.csv")
pc_seq_matrix = calc_matrix(pc_seq_files)
ax.plot( pc_seq_matrix[:,0], pc_seq_matrix[:,1], label="piece_chain_seq" )

plt.title('Access - Size of Sequence')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Structures')
# plt.savefig("access.png")
plt.show()

################################################################################
################################################################################
################################################################################

fig = plt.figure()
ax = fig.add_subplot(111)

arr_seq_files  = glob.glob("../build/*array_seq_seqtest_insert.csv")
arr_seq_matrix = calc_matrix(arr_seq_files)
ax.plot( arr_seq_matrix[:,0], arr_seq_matrix[:,1], label="array_seq" )

gap_seq_files  = glob.glob("../build/*gap_seq_seqtest_insert.csv")
gap_seq_matrix = calc_matrix(gap_seq_files)
ax.plot( gap_seq_matrix[:,0], gap_seq_matrix[:,1], label="gap_seq" )

list_seq_files  = glob.glob("../build/*list_seq_seqtest_insert.csv")
list_seq_matrix = calc_matrix(list_seq_files)
ax.plot( list_seq_matrix[:,0], list_seq_matrix[:,1], label="list_seq" )

pc_seq_files  = glob.glob("../build/*piece_chain_seq_seqtest_insert.csv")
pc_seq_matrix = calc_matrix(pc_seq_files)
ax.plot( pc_seq_matrix[:,0], pc_seq_matrix[:,1], label="piece_chain_seq" )

plt.title('Insert - Size of Sequence (seq) ')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Structures')
# plt.savefig("insert.png")
plt.show()

#border access
fig = plt.figure()
ax = fig.add_subplot(111)

ax.plot( arr_seq_matrix[:,0], arr_seq_matrix[:,2], label="array_seq" )
ax.plot( gap_seq_matrix[:,0], gap_seq_matrix[:,2], label="gap_seq" )
ax.plot( list_seq_matrix[:,0], list_seq_matrix[:,2], label="list_seq" )
ax.plot( pc_seq_matrix[:,0], pc_seq_matrix[:,2], label="piece_chain_seq" )

plt.title('Border Access - Size of Sequence (seq) ')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Structures')
# plt.savefig("border_access.png")
plt.show()

# complet access
fig = plt.figure()
ax = fig.add_subplot(111)

arr_seq_files  = glob.glob("../build/*array_seq_seqtest_access.csv")
arr_seq_matrix = calc_matrix(arr_seq_files)
ax.plot( arr_seq_matrix[:,0], arr_seq_matrix[:,1], label="array_seq" )

gap_seq_files  = glob.glob("../build/*gap_seq_seqtest_access.csv")
gap_seq_matrix = calc_matrix(gap_seq_files)
ax.plot( gap_seq_matrix[:,0], gap_seq_matrix[:,1], label="gap_seq" )

list_seq_files  = glob.glob("../build/*list_seq_seqtest_access.csv")
list_seq_matrix = calc_matrix(list_seq_files)
ax.plot( list_seq_matrix[:,0], list_seq_matrix[:,1], label="list_seq" )

pc_seq_files  = glob.glob("../build/*piece_chain_seq_seqtest_access.csv")
pc_seq_matrix = calc_matrix(pc_seq_files)
ax.plot( pc_seq_matrix[:,0], pc_seq_matrix[:,1], label="piece_chain_seq" )

plt.title('Access - Size of Sequence (seq) ')
plt.ylabel('ns/call')
plt.xlabel('size')
plt.legend(loc='best', title='Structures')
# plt.savefig("access.png")
plt.show()