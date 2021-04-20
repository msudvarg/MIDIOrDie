extends Control

var dot_prefab = preload("res://Fretboard/dot.tscn")

onready var note_origin = $NoteOrigin

const xoffset = 100
const yoffset = 50

func fret(note):
	var idx = Util.index(note)
	var dot = dot_prefab.instance()
	note_origin.add_child(dot)
	dot.translate(Vector2(idx[1] * xoffset, idx[0] * yoffset))
