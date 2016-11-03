#!/usr/bin/python
#---------------------------------------------------------------------------
#
# compute entropies at each position from hmm emission probabilities
#---------------------------------------------------------------------------

from __future__ import division
import sys, os, math

if len(sys.argv) < 4:
  print 'usage: entropy hmmFile hmmfile outfileName'
  sys.exit()


##### load hmm emission probabilities for two hmms
import hmmerio
hmmerio.importFile(sys.argv[1])
em1 = hmmerio.emissions
nule1 = hmmerio.nule
print len(em1)

reload(hmmerio)
hmmerio.importFile(sys.argv[2])
em2 = hmmerio.emissions
nule2 = hmmerio.nule
print len(em2)

labels = hmmerio.labels
#print labels


#### find shortest sequence - used when working with both sequences together
seqLength = 0 # length of the shortest sequence
if len(em1) < len(em2):
  seqLength = len(em1)
else:
  seqLength = len(em2)


##### convert scores to probabilities  #######
## family 1
for i in range(len(em1)):  #position
  sum = 0
  for j in range(len(em1[0])):  #amino acid type - convert back to probability
    em1[i][j] = ( math.pow(2, (nule1[j])/1000) ) * ( math.pow(2, (em1[i][j] / 1000)) )
    sum += em1[i][j]
  for j in range(len(em1[0])):  #amino acid type - renormalize to compensate for rounding error
    em1[i][j] = em1[i][j] / sum

## family 2
for i in range(len(em2)):  #position
  sum = 0
  for j in range(len(em2[0])):  #amino acid type - convert back to probability
    em2[i][j] = ( math.pow(2, (nule2[j])/1000) ) * ( math.pow(2, (em2[i][j] / 1000)) )
    sum += em2[i][j]
  for j in range(len(em2[0])):  #amino acid type - renormalize to compensate for rounding error
    em2[i][j] = em2[i][j] / sum

## joint probability - defined this way, consistent with the individual seq fam entropies, 
## the joint entropy equals the sum of the individual seq fam entropies and is useless for our purposes here.
## it is in the code for reference & demonstration should any one care to compare definitions in the future
#jem = [] # joint probability over all sequence positions - each cell holds a matrix of joint probs at a sequence position
#for i in range(seqLength): # each position- ignore the tail end of the longer
#  jp = [] # joint probability matrix at a given position
#  for j in range(len(em1[0])): #amino acid type 
#    pv = []  # one row of probability (vector)
#    for k in range(len(em2[0])): #amino acid type 
#      pv.append(em1[i][j] * em2[i][k]) # single matrix entry - one joint probability value at one position
#    jp.append(pv) # one row of probability vectors at the current position
#  jem.append(jp) # add the probability matrix for the current position


## joint probability: two possible values, is the aa type the same or is not the same
##  ** NOTE!! the joint probability is defined over a different data array than the individual seq fam entropies
jem = [] # joint probability over all sequence positions - each cell holds a 20 X 2 matrix of joint probs at a sequence position
for i in range(seqLength): # each position- ignore the tail end of the longer
  jp = [] # joint probability matrix at a given position
  for j in range(len(em1[0])): #amino acid type 
    pv = []  # one row of probability value vector: one prob. for same aa type & one for diff aa type
    pv.append(em1[i][j] * em2[i][j]) # probability that both aa will be the same in the two sequence families
    pv.append(1 - pv[0])  # probability that the aa type will be different in the two sequence families
    jp.append(pv) # one row of probability vectors at the current position
  jem.append(jp) # add the probability matrix for the current position



###### compute descriptors at each position  ######
##### inner product
ip = [] # inner product at each position
for i in range(seqLength): # each position- ignore the tail end of the longer
  ip.append(0) # initialize at each position
  for j in range(len(em1[i])): #amino acid type 
    ip[i] += em1[i][j]*em2[i][j]
  if ip[i] > 1:
    print "error in IP computation at: ",i," IP too large ",ip[i]

print 'compute entropies'

##### entropies
h1 = []  # entropy of each position in the first sequence family
h2 = []  # entropy of each position in the second sequence family
hj = []  # join entropy of each position 

for p in range(len(jem)):
  sumj = 0 # temp for joint entropy
  sum1 = 0 # temp for seq 1 entropy
  sum2 = 0 # temp for seq 2 entropy

  for i in range(len(jem[0][0])):
   sum1 += -1 * em1[p][i] * math.log(em1[p][i],2) # seq 1 entropy
   sum2 += -1 * em2[p][i] * math.log(em2[p][i],2) # seq 2 entropy
   for j in range(len(jem[0][i])):
     sumj += -1 * jem[p][i][j] * math.log(jem[p][i][j],2) # joint entropy
  hj.append(sumj)
  h1.append(sum1)
  h2.append(sum2)

#### load output matrix with descriptors (some are computed from base entropies while loading)
sim = []  # similarity metrics
for p in range(len(jem)):
  simr = []  # temp - row of sim metrics
  simr.append(p)  # position
  simr.append(h1[p])  # seq fam 1 entropy
  simr.append(h2[p])  # seq fam 2 entropy
  simr.append(h1[p] + h2[p])  # sum of seq fam entropies
  simr.append(hj[p])  # joint entropy
  simr.append(hj[p]/(h1[p] + h2[p]))  # relative joint entropy
  simr.append(h1[p] + h2[p] - hj[p])  # mutual information
  simr.append((h1[p] + h2[p] - hj[p])/(h1[p] + h2[p]))  # redundancy (relative mutual information)
  simr.append(ip[p])  # inner product
  sim.append(simr)  # all metrics for one position


######## write descriptors to file
outFile = open(sys.argv[3], 'w')
outFile.write('position\tseq 1 entropy\tseq 2 entropy\tentropies sum\tjoint entropy\trelative joint entropy\tmutual information\tredundancy\tinner product\n')  # header
for p in range(len(sim)):
  outFile.write(str(sim[p][0]+1))  # position
  outFile.write('\t')
  outFile.write(str(sim[p][1]))  # seq 1 entropy
  outFile.write('\t')
  outFile.write(str(sim[p][2]))  # seq 2 entropy
  outFile.write('\t')
  outFile.write(str(sim[p][3]))  # sum of seq fam entropies
  outFile.write('\t')
  outFile.write(str(sim[p][4]))  # joint entropy
  outFile.write('\t')
  outFile.write(str(sim[p][5]))  # relative joint entropy
  outFile.write('\t')
  outFile.write(str(sim[p][6]))  # mutual information
  outFile.write('\t')
  outFile.write(str(sim[p][7]))  # redundancy (relative mutual information)
  outFile.write('\t')
  outFile.write(str(sim[p][8]))  # inner product
  outFile.write('\n')
outFile.close()


