#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 1) uniform sampler2D terrain_texture_sampler;
layout (binding = 3) uniform sampler2DArray terrainsheet_texture_sampler;
layout (binding = 6) uniform sampler2D colormap_terrain;
layout (binding = 8) uniform sampler2DArray province_color;
layout (binding = 10) uniform sampler2D province_highlight;
layout (binding = 11) uniform sampler2D stripes_texture;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;

float xx = 1 / map_size.x;
float yy = 1 / map_size.y;
vec2 pix = vec2(xx, yy);

// The terrain color from the current texture coordinate offset with one pixel in the "corner" direction
vec4 get_terrain(vec2 corner, vec2 offset) {
	float index = texture(terrain_texture_sampler, tex_coord + 0.5 * pix * corner).r;
	index = floor(index * 256);
	float is_water = step(64, index);
	vec4 colour = texture(terrainsheet_texture_sampler, vec3(offset, index));
	return mix(colour, vec4(0.), is_water);
}

vec4 get_terrain_mix() {
	// Pixel size on map texture
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;

	vec2 offset = tex_coord / (16. * pix);

	vec4 colourlu = get_terrain(vec2(-1, -1), offset);
	vec4 colourld = get_terrain(vec2(-1, +1), offset);
	vec4 colourru = get_terrain(vec2(+1, -1), offset);
	vec4 colourrd = get_terrain(vec2(+1, +1), offset);

	// Mix together the terrains based on close they are to the current texture coordinate
	vec4 colour_u = mix(colourlu, colourru, scaling.x);
	vec4 colour_d = mix(colourld, colourrd, scaling.x);
	return mix(colour_u, colour_d, scaling.y);
}

const vec3 GREYIFY = vec3( 0.212671, 0.715160, 0.072169 );

// The political map at close distance
// Will mix together the map mode color with the terrain
void main() {
	vec4 terrain_background = texture(colormap_terrain, tex_coord);
	vec4 terrain = get_terrain_mix();
	// Mixes the terrains from "texturesheet.tga" with the "colormap.dds" background color.
	//terrain = (terrain * 2. + terrain_background) / 3.;
	
	// Make the terrain a gray scale color
    float grey = dot( terrain.rgb, GREYIFY );
 	terrain.rgb = vec3(grey);

	vec2 prov_id = texture(provinces_texture_sampler, tex_coord).xy;

	// The primary and secondary map mode province colors
	vec4 prov_color = texture(province_color, vec3(prov_id, 0.));
	vec4 stripe_color = texture(province_color, vec3(prov_id, 1.));

	vec2 stripe_coord = tex_coord * vec2(512., 512. * map_size.y / map_size.x);

	// Mix together the primary and secondary colors with the stripes
	float stripeFactor = texture(stripes_texture, stripe_coord).a;
	vec3 political = clamp(mix(prov_color, stripe_color, stripeFactor) + texture(province_highlight, prov_id), 0.0, 1.0).rgb;
	political = political - 0.7;

	// Mix together the terrain and map mode color
	frag_color.rgb = mix(terrain.rgb, political, 0.3);
	frag_color.rgb *= 1.5;
	frag_color.a = 1;
}
