#include "canvas.h"
#include "map.h"

#define UPSCALE      0.1
#define MAX_BEAMS    100
#define MAX_MIRRORS  100
#define MIRROR_WIDTH 0.8

void camera_compute_movement(Camera* cam, u8 shader);
void char_press(GLFWwindow*, u32);

// ---

CanvasConfig config = { 
  .title = "LASERINATOR",
  .capture_mouse = 1, 
  .fullscreen = 0,
  .screen_size = 0.5,
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

// ---

typedef struct {
  vec3 pos;
  f32 rot;
} Laser;

typedef struct {
  vec3 pos;
  f32 rot, size;
} Beam;

typedef struct {
  vec3 pos;
  f32 rot;
} Mirror;

Laser laser = { 0 };
Beam beams[MAX_BEAMS];
Mirror mirrors[MAX_MIRRORS];

u8 mirror_c = 0;

// ---

int main() {
  canvas_init(&cam, config);
  glfwSetCharCallback(cam.window, char_press);

  u32 shader = shader_create_program("obj");
  generate_proj_mat(&cam, shader);
  generate_view_mat(&cam, shader);

  Model* mo_wall       = model_create("cube",  (Material) { WHITE,       0.5, 1.6, 1.0, .lig = 0 });
  Model* mo_pillar_off = model_create("cube",  (Material) { DEEP_RED,    0.5, 1.6, 0.3, .lig = 0 });
  Model* mo_pillar_on  = model_create("cube",  (Material) { DEEP_GREEN,  0.5, 1.6, 0.3, .lig = 0 });
  Model* mo_laser      = model_create("tower", (Material) { DEEP_PURPLE, 0.5, 1.6, 1.0, .lig = 0 });
  Model* mo_beam       = model_create("tower", (Material) { DEEP_RED,    0.5, 1.6, 0.2, .lig = 1 });
  Model* mo_mirror     = model_create("tower", (Material) { GRAY,        0.5, 1.6, 1.0, .lig = 0 });

  u32 lowres_fbo = canvas_create_FBO(cam.width * UPSCALE, cam.height * UPSCALE, GL_NEAREST, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  while (!glfwWindowShouldClose(cam.window)) {
    canvas_set_pnt_lig(shader, (PntLig) { WHITE, { cam.pos[0], cam.pos[1], cam.pos[2] }, 1, 0.22, 0.2 }, 0);

    model_bind(mo_wall, shader);
    glm_translate(mo_wall->model, VEC3(0, 0, 0));
    glm_scale(mo_wall->model, VEC3((u8) LEN(map), 0.1, (u8) LEN(map[0])));
    model_draw(mo_wall, shader);

    model_bind(mo_wall, shader);
    glm_translate(mo_wall->model, VEC3(0, 2.9, 0));
    glm_scale(mo_wall->model, VEC3((u8) LEN(map), 0.1, (u8) LEN(map[0])));
    model_draw(mo_wall, shader);

    for (u8 y = 0; y < LEN(map); y++) {
      for (u8 x = 0; x < LEN(map[0]); x++) {
        if (map[y][x] != 1) continue;

        model_bind(mo_wall, shader);
        glm_translate(mo_wall->model, VEC3(x, 0, y));
        glm_scale(mo_wall->model, VEC3(1, 3, 1));
        model_draw(mo_wall, shader);
      }
    }

    for (u8 i = 0; i < mirror_c; i++) {
      model_bind(mo_mirror, shader);
      glm_translate(mo_mirror->model, mirrors[i].pos);
      glm_rotate(mo_mirror->model, mirrors[i].rot, VEC3(0, -1, 0));
      glm_scale(mo_mirror->model, VEC3(MIRROR_WIDTH, 1.5, 0.1));
      model_draw(mo_mirror, shader);
    }

    if (laser.pos[0]) {
      model_bind(mo_laser, shader);
      glm_translate(mo_laser->model, VEC3(laser.pos[0], 0, laser.pos[2]));
      glm_rotate(mo_laser->model, laser.rot, VEC3(0, -1, 0));
      glm_scale(mo_laser->model, VEC3(0.5, 1.5, 0.5));
      model_draw(mo_laser, shader);

      model_bind(mo_beam, shader);
      glm_translate(mo_beam->model, beams[0].pos);
      glm_rotate(mo_beam->model, -PI2, VEC3(0, 0, 1));
      glm_rotate(mo_beam->model, beams[0].rot, VEC3(1, 0, 0));
      glm_scale(mo_beam->model, VEC3(0.1, beams[0].size, 0.1));
      model_draw(mo_beam, shader);
    }

    for (u8 y = 0; y < LEN(map); y++) {
      for (u8 x = 0; x < LEN(map[0]); x++) {
        Model* model;
        if      (map[y][x] == 2) model = mo_pillar_off;
        else if (map[y][x] == 3) model = mo_pillar_on;
        else continue;

        model_bind(model, shader);
        glm_translate(model->model, VEC3(x, 0, y));
        glm_scale(model->model, VEC3(1, 3, 1));
        model_draw(model, shader);
      }
    }

    // Lowres
    glBlitNamedFramebuffer(0, lowres_fbo, 0, 0, cam.width, cam.height, 0, 0, cam.width * UPSCALE, cam.height * UPSCALE, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitNamedFramebuffer(lowres_fbo, 0, 0, 0, cam.width * UPSCALE, cam.height * UPSCALE, 0, 0, cam.width, cam.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Finish
    glfwSwapBuffers(cam.window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera_handle_inputs(&cam, shader);
    camera_compute_movement(&cam, shader);
    glfwPollEvents();
    update_fps(&cam);
  }

  glfwTerminate();
  return 0;
}

// ---

f32 xz_angle(vec3 dir) {
  dir[1] = 0;
  glm_vec3_normalize(dir);
  return fmod(atan2(dir[2], dir[0]) + TAU, TAU);
}

void camera_compute_movement(Camera* cam, u8 shader) {
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

  if (map[(u8) old_z][(u8) old_x + 1] && ((u8) (cam->pos[0] + 0.1)) == ((u8) old_x + 1)) cam->pos[0] = old_x;
  if (map[(u8) old_z][(u8) old_x - 1] && ((u8) (cam->pos[0] - 0.1)) == ((u8) old_x - 1)) cam->pos[0] = old_x;
  if (map[(u8) old_z + 1][(u8) old_x] && ((u8) (cam->pos[2] + 0.1)) == ((u8) old_z + 1)) cam->pos[2] = old_z;
  if (map[(u8) old_z - 1][(u8) old_x] && ((u8) (cam->pos[2] - 0.1)) == ((u8) old_z - 1)) cam->pos[2] = old_z;

  generate_view_mat(cam, shader);
}

void compute_laser() {
    vec3 pos, front;
    VEC3_COPY(VEC3(cos(laser.rot), 0, sin(laser.rot)), front);
    VEC3_COPY(laser.pos, pos);
    glm_vec3_scale(front, 0.05, front);

    while (map[(u8) pos[2]][(u8) pos[0]] != 1)
      glm_vec3_add(pos, front, pos);

    VEC3_COPY(laser.pos, beams[0].pos);
    beams[0].rot = laser.rot;

    f32 closest_wall_dist = sqrt(pow(fabs(pos[0] - laser.pos[0]), 2) + pow(fabs(pos[2] - laser.pos[2]), 2));
    f32 closest_mirror_dist = FLT_MAX;

    for (u8 m = 0; m < mirror_c; m++) {
      f32 beam_slope = tan(beams[0].rot);
      f32 mirror_slope = tan(mirrors[m].rot);

      f32 beam_y_offset = beams[0].pos[2] - (beams[0].pos[0] * beam_slope);
      f32 mirror_y_offset = mirrors[m].pos[2] - (mirrors[m].pos[0] * mirror_slope);

      f32 x_intersect = (mirror_y_offset - beam_y_offset) / (beam_slope - mirror_slope);
      f32 y_intersect = x_intersect * beam_slope + beam_y_offset;

      if (sqrt(pow(fabs(mirrors[m].pos[0] - x_intersect), 2) + pow(fabs(mirrors[m].pos[2] - y_intersect), 2)) > MIRROR_WIDTH / 2) continue;

      closest_mirror_dist = MIN(closest_mirror_dist, sqrt(pow(fabs(laser.pos[0] - x_intersect), 2) + pow(fabs(laser.pos[2] - y_intersect), 2)));
    }

    beams[0].size = MIN(closest_mirror_dist, closest_wall_dist);
}

void char_press(GLFWwindow* window, u32 key) {
  if (key == 'e') {
    vec3 front = { cam.dir[0], 0, cam.dir[2] };
    glm_vec3_normalize(front);

    VEC3_COPY(VEC3(cam.pos[0], 1, cam.pos[2]), laser.pos);
    glm_vec3_add(laser.pos, front, laser.pos);

    laser.rot = xz_angle(front);

    compute_laser();
  }

  if (key == 'q') {
    vec3 front = { cam.dir[0], 0, cam.dir[2] };
    glm_vec3_normalize(front);

    mirrors[mirror_c].rot = xz_angle(front) + PI2;
    VEC3_COPY(VEC3(cam.pos[0], 0, cam.pos[2]), mirrors[mirror_c].pos);
    glm_vec3_add(mirrors[mirror_c].pos, front, mirrors[mirror_c].pos);

    mirror_c++;

    compute_laser();
  }

  if (key == ' ') {
    vec3 front = { cam.dir[0], 0, cam.dir[2] };
    glm_vec3_normalize(front);

    printf("P.X %.2f P.Y %.2f\n", cam.pos[0], cam.pos[2]);
    printf("P.DIR = (%.2f, %.2f, %.2f)\n", cam.dir[0], cam.dir[1], cam.dir[2]);
    printf("P.ROT = %.2f\n", xz_angle(front));
  }
}
