extends Spatial

export(PoolColorArray) var string_colors

var note_prefab = preload("res://Note.tscn")

const xoffset = 0.222
const zoffset = -0.12

func spawn(note: int):
	if not is_playable_range(note):
		print("Note is out of range")
	else:
		var note_inst = note_prefab.instance()
		var idx = index(note)
		add_child(note_inst)
		note_inst.translate(Vector3(idx[1] * xoffset, 1.0, idx[0] * zoffset))
		note_inst.set_color(string_colors[idx[0]])

func is_playable_range(note: int):
	return note >= 52 and note <= 81

func index(note: int):
	note = note - 52
	var string = note / 5
	var fret = note % 5
	return [string, fret]
