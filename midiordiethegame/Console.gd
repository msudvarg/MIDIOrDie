extends TextEdit

var note_spawner
var midi_reader
var fretboard

onready var start_timer = $StartTimer
onready var audio_player = $AudioStreamPlayer

func _ready():
	note_spawner = $"../NoteSpawner"
	midi_reader = $"../MidiReader"
	fretboard = $"../Viewport/Fretboard"

func parse(line : String):
	if line == "":
		return
	var tokens = line.split(" ")
	match tokens[0]:
		"note":
			assert(note_spawner)
			cprint("emitting midi note "  + tokens[1] + "\n")
			note_spawner.spawn(int(tokens[1]))
		"fret":
			assert(fretboard)
			cprint("fretting midi note " + tokens[1] + "\n")
			fretboard.fret(int(tokens[1]))
			note_spawner.fret(int(tokens[1]))
		"load":
			var name = tokens[1]
			midi_reader.open(name + ".mid")
			audio_player.stream = load(name + ".ogg")
		"play":
			note_spawner.play()
			start_timer.start()
		"stop":
			note_spawner.stop()
			audio_player.stop()
		_:
			cprint("Unknown command\n")
	
func cprint(line):
	insert_text_at_cursor(line)

func _input(event):
	if event is InputEventKey:
		if event.is_pressed() and event.scancode == KEY_ENTER:
			get_tree().set_input_as_handled()
			var line = get_line(get_line_count() - 1)
			cprint("\n")
			parse(line)
		if event.is_pressed() and event.scancode == KEY_QUOTELEFT: ## Grave
			get_tree().set_input_as_handled()
			visible = not visible
			if visible:
				grab_focus()

func start_timer_timeout():
	audio_player.play()
