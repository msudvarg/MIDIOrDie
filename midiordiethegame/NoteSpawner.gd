extends Spatial

export(PoolColorArray) var string_colors

var midi_reader
var playing = false
var time = 0

var note_prefab = preload("res://Note/Note.tscn")
var popper_prefab = preload("res://Note/NotePopper.tscn")

const xoffset = 0.222
const zoffset = -0.12

func _ready():
	midi_reader = $"../MidiReader"
	
func _process(delta):
	if playing:
		var new_time = time + delta
		## Get notes starting between time and new time
		var notes = midi_reader.get_notes_in_range(get_ticks(time), get_ticks(new_time))
		for note in notes:
			spawn(note)
		time = new_time
		
func get_ticks(seconds):
	## 480 midi ticks / 2 seconds
	return seconds / 0.5 * 480

func spawn(note: int):
	if not is_playable_range(note):
		print("Note is out of range")
	else:
		var note_inst = note_prefab.instance()
		var idx = Util.index(note)
		add_child(note_inst)
		note_inst.translate(Vector3(idx[1] * xoffset, 1.0, idx[0] * zoffset))
		note_inst.set_color(string_colors[idx[0]])

func fret(note: int):
	if not is_playable_range(note):
		print("Fret is out of range")
	else:
		var popper_inst = popper_prefab.instance()
		var idx = Util.index(note)
		add_child(popper_inst)
		popper_inst.translate(Vector3(idx[1] * xoffset, 0.0, idx[0] * zoffset))

func is_playable_range(note: int):
	return note >= 52 and note <= 81
	
func play():
	time = 0
	playing = true

func stop():
	playing = false
