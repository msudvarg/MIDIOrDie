shader_type spatial;

uniform vec4 color : hint_color;

void fragment() {
	ALBEDO = color.rgb;
	ALPHA = 0.7;
}
