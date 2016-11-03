#!/usr/bin/python
#---------------------------------------------
#
# import emission probabilities from hmmer file
#----------------------------------------------

from __future__ import division
import sys, string, os

emissions = []  # holds list of em prob for each position
labels = []  # holds amino acid abbreviations, same order as emission probabilities
nule = []  # null emission probabilities

def importFile(file):
  print 'import: ' + file
  count = 0  # position counter

  lines = open(file, "r").readlines()
  for line in lines:
    snips = string.split(line)

    ## read emission probs
    if count > 0:
      try:
        if string.atoi(snips[0]) == count:
          em = []  # emission probabilities
          i = 1  # index, 1 is first amino acid
          while i <= 20:  # go through all 20 amino acids
            em.append(string.atoi(snips[i]))
            i = i+1
          count = count + 1
          emissions.append(em)

      except ValueError:
        nothing = 0;  # try used for logic & no exception handling needed

    ## read residue label sequence
    if snips[0] == "HMM":
      count = 1  # set flag to start collecting emission probabilites
      snips = string.split(line)
      i = 1  # index, 1 is first amino acid
      while i <= 20:  # go through all 20 amino acids
        labels.append(snips[i])
        i = i+1

    ## read null emission probs
    if snips[0] == "NULE":
      snips = string.split(line)
      i = 1  # index, 1 is first amino acid
      while i <= 20:  # go through all 20 amino acids
        nule.append(string.atoi(snips[i]))
        i = i+1



