#include "canvas.h"
#include "map.h"
#include "utils.h"

#define UPSCALE       0.1
#define MAX_BEAMS     100
#define MAX_MIRRORS   100
#define MIRROR_WIDTH  0.8
#define FOV_BASE      PI4
#define FOV_MAX       PI2
#define FOV_STEP      0.02

// ---

typedef struct {
  vec3 pos;
  f32 rot;
  u8 valid;
} Laser;

typedef struct {
  vec3 pos;
  f32 rot, size;
} Beam;

typedef struct {
  vec3 pos;
  f32 rot;
} Mirror;

// ---

void apply_pillar_lights();
void camera_compute_movement(Camera* cam, u8 shader);
void char_press(GLFWwindow*, u32);
void compute_laser();
void place_laser();
void place_mirror();
void draw_floor();
void draw_roof();
void draw_wall(u8 x, u8 y);
void draw_pillar(u8 x, u8 y, u8 active);
void move_gate(Gate* gate);
void draw_gate(u8 x, u8 y, f32 offset);
void draw_laser();
void draw_beam(u8 i);
void draw_mirror(Mirror m, u8 i, u8 held);

// ---

CanvasConfig config = { 
  .title = "LASERINATOR",
  .capture_mouse = 1, 
  .fullscreen = 1,
  .screen_size = 1,
  .clear_color = BLACK 
};

Camera cam = { 
  .pos = {1.5, 2, 1.5},
  .yaw = PI - PI4,
  .fov = PI4, 
  .near_plane = 0.01, 
  .far_plane = 100, 
  .sensitivity = 0.001,
  .camera_lock = PI2 * 0.9,
  .speed = 3
};

u32 lowres_fbo, click_fbo;
u8 click_view = 0;

u32 shader;
u32 hud_shader;

Model* mo_cube;
Model* mo_wall;
Model* mo_pillar_base_off;
Model* mo_pillar_base_on;
Model* mo_pillar_off;
Model* mo_pillar_on;
Model* mo_laser;
Model* mo_beam;
Model* mo_mirror;

// ---

Laser laser;
Beam beams[MAX_BEAMS];
Mirror mirrors[MAX_MIRRORS];

u8 beam_c = 0;
u8 mirror_c = 0;
u8 mirror_valid = 1;

u8 holding_laser = 0;
u8 holding_mirror = 0;

// ---

