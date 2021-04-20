extends Node

var fretboard
var notespawner

func _ready():
	OS.open_midi_inputs()
	fretboard = $"../Viewport/Fretboard"
	notespawner = $"../NoteSpawner"

func _input(event):
	if event is InputEventMIDI:
		if event.message == MIDI_MESSAGE_NOTE_ON:
			fretboard.fret(event.pitch)
			notespawner.fret(event.pitch)
