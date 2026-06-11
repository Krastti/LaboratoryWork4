#include "../include/StateMachine/state_machine.h"
#include <cstring>
#include <random>
#include <string>
namespace {
const int k_window_width = 640;
const int k_window_height = 500;
const int k_step_delay_ms = 500;
State locked(1, "Locked");
State unlocked(2, "Unlocked");
Event coin(1, "Coin");
Event push(2, "Push");
StateMachine turnstile;
std::wstring current_event_text = L"No";
std::wstring current_action_text = L"Waiting for first event";
int current_step = 0;
bool is_running = false;
std::random_device random_device;
std::mt19937 rng(random_device());
std::uniform_int_distribution<int> percent(1, 100);
std::wstring state_text() {
  return turnstile.get_current_state().id == locked.id ? L"Locked"
                                                       : L"Unlocked";
}
void initialize_turnstile() {
  turnstile.add_state(locked);
  turnstile.add_state(unlocked);
  turnstile.set_initial_state(locked);
  turnstile.add_transition(
      Transition(locked, coin, unlocked, "Unlock turnstile", []() {
        current_action_text = L"Coin accepted. Turnstile opened.";
      }));
  turnstile.add_transition(Transition(
      unlocked, push, locked, "Allow passage and lock turnstile", []() {
        current_action_text = L"Pass completed. Turnstile locked again.";
      }));
  turnstile.add_transition(
      Transition(locked, push, locked, "Deny passage", []() {
        current_action_text = L"Access denied. Coin required first.";
      }));
  turnstile.add_transition(
      Transition(unlocked, coin, unlocked, "Keep turnstile open", []() {
        current_action_text =
            L"Turnstile already open. Coin does not change state.";
      }));
  turnstile.open();
}
Event next_generated_event() {
  if (current_step == 0)
    return push;
  if (current_step == 1)
    return coin;
  return percent(rng) <= 60 ? push : coin;
}
void do_process_event(const Event &event) {
  current_event_text = event.id == coin.id ? L"Coin" : L"Push";
  current_step++;
  try {
    turnstile.process_event(event);
  } catch (const std::exception &) {
    current_action_text = L"No transition for this event.";
  }
}
void do_reset() {
  is_running = false;
  current_step = 0;
  current_event_text = L"No";
  current_action_text = L"Waiting for first event";
  turnstile.reset();
}
} // namespace
// ════════════════════════════════════════════════════════════════════════════
// Windows — WinAPI
// ════════════════════════════════════════════════════════════════════════════
#ifdef _WIN32
#include <windows.h>
namespace {
const int k_timer_id = 1;
const int k_start_button_id = 101;
const int k_stop_button_id = 102;
const int k_coin_button_id = 103;
const int k_push_button_id = 104;
const int k_reset_button_id = 105;
std::wstring to_wstring(const std::wstring &s) { return s; }
void process_event(HWND window, const Event &event) {
  do_process_event(event);
  InvalidateRect(window, nullptr, TRUE);
}
void draw_lamp(HDC context, int x, int y, COLORREF active_color, bool is_active,
               const wchar_t *caption) {
  HBRUSH brush = CreateSolidBrush(is_active ? active_color : RGB(85, 85, 85));
  HBRUSH old_brush = static_cast<HBRUSH>(SelectObject(context, brush));
  HPEN pen = CreatePen(PS_SOLID, 2, RGB(35, 35, 35));
  HPEN old_pen = static_cast<HPEN>(SelectObject(context, pen));
  Ellipse(context, x, y, x + 110, y + 110);
  SelectObject(context, old_pen);
  DeleteObject(pen);
  SelectObject(context, old_brush);
  DeleteObject(brush);
  RECT text_rect = {x - 30, y + 125, x + 140, y + 155};
  DrawTextW(context, caption, -1, &text_rect, DT_CENTER | DT_SINGLELINE);
}
void draw_interface(HWND window, HDC context) {
  RECT client;
  GetClientRect(window, &client);
  HBRUSH background = CreateSolidBrush(RGB(245, 247, 250));
  FillRect(context, &client, background);
  DeleteObject(background);
  SetBkMode(context, TRANSPARENT);
  SetTextColor(context, RGB(25, 30, 36));
  HFONT title_font =
      CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
  HFONT text_font =
      CreateFontW(19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                  OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
  HFONT old_font = static_cast<HFONT>(SelectObject(context, title_font));
  std::wstring title = L"State Machine: Turnstile";
  TextOutW(context, 30, 24, title.c_str(), static_cast<int>(title.size()));
  SelectObject(context, text_font);
  std::wstring step_line = L"Step: " + std::to_wstring(current_step);
  std::wstring event_line = L"Event: " + to_wstring(current_event_text);
  std::wstring action_line = L"Action: " + to_wstring(current_action_text);
  std::wstring state_line = L"Current state: " + to_wstring(state_text());
  TextOutW(context, 30, 72, step_line.c_str(),
           static_cast<int>(step_line.size()));
  TextOutW(context, 30, 102, event_line.c_str(),
           static_cast<int>(event_line.size()));
  TextOutW(context, 30, 132, action_line.c_str(),
           static_cast<int>(action_line.size()));
  TextOutW(context, 30, 162, state_line.c_str(),
           static_cast<int>(state_line.size()));
  bool is_locked = turnstile.get_current_state().id == locked.id;
  draw_lamp(context, 145, 215, RGB(220, 55, 55), is_locked, L"Locked");
  draw_lamp(context, 385, 215, RGB(40, 170, 95), !is_locked, L"Unlocked");
  SelectObject(context, old_font);
  DeleteObject(title_font);
  DeleteObject(text_font);
}
void create_button(HWND window, const wchar_t *text, int id, int x, int y,
                   int width) {
  CreateWindowW(L"BUTTON", text,
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, x, y, width,
                32, window, reinterpret_cast<HMENU>(id),
                GetModuleHandleW(nullptr), nullptr);
}
void reset_demo(HWND window) {
  KillTimer(window, k_timer_id);
  do_reset();
  InvalidateRect(window, nullptr, TRUE);
}
LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param,
                             LPARAM l_param) {
  switch (message) {
  case WM_CREATE:
    initialize_turnstile();
    create_button(window, L"Start", k_start_button_id, 30, 410, 90);
    create_button(window, L"Stop", k_stop_button_id, 130, 410, 90);
    create_button(window, L"Coin", k_coin_button_id, 240, 410, 90);
    create_button(window, L"Push", k_push_button_id, 340, 410, 100);
    create_button(window, L"Reset", k_reset_button_id, 460, 410, 90);
    return 0;
  case WM_COMMAND:
    switch (LOWORD(w_param)) {
    case k_start_button_id:
      if (!is_running) {
        is_running = true;
        SetTimer(window, k_timer_id, k_step_delay_ms, nullptr);
      }
      return 0;
    case k_stop_button_id:
      KillTimer(window, k_timer_id);
      is_running = false;
      return 0;
    case k_coin_button_id:
      process_event(window, coin);
      return 0;
    case k_push_button_id:
      process_event(window, push);
      return 0;
    case k_reset_button_id:
      reset_demo(window);
      return 0;
    default:
      return 0;
    }
  case WM_TIMER:
    if (w_param == k_timer_id) {
      process_event(window, next_generated_event());
    }
    return 0;
  case WM_PAINT: {
    PAINTSTRUCT paint;
    HDC context = BeginPaint(window, &paint);
    draw_interface(window, context);
    EndPaint(window, &paint);
    return 0;
  }
  case WM_DESTROY:
    KillTimer(window, k_timer_id);
    turnstile.close();
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProcW(window, message, w_param, l_param);
  }
}
} // namespace
int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show_command) {
  const wchar_t class_name[] = L"TurnstileStateMachineWindow";
  WNDCLASSW window_class = {};
  window_class.lpfnWndProc = window_proc;
  window_class.hInstance = instance;
  window_class.lpszClassName = class_name;
  window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
  window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
  RegisterClassW(&window_class);
  HWND window =
      CreateWindowExW(0, class_name, L"Turnstile: State Machine",
                      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                      CW_USEDEFAULT, CW_USEDEFAULT, k_window_width,
                      k_window_height, nullptr, nullptr, instance, nullptr);
  if (window == nullptr)
    return 0;
  ShowWindow(window, show_command);
  UpdateWindow(window);
  MSG message = {};
  while (GetMessageW(&message, nullptr, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }
  return static_cast<int>(message.wParam);
}
// ════════════════════════════════════════════════════════════════════════════
// Linux / macOS — X11 (Xlib)
// ════════════════════════════════════════════════════════════════════════════
#else
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <chrono>
#include <time.h>
namespace {
struct Color {
  unsigned char r, g, b;
};
unsigned long alloc_color(Display *display, Colormap colormap, Color c) {
  XColor xc;
  xc.red = static_cast<unsigned short>(c.r) << 8;
  xc.green = static_cast<unsigned short>(c.g) << 8;
  xc.blue = static_cast<unsigned short>(c.b) << 8;
  xc.flags = DoRed | DoGreen | DoBlue;
  XAllocColor(display, colormap, &xc);
  return xc.pixel;
}
struct Button {
  int x, y, width, height;
  const char *label; // UTF-8
};
const Button k_buttons[] = {
    {30, 410, 90, 32, "Start"},  {130, 410, 90, 32, "Stop"},
    {240, 410, 90, 32, "Coin"},  {340, 410, 100, 32, "Push"},
    {460, 410, 90, 32, "Reset"},
};
const int k_button_count = 5;
const int k_btn_start = 0;
const int k_btn_stop = 1;
const int k_btn_coin = 2;
const int k_btn_push = 3;
const int k_btn_reset = 4;
// ── Xft helpers ──────────────────────────────────────────────────────────────
void xft_draw_string(Display *display, Drawable drawable, Visual *visual,
                     Colormap colormap, XftFont *font, Color fg, int x, int y,
                     const char *utf8_text) {
  XftDraw *xft = XftDrawCreate(display, drawable, visual, colormap);
  XRenderColor rc = {static_cast<unsigned short>(fg.r * 257u),
                     static_cast<unsigned short>(fg.g * 257u),
                     static_cast<unsigned short>(fg.b * 257u), 0xFFFFu};
  XftColor xft_color;
  XftColorAllocValue(display, visual, colormap, &rc, &xft_color);
  XftDrawStringUtf8(xft, &xft_color, font, x, y,
                    reinterpret_cast<const FcChar8 *>(utf8_text),
                    static_cast<int>(strlen(utf8_text)));
  XftColorFree(display, visual, colormap, &xft_color);
  XftDrawDestroy(xft);
}
int xft_text_width(XftFont *font, const char *utf8_text) {
  XGlyphInfo extents;
  XftTextExtentsUtf8(XtDisplay(nullptr) /* not used — need Display* */, font,
                     reinterpret_cast<const FcChar8 *>(utf8_text),
                     static_cast<int>(strlen(utf8_text)), &extents);
  return extents.xOff;
}
// Wrapper that carries Display* for xft_text_width
struct XftContext {
  Display *display;
  Visual *visual;
  Colormap colormap;
};
void xft_draw_centered(const XftContext &ctx, Drawable drawable, XftFont *font,
                       Color fg, int cx, int y, const char *text) {
  XGlyphInfo ext;
  XftTextExtentsUtf8(ctx.display, font, reinterpret_cast<const FcChar8 *>(text),
                     static_cast<int>(strlen(text)), &ext);
  int x = cx - ext.xOff / 2;
  xft_draw_string(ctx.display, drawable, ctx.visual, ctx.colormap, font, fg, x,
                  y, text);
}
// ── Drawing functions
// ─────────────────────────────────────────────────────────
void draw_centered_text(const XftContext &ctx, Drawable drawable, XftFont *font,
                        int cx, int y, const char *text) {
  xft_draw_centered(ctx, drawable, font, {25, 30, 36}, cx, y, text);
}
void draw_lamp(Display *display, Window window, GC gc, const XftContext &ctx,
               XftFont *font, int x, int y, Color active_color, bool is_active,
               const char *caption) {
  XSetForeground(display, gc,
                 alloc_color(display, ctx.colormap,
                             is_active ? active_color : Color{85, 85, 85}));
  XFillArc(display, window, gc, x, y, 110, 110, 0, 360 * 64);
  XSetForeground(display, gc, alloc_color(display, ctx.colormap, {35, 35, 35}));
  XDrawArc(display, window, gc, x, y, 110, 110, 0, 360 * 64);
  xft_draw_centered(ctx, window, font, {25, 30, 36}, x + 55,
                    y + 140 + font->ascent, caption);
}
void draw_button(Display *display, Window window, GC gc, const XftContext &ctx,
                 XftFont *font, const Button &btn) {
  XSetForeground(display, gc,
                 alloc_color(display, ctx.colormap, {220, 224, 230}));
  XFillRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);
  XSetForeground(display, gc,
                 alloc_color(display, ctx.colormap, {170, 175, 185}));
  XDrawRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);
  XGlyphInfo ext;
  XftTextExtentsUtf8(display, font,
                     reinterpret_cast<const FcChar8 *>(btn.label),
                     static_cast<int>(strlen(btn.label)), &ext);
  int text_x = btn.x + (btn.width - ext.xOff) / 2;
  int text_y = btn.y + (btn.height + font->ascent) / 2 - 2;
  xft_draw_string(ctx.display, window, ctx.visual, ctx.colormap, font,
                  {25, 30, 36}, text_x, text_y, btn.label);
}
// Convert wstring (shared state) to UTF-8 for Xft rendering
std::string to_utf8(const std::wstring &ws) {
  if (ws.empty())
    return {};
  std::string result;
  for (wchar_t wc : ws) {
    unsigned int cp = static_cast<unsigned int>(wc);
    if (cp < 0x80) {
      result += static_cast<char>(cp);
    } else if (cp < 0x800) {
      result += static_cast<char>(0xC0 | (cp >> 6));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
      result += static_cast<char>(0xE0 | (cp >> 12));
      result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
      result += static_cast<char>(0x80 | (cp & 0x3F));
    }
  }
  return result;
}
void draw_interface(Display *display, Window window, GC gc, XftFont *title_font,
                    XftFont *text_font, const XftContext &ctx) {
  XSetForeground(display, gc,
                 alloc_color(display, ctx.colormap, {245, 247, 250}));
  XFillRectangle(display, window, gc, 0, 0, k_window_width, k_window_height);
  auto draw_line = [&](XftFont *font, int x, int y, const std::string &utf8) {
    xft_draw_string(display, window, ctx.visual, ctx.colormap, font,
                    {25, 30, 36}, x, y, utf8.c_str());
  };
  draw_line(title_font, 30, 50, "State Machine: Turnstile");
  draw_line(text_font, 30, 90, "Step: " + std::to_string(current_step));
  draw_line(text_font, 30, 120, "Event: " + to_utf8(current_event_text));
  draw_line(text_font, 30, 150, "Action: " + to_utf8(current_action_text));
  draw_line(text_font, 30, 180, "Current state: " + to_utf8(state_text()));
  bool is_locked = turnstile.get_current_state().id == locked.id;
  draw_lamp(display, window, gc, ctx, text_font, 145, 215, {220, 55, 55},
            is_locked, "Locked");
  draw_lamp(display, window, gc, ctx, text_font, 385, 215, {40, 170, 95},
            !is_locked, "Unlocked");
  for (int i = 0; i < k_button_count; i++) {
    draw_button(display, window, gc, ctx, text_font, k_buttons[i]);
  }
}
int hit_button(int mx, int my) {
  for (int i = 0; i < k_button_count; i++) {
    const Button &b = k_buttons[i];
    if (mx >= b.x && mx < b.x + b.width && my >= b.y && my < b.y + b.height) {
      return i;
    }
  }
  return -1;
}
} // namespace
int main() {
  Display *display = XOpenDisplay(nullptr);
  if (display == nullptr)
    return 1;
  initialize_turnstile();
  int screen = DefaultScreen(display);
  Visual *visual = DefaultVisual(display, screen);
  Colormap colormap = DefaultColormap(display, screen);
  XftContext ctx = {display, visual, colormap};
  Window window = XCreateSimpleWindow(display, RootWindow(display, screen), 0,
                                      0, k_window_width, k_window_height, 0,
                                      BlackPixel(display, screen),
                                      WhitePixel(display, screen));
  XStoreName(display, window, "Turnstile: State Machine");
  Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);
  XSizeHints *hints = XAllocSizeHints();
  hints->flags = PMinSize | PMaxSize;
  hints->min_width = hints->max_width = k_window_width;
  hints->min_height = hints->max_height = k_window_height;
  XSetWMNormalHints(display, window, hints);
  XFree(hints);
  XSelectInput(display, window,
               ExposureMask | ButtonPressMask | StructureNotifyMask);
  // Load fonts via Xft — supports UTF-8 / Cyrillic out of the box
  XftFont *title_font = XftFontOpen(
      display, screen, XFT_FAMILY, XftTypeString, "DejaVu Sans", XFT_SIZE,
      XftTypeDouble, 16.0, XFT_WEIGHT, XftTypeInteger, FC_WEIGHT_BOLD, nullptr);
  if (title_font == nullptr)
    title_font =
        XftFontOpen(display, screen, XFT_SIZE, XftTypeDouble, 16.0, XFT_WEIGHT,
                    XftTypeInteger, FC_WEIGHT_BOLD, nullptr);
  XftFont *text_font =
      XftFontOpen(display, screen, XFT_FAMILY, XftTypeString, "DejaVu Sans",
                  XFT_SIZE, XftTypeDouble, 12.0, nullptr);
  if (text_font == nullptr)
    text_font =
        XftFontOpen(display, screen, XFT_SIZE, XftTypeDouble, 12.0, nullptr);
  GC gc = XCreateGC(display, window, 0, nullptr);
  XMapWindow(display, window);
  XFlush(display);
  auto last_tick = std::chrono::steady_clock::now();
  while (true) {
    while (XPending(display)) {
      XEvent event;
      XNextEvent(display, &event);
      if (event.type == ClientMessage &&
          static_cast<Atom>(event.xclient.data.l[0]) == wm_delete) {
        turnstile.close();
        goto done;
      }
      if (event.type == Expose && event.xexpose.count == 0) {
        draw_interface(display, window, gc, title_font, text_font, ctx);
      }
      if (event.type == ButtonPress && event.xbutton.button == Button1) {
        int idx = hit_button(event.xbutton.x, event.xbutton.y);
        switch (idx) {
        case k_btn_start:
          is_running = true;
          last_tick = std::chrono::steady_clock::now();
          break;
        case k_btn_stop:
          is_running = false;
          break;
        case k_btn_coin:
          do_process_event(coin);
          draw_interface(display, window, gc, title_font, text_font, ctx);
          break;
        case k_btn_push:
          do_process_event(push);
          draw_interface(display, window, gc, title_font, text_font, ctx);
          break;
        case k_btn_reset:
          do_reset();
          draw_interface(display, window, gc, title_font, text_font, ctx);
          break;
        default:
          break;
        }
      }
    }
    if (is_running) {
      auto now = std::chrono::steady_clock::now();
      auto elapsed =
          std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick)
              .count();
      if (elapsed >= k_step_delay_ms) {
        do_process_event(next_generated_event());
        draw_interface(display, window, gc, title_font, text_font, ctx);
        last_tick = now;
      }
    }
    struct timespec ts = {0, 10'000'000};
    nanosleep(&ts, nullptr);
  }
done:
  XFreeGC(display, gc);
  XftFontClose(display, title_font);
  XftFontClose(display, text_font);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return 0;
}
#endif