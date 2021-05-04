extends Spatial

const SPEED = 0.1
const ANGLE = PI/16

var rot = 0.0
var dir = SPEED

func _process(delta):
	if rot > ANGLE:
		dir = -SPEED
	elif rot < -ANGLE:
		dir = SPEED
	rotate(Vector3.UP, dir * delta)
	rot += dir * delta
