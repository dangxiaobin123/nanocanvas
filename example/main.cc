#include <GLFW/glfw3.h>
#include "nanovg.h"
#define NANOVG_GL2_IMPLEMENTATION
#include "nanovg_gl.h"

#include "perf.h"

#include "duktape.h"

#include <cassert>
#include <string>
#include <fstream>
#include <vector>

NVGcontext* vg;

unsigned char fclamp(float x)
{
  int i = (int)x;
  
  if (i < 0) i = 0;
  if (i > 255) i = 255;

  return (unsigned char)(i);
}

void fatal_function(duk_context *ctx, duk_errcode_t code, const char *msg)
{
  printf("FATAL: %d\n", code);
  printf("FATAL: %s\n", msg);
  exit(-1);
}

duk_ret_t beginPath(duk_context *ctx)
{
  nvgBeginPath(vg);
  return 0;
}

duk_ret_t closePath(duk_context *ctx)
{
  nvgClosePath(vg);
  return 0;
}

duk_ret_t moveTo(duk_context *ctx)
{
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  
  nvgMoveTo(vg, x, y);
  return 0;
}

duk_ret_t lineTo(duk_context *ctx)
{
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  
  nvgLineTo(vg, x, y);
  return 0;
}

duk_ret_t lineCap(duk_context *ctx)
{
  duk_int_t cap = duk_require_int(ctx, 0);
  
  nvgLineCap(vg, cap);
  return 0;
}

duk_ret_t lineJoin(duk_context *ctx)
{
  duk_int_t join = duk_require_int(ctx, 0);
  
  nvgLineJoin(vg, join);
  return 0;
}

duk_ret_t translate(duk_context *ctx)
{
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  
  nvgTranslate(vg, x, y);
  return 0;
}

duk_ret_t rotate(duk_context *ctx)
{
  float r = duk_require_number(ctx, 0);
  
  nvgRotate(vg, r);
  return 0;
}

duk_ret_t arc(duk_context *ctx)
{
  float cx = duk_require_number(ctx, 0);
  float cy = duk_require_number(ctx, 1);
  float radius = duk_require_number(ctx, 2);
  float sAngle = duk_require_number(ctx, 3);
  float eAngle = duk_require_number(ctx, 4);
  duk_uint_t flag = duk_require_number(ctx, 5);
  
  nvgArc(vg, cx, cy, radius, sAngle, eAngle, flag);
  return 0;
}

duk_ret_t fillColor(duk_context *ctx)
{
  // Assume 0~255
  float r = duk_require_number(ctx, 0);
  float g = duk_require_number(ctx, 1);
  float b = duk_require_number(ctx, 2);
  float a = duk_require_number(ctx, 3);
  
  nvgFillColor(vg, nvgRGBA(fclamp(r), fclamp(g), fclamp(b), fclamp(a)));
  return 0;
}

duk_ret_t stroke(duk_context *ctx)
{
  nvgStroke(vg);
  return 0;
}

duk_ret_t strokeColor(duk_context *ctx)
{
  // Assume 0~255
  float r = duk_require_number(ctx, 0);
  float g = duk_require_number(ctx, 1);
  float b = duk_require_number(ctx, 2);
  float a = duk_require_number(ctx, 3);
  
  nvgStrokeColor(vg, nvgRGBA(fclamp(r), fclamp(g), fclamp(b), fclamp(a)));
  return 0;
}

duk_ret_t strokeWidth(duk_context *ctx)
{
  float w = duk_require_number(ctx, 0);
  
  nvgStrokeWidth(vg, w);
  return 0;
}

duk_ret_t linearGradient(duk_context *ctx)
{
  float ax = duk_require_number(ctx, 0);
  float ay = duk_require_number(ctx, 1);
  float bx = duk_require_number(ctx, 2);
  float by = duk_require_number(ctx, 3);
  // Assume 0~255
  float r0 = duk_require_number(ctx, 4);
  float g0 = duk_require_number(ctx, 5);
  float b0 = duk_require_number(ctx, 6);
  float a0 = duk_require_number(ctx, 7);
  float r1 = duk_require_number(ctx, 8);
  float g1 = duk_require_number(ctx, 9);
  float b1 = duk_require_number(ctx, 10);
  float a1 = duk_require_number(ctx, 11);
  
  NVGpaint paint = nvgLinearGradient(vg, ax, ay, bx, by, nvgRGBA(fclamp(r0), fclamp(g0), fclamp(b0), fclamp(a0)), nvgRGBA(fclamp(r1), fclamp(g1), fclamp(b1), fclamp(a1)));
  nvgFillPaint(vg, paint);
  //nvgFill(vg);

  return 0;
}

