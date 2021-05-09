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
			fretboard.call_deferred("fret", event.pitch+1)
			notespawner.call_deferred("fret", event.pitch+1)
