#!/usr/bin/env python
###########################################################################
#
# xasy implements a graphical interface for Asymptote.
#
#
# Author: Orest Shardt
# Created: June 29, 2007
#
############################################################################

import getopt,sys,signal
from Tkinter import *
import xasyMainWin

signal.signal(signal.SIGINT,signal.SIG_IGN)

root = Tk()
mag = 1.0
try:
  opts,args = getopt.getopt(sys.argv[1:],"x:")
  if(len(opts)>=1):
    mag = float(opts[0][1])
except:
  print "Invalid arguments."
  print "Usage: xasy.py [-x magnification] [filename]"
  sys.exit(1)
if(mag <= 0.0):
  print "Magnification must be positive."
  sys.exit(1)
if(len(args)>=1):
  app = xasyMainWin.xasyMainWin(root,args[0],mag)
else:
  app = xasyMainWin.xasyMainWin(root,magnification=mag)
root.mainloop()