duk_ret_t boxGradient(duk_context *ctx)
{
  float ax = duk_require_number(ctx, 0);
  float ay = duk_require_number(ctx, 1);
  float bx = duk_require_number(ctx, 2);
  float by = duk_require_number(ctx, 3);
  float radius = duk_require_number(ctx, 4);
  float f = duk_require_number(ctx, 5);
  // Assume 0~255
  float r0 = duk_require_number(ctx, 6);
  float g0 = duk_require_number(ctx, 7);
  float b0 = duk_require_number(ctx, 8);
  float a0 = duk_require_number(ctx, 9);
  float r1 = duk_require_number(ctx, 10);
  float g1 = duk_require_number(ctx, 11);
  float b1 = duk_require_number(ctx, 12);
  float a1 = duk_require_number(ctx, 13);
  
  NVGpaint paint = nvgBoxGradient(vg, ax, ay, bx, by, radius, f, nvgRGBA(fclamp(r0), fclamp(g0), fclamp(b0), fclamp(a0)), nvgRGBA(fclamp(r1), fclamp(g1), fclamp(b1), fclamp(a1)));
  nvgFillPaint(vg, paint);

  return 0;
}

duk_ret_t radialGradient(duk_context *ctx)
{
  float ax = duk_require_number(ctx, 0);
  float ay = duk_require_number(ctx, 1);
  float bx = duk_require_number(ctx, 2);
  float by = duk_require_number(ctx, 3);
  // Assume 0~255
  float r0 = duk_require_number(ctx, 4);
  float g0 = duk_require_number(ctx, 5);
  float b0 = duk_require_number(ctx, 6);
  float a0 = duk_require_number(ctx, 7);
  float r1 = duk_require_number(ctx, 8);
  float g1 = duk_require_number(ctx, 9);
  float b1 = duk_require_number(ctx, 10);
  float a1 = duk_require_number(ctx, 11);
  
  NVGpaint paint = nvgRadialGradient(vg, ax, ay, bx, by, nvgRGBA(fclamp(r0), fclamp(g0), fclamp(b0), fclamp(a0)), nvgRGBA(fclamp(r1), fclamp(g1), fclamp(b1), fclamp(a1)));
  nvgFillPaint(vg, paint);
  //nvgFill(vg);

  return 0;
}



duk_ret_t fill(duk_context *ctx)
{
  nvgFill(vg);
  return 0;
}

duk_ret_t scissor(duk_context *ctx)
{
  int n = duk_get_top(ctx);
  assert(n == 4);
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  float w = duk_require_number(ctx, 2);
  float h = duk_require_number(ctx, 3);
  nvgScissor(vg, x, y, w, h);
  return 0;
}

duk_ret_t rect(duk_context *ctx)
{
  int n = duk_get_top(ctx);
  assert(n == 4);
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  float w = duk_require_number(ctx, 2);
  float h = duk_require_number(ctx, 3);
  nvgRect(vg, x, y, w, h);
  return 0;
}

duk_ret_t roundedRect(duk_context *ctx)
{
  int n = duk_get_top(ctx);
  assert(n == 5);
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  float w = duk_require_number(ctx, 2);
  float h = duk_require_number(ctx, 3);
  float r = duk_require_number(ctx, 4);
  nvgRoundedRect(vg, x, y, w, h, r);
  return 0;
}

duk_ret_t circle(duk_context *ctx)
{
  int n = duk_get_top(ctx);
  assert(n == 3);
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  float r = duk_require_number(ctx, 2);
  nvgCircle(vg, x, y, r);
  return 0;
}

duk_ret_t ellipse(duk_context *ctx)
{
  int n = duk_get_top(ctx);
  assert(n == 4);
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  float ra = duk_require_number(ctx, 2);
  float rb = duk_require_number(ctx, 3);
  nvgEllipse(vg, x, y, ra, rb);
  return 0;
}

duk_ret_t bezierTo(duk_context *ctx)
{
  int n = duk_get_top(ctx);
  assert(n == 6);
  float x0 = duk_require_number(ctx, 0);
  float y0 = duk_require_number(ctx, 1);
  float x1 = duk_require_number(ctx, 2);
  float y1 = duk_require_number(ctx, 3);
  float x2 = duk_require_number(ctx, 4);
  float y2 = duk_require_number(ctx, 5);
  nvgBezierTo(vg, x0, y0, x1, y1, x2, y2);
  return 0;
}

