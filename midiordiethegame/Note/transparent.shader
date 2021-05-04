shader_type spatial;

render_mode depth_draw_always;

uniform vec4 color : hint_color;

void fragment() {
	ALBEDO = color.rgb;
	ALPHA = 0.7;
}
