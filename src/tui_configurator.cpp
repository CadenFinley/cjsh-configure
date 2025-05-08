#include "../include/tui_configurator.h"

#include <ncurses.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../include/cjsh_filesystem.h"

const std::string version = "1.0.0";
const std::string main_repo_plugins = "github.com/cadenfinley/cjsshell/plugins";
const std::string main_repo_themes = "github.com/cadenfinley/cjsshell/themes";

namespace tui {

static const std::vector<std::string> splash = {
    "   ______       __   _____    __  __",
    "  / ____/      / /  / ___/   / / / /",
    " / /      __  / /   \\__ \\   / /_/ / ",
    "/ /___   / /_/ /   ___/ /  / __  /  ",
    "\\____/   \\____/   /____/  /_/ /_/   ",
};

static const std::vector<std::string> main_menu = {
    "1) Edit Interactive (~/.cjshrc)", "2) Edit Login Profile (~/.cjprofile)",
    "3) Manage Themes", "4) Manage Plugins", "5) Exit"};

static const std::vector<std::string> edit_items_cjshrc = {
    "1) Set alias",
    "2) Add startup command",
    "3) Set environment variable",
    "4) Set theme",
    "5) Add plugin",
    "6) Remove line",
    "7) Wipe file",
    "8) Exit",
};
static const std::vector<std::string> edit_items_cjprofile = {
    "1) Set alias",
    "2) Add startup command",
    "3) Set environment variable",
    "4) Add startup argument",
    "5) Remove line",
    "6) Wipe file",
    "7) Exit",
};

static const std::vector<std::string> theme_menu = {
    "1) List Themes", "2) Download Themes", "3) Uninstall Themes", "4) Exit"};
static const std::vector<std::string> plugin_menu = {
    "1) List Installed Plugins", "2) Download Plugins", "3) Uninstall Plugins",
    "4) Exit"};

static void showMenu(const std::string& title,
                     const std::vector<std::string>& menu) {
  int choice = 0;
  while (true) {
    clear();
    {
      int rows, cols;
      getmaxyx(stdscr, rows, cols);
      int splash_col = cols / 2;
      for (size_t i = 0; i < splash.size() && (int)i < rows; ++i)
        mvprintw((int)i, splash_col, splash[i].c_str());
    }
    mvprintw(0, 0, title.c_str());
    for (size_t i = 0; i < menu.size(); ++i) {
      if ((int)i == choice) attron(A_REVERSE);
      mvprintw((int)i + 2, 2, menu[i].c_str());
      if ((int)i == choice) attroff(A_REVERSE);
    }
    int c = getch();
    switch (c) {
      case KEY_UP:
        choice = (choice + menu.size() - 1) % menu.size();
        break;
      case KEY_DOWN:
        choice = (choice + 1) % menu.size();
        break;
      case '\n':
        if (choice == (int)menu.size() - 1) return;
        mvprintw(menu.size() + 3, 2, "Not yet implemented. Press any key...");
        getch();
        break;
    }
  }
}

static void add_alias_menu(const std::string& path) {
  clear();
  mvprintw(0, 0, "Alias name: ");
  echo();
  curs_set(1);
  char name[128];
  getnstr(name, 127);
  mvprintw(1, 0, "Command  : ");
  char cmd[1024];
  getnstr(cmd, 1023);
  noecho();
  curs_set(0);

  {
    std::vector<std::string> lines;
    std::ifstream ifs(path);
    std::string l;
    while (std::getline(ifs, l))
      if (l.rfind(std::string("alias ") + name + "=", 0) != 0)
        lines.push_back(l);
    std::ofstream ofs(path);
    for (auto& ln : lines) ofs << ln << "\n";
    ofs << "alias " << name << "='" << cmd << "'\n";
  }

  mvprintw(3, 0, "Alias added. Press any key...");
  getch();
}

static void add_startup_command_menu(const std::string& path) {
  clear();
  mvprintw(0, 0, "Startup command: ");
  echo();
  curs_set(1);
  char cmd[1024];
  getnstr(cmd, 1023);
  noecho();
  curs_set(0);

  {
    std::ifstream ifs(path);
    bool exists = false;
    std::string l;
    while (std::getline(ifs, l))
      if (l == cmd) {
        exists = true;
        break;
      }
    std::ofstream ofs(path, std::ios::app);
    if (!exists) {
      ofs << cmd << "\n";
      mvprintw(2, 0, "Command added. Press any key...");
    } else {
      mvprintw(2, 0, "Command already exists. Press any key...");
    }
    getch();
  }
}

static void add_env_var_menu(const std::string& path) {
  clear();
  mvprintw(0, 0, "Variable name : ");
  echo();
  curs_set(1);
  char var[128];
  getnstr(var, 127);
  mvprintw(1, 0, "Variable value: ");
  char val[1024];
  getnstr(val, 1023);
  noecho();
  curs_set(0);

  {
    std::vector<std::string> lines;
    std::ifstream ifs(path);
    std::string l;
    while (std::getline(ifs, l))
      if (l.rfind(std::string("export ") + var + "=", 0) != 0)
        lines.push_back(l);
    std::ofstream ofs(path);
    for (auto& ln : lines) ofs << ln << "\n";
    ofs << "export " << var << "=" << val << "\n";
  }

  mvprintw(3, 0, "Export added. Press any key...");
  getch();
}

static void add_theme_menu(const std::string& path) {
  clear();
  mvprintw(0, 0, "Theme name: ");
  echo();
  curs_set(1);
  char theme[128];
  getnstr(theme, 127);
  noecho();
  curs_set(0);

  {
    std::vector<std::string> lines;
    std::ifstream ifs(path);
    std::string l;
    while (std::getline(ifs, l))
      if (l.rfind("theme load ", 0) != 0) lines.push_back(l);
    std::ofstream ofs(path);
    for (auto& ln : lines) ofs << ln << "\n";
    ofs << "theme load " << theme << "\n";
  }

  mvprintw(2, 0, "Theme set. Press any key...");
  getch();
}

static void add_plugin_menu(const std::string& path) {
  clear();
  mvprintw(0, 0, "Plugin name: ");
  echo();
  curs_set(1);
  char plugin[128];
  getnstr(plugin, 127);
  noecho();
  curs_set(0);

  std::ofstream ofs(path, std::ios::app);
  ofs << "plugin " << plugin << " enable\n";
  mvprintw(2, 0, "Plugin added. Press any key...");
  getch();
}

static void add_startup_arg(const std::string& path) {
  clear();
  mvprintw(0, 0, "Startup argument: ");
  echo();
  curs_set(1);
  char arg[1024];
  getnstr(arg, 1023);
  noecho();
  curs_set(0);

  {
    std::ifstream ifs(path);
    bool exists = false;
    std::string l;
    while (std::getline(ifs, l))
      if (l == arg) {
        exists = true;
        break;
      }
    std::ofstream ofs(path, std::ios::app);
    if (!exists) {
      ofs << arg << "\n";
      mvprintw(2, 0, "Argument added. Press any key...");
    } else {
      mvprintw(2, 0, "Argument already exists. Press any key...");
    }
    getch();
  }
}

static void list_themes() {
  clear();
  mvprintw(0, 0, "Installed Themes:");
  int row = 1;
  for (auto& entry : cjsh_filesystem::fs::directory_iterator(
           cjsh_filesystem::g_cjsh_theme_path)) {
    auto ext = entry.path().extension().string();
    if (ext == ".json") mvprintw(row++, 0, entry.path().filename().c_str());
  }
  mvprintw(row + 1, 0, "Press any key...");
  getch();
}

static void manageThemes() {
  int choice = 0;
  while (true) {
    clear();
    mvprintw(0, 0, "Manage Themes");
    for (size_t i = 0; i < theme_menu.size(); ++i) {
      if ((int)i == choice) attron(A_REVERSE);
      mvprintw((int)i + 2, 2, theme_menu[i].c_str());
      if ((int)i == choice) attroff(A_REVERSE);
    }
    int c = getch();
    switch (c) {
      case KEY_UP:
        choice = (choice + theme_menu.size() - 1) % theme_menu.size();
        break;
      case KEY_DOWN:
        choice = (choice + 1) % theme_menu.size();
        break;
      case '\n':
        if (choice == (int)theme_menu.size() - 1) return;
        if (choice == 0) {
          list_themes();
        } else {
          clear();
          mvprintw(0, 0, "Not yet implemented. Press any key...");
          getch();
        }
        break;
    }
  }
}

static void list_plugins() {
  clear();
  mvprintw(0, 0, "Installed Plugins:");
  int row = 1;
  for (auto& entry : cjsh_filesystem::fs::directory_iterator(
           cjsh_filesystem::g_cjsh_plugin_path)) {
    auto ext = entry.path().extension().string();
    if (ext == ".dylib" || ext == ".so")
      mvprintw(row++, 0, entry.path().filename().c_str());
  }
  mvprintw(row + 1, 0, "Press any key...");
  getch();
}

static void managePlugins() {
  int choice = 0;
  while (true) {
    clear();
    mvprintw(0, 0, "Manage Plugins");
    for (size_t i = 0; i < plugin_menu.size(); ++i) {
      if ((int)i == choice) attron(A_REVERSE);
      mvprintw((int)i + 2, 2, plugin_menu[i].c_str());
      if ((int)i == choice) attroff(A_REVERSE);
    }
    int c = getch();
    switch (c) {
      case KEY_UP:
        choice = (choice + plugin_menu.size() - 1) % plugin_menu.size();
        break;
      case KEY_DOWN:
        choice = (choice + 1) % plugin_menu.size();
        break;
      case '\n':
        if (choice == (int)plugin_menu.size() - 1) return;
        if (choice == 0) {
          list_plugins();
        } else {
          clear();
          mvprintw(0, 0, "Not yet implemented. Press any key...");
          getch();
        }
        break;
    }
  }
}

static void configureFile(const std::string& orig_path,
                          const std::vector<std::string>& edit_items) {
  std::string temp_path = orig_path + ".tmp";
  cjsh_filesystem::fs::copy_file(
      orig_path, temp_path,
      cjsh_filesystem::fs::copy_options::overwrite_existing);
  std::string path = temp_path;
  bool exitLoop = false;
  int choice = 0;
  while (true) {
    clear();
    {
      std::vector<std::string> lines;
      std::ifstream ifs(path);
      std::string l;
      while (std::getline(ifs, l)) lines.push_back(l);
      int rows, cols;
      getmaxyx(stdscr, rows, cols);
      int preview_col = cols / 2;
      mvprintw(1, preview_col, "Preview:");
      for (size_t i = 0; i < lines.size() && (int)i < rows - 3; ++i) {
        mvprintw((int)i + 2, preview_col,
                 lines[i].substr(0, cols - preview_col - 1).c_str());
      }
    }

    mvprintw(0, 0, ("Configure " + path).c_str());
    for (size_t i = 0; i < edit_items.size(); ++i) {
      if ((int)i == choice) attron(A_REVERSE);
      mvprintw((int)i + 2, 2, edit_items[i].c_str());
      if ((int)i == choice) attroff(A_REVERSE);
    }
    int c = getch();
    switch (c) {
      case KEY_UP:
        choice = (choice + edit_items.size() - 1) % edit_items.size();
        break;
      case KEY_DOWN:
        choice = (choice + 1) % edit_items.size();
        break;
      case '\n': {
        int exit_idx = edit_items.size() - 1;
        int wipe_idx = edit_items.size() - 2;
        int remove_idx = edit_items.size() - 3;
        if (choice == exit_idx) {
          exitLoop = true;
          break;
        }
        if (choice == wipe_idx) {
          std::ofstream ofs(path);
          clear();
          mvprintw(0, 0, "File wiped. Press any key...");
          getch();
          continue;
        }
        if (choice == remove_idx) {
          std::vector<std::string> lines;
          std::ifstream ifs2(path);
          std::string l;
          while (std::getline(ifs2, l)) lines.push_back(l);
          clear();
          for (size_t i = 0; i < lines.size(); ++i)
            mvprintw((int)i, 0, "%zu: %s", i + 1, lines[i].c_str());
          mvprintw((int)lines.size() + 1, 0, "Line # to remove: ");
          echo();
          curs_set(1);
          char num[16];
          getnstr(num, 15);
          int idx = atoi(num) - 1;
          if (idx >= 0 && idx < (int)lines.size())
            lines.erase(lines.begin() + idx);
          std::ofstream ofs2(path);
          for (auto& ln : lines) ofs2 << ln << "\n";
          noecho();
          curs_set(0);
          mvprintw((int)lines.size() + 3, 0, "Removed. Press any key...");
          getch();
        } else {
          if (path.find(".cjshrc") != std::string::npos) {
            switch (choice) {
              case 0:
                add_alias_menu(path);
                break;
              case 1:
                add_startup_command_menu(path);
                break;
              case 2:
                add_env_var_menu(path);
                break;
              case 3:
                add_theme_menu(path);
                break;
              case 4:
                add_plugin_menu(path);
                break;
            }
          } else if (path.find(".cjprofile") != std::string::npos) {
            switch (choice) {
              case 0:
                add_alias_menu(path);
                break;
              case 1:
                add_startup_command_menu(path);
                break;
              case 2:
                add_env_var_menu(path);
                break;
              case 3:
                add_startup_arg(path);
                break;
            }
          }
        }
      } break;
    }
    if (exitLoop) break;
  }

  bool changed = false;
  {
    std::ifstream orig_fs(orig_path, std::ios::binary);
    std::ifstream tmp_fs(temp_path, std::ios::binary);
    char c1, c2;
    while (orig_fs.get(c1) && tmp_fs.get(c2)) {
      if (c1 != c2) {
        changed = true;
        break;
      }
    }
    if (!changed && (orig_fs.get(c1) || tmp_fs.get(c2))) changed = true;
  }

  if (changed) {
    clear();
    mvprintw(0, 0, "Save changes? (y/n)");
    int c = getch();
    if (c == 'y' || c == 'Y') {
      cjsh_filesystem::fs::copy_file(
          temp_path, orig_path,
          cjsh_filesystem::fs::copy_options::overwrite_existing);
    }
  }

  cjsh_filesystem::fs::remove(temp_path);
}

void Configurator::run() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);

  int choice = 0;
  std::string rc = cjsh_filesystem::g_cjsh_source_path.string();
  std::string profile = cjsh_filesystem::g_cjsh_profile_path.string();

  while (true) {
    clear();
    {
      int rows, cols;
      getmaxyx(stdscr, rows, cols);
      int splash_col = cols / 2;
      for (size_t i = 0; i < splash.size() && (int)i < rows; ++i)
        mvprintw((int)i, splash_col, splash[i].c_str());
    }
    mvprintw(0, 0, "CJ's Shell Configurator");
    mvprintw(1, 0, ("Version: " + version).c_str());
    for (size_t i = 0; i < main_menu.size(); ++i) {
      if ((int)i == choice) attron(A_REVERSE);
      mvprintw((int)i + 3, 2, main_menu[i].c_str());
      if ((int)i == choice) attroff(A_REVERSE);
    }
    int c = getch();
    switch (c) {
      case KEY_UP:
        choice = (choice + main_menu.size() - 1) % main_menu.size();
        break;
      case KEY_DOWN:
        choice = (choice + 1) % main_menu.size();
        break;
      case '\n':
        endwin();
        if (choice == 0) {
          configureFile(rc, edit_items_cjshrc);
        } else if (choice == 1) {
          configureFile(profile, edit_items_cjprofile);
        } else if (choice == 2) {
          manageThemes();
        } else if (choice == 3) {
          managePlugins();
        } else if (choice == 4) {
          return;
        }
        initscr();
        break;
    }
  }
}

}  // namespace tui
