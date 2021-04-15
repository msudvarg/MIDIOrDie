extends MeshInstance

onready var shader = get_surface_material(0)

func set_color(color : Color):
	shader.set_shader_param("color", color)

func _physics_process(delta):
	translate(Vector3(0.0, -delta, 0.0))
	if transform.origin.z < -30.0:
		queue_free()