duk_ret_t fontSize(duk_context *ctx)
{
  float sz = duk_require_number(ctx, 0);
  nvgFontSize(vg, sz);

  return 0;
}

duk_ret_t fontFace(duk_context *ctx)
{
  const char* face = duk_to_string(ctx, 0);
  nvgFontFace(vg, face);

  return 0;
}

duk_ret_t fontBlur(duk_context *ctx)
{
  float blur = duk_require_number(ctx, 0);
  nvgFontBlur(vg, blur);

  return 0;
}

duk_ret_t textAlign(duk_context *ctx)
{
  duk_uint_t flag = duk_require_uint(ctx, 0);
  nvgTextAlign(vg, flag);

  return 0;
}


duk_ret_t text(duk_context *ctx)
{
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  const char* text = duk_safe_to_string(ctx, 2);
  
  nvgText(vg, x, y, text, NULL);

  return 0;
}

duk_ret_t textBounds(duk_context *ctx)
{
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  const char* text = duk_safe_to_string(ctx, 2);
  
  float tw = nvgTextBounds(vg, x, y, text, NULL, NULL);

  duk_push_number(ctx, (double)tw);

  return 1;
}

duk_ret_t textMetrics(duk_context *ctx)
{
  float ascender;
  float descender;
  float lineh;
  nvgTextMetrics(vg, &ascender, &descender, &lineh);

  duk_idx_t obj_idx = duk_push_object(ctx);
  duk_push_number(ctx, ascender);
  duk_put_prop_string(ctx, obj_idx, "ascender");
  duk_push_number(ctx, descender);
  duk_put_prop_string(ctx, obj_idx, "descender");
  duk_push_number(ctx, lineh);
  duk_put_prop_string(ctx, obj_idx, "lineh");

  return 1;
}

duk_ret_t createImage(duk_context *ctx)
{
  const char* filename = duk_safe_to_string(ctx, 0);
  duk_uint_t flag = duk_require_uint(ctx, 1);
  
  int id = nvgCreateImage(vg, filename, flag);

  duk_push_uint(ctx, id);

  return 1;
}

duk_ret_t imageSize(duk_context *ctx)
{
  duk_uint_t id = duk_require_uint(ctx, 0);
  
  int w, h;
  nvgImageSize(vg, id, &w, &h);

  duk_idx_t obj_idx = duk_push_object(ctx);
  duk_push_uint(ctx, w);
  duk_put_prop_string(ctx, obj_idx, "width");
  duk_push_uint(ctx, h);
  duk_put_prop_string(ctx, obj_idx, "height");

  return 1;
}

duk_ret_t deleteImage(duk_context *ctx)
{
  duk_uint_t id = duk_require_uint(ctx, 0);
  
  nvgDeleteImage(vg, id);

  return 0;
}

duk_ret_t imagePattern(duk_context *ctx)
{
  float ox = duk_require_number(ctx, 0);
  float oy = duk_require_number(ctx, 1);
  float ex = duk_require_number(ctx, 2);
  float ey = duk_require_number(ctx, 3);
  float angle = duk_require_number(ctx, 4);
  duk_int_t imgId = duk_require_int(ctx, 5);
  float alpha = duk_require_number(ctx, 6);

  NVGpaint paint = nvgImagePattern(vg, ox, oy, ex, ey, angle, imgId, alpha);
  nvgFillPaint(vg, paint);

  return 0;
}

duk_ret_t save(duk_context *ctx)
{
  nvgSave(vg);
  return 0;
}

duk_ret_t restore(duk_context *ctx)
{
  nvgRestore(vg);
  return 0;
}

duk_ret_t pathWinding(duk_context *ctx)
{
  duk_uint_t flag = duk_require_uint(ctx, 0);
  nvgPathWinding(vg, flag);
  return 0;
}

duk_ret_t intersectScissor(duk_context *ctx)
{
  float x = duk_require_number(ctx, 0);
  float y = duk_require_number(ctx, 1);
  float w = duk_require_number(ctx, 2);
  float h = duk_require_number(ctx, 3);

  nvgIntersectScissor(vg, x, y, w, h);

  return 0;
}

duk_ret_t resetScissor(duk_context *ctx)
{
  nvgResetScissor(vg);

  return 0;
}

duk_ret_t onClick(duk_context *ctx)
{
  printf("On click\n");
  return 0;
}

