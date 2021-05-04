extends Node

var fretboard
var notespawner
var timinglog

func _ready():
	OS.open_midi_inputs()
	fretboard = $"../Viewport/Fretboard"
	notespawner = $"../NoteSpawner"
	timinglog = File.new()
	timinglog.open("user://godot_times.txt")

func _input(event):
	if event is InputEventMIDI:
		if event.message == MIDI_MESSAGE_NOTE_ON:
			fretboard.call_deferred("fret", event.pitch)
			notespawner.call_deferred("fret", event.pitch)
		if event.message == MIDI_MESSAGE_PITCH_BEND:
			us = os.get_system_time_usecs()
			id = event.pitch
			timinglog.store_16(id)
			timinglog.store_string(" ")
			timinglog.store_line()


