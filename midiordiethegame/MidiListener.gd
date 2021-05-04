extends Node

var fretboard
var notespawner
var timinglog

func _ready():
	OS.open_midi_inputs()
	fretboard = $"../Viewport/Fretboard"
	notespawner = $"../NoteSpawner"
	timinglog = File.new()
	timinglog.open("user://godot_times.txt",File.WRITE)

func _input(event):
	if event is InputEventMIDI:
		if event.message == MIDI_MESSAGE_NOTE_ON:
			fretboard.call_deferred("fret", event.pitch)
			notespawner.call_deferred("fret", event.pitch)
		if event.message == MIDI_MESSAGE_PITCH_BEND:
			var id = event.pitch
			var us = OS.get_system_time_usecs()
			var line = str(id) + " " + str(us)
			timinglog.store_line(line)
