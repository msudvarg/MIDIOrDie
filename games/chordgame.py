#!/usr/bin/env python3
from controller import Tone
from localcontroller.localcontroller import LocalController
from tkinter import *

class ChordGame(Frame):
  delaytime = 10
  def __init__(self, master=None):
    super().__init__(master)
    self.master = master
    self.localcontroller = LocalController()
    self.samplefreq = StringVar()
    self.samplefreq.set("131")
    self.samplestrength = StringVar()
    self.samplestrength.set("Bad")
    self.pack()
    self.create_widgets()
  def create_widgets(self):
    self.sample_spinner = Spinbox(self, from_=0, to=1000, textvariable=self.samplefreq)
    self.sample_spinner.pack()
    self.strength_label = Label(self, textvariable=self.samplestrength)
    self.strength_label.pack()
  def read_shared_memory(self):
    tone = Tone(self.localcontroller.GetData())
    tone.SetThreshold(3)
    pitch = int(self.samplefreq.get())
    if tone.HasPitch(pitch):
      self.samplestrength.set("Good")
    self.after(self.delaytime, self.read_shared_memory)
    

root = Tk()
app = ChordGame(master=root)
app.after(app.delaytime, app.read_shared_memory)
app.mainloop()
