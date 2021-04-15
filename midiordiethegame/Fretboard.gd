extends Control

var dot_prefab = preload("res://dot.tscn")

var fretted = {}

func fret(note):
	if not fretted.has(note) or not fretted[note]:
		var dot = dot_prefab.instance()
		add_child(dot)
		dot.translate(Vector2(note, note))
		fretted[note] = dot
		
func unfret(note):
	if fretted.has(note) and fretted[note]:
		var dot = fretted[note]
		dot.queue_free()
		fretted.erase(dot)
