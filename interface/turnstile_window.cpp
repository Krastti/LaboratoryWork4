#include <random>
#include <string>
#include <cstring>

#include "../include/StateMachine/state_machine.h"

namespace {

const int k_window_width  = 640;
const int k_window_height = 500;
const int k_step_delay_ms = 500;

State locked(1, "Закрыт");
State unlocked(2, "Открыт");
Event coin(1, "Монета");
Event push(2, "Толкнуть");

StateMachine turnstile;
std::string current_event_text  = "Нет";
std::string current_action_text = "Ожидание первого события";
int  current_step = 0;
bool is_running   = false;

std::random_device random_device;
std::mt19937 rng(random_device());
std::uniform_int_distribution<int> percent(1, 100);

std::string state_text() {
  return turnstile.get_current_state().id == locked.id ? "Закрыт" : "Открыт";
}

void initialize_turnstile() {
  turnstile.add_state(locked);
  turnstile.add_state(unlocked);
  turnstile.set_initial_state(locked);

  turnstile.add_transition(Transition(
      locked, coin, unlocked,
      "Разблокировать турникет",
      []() { current_action_text = "Монета принята. Турникет открыт."; }));

  turnstile.add_transition(Transition(
      unlocked, push, locked,
      "Пропустить человека и заблокировать турникет",
      []() { current_action_text = "Проход выполнен. Турникет снова закрыт."; }));

  turnstile.add_transition(Transition(
      locked, push, locked,
      "Отказать в проходе",
      []() { current_action_text = "Проход запрещен. Сначала нужна монета."; }));

  turnstile.add_transition(Transition(
      unlocked, coin, unlocked,
      "Оставить турникет открытым",
      []() { current_action_text = "Турникет уже открыт. Монета не меняет состояние."; }));

  turnstile.open();
}

Event next_generated_event() {
  if (current_step == 0) return push;
  if (current_step == 1) return coin;
  return percent(rng) <= 60 ? push : coin;
}

void do_process_event(const Event &event) {
  current_event_text = event.id == coin.id ? "Монета" : "Толкнуть";
  current_step++;

  try {
    turnstile.process_event(event);
  } catch (const std::exception&) {
    current_action_text = "Для этого события нет перехода.";
  }
}

void do_reset() {
  is_running   = false;
  current_step = 0;
  current_event_text  = "Нет";
  current_action_text = "Ожидание первого события";
  turnstile.reset();
}

}

// ════════════════════════════════════════════════════════════════════════════
// Windows — WinAPI
// ════════════════════════════════════════════════════════════════════════════
#ifdef _WIN32

#include <windows.h>

namespace {

const int k_timer_id        = 1;
const int k_start_button_id = 101;
const int k_stop_button_id  = 102;
const int k_coin_button_id  = 103;
const int k_push_button_id  = 104;
const int k_reset_button_id = 105;

std::wstring to_wstring(const std::string &s) {
  int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
  std::wstring result(len, L'\0');
  MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, result.data(), len);
  return result;
}

void process_event(HWND window, const Event &event) {
  do_process_event(event);
  InvalidateRect(window, nullptr, TRUE);
}

