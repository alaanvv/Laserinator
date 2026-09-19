#include "canvas.h"
#include "map.h"

CanvasConfig config = { 
  .title = "LASERINATOR",
  .capture_mouse = 1, 
  .fullscreen = 1,
  .screen_size = 1,
  .clear_color = BLACK 
};

Camera cam = { 
  .fov = PI4, 
  .near_plane = 0.01, 
  .far_plane = 100, 
  .sensitivity = 0.001, 
  .camera_lock = PI2 * 0.9,
  .speed = 3
};

// ---

int main() {
  canvas_init(&cam, config);

  u32 shader = shader_create_program("obj");
  generate_proj_mat(&cam, shader);
  generate_view_mat(&cam, shader);
  u32 hud_shader = shader_create_program("hud");
  generate_ortho_mat(&cam, hud_shader);

  Model* wall_m = model_create("cube", (Material) { WHITE, 0.5, 1.6 });

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
    // Finish
    glfwSwapBuffers(cam.window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera_handle_inputs(&cam, shader);
    glfwPollEvents();
    update_fps(&cam);
  }

  glfwTerminate();
  return 0;
}
