import os,sys,zlib

def findgit(cwd):
	path = cwd+"/.git"
	if os.path.isdir(path):
		return path
	# If .git is not in cwd, check if you've reacehd the end
	if cwd == '/':
		sys.stderr.write("Not inside a Git repository")
		exit(1)
	# Check permission of parent
	if os.access('..', os.R_OK):
		os.chdir('..')
		return findgit(os.getcwd())
	else:
		sys.stderr.write("permission denied\n")
		exit(1)

# RETURNS LIST OF HASHES
def getBranches(git_dir):
	path = git_dir+'/refs/heads'
	# ******check if directory exists******
	if os.path.isdir(path) and os.access(path, os.R_OK):
		os.chdir(path)
	else:
		sys.stderr.write("not a directory or permission denied\n")
		exit(1)

	leaf_paths = []
	branch_names = []
	i = len(path)+1
	for dirpath, d, files in os.walk(path):
		for name in files:
			branch = os.path.join(dirpath,name)
			branch_names.append(branch[i:])
			leaf_paths.append( os.path.abspath(branch) )

	return (leaf_paths, branch_names)


class CommitNode:
	def __init__(self, commit_hash):
		self.commit_hash = commit_hash
		self.parents = set()
		self.children = set()


	
# RETURNS THE HASH OF THE BRANCH AS A STRING
def getLeafHash(path):
	if os.path.isfile(path):
		file = open(path, 'rb')
		content = file.read()[:-1] #get rid of trailing newline
		file.close()
		return content.decode("utf8") #cast to string
	return None


# BUILD THE COMMIT NODE GRAPH FOR 1 BRANCH
def buildGraph(myhash, child, nodeList, nodeDict, objpath):
	if myhash is None:
		return
	# create a new node, check if node already exists
	if myhash not in nodeDict:
		nodeDict[myhash] = CommitNode(myhash)
	# work on the new/update existing node
	newNode = nodeDict[myhash]

	if child != None: 
		newNode.children.add(child)

	# Get my parents:
	os.chdir(objpath)
	# ****check if subdirectory exists******
	os.chdir("./"+myhash[:2]) #got inside hash's subdirectory
	
	# ***** check if leaf[2:] exists
	#read contents of file
	with open("./"+myhash[2:], 'rb') as file:
		content = file.read()
	decomp = zlib.decompress(content).decode('utf8')
	lines = decomp.split('\n') # get the file contents into array
	
	for line in lines:
		if line[:7] == "parent " :
			newNode.parents.add(line[7:])

	# If my parent is already in nodeList, just add me as a child. 
	# Else, create my parent (with me as a child) 
	
	# Don't Call Recursion, if Node has no Parents
	if len(newNode.parents) != 0:
		for p_hash in newNode.parents:
			myParentExists = False
			# If my parent already exists, just add me as their child
			if p_hash in nodeDict:
				nodeDict[p_hash].children.add(myhash)
			else:
				buildGraph(p_hash, myhash, nodeList, nodeDict, objpath)

	nodeList.append(newNode) # now add me to the list!

	return nodeList

def createCommitGraph(leaf_hashes,objpath):
	nodeList = []
	root_commits = []
	nodeDict = {}
	i = 0
	startOfBranch = 0
	for leaf in leaf_hashes:
		root_commits.append([])
		root_commits[i] = buildGraph(leaf,None,nodeList,nodeDict, objpath)
		root_commits[i] = root_commits[i][startOfBranch].commit_hash
		i += 1
		startOfBranch = len(root_commits)
	return (root_commits, nodeList, nodeDict)



def get_topo_ordered_vertices(vertex_id_to_nodes, root_vertices):
	order = []
	visited = set()
	grey_stack = []
	stack = list(root_vertices)

	while stack:
		v = stack.pop()
		if v in visited:
			continue
		visited.add(v)

		while grey_stack and v not in vertex_id_to_nodes[grey_stack[-1]].children:
			g = grey_stack.pop()
			order.append(g)
		grey_stack.append(v)

		for c in vertex_id_to_nodes[v].children:
			if c in visited:
				continue
			stack.append(c)

	while grey_stack:
		g = grey_stack.pop()
		order.append(g)

	return order

def printCommit(ordered, nodeDict, rootDict):
	newLine = False
	i = 0
	while True:
		h = ordered[i]
		
		# add sticky starts
		if newLine:
			prevNode = ""
			for child in nodeDict[h].children:
				prevNode += child + ' '
			print('='+ prevNode[:-1])# dont include last space
			newLine = False

		# add branch name if it is leaf_hash
		myRoots = ""
		if h in rootDict:
			for branch in rootDict[h]:
				myRoots += ' '+branch #appending branch name
		print(h+myRoots)

		i += 1 #iteration
		#reached the end of List
		if i == len(ordered):
			break;

		# add sticky ends

		if ordered[i] not in nodeDict[h].parents:
			nextNode = "" 
			for parent in nodeDict[h].parents:
				nextNode += parent+' '
			print(nextNode[:-1]+'=\n') #this is last element
			newLine = True	


def topo_order_commits():
	sys.setrecursionlimit(10000)
	path = findgit(os.getcwd()) #get path to .git directory
	(branches, branch_names) = getBranches(path) #go into refs/heads and get list of branch names
	
	leaf_hashes = []
	rootDict = {}
	for i in range(len(branches)):
		leaf_hash = getLeafHash(branches[i]) #get hash from each branch
		#######
		if leaf_hash is None:
			continue #pass in a none hash, ignore it
		######
		leaf_hashes.append(leaf_hash) #add hash to list
		
		if leaf_hash in rootDict:
			rootDict[leaf_hash].append(branch_names[i]) #dict[hash] = name of branch
		else:
			rootDict[leaf_hash] = [branch_names[i]]

	# get path to objects
	objpath = path+'/objects'

	# create graph: get list of nodes, and the root nodes
	(root_commits,nodeList,nodeDict) = createCommitGraph(leaf_hashes,objpath)

	# get list of all the nodes in order
	ordered = get_topo_ordered_vertices(nodeDict, root_commits)

	printCommit(ordered, nodeDict, rootDict)



if __name__ == '__main__':
    topo_order_commits()
