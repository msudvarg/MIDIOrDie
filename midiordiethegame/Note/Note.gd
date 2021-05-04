extends Spatial

onready var mesh = $Mesh
onready var particles = $Particles
onready var shader = mesh.get_surface_material(0)
onready var particle_shader = particles.process_material

func set_color(color : Color):
	shader.set_shader_param("color", color)
	particle_shader.color = color

func _physics_process(delta):
	translate(Vector3(0.0, -delta, 0.0))
	if transform.origin.z < -30.0:
		queue_free()

func pop():
	mesh.visible = false
	particles.emitting = true


func kill_timer_timeout():
	queue_free()
