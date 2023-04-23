#pragma once

#include "system_state.hpp"
#include "map_modes.hpp"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "parsers_declarations.hpp"

namespace map {

struct image {
	uint8_t* data = nullptr;
	int32_t size_x = 0;
	int32_t size_y = 0;
	int32_t channels = 0;

	image(uint8_t* data, int32_t size_x, int32_t size_y, int32_t channels) {
		this->data = data;
		this->size_x = size_x;
		this->size_y = size_y;
		this->channels = channels;
	}

	~image() {
		if(data)
			free(data);
	}
};
struct map_vertex {
	map_vertex(float x, float y) : position(x, y) {};
	glm::vec2 position;
};
struct border_vertex {
	border_vertex() {};
	border_vertex(glm::vec2 position, glm::vec2 normal_direction, glm::vec2 direction, int32_t border_id)
		: position_(position), normal_direction_(normal_direction), direction_(direction), border_id_(border_id) {};
	glm::vec2 position_;
	glm::vec2 normal_direction_;
	glm::vec2 direction_;
	int32_t border_id_;
};
struct border {
	int start_index = -1;
	int count = -1;
	uint8_t type_flag;
};
enum class map_view {
	globe,
	flat
};
class display_data {
public:
	display_data() {};
	~display_data();

	// Called to load the terrain and province map data
	void load_map_data(parsers::scenario_building_context& context);
	// Called to load the map. Will load the texture and shaders from disk
	void load_map(sys::state& state);

	map_mode::mode active_map_mode = map_mode::mode::terrain;
	dcon::province_id selected_province = dcon::province_id{};

	void render(sys::state& state, uint32_t screen_x, uint32_t screen_y);
	void set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode map_mode);
	void set_terrain_map_mode();
	void update_borders(sys::state& state);
	void set_view_mode(map_view map_view_mode);

	// Set the position of camera. Position relative from 0-1
	void set_pos(glm::vec2 pos);

	// Input methods
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_key_up(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod, float amount);
	void on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod);
	void on_lbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);

	dcon::province_id get_selected_province();
	void set_selected_province(dcon::province_id prov_id);

	uint32_t size_x;
	uint32_t size_y;

	std::vector<border> borders;
	std::vector<border_vertex> border_vertices;
	std::vector<uint8_t> terrain_id_map;
	std::vector<uint8_t> median_terrain_type;

	// map pixel -> province id
	std::vector<uint16_t> province_id_map;
private:
	// Last update time, used for smooth map movement
	std::chrono::time_point<std::chrono::system_clock> last_update_time{};

	// Time in seconds, send to the map shader for animations
	float time_counter = 0;

	// interaction
	bool unhandled_province_selection = false;

	// Meshes
	GLuint water_vao = 0;
	GLuint water_vbo = 0;
	GLuint land_vao = 0;
	GLuint land_vbo = 0;
	GLuint border_vao = 0;
	GLuint border_vbo = 0;
	uint32_t water_vertex_count = 0;
	uint32_t land_vertex_count = 0;

	// Textures
	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint rivers_texture_handle = 0;
	GLuint terrainsheet_texture_handle = 0;
	GLuint water_normal = 0;
	GLuint colormap_water = 0;
	GLuint colormap_terrain = 0;
	GLuint colormap_political = 0;
	GLuint overlay = 0;
	GLuint province_color = 0;
	GLuint border_texture = 0;
	GLuint province_highlight = 0;
	GLuint stripes_texture = 0;

	// Shaders
	GLuint terrain_shader = 0;
	GLuint terrain_political_far_shader = 0;
	GLuint terrain_political_close_shader = 0;
	GLuint water_shader = 0;
	GLuint water_political_shader = 0;
	GLuint line_border_shader = 0;

	// Position and movement
	map_view map_view_mode = map_view::globe;
	glm::vec2 pos = glm::vec2(0.5f, 0.5f);
	glm::vec2 pos_velocity = glm::vec2(0.f);
	glm::vec2 last_camera_drag_pos;
	glm::mat4 globe_rotation = glm::mat4(1.0f);
	bool is_dragging = false;
	//glm::vec2 size; // Map size
	float offset_x = 0.f;
	float offset_y = 0.f;
	float zoom = 1.f;
	float zoom_change = 1.f;
	bool has_zoom_changed = false;
	bool left_arrow_key_down = false;
	bool right_arrow_key_down = false;
	bool up_arrow_key_down = false;
	bool down_arrow_key_down = false;
	glm::vec2 scroll_pos_velocity = glm::vec2(0.f);
	std::chrono::time_point<std::chrono::system_clock> last_zoom_time{};

	void update(sys::state& state);

	glm::vec2 screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size);

	void load_border_data(parsers::scenario_building_context& context);
	void create_border_ogl_objects();
	void load_province_data(parsers::scenario_building_context& context, image& image);
	void load_provinces_mid_point(parsers::scenario_building_context& context);
	void load_terrain_data(parsers::scenario_building_context& context);
	void load_median_terrain_type(parsers::scenario_building_context& context);

	void load_shaders(simple_fs::directory& root);
	void create_meshes();
	void gen_prov_color_texture(GLuint texture_handle, std::vector<uint32_t> const& prov_color, uint8_t layers = 1);
};
}
