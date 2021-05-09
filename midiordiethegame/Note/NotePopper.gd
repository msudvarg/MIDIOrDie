extends Area

func note_entered(note):
	if note.has_method("pop"):
		note.pop()
