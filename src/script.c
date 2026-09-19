#include "canvas.h"
#include "map.h"

#define UPSCALE 0.1

CanvasConfig config = { 
  .title = "LASERINATOR",
  .capture_mouse = 1, 
  .fullscreen = 1,
  .screen_size = 1,
  .clear_color = BLACK 
};

Camera cam = { 
  .pos = {1.5, 2, 2.5},
  .fov = PI4, 
  .near_plane = 0.01, 
  .far_plane = 100, 
  .sensitivity = 0.003,
  .camera_lock = PI2 * 0.9,
  .speed = 3
};

void camera_compute_movement(Camera* cam, u8 shader);

// ---

int main() {
  canvas_init(&cam, config);

  u32 shader = shader_create_program("obj");
  generate_proj_mat(&cam, shader);
  generate_view_mat(&cam, shader);
  u32 hud_shader = shader_create_program("hud");
  generate_ortho_mat(&cam, hud_shader);

  Model* wall_m = model_create("cube", (Material) { WHITE, 0.5, 1.6 });

  // FBO
  u32 lowres_fbo = canvas_create_FBO(cam.width * UPSCALE, cam.height * UPSCALE, GL_NEAREST, GL_NEAREST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  while (!glfwWindowShouldClose(cam.window)) {
    canvas_set_pnt_lig(shader, (PntLig) { WHITE, { cam.pos[0], cam.pos[1], cam.pos[2] }, 1, 0.22, 0.2 }, 0);
    glUseProgram(shader);
    for (u8 y = 0; y < LEN(map); y++)
      for (u8 x = 0; x < LEN(map[0]); x++) {
        if (!map[y][x]) continue;
        model_bind(wall_m, shader);
        glm_translate(wall_m->model, VEC3(x, 0, y));
        glm_scale(wall_m->model, VEC3(1, 3, 1));
        model_draw(wall_m, shader);
      }

    model_bind(wall_m, shader);
    glm_translate(wall_m->model, VEC3(0, 0, 0));
    glm_scale(wall_m->model, VEC3((u8) LEN(map), 0.1, (u8) LEN(map[0])));
    model_draw(wall_m, shader);

    model_bind(wall_m, shader);
    glm_translate(wall_m->model, VEC3(0, 3, 0));
    glm_scale(wall_m->model, VEC3((u8) LEN(map), 0.1, (u8) LEN(map[0])));
    model_draw(wall_m, shader);

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

void camera_compute_movement(Camera* cam, u8 shader) {
  vec3 prompted_move = {
    (glfwGetKey(cam->window, GLFW_KEY_D) == GLFW_PRESS ? cam->speed / cam->fps : 0) + (glfwGetKey(cam->window, GLFW_KEY_A) == GLFW_PRESS ? -cam->speed / cam->fps : 0),
    0,
    (glfwGetKey(cam->window, GLFW_KEY_W) == GLFW_PRESS ? cam->speed / cam->fps : 0) + (glfwGetKey(cam->window, GLFW_KEY_S) == GLFW_PRESS ? -cam->speed / cam->fps : 0)
  };

  if (prompted_move[0] || prompted_move[1] || prompted_move[2]) {
    vec3 lateral  = { 0, 0, 0 };
    glm_vec3_scale(cam->rig, prompted_move[0], lateral);
    vec3 frontal  = { 0, 0, 0 };
    vec3 front = {cam->dir[0], 0, cam->dir[2]};
    glm_vec3_normalize(front);
    glm_vec3_scale(front, prompted_move[2], frontal);

    glm_vec3_add(cam->pos, lateral,  cam->pos);
    glm_vec3_add(cam->pos, frontal,  cam->pos);

    glUseProgram(shader);
    generate_view_mat(cam, shader);
  };
}
