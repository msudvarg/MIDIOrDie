import controller
from tkinter import *

class ChordGame(Frame):
  delaytime = 100
  def __init__(self, master=None):
    super().__init__(master)
    self.master = master
    self.samplefreq = StringVar()
    self.samplestrength = StringVar()
    self.pack()
    self.create_widgets()
  def create_widgets(self):
    self.sample_spinner = Spinbox(self, from_=0, to=1000, textvariable=self.samplefreq)
    self.sample_spinner.pack()
    self.strength_label = Label(self, textvariable=self.samplestrength)
    self.strength_label.pack()
  def read_shared_memory(self):
    tone = controller.ToneFromSharedMemory();
    self.samplestrength.set(str(tone.GetPitchStrength(int(self.samplefreq.get()))))
    self.after(self.delaytime, self.read_shared_memory)
    

root = Tk()
app = ChordGame(master=root)
app.after(app.delaytime, app.read_shared_memory)
app.mainloop()
