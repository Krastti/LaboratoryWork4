#include <windows.h>

#include <random>
#include <string>

#include "../include/StateMachine/state_machine.h"

namespace {

const int k_window_width = 640;
const int k_window_height = 500;
const int k_timer_id = 1;
const int k_step_delay_ms = 500;

const int k_start_button_id = 101;
const int k_stop_button_id = 102;
const int k_coin_button_id = 103;
const int k_push_button_id = 104;
const int k_reset_button_id = 105;

State locked(1, "Закрыт");
State unlocked(2, "Открыт");
Event coin(1, "Монета");
Event push(2, "Толкнуть");

StateMachine turnstile;
std::wstring current_event_text = L"Нет";
std::wstring current_action_text = L"Ожидание первого события";
int current_step = 0;
bool is_running = false;

std::random_device random_device;
std::mt19937 generator(random_device());
std::uniform_int_distribution<int> percent(1, 100);

std::wstring state_text() {
  return turnstile.get_current_state().id == locked.id ? L"Закрыт" : L"Открыт";
}

void initialize_turnstile() {
  turnstile.add_state(locked);
  turnstile.add_state(unlocked);
  turnstile.set_initial_state(locked);

  turnstile.add_transition(Transition(
      locked,
      coin,
      unlocked,
      "Разблокировать турникет",
      []() {
        current_action_text = L"Монета принята. Турникет открыт.";
      }));

  turnstile.add_transition(Transition(
      unlocked,
      push,
      locked,
      "Пропустить человека и заблокировать турникет",
      []() {
        current_action_text = L"Проход выполнен. Турникет снова закрыт.";
      }));

  turnstile.add_transition(Transition(
      locked,
      push,
      locked,
      "Отказать в проходе",
      []() {
        current_action_text = L"Проход запрещен. Сначала нужна монета.";
      }));

  turnstile.add_transition(Transition(
      unlocked,
      coin,
      unlocked,
      "Оставить турникет открытым",
      []() {
        current_action_text = L"Турникет уже открыт. Монета не меняет состояние.";
      }));

  turnstile.open();
}

Event next_generated_event() {
  if (current_step == 0) {
    return push;
  }

  if (current_step == 1) {
    return coin;
  }

  return percent(generator) <= 60 ? push : coin;
}

void process_event(HWND window, const Event &event) {
  current_event_text = event.id == coin.id ? L"Монета" : L"Толкнуть";
  current_step++;

  try {
    turnstile.process_event(event);
  } catch (const std::exception&) {
    current_action_text = L"Для этого события нет перехода.";
  }

  InvalidateRect(window, nullptr, TRUE);
}

void draw_lamp(
    HDC context,
    int x,
    int y,
    COLORREF active_color,
    bool is_active,
    const wchar_t* caption) {
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

  std::wstring step_line = L"Шаг: " + std::to_wstring(current_step);
  std::wstring event_line = L"Событие: " + current_event_text;
  std::wstring action_line = L"Действие: " + current_action_text;
  std::wstring state_line = L"Текущее состояние: " + state_text();

  TextOutW(context, 30, 72, step_line.c_str(), static_cast<int>(step_line.size()));
  TextOutW(context, 30, 102, event_line.c_str(), static_cast<int>(event_line.size()));
  TextOutW(context, 30, 132, action_line.c_str(), static_cast<int>(action_line.size()));
  TextOutW(context, 30, 162, state_line.c_str(), static_cast<int>(state_line.size()));

  bool is_locked = turnstile.get_current_state().id == locked.id;
  draw_lamp(context, 145, 215, RGB(220, 55, 55), is_locked, L"Закрыт");
  draw_lamp(context, 385, 215, RGB(40, 170, 95), !is_locked, L"Открыт");

  SelectObject(context, old_font);
  DeleteObject(title_font);
  DeleteObject(text_font);
}

void create_button(HWND window, const wchar_t* text, int id, int x, int y, int width) {
  CreateWindowW(
      L"BUTTON",
      text,
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
      x,
      y,
      width,
      32,
      window,
      reinterpret_cast<HMENU>(id),
      GetModuleHandleW(nullptr),
      nullptr);
}

void reset_demo(HWND window) {
  KillTimer(window, k_timer_id);
  is_running = false;
  current_step = 0;
  current_event_text = L"Нет";
  current_action_text = L"Ожидание первого события";
  turnstile.reset();
  InvalidateRect(window, nullptr, TRUE);
}

LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
  switch (message) {
  case WM_CREATE:
    initialize_turnstile();
    create_button(window, L"Старт", k_start_button_id, 30, 410, 90);
    create_button(window, L"Стоп", k_stop_button_id, 130, 410, 90);
    create_button(window, L"Монета", k_coin_button_id, 240, 410, 90);
    create_button(window, L"Толкнуть", k_push_button_id, 340, 410, 100);
    create_button(window, L"Сброс", k_reset_button_id, 460, 410, 90);
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

  HWND window = CreateWindowExW(
      0,
      class_name,
      L"Турникет: машина состояний",
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      k_window_width,
      k_window_height,
      nullptr,
      nullptr,
      instance,
      nullptr);

  if (window == nullptr) {
    return 0;
  }

  ShowWindow(window, show_command);
  UpdateWindow(window);

  MSG message = {};
  while (GetMessageW(&message, nullptr, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessageW(&message);
  }

  return static_cast<int>(message.wParam);
}
