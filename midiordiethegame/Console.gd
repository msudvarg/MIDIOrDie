extends TextEdit

var note_spawner
func _ready():
	note_spawner = $"../NoteSpawner"

func parse(line : String):
	if line == "":
		return
	var tokens = line.split(" ")
	match tokens[0]:
		"note":
			assert(note_spawner)
			cprint("emitting midi note "  + tokens[1] + "\n")
			note_spawner.spawn(int(tokens[1]))
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
			
