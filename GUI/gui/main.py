from tkgui import *
from tkinter import *

root, gui = gui_init()
root.after(1000, gui.tkinter_init)  # refresh function for tkinter : after ( ms period for refresh, function to call )
root.mainloop()  # idk, some function needed for tkinter


# DONE List:
# - 


# TODO List:
# - turn intersectionID into an array for fog of war
#   this way it isn't a shitty update to a variable that literally does nothing
#
# - collision detection
# 
# - 