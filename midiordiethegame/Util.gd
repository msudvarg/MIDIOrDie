extends Node

class_name Util

static func index(note: int):
	note = note - 52
	var string = note / 5
	var fret = note % 5
	return [string, fret]
