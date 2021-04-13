extends Control

func _ready():
	OS.open_midi_inputs()
	print("MIDI initialized")
	
func _process(_delta):
	if len(OS.get_connected_midi_inputs()) > 0:
		print(OS.get_connected_midi_inputs())

func _input(event):
	if event is InputEventMIDI:
		print(event.pitch)