int main() {
  canvas_init(&cam, config);
  glfwSetCharCallback(cam.window, char_press);

  hud_shader = shader_create_program("hud");
  generate_ortho_mat(&cam, hud_shader);
  shader = shader_create_program("obj");
  generate_proj_mat(&cam, shader);
  generate_view_mat(&cam, shader);

  lowres_fbo = canvas_create_FBO(cam.width * UPSCALE, cam.height * UPSCALE, GL_NEAREST, GL_NEAREST);
  click_fbo  = canvas_create_FBO(cam.width, cam.height, GL_NEAREST, GL_NEAREST);

  Font font = { texture( "font"), 20, 5, 7.0 / 5 };

  mo_cube            = model_create("cube",   (Material) { WHITE,       0.5, 1.6, 1.0, .lig = 0 });
  mo_wall            = model_create("wall",   (Material) { WHITE,       0.5, 1.6, 1.0, .lig = 0 });
  mo_pillar_base_off = model_create("cube",   (Material) { DEEP_RED,    0.5, 1.6, 1.0, .lig = 0 });
  mo_pillar_base_on  = model_create("cube",   (Material) { DEEP_GREEN,  0.5, 2.6, 1.0, .lig = 0 });
  mo_pillar_off      = model_create("wall",   (Material) { DEEP_RED,    0.5, 1.6, 0.3, .lig = 0 });
  mo_pillar_on       = model_create("wall",   (Material) { DEEP_GREEN,  0.5, 2.6, 0.3, .lig = 1 });
  mo_laser           = model_create("laser",  (Material) { DEEP_PURPLE, 0.5, 1.6, 1.0, .lig = 0 });
  mo_beam            = model_create("tower",  (Material) { DEEP_RED,    0.5, 1.6, 0.8, .lig = 1 });
  mo_mirror          = model_create("mirror", (Material) { PASTEL_BLUE, 0.5, 1.6, 1.0, .lig = 0 });

  // ---

  read_map("map/map.txt");

  VEC3_COPY(VEC3(1.5, 1, 1.5), laser.pos);
  laser.valid = 1;

  compute_laser();

  apply_pillar_lights();

  while (!glfwWindowShouldClose(cam.window)) {
    if (holding_laser)  place_laser();
    if (holding_mirror) place_mirror();

    if (holding_laser || holding_mirror) cam.fov = MIN(FOV_MAX,  cam.fov + FOV_STEP);
    else                                 cam.fov = MAX(FOV_BASE, cam.fov - FOV_STEP);

    // ---

    generate_proj_mat(&cam, shader);
    canvas_set_pnt_lig(shader, (PntLig) { WHITE, { cam.pos[0], cam.pos[1], cam.pos[2] }, 1, 0.22, 0.2, 1000 }, 0);

    draw_floor();
    draw_roof();

    for (u8 x = 0; x < map.width; x++) {
      for (u8 y = 0; y < map.height; y++) {
        Cell* cell = map_at(x, y);
        if (cell->type == WALL) { draw_wall(x, y); }
        if (cell->type == GATE) { draw_gate(x, y, -cell->d.gate.offset); move_gate(&cell->d.gate); }
      }
    }

    for (u8 i = 0; i < mirror_c - holding_mirror; i++) draw_mirror(mirrors[i], i, 0);

    for (u8 i = 0; i < beam_c; i++) draw_beam(i);


    for (u8 x = 0; x < map.width; x++)
      for (u8 y = 0; y < map.height; y++)
        if (map_at(x, y)->type == PILLAR)
          draw_pillar(x, y, map_at(x, y)->d.pillar.active);

    if (holding_mirror) draw_mirror(mirrors[mirror_c - 1], mirror_c - 1, 1);
    if (holding_laser) glDisable(GL_DEPTH_TEST);
    draw_laser();
    glEnable(GL_DEPTH_TEST);

    // Lowres
    if (click_view) glBlitNamedFramebuffer(click_fbo, 0, 0, 0, cam.width, cam.height, 0, 0, cam.width, cam.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitNamedFramebuffer(0, lowres_fbo, 0, 0, cam.width, cam.height, 0, 0, cam.width * UPSCALE, cam.height * UPSCALE, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitNamedFramebuffer(lowres_fbo, 0, 0, 0, cam.width * UPSCALE, cam.height * UPSCALE, 0, 0, cam.width, cam.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // HUD
    glUseProgram(hud_shader);
    if (!holding_laser && !holding_mirror) hud_draw_rec(hud_shader, 0, (vec3) BLACK, cam.width / 2 - 10, cam.height / 2 - 10, 20, 20);
    c8 buffer[10];
    sprintf(buffer, "%d FPS", (i32) cam.fps);
    hud_draw_text(hud_shader, buffer, 10, cam.height - font.size * font.ratio - 10, font, (vec3) WHITE);
    hud_draw_text(hud_shader, buffer, 10, 10, font, (vec3) WHITE);

    // Finish
    glUseProgram(shader);
    glfwSwapBuffers(cam.window);

    camera_compute_movement(&cam, shader);
    camera_handle_inputs(&cam, shader);
    glfwPollEvents();
    update_fps(&cam);

    glBindFramebuffer(GL_FRAMEBUFFER, click_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  glfwTerminate();
  return 0;
}

// ---

void grab_mirror(u8 i) {
  for (u8 m = i; m < mirror_c - 1; m++)
    mirrors[m] = mirrors[m + 1];
}

void interact() {
  if (holding_laser) {
    if (!laser.valid) {
      play_audio("invalid");
      return;
    }
    play_audio("place");
    holding_laser = 0;
    mo_laser->material.alp = 1;
    return;
  }

  if (holding_mirror) {
    if (!mirror_valid) {
      play_audio("invalid");
      return;
    }
    play_audio("place");
    holding_mirror = 0;
    return;
  }

  f32* buffer = malloc(sizeof(f32) * 3);
  glBindFramebuffer(GL_FRAMEBUFFER, click_fbo);
  glReadPixels(cam.width / 2, cam.height / 2, 1, 1, GL_RGB, GL_FLOAT, buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  if ((i8) roundf(buffer[0] * 2) == 2) {
    if (xz_dist(cam.pos, laser.pos) > 3) return;
    play_audio("grab");
    holding_laser = 1;
    mo_laser->material.alp = 0.2;
  }
  if ((i8) roundf(buffer[0] * 2) == 1) {
    u8 m_i = (i8) roundf(buffer[1] * MAX_MIRRORS);
    if (xz_dist(cam.pos, mirrors[m_i].pos) > 3) return;
    play_audio("grab");
    grab_mirror(m_i);
    holding_mirror = !holding_mirror;
  }
}

void create_beam(vec3 origin, f32 rot, i8 ignored_mirror) {
  vec3 closest_mirror_intersect;
  f32 closest_mirror_dist = FLT_MAX;
  u8 closest_mirror_i;

  for (u8 m = 0; m < mirror_c; m++) {
    if (m == ignored_mirror) continue;

    f32 beam_slope = tan(rot);
    f32 mirror_slope = tan(mirrors[m].rot);

    f32 beam_offset = origin[2] - (origin[0] * beam_slope);
    f32 mirror_offset = mirrors[m].pos[2] - (mirrors[m].pos[0] * mirror_slope);

    vec3 xz_intersection;
    xz_intersection[0] = (mirror_offset - beam_offset) / (beam_slope - mirror_slope);
    xz_intersection[1] = 1;
    xz_intersection[2] = xz_intersection[0] * beam_slope + beam_offset;

    f32 angle_to_intersection = atan2(xz_intersection[2] - origin[2], xz_intersection[0] - origin[0]);
    angle_to_intersection = fmod(angle_to_intersection + TAU, TAU);


    if (fmod(fabs(angle_to_intersection - fmod(rot + TAU, TAU)) + TAU, TAU) > 0.1) continue;
    if (xz_dist(mirrors[m].pos, xz_intersection) > MIRROR_WIDTH / 2) continue;

    f32 dist = xz_dist(origin, xz_intersection);

    if (dist > closest_mirror_dist) continue;

    VEC3_COPY(xz_intersection, closest_mirror_intersect);
    closest_mirror_dist = dist;
    closest_mirror_i = m;
  }

  vec3 pos, front;
  VEC3_COPY(origin, pos);
  VEC3_COPY(VEC3(cos(rot), 0, sin(rot)), front);
  glm_vec3_scale(front, 0.1, front);

  while (xz_dist(pos, origin) < closest_mirror_dist) {
    Cell* cell = map_at((u8) pos[0], (u8) pos[2]);
    if (cell->type == WALL) break;
    if (cell->type == GATE && cell->d.gate.offset < 1.5) break;
    if (cell->type == PILLAR) {
      cell->d.pillar.active = 1;
      apply_pillar_lights();
      i8 gate_id = cell->id;
      if (gate_id != -1) {
        for (u8 y = 0; y < map.height; y++)
          for (u8 x = 0; x < map.width; x++)
            if (map_at(x, y)->type == GATE && map_at(x, y)->id == gate_id)
              map_at(x, y)->d.gate.active = 1;
      }
    }

    glm_vec3_add(pos, front, pos);
  }

  f32 closest_wall_dist = xz_dist(pos, origin);

  VEC3_COPY(origin, beams[beam_c].pos);
  beams[beam_c].rot = rot;
  beams[beam_c].size = MIN(closest_mirror_dist, closest_wall_dist);
  beam_c++;
  ASSERT(beam_c <= MAX_BEAMS, "MAX_BEAMS EXCEEDED");

  if (closest_mirror_dist > closest_wall_dist) return;

  create_beam(closest_mirror_intersect, 2 * mirrors[closest_mirror_i].rot - rot, closest_mirror_i);
}

void compute_laser() {
  for (u8 y = 0; y < map.height; y++)
    for (u8 x = 0; x < map.width; x++)
      if (map_at(x, y)->type == PILLAR) {
        map_at(x, y)->d.pillar.active = 0;
        apply_pillar_lights();
        i8 gate_id = map_at(x, y)->id;
        if (gate_id != 0) {
          for (u8 yy = 0; yy < map.height; yy++)
            for (u8 xx = 0; xx < map.width; xx++)
              if (map_at(xx, yy)->type == GATE && map_at(xx, yy)->id == gate_id) 
                map_at(xx, yy)->d.gate.active = 0;
        }
      }

  beam_c = 0;
  create_beam(laser.pos, laser.rot, -1);
}

void apply_pillar_lights() {
  u8 pi = 1;
  for (u8 x = 0; x < map.width; x++)
    for (u8 y = 0; y < map.height; y++)
      if (map_at(x, y)->type == PILLAR) {
        PntLig lig;
        if (map_at(x, y)->d.pillar.active) lig = (PntLig) { DEEP_GREEN, { x + 0.5, 1.5, y + 0.5 }, 1, 2.5, 5.0, 3 };
        else                               lig = (PntLig) { DEEP_RED,   { x + 0.5, 1.5, y + 0.5 }, 1, 2.5, 5.0, 3 };
        canvas_set_pnt_lig(shader, lig, pi++);
      }
}

void place_laser() {
  vec3 target_pos;

  vec3 front = { cam.dir[0], 0, cam.dir[2] };

  VEC3_COPY(VEC3(cam.pos[0], 1, cam.pos[2]), target_pos);
  glm_vec3_add(target_pos, front, target_pos);

  if (map_at((u8) target_pos[0], (u8) target_pos[2])->type != EMPTY) {
    laser.valid = 0;
    VEC3_COPY((vec3) DEEP_RED, mo_laser->material.col);
    mo_laser->material.alp = 1;
  }
  else {
    laser.valid = 1;
    VEC3_COPY((vec3) DEEP_PURPLE, mo_laser->material.col);
    mo_laser->material.alp = 0.2;
  }
  VEC3_COPY(target_pos, laser.pos);

  laser.rot = xz_angle(front);

  compute_laser();
}

void place_mirror() {
  vec3 target_pos;

  vec3 front = { cam.dir[0], 0, cam.dir[2] };

  VEC3_COPY(VEC3(cam.pos[0], 0, cam.pos[2]), target_pos);
  glm_vec3_add(target_pos, front, target_pos);

  if (map_at((u8) target_pos[0], (u8) target_pos[2])->type != EMPTY) {
    mirror_valid = 0;
  }
  else {
    mirror_valid = 1;
  }
  VEC3_COPY(target_pos, mirrors[mirror_c - 1].pos);

  mirrors[mirror_c - 1].rot = xz_angle(front) + PI2;

  compute_laser();
}

void move_gate(Gate* gate) {
  if (gate->active) gate->offset = MIN(3, gate->offset + 0.1);
  else              gate->offset = MAX(0, gate->offset - 0.1);
}

// ---

void char_press(GLFWwindow* window, u32 key) {
  if (key == 'e') interact();

  if (key == 'q') {
    mirror_c++;
    holding_mirror = 1;
  }

  if (key == ' ') {
    click_view = !click_view;
    vec3 front = { cam.dir[0], 0, cam.dir[2] };
    glm_vec3_normalize(front);

    printf("P.X %.2f P.Y %.2f\n", cam.pos[0], cam.pos[2]);
    printf("P.DIR = (%.2f, %.2f, %.2f)\n", cam.dir[0], cam.dir[1], cam.dir[2]);
    printf("P.ROT = %.2f\n", xz_angle(front));
  }
}

void camera_compute_movement(Camera* cam, u8 shader) {
  static u8 mouse_down = 0;
  if (glfwGetMouseButton(cam->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) mouse_down = 0;
  if (glfwGetMouseButton(cam->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mouse_down) {
    mouse_down = 1;
    interact();
  }

  vec3 prompted_move = {
    (glfwGetKey(cam->window, GLFW_KEY_D) == GLFW_PRESS ? cam->speed / cam->fps : 0) + (glfwGetKey(cam->window, GLFW_KEY_A) == GLFW_PRESS ? -cam->speed / cam->fps : 0), 0,
    (glfwGetKey(cam->window, GLFW_KEY_W) == GLFW_PRESS ? cam->speed / cam->fps : 0) + (glfwGetKey(cam->window, GLFW_KEY_S) == GLFW_PRESS ? -cam->speed / cam->fps : 0)
  };

  if (!prompted_move[0] && !prompted_move[2]) return;

  f32 old_x = cam->pos[0];
  f32 old_z = cam->pos[2];

  vec3 front = { cam->dir[0], 0, cam->dir[2] };
  glm_vec3_normalize(front);

  vec3 lateral;
  glm_vec3_scale(cam->rig, prompted_move[0], lateral);
  glm_vec3_add(cam->pos, lateral,  cam->pos);

  vec3 frontal;
  glm_vec3_scale(front, prompted_move[2], frontal);
  glm_vec3_add(cam->pos, frontal,  cam->pos);

  if ((map_at((u8) old_x + 1, (u8) old_z)->type != EMPTY && (map_at((u8) old_x + 1, (u8) old_z)->type != GATE || map_at((u8) old_x + 1, (u8) old_z)->d.gate.offset < 2.9)) && ((u8) (cam->pos[0] + 0.1)) == ((u8) old_x + 1)) cam->pos[0] = old_x;
  if ((map_at((u8) old_x - 1, (u8) old_z)->type != EMPTY && (map_at((u8) old_x - 1, (u8) old_z)->type != GATE || map_at((u8) old_x - 1, (u8) old_z)->d.gate.offset < 2.9)) && ((u8) (cam->pos[0] - 0.1)) == ((u8) old_x - 1)) cam->pos[0] = old_x;
  if ((map_at((u8) old_x, (u8) old_z + 1)->type != EMPTY && (map_at((u8) old_x, (u8) old_z + 1)->type != GATE || map_at((u8) old_x, (u8) old_z + 1)->d.gate.offset < 2.9)) && ((u8) (cam->pos[2] + 0.1)) == ((u8) old_z + 1)) cam->pos[2] = old_z;
  if ((map_at((u8) old_x, (u8) old_z - 1)->type != EMPTY && (map_at((u8) old_x, (u8) old_z - 1)->type != GATE || map_at((u8) old_x, (u8) old_z - 1)->d.gate.offset < 2.9)) && ((u8) (cam->pos[2] - 0.1)) == ((u8) old_z - 1)) cam->pos[2] = old_z;

  generate_view_mat(cam, shader);
}

// Draw functions

void draw_clickable(u32 shader, Model* model, vec3 info) {
  static Material absolute    = { .alp = 1, .lig = 1 };
  canvas_set_material(shader, absolute);
  canvas_uni3f(shader, "MAT.COL", info[0], info[1], info[2]);
  glBindFramebuffer(GL_FRAMEBUFFER, click_fbo);
  model_draw(model, shader);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_floor() {
  model_bind(mo_cube, shader);
  glm_translate(mo_cube->model, VEC3(0, -1, 0));
  glm_scale(mo_cube->model, VEC3((u8) map.width, 1, (u8) map.height));
  model_draw(mo_cube, shader);
}

void draw_roof() {
  model_bind(mo_cube, shader);
  glm_translate(mo_cube->model, VEC3(0, 3, 0));
  glm_scale(mo_cube->model, VEC3((u8) map.width, 1, (u8) map.height));
  model_draw(mo_cube, shader);
}

void draw_wall(u8 x, u8 y) {
  model_bind(mo_wall, shader);
  glm_translate(mo_wall->model, VEC3(x, 0, y));
  model_draw(mo_wall, shader);
  draw_clickable(shader, mo_wall, VEC3(0, 0, 0));
}

void draw_gate(u8 x, u8 y, f32 offset) {
  model_bind(mo_wall, shader);
  glm_translate(mo_wall->model, VEC3(x, offset, y));
  model_draw(mo_wall, shader);
  draw_clickable(shader, mo_wall, VEC3(0, 0, 0));
}

void draw_pillar(u8 x, u8 y, u8 active) {
  Model* model = active ? mo_pillar_base_on : mo_pillar_base_off;
  model_bind(model, shader);
  glm_translate(model->model, VEC3(x - 0.025, 0, y - 0.025));
  glm_scale(model->model, VEC3(1.05, 0.2, 1.05));
  model_draw(model, shader);
  model_bind(model, shader);
  glm_translate(model->model, VEC3(x - 0.025, 2.8, y - 0.025));
  glm_scale(model->model, VEC3(1.05, 0.2, 1.05));
  model_draw(model, shader);
  model = active ? mo_pillar_on : mo_pillar_off;
  model_bind(model, shader);
  glm_translate(model->model, VEC3(x, 0, y));
  model_draw(model, shader);
  draw_clickable(shader, model, VEC3(0, 0, 0));
}

void draw_laser() {
  model_bind(mo_laser, shader);
  glm_translate(mo_laser->model, VEC3(laser.pos[0], 0, laser.pos[2]));
  glm_rotate(mo_laser->model, laser.rot, VEC3(0, -1, 0));
  model_draw(mo_laser, shader);
  draw_clickable(shader, mo_laser, VEC3(1, 0, 0));
}

void draw_beam(u8 i) {
  model_bind(mo_beam, shader);
  glm_translate(mo_beam->model, beams[i].pos);
  glm_rotate(mo_beam->model, -PI2, VEC3(0, 0, 1));
  glm_rotate(mo_beam->model, beams[i].rot, VEC3(1, 0, 0));
  glm_scale(mo_beam->model, VEC3(0.1, beams[i].size, 0.1));
  model_draw(mo_beam, shader);
}

void draw_mirror(Mirror m, u8 i, u8 held) {
  model_bind(mo_mirror, shader);
  if (held && mirror_valid) canvas_uni1f(shader, "MAT.ALP", 0.2);
  if (held && !mirror_valid) canvas_uni3f(shader, "MAT.COL", (vec3)DEEP_RED[0], (vec3)DEEP_RED[1], (vec3)DEEP_RED[2]);
  if (held) glDisable(GL_DEPTH_TEST);
  glm_translate(mo_mirror->model, m.pos);
  glm_rotate(mo_mirror->model, m.rot, VEC3(0, -1, 0));
  model_draw(mo_mirror, shader);
  glEnable(GL_DEPTH_TEST);
  draw_clickable(shader, mo_mirror, VEC3((f32) 1 / 2, (f32) i / MAX_MIRRORS, 1));
}
