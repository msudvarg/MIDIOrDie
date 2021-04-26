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
	get_node("String" + str(idx[0] + 1)).freq = Util.freq(note)
	dot.get_node("DestroyTimer").connect("timeout", self, "unfret", [idx[0] + 1])

func unfret(string):
	get_node("String" + str(string)).freq = 0
