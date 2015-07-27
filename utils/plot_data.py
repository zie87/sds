
import matplotlib.pyplot as plt
import numpy as np
import glob

path_str  = "../build/"
sequences = ["array_seq", "piece_chain_seq", "gap_seq", "list_seq"]


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

def plot_insert(path,ending, save = False):
  fig = plt.figure()
  ax = fig.add_subplot(111)

  matrices = []
  for seq in sequences:
    glob_pattern  = path + "*" + seq  + ending + ".csv"
    files         = glob.glob(glob_pattern)
    matrix        = calc_matrix(files)
    ax.plot( matrix[:,0], matrix[:,1], label=seq)
    matrices.append(matrix)

  plt.title('Insert - Size of Sequence')
  plt.ylabel('ns/call')
  plt.xlabel('size')
  plt.legend( loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=len(sequences))

  if(save):
    file_name = "plot" + ending + "-insert.png"
    plt.savefig(file_name)
  else:
    plt.show()

  fig = plt.figure()
  ax = fig.add_subplot(111)

  for i in range(0, len(sequences)):
    ax.plot( matrices[i][:,0], matrices[i][:,2], label=sequences[i] )

  plt.title('Border Access - Size of Sequence')
  plt.ylabel('ns/call')
  plt.xlabel('size')
  plt.legend( loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=len(sequences))

  if(save):
    file_name = "plot" + ending + "-border_access.png"
    plt.savefig(file_name)
  else:
    plt.show()

def plot_access(path, ending, save = False):
  # complet access
  fig = plt.figure()
  ax = fig.add_subplot(111)

  for seq in sequences:
    glob_pattern  = path + "*" + seq  + ending + ".csv"
    files         = glob.glob(glob_pattern)
    matrix        = calc_matrix(files)
    ax.plot( matrix[:,0], matrix[:,1], label=seq)

  plt.title('Access - Size of Sequence')
  plt.ylabel('ns/call')
  plt.xlabel('size')
  plt.legend( loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=len(sequences))
  if(save):
    file_name = "plot" + ending + ".png"
    plt.savefig(file_name)
  else:
    plt.show()

plot_insert(path_str, "_insert")
plot_access(path_str, "_access")

plot_insert(path_str, "_seqtest_insert")
plot_access(path_str, "_seqtest_access")

# plot_insert(path_str, "_insert", True)
# plot_access(path_str, "_access", True)

# plot_insert(path_str, "_seqtest_insert", True)
# plot_access(path_str, "_seqtest_access", True)