void draw_lamp(
    HDC context,
    int x, int y,
    COLORREF active_color,
    bool is_active,
    const wchar_t* caption) {
  HBRUSH brush     = CreateSolidBrush(is_active ? active_color : RGB(85, 85, 85));
  HBRUSH old_brush = static_cast<HBRUSH>(SelectObject(context, brush));
  HPEN pen         = CreatePen(PS_SOLID, 2, RGB(35, 35, 35));
  HPEN old_pen     = static_cast<HPEN>(SelectObject(context, pen));

  Ellipse(context, x, y, x + 110, y + 110);

  SelectObject(context, old_pen);   DeleteObject(pen);
  SelectObject(context, old_brush); DeleteObject(brush);

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

  HFONT title_font = CreateFontW(
      28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
  HFONT text_font = CreateFontW(
      19, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
      DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

  HFONT old_font = static_cast<HFONT>(SelectObject(context, title_font));
  std::wstring title = L"Машина состояний: турникет";
  TextOutW(context, 30, 24, title.c_str(), static_cast<int>(title.size()));

  SelectObject(context, text_font);

  std::wstring step_line   = L"Шаг: "               + std::to_wstring(current_step);
  std::wstring event_line  = L"Событие: "            + to_wstring(current_event_text);
  std::wstring action_line = L"Действие: "           + to_wstring(current_action_text);
  std::wstring state_line  = L"Текущее состояние: "  + to_wstring(state_text());

  TextOutW(context, 30,  72, step_line.c_str(),   static_cast<int>(step_line.size()));
  TextOutW(context, 30, 102, event_line.c_str(),  static_cast<int>(event_line.size()));
  TextOutW(context, 30, 132, action_line.c_str(), static_cast<int>(action_line.size()));
  TextOutW(context, 30, 162, state_line.c_str(),  static_cast<int>(state_line.size()));

  bool is_locked = turnstile.get_current_state().id == locked.id;
  draw_lamp(context, 145, 215, RGB(220, 55, 55), is_locked,  L"Закрыт");
  draw_lamp(context, 385, 215, RGB(40, 170, 95), !is_locked, L"Открыт");

  SelectObject(context, old_font);
  DeleteObject(title_font);
  DeleteObject(text_font);
}

void create_button(HWND window, const wchar_t* text, int id, int x, int y, int width) {
  CreateWindowW(
      L"BUTTON", text,
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
      x, y, width, 32,
      window, reinterpret_cast<HMENU>(id),
      GetModuleHandleW(nullptr), nullptr);
}

void reset_demo(HWND window) {
  KillTimer(window, k_timer_id);
  do_reset();
  InvalidateRect(window, nullptr, TRUE);
}

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
  switch (message) {
  case WM_CREATE:
    initialize_turnstile();
    create_button(window, L"Старт",    k_start_button_id,  30, 410,  90);
    create_button(window, L"Стоп",     k_stop_button_id,  130, 410,  90);
    create_button(window, L"Монета",   k_coin_button_id,  240, 410,  90);
    create_button(window, L"Толкнуть", k_push_button_id,  340, 410, 100);
    create_button(window, L"Сброс",    k_reset_button_id, 460, 410,  90);
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
    case k_coin_button_id:  process_event(window, coin); return 0;
    case k_push_button_id:  process_event(window, push); return 0;
    case k_reset_button_id: reset_demo(window);          return 0;
    default: return 0;
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

  WNDCLASSW window_class     = {};
  window_class.lpfnWndProc   = window_proc;
  window_class.hInstance     = instance;
  window_class.lpszClassName = class_name;
  window_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);
  window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

  RegisterClassW(&window_class);

  HWND window = CreateWindowExW(
      0, class_name, L"Турникет: машина состояний",
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, CW_USEDEFAULT,
      k_window_width, k_window_height,
      nullptr, nullptr, instance, nullptr);

  if (window == nullptr) return 0;

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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include <chrono>
#include <time.h>

namespace {

struct Color { unsigned char r, g, b; };

unsigned long alloc_color(Display* display, Colormap colormap, Color c) {
  XColor xc;
  xc.red   = static_cast<unsigned short>(c.r) << 8;
  xc.green = static_cast<unsigned short>(c.g) << 8;
  xc.blue  = static_cast<unsigned short>(c.b) << 8;
  xc.flags = DoRed | DoGreen | DoBlue;
  XAllocColor(display, colormap, &xc);
  return xc.pixel;
}

struct Button {
  int x, y, width, height;
  const char* label;
};

const Button k_buttons[] = {
  {  30, 410,  90, 32, "Старт"    },
  { 130, 410,  90, 32, "Стоп"     },
  { 240, 410,  90, 32, "Монета"   },
  { 340, 410, 100, 32, "Толкнуть" },
  { 460, 410,  90, 32, "Сброс"    },
};
const int k_button_count = 5;
const int k_btn_start  = 0;
const int k_btn_stop   = 1;
const int k_btn_coin   = 2;
const int k_btn_push   = 3;
const int k_btn_reset  = 4;

void draw_centered_text(
    Display* display, Window window, GC gc,
    XFontStruct* font, int cx, int y, const char* text) {
  int len = static_cast<int>(strlen(text));
  int text_width = XTextWidth(font, text, len);
  XDrawString(display, window, gc, cx - text_width / 2, y, text, len);
}

void draw_lamp(
    Display* display, Window window, GC gc,
    XFontStruct* font, Colormap colormap,
    int x, int y, Color active_color, bool is_active, const char* caption) {
  XSetForeground(display, gc, alloc_color(display, colormap,
      is_active ? active_color : Color{85, 85, 85}));
  XFillArc(display, window, gc, x, y, 110, 110, 0, 360 * 64);

  XSetForeground(display, gc, alloc_color(display, colormap, {35, 35, 35}));
  XDrawArc(display, window, gc, x, y, 110, 110, 0, 360 * 64);

  XSetForeground(display, gc, alloc_color(display, colormap, {25, 30, 36}));
  draw_centered_text(display, window, gc, font, x + 55, y + 140, caption);
}

void draw_button(
    Display* display, Window window, GC gc,
    XFontStruct* font, Colormap colormap, const Button &btn) {
  XSetForeground(display, gc, alloc_color(display, colormap, {220, 224, 230}));
  XFillRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);

  XSetForeground(display, gc, alloc_color(display, colormap, {170, 175, 185}));
  XDrawRectangle(display, window, gc, btn.x, btn.y, btn.width, btn.height);

  XSetForeground(display, gc, alloc_color(display, colormap, {25, 30, 36}));
  int len = static_cast<int>(strlen(btn.label));
  int text_x = btn.x + (btn.width  - XTextWidth(font, btn.label, len)) / 2;
  int text_y = btn.y + (btn.height + font->ascent) / 2 - 2;
  XDrawString(display, window, gc, text_x, text_y, btn.label, len);
}

void draw_interface(
    Display* display, Window window, GC gc,
    XFontStruct* title_font, XFontStruct* text_font, Colormap colormap) {
  XSetForeground(display, gc, alloc_color(display, colormap, {245, 247, 250}));
  XFillRectangle(display, window, gc, 0, 0, k_window_width, k_window_height);

  auto draw_line = [&](XFontStruct* font, int x, int y, const std::string &text) {
    XSetFont(display, gc, font->fid);
    XSetForeground(display, gc, alloc_color(display, colormap, {25, 30, 36}));
    XDrawString(display, window, gc, x, y, text.c_str(), static_cast<int>(text.size()));
  };

  draw_line(title_font, 30,  50, "Машина состояний: турникет");
  draw_line(text_font,  30,  90, "Шаг: "               + std::to_string(current_step));
  draw_line(text_font,  30, 120, "Событие: "            + current_event_text);
  draw_line(text_font,  30, 150, "Действие: "           + current_action_text);
  draw_line(text_font,  30, 180, "Текущее состояние: "  + state_text());

  bool is_locked = turnstile.get_current_state().id == locked.id;
  draw_lamp(display, window, gc, text_font, colormap, 145, 215, {220, 55, 55}, is_locked,  "Закрыт");
  draw_lamp(display, window, gc, text_font, colormap, 385, 215, {40, 170, 95}, !is_locked, "Открыт");

  for (int i = 0; i < k_button_count; i++) {
    draw_button(display, window, gc, text_font, colormap, k_buttons[i]);
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
  Display* display = XOpenDisplay(nullptr);
  if (display == nullptr) return 1;

  initialize_turnstile();

  int screen      = DefaultScreen(display);
  Colormap colormap = DefaultColormap(display, screen);

  Window window = XCreateSimpleWindow(
      display, RootWindow(display, screen),
      0, 0, k_window_width, k_window_height, 0,
      BlackPixel(display, screen),
      WhitePixel(display, screen));

  XStoreName(display, window, "Турникет: машина состояний");

  Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);

  XSizeHints* hints  = XAllocSizeHints();
  hints->flags       = PMinSize | PMaxSize;
  hints->min_width   = hints->max_width  = k_window_width;
  hints->min_height  = hints->max_height = k_window_height;
  XSetWMNormalHints(display, window, hints);
  XFree(hints);

  XSelectInput(display, window, ExposureMask | ButtonPressMask | StructureNotifyMask);

  XFontStruct* title_font = XLoadQueryFont(display, "-*-helvetica-bold-r-*-*-18-*-*-*-*-*-*-*");
  if (title_font == nullptr) title_font = XLoadQueryFont(display, "fixed");

  XFontStruct* text_font = XLoadQueryFont(display, "-*-helvetica-medium-r-*-*-14-*-*-*-*-*-*-*");
  if (text_font == nullptr) text_font = XLoadQueryFont(display, "fixed");

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
        draw_interface(display, window, gc, title_font, text_font, colormap);
      }

      if (event.type == ButtonPress && event.xbutton.button == Button1) {
        int idx = hit_button(event.xbutton.x, event.xbutton.y);
        switch (idx) {
        case k_btn_start:
          is_running = true;
          last_tick  = std::chrono::steady_clock::now();
          break;
        case k_btn_stop:
          is_running = false;
          break;
        case k_btn_coin:
          do_process_event(coin);
          draw_interface(display, window, gc, title_font, text_font, colormap);
          break;
        case k_btn_push:
          do_process_event(push);
          draw_interface(display, window, gc, title_font, text_font, colormap);
          break;
        case k_btn_reset:
          do_reset();
          draw_interface(display, window, gc, title_font, text_font, colormap);
          break;
        default: break;
        }
      }
    }

    if (is_running) {
      auto now     = std::chrono::steady_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_tick).count();
      if (elapsed >= k_step_delay_ms) {
        do_process_event(next_generated_event());
        draw_interface(display, window, gc, title_font, text_font, colormap);
        last_tick = now;
      }
    }

    struct timespec ts = {0, 10'000'000};
    nanosleep(&ts, nullptr);
  }

done:
  XFreeGC(display, gc);
  XFreeFont(display, title_font);
  XFreeFont(display, text_font);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return 0;
}

#endif
