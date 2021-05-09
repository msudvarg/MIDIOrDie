extends Control

onready var fretboard = $Fretboard
onready var pitch_spinner = $Pitch

func _on_Fret_pressed():
	fretboard.fret(pitch_spinner.value)


func _on_Unfret_pressed():
	fretboard.unfret(pitch_spinner.value)