/* MyObject */
duk_ret_t myobject_constructor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  /* Set this.name = name; */
  duk_push_this(ctx);
  duk_dup(ctx, 0);
  duk_put_prop_string(ctx, -2, "name");

  return 0;  /* use default instance */
}

/* NanoVG */
duk_ret_t nanocanvas_constructor(duk_context *ctx) {
  if (!duk_is_constructor_call(ctx)) {
    return DUK_RET_TYPE_ERROR;
  }

  /* Set some constant/enum value; */
  duk_push_this(ctx);

  duk_push_uint(ctx, NVG_CCW);
  duk_put_prop_string(ctx, -2, "CCW");
  duk_push_uint(ctx, NVG_CW);
  duk_put_prop_string(ctx, -2, "CW");

  duk_push_uint(ctx, NVG_SOLID);
  duk_put_prop_string(ctx, -2, "SOLID");
  duk_push_uint(ctx, NVG_HOLE);
  duk_put_prop_string(ctx, -2, "HOLE");

  duk_push_uint(ctx, NVG_BUTT);
  duk_put_prop_string(ctx, -2, "BUTT");
  duk_push_uint(ctx, NVG_ROUND);
  duk_put_prop_string(ctx, -2, "ROUND");
  duk_push_uint(ctx, NVG_SQUARE);
  duk_put_prop_string(ctx, -2, "SQUARE");
  duk_push_uint(ctx, NVG_BEVEL);
  duk_put_prop_string(ctx, -2, "BEVEL");
  duk_push_uint(ctx, NVG_MITER);
  duk_put_prop_string(ctx, -2, "MITER");

  duk_push_uint(ctx, NVG_ALIGN_LEFT);
  duk_put_prop_string(ctx, -2, "ALIGN_LEFT");
  duk_push_uint(ctx, NVG_ALIGN_CENTER);
  duk_put_prop_string(ctx, -2, "ALIGN_CENTER");
  duk_push_uint(ctx, NVG_ALIGN_RIGHT);
  duk_put_prop_string(ctx, -2, "ALIGN_RIGHT");
  duk_push_uint(ctx, NVG_ALIGN_TOP);
  duk_put_prop_string(ctx, -2, "ALIGN_TOP");
  duk_push_uint(ctx, NVG_ALIGN_MIDDLE);
  duk_put_prop_string(ctx, -2, "ALIGN_MIDDLE");
  duk_push_uint(ctx, NVG_ALIGN_BOTTOM);
  duk_put_prop_string(ctx, -2, "ALIGN_BOTTOM");
  duk_push_uint(ctx, NVG_ALIGN_BASELINE);
  duk_put_prop_string(ctx, -2, "ALIGN_BASELINE");

  return 0;  /* use default instance */
}


duk_ret_t print_name(duk_context *ctx)
{
  duk_push_this(ctx);
  duk_get_prop_string(ctx, -1, "name");
  printf("My name is: %s\n", duk_safe_to_string(ctx, -1));
  return 0;
}

void decode_test(duk_context* ctx)
{
  duk_set_top(ctx, 0);
  duk_push_string(ctx, "dGVzdCBzdHJpbmc=");
  duk_base64_decode(ctx, -1);
  printf("base64 decode: %s\n", duk_to_string(ctx, -1));
  duk_set_top(ctx, 0);
}

void keyboardFunc(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS || action == GLFW_REPEAT){
    if(key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, GL_TRUE);
    }
  }
}

std::string ReadJSFile(const char* filename)
{
  std::ifstream f(filename, std::ifstream::binary);
  if (!f) {
    return std::string();
  }

  f.seekg(0, f.end);
  size_t sz = f.tellg();
  std::vector<char> buf(sz);

  f.seekg(0, f.beg);
  f.read(&buf.at(0), sz);
  f.close();

  return std::string(&buf.at(0));
}

