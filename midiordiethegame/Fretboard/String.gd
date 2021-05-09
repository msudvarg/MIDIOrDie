tool
extends Control

export(float) var freq

var total = 0.0

func _process(delta):
	total += delta
	update()

func _draw():
	for x in range(rect_size.x):
		var y = sin(x*2*PI / rect_size.x + total * freq) * (1.0 - (1 / (freq + 1)))
		draw_rect(Rect2(x, y * 30, 1, 6), Color())