int main(int argc, char** argv)
{

  int width = 1024;
  int height = 600;

  std::string js = ReadJSFile("../example/input.js");
  printf("js = %s\n", js.c_str());

  PerfGraph fps;
  initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");

  GLFWwindow* window;

  if (!glfwInit()) {
    printf("Failed to init GLFW.");
    return -1;
  }


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window = glfwCreateWindow(width, height, "NanoCanvas", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(window, keyboardFunc);

  glfwMakeContextCurrent(window);

  vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

  nvgCreateFont(vg, "sans", "../example/Roboto-Regular.ttf");
  nvgCreateFont(vg, "sans-bold", "../example/Roboto-Bold.ttf");
  nvgCreateFont(vg, "icons", "../example/entypo.ttf");

  duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, fatal_function);

  const duk_function_list_entry my_module_funcs[] = {
      { "beginPath", beginPath, 0 /* no args */ },
      { "closePath", closePath, 0 },
      { "moveTo", moveTo, 2 },
      { "lineTo", lineTo, 2 },
      { "lineCap", lineCap, 1 },
      { "lineJoin", lineJoin, 1 },
      { "translate", translate, 2 },
      { "rotate", rotate, 1 },
      { "fillColor", fillColor, 4 },
      { "fill", fill, 0 },
      { "scissor", scissor, 4 },
      { "rect", rect, 4 },
      { "arc", arc, 6 },
      { "circle", circle, 3 },
      { "ellipse", ellipse, 4 },
      { "bezierTo", bezierTo, 6 },
      { "roundedRect", roundedRect, 5 },
      { "linearGradient", linearGradient, 12 },
      { "radialGradient", radialGradient, 12 },
      { "stroke", stroke, 0 },
      { "strokeWidth", strokeWidth, 1 },
      { "strokeColor", strokeColor, 4 },
      { "save", save, 0 },
      { "restore", restore, 0 },
      { "text", text, 3 },
      { "pathWinding", pathWinding, 1 },
      { "boxGradient", boxGradient,14 },
      { "fontSize", fontSize, 1 },
      { "fontFace", fontFace, 1 },
      { "fontBlur", fontBlur, 1 },
      { "textAlign", textAlign, 1 },
      { "textBounds", textBounds, 3 },
      { "textMetrics", textMetrics, 1 },
      { "createImage", createImage, 2 },
      { "imageSize", imageSize, 1 },
      { "deleteImage", deleteImage, 1 },
      { "imagePattern", imagePattern, 7 },
      { "intersectScissor", intersectScissor, 4 },
      { "resetScissor", resetScissor, 0 },
      { NULL, NULL, 0 }
  };

  duk_push_c_function(ctx, nanocanvas_constructor, /* nargs */0);
  duk_push_object(ctx);  // push NanoVG.prototype.
  duk_put_function_list(ctx, -1, my_module_funcs);
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "NanoVG");  /* -> [ ... global ] */
  //duk_pop(ctx);

#if 0
  const duk_function_list_entry my_event_funcs[] = {
      { "onClick", onClick, 0 /* no args */ },
  };
  duk_push_c_function(ctx, myobject_constructor, 1);
  duk_push_object(ctx);  /* -> [ ... global obj ] */
  duk_put_function_list(ctx, -1, my_event_funcs);
  //duk_push_c_function(ctx, print_name, 0);
  //duk_put_prop_string(ctx, -2, "printName");
  duk_put_prop_string(ctx, -2, "prototype");
  duk_put_global_string(ctx, "Slider");  /* -> [ ... global ] */
  //duk_pop(ctx);

  decode_test(ctx);
#endif

  duk_peval_string(ctx, js.c_str());

  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1 /*index*/, "onInit");
  if (duk_pcall(ctx, 0 /*nargs*/) != 0) {
      printf("Error: %s\n", duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* pop result/error */

  glfwSwapInterval(0);
  glfwSetTime(0);
  double prevt = 0.0;
  prevt = glfwGetTime();

  while (!glfwWindowShouldClose(window))
  {

    double t, dt;

    t = glfwGetTime();
    dt = t - prevt;
    prevt = t;
    updateGraph(&fps, dt);

    // Update and render
    glViewport(0, 0, width, height);
    glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    nvgBeginFrame(vg, width, height, width / (float)height);

    duk_push_global_object(ctx);
    duk_get_prop_string(ctx, -1 /*index*/, "onDraw");
    if (duk_pcall(ctx, 0 /*nargs*/) != 0) {
        printf("Error: %s\n", duk_safe_to_string(ctx, -1));
    } else {
        //printf("%s\n", duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx);  /* pop result/error */


    renderGraph(vg, 5,5, &fps);

    nvgEndFrame(vg);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  duk_push_global_object(ctx);
  duk_get_prop_string(ctx, -1 /*index*/, "onQuit");
  if (duk_pcall(ctx, 0 /*nargs*/) != 0) {
      printf("Error: %s\n", duk_safe_to_string(ctx, -1));
  } else {
      //printf("%s\n", duk_safe_to_string(ctx, -1));
  }
  duk_pop(ctx);  /* pop result/error */

  duk_destroy_heap(ctx);

  nvgDeleteGL2(vg);

  glfwTerminate();

  return 0;
}

