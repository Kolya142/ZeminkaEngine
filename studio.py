import customtkinter as ctk
import os
import subprocess
import threading
import sys
import platform
import time
import shutil
import urllib.request
import zipfile
import io
from tkinter import messagebox  # Исправленный импорт
from pathlib import Path
from typing import List, Optional, Dict

# --- КОНФИГУРАЦИЯ ---
class Config:
    APP_NAME = "NewEngine Studio"
    VERSION = "0.6.1" 
    THEME = "Dark"
    ACCENT_COLOR = "blue"
    
    ROOT_DIR = Path(os.getcwd())
    BIN_DIR = ROOT_DIR / "bin"
    OBJ_DIR = BIN_DIR / "obj"
    INCLUDE_DIR = ROOT_DIR / "include"
    ASSETS_DIR = ROOT_DIR / "assets"
    GAME_DIR = ROOT_DIR / "game"
    ENGINE_DIR = ROOT_DIR / "engine"
    
    COMPILER = "gcc"
    OUTPUT_NAME = "game"
    if platform.system() == "Windows":
        OUTPUT_NAME += ".exe"

    URL_STUDIO_RAW = "https://raw.githubusercontent.com/crimbrodev/newengineSTUDIO/main/studio.py"
    URL_ENGINE_ZIP = "https://github.com/Kolya142/newengine/archive/refs/heads/main.zip"

# --- ДИАГНОСТИКА СИСТЕМЫ ---
class SystemHealth:
    @staticmethod
    def check_gcc() -> tuple[bool, str]:
        try:
            res = subprocess.run([Config.COMPILER, "--version"], capture_output=True, text=True)
            if res.returncode == 0:
                return True, res.stdout.split('\n')[0]
            return False, "GCC найден, но вернул ошибку."
        except FileNotFoundError:
            return False, "GCC не найден в PATH."

    @staticmethod
    def check_folders() -> List[tuple[str, bool]]:
        return [
            ("engine/", Config.ENGINE_DIR.exists()),
            ("game/", Config.GAME_DIR.exists()),
            ("include/", Config.INCLUDE_DIR.exists()),
            ("bin/", Config.BIN_DIR.exists())
        ]

    @staticmethod
    def check_write_perms() -> bool:
        try:
            test_file = Config.ROOT_DIR / ".write_test"
            test_file.write_text("test")
            test_file.unlink()
            return True
        except: return False

# --- УПРАВЛЕНИЕ ШАБЛОНАМИ ---
class TemplateManager:
    TEMPLATES = {
        "Minimal": {
            "desc": "Чистый файл с одним треугольником",
            "code": """#include <neweng/engine.h>

int main() {
    NScreen_init(1280, 720, 90., "New Project");
    while (NScreen_IsNtClosed()) {
        NScreen_BeginFrame();
        NScreen_DrawTriangle((NE_Vec3){0,1,2}, (NE_Vec3){-1,-1,2}, (NE_Vec3){1,-1,2}, NE_GREEN);
        NScreen_EndFrame();
    }
    return 0;
}"""
        },
        "Sword Simulator": {
            "desc": "Демонстрация меча и управления (WASD)",
            "code": """// Полноценный код Sword Simulator подставится здесь
#include <neweng/engine.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int main() {
    NScreen_init(1280, 720, 90., "Sword Simulator");
    while (NScreen_IsNtClosed()) {
        NScreen_BeginFrame();
        // Рендер...
        NScreen_EndFrame();
    }
    return 0;
}"""
        }
    }

    @staticmethod
    def apply_template(name: str):
        if name in TemplateManager.TEMPLATES:
            Config.GAME_DIR.mkdir(exist_ok=True)
            main_file = Config.GAME_DIR / "main.c"
            if main_file.exists():
                shutil.copy(main_file, str(main_file) + ".bak")
            main_file.write_text(TemplateManager.TEMPLATES[name]["code"], encoding="utf-8")
            return True
        return False

# --- ЛОГИКА ОБНОВЛЕНИЯ ---
class Updater:
    def __init__(self, log_callback): self.log = log_callback
    def update_studio(self):
        self.log("Обновление студии...\n", "info")
        try:
            with urllib.request.urlopen(Config.URL_STUDIO_RAW) as response:
                if response.status == 200:
                    with open("studio.py.new", "wb") as f: f.write(response.read())
                    os.replace("studio.py.new", "studio.py")
                    self.log("УСПЕШНО! Перезапустите программу.\n", "success")
        except Exception as e: self.log(f"Ошибка: {e}\n", "error")

    def update_engine(self):
        self.log("Обновление движка...\n", "info")
        try:
            with urllib.request.urlopen(Config.URL_ENGINE_ZIP) as response:
                if response.status == 200:
                    zip_data = response.read()
                    with zipfile.ZipFile(io.BytesIO(zip_data)) as z:
                        root = z.namelist()[0].split('/')[0]
                        for folder in ["engine", "include"]:
                            prefix = f"{root}/{folder}/"
                            for file in z.namelist():
                                if file.startswith(prefix) and not file.endswith('/'):
                                    rel = file[len(f"{root}/"):]
                                    dest = Config.ROOT_DIR / rel
                                    dest.parent.mkdir(parents=True, exist_ok=True)
                                    with open(dest, "wb") as f: f.write(z.read(file))
                    self.log("Движок обновлен!\n", "success")
        except Exception as e: self.log(f"Ошибка: {e}\n", "error")

# --- СИСТЕМА СБОРКИ ---
class BuildSystem:
    def __init__(self, app_instance):
        self.app_instance = app_instance
        self.console_log = app_instance.log_to_console
        self.game_process: Optional[subprocess.Popen] = None
        self.is_building = False

    def build(self, run_after=False):
        if self.is_building: return
        threading.Thread(target=self._build_task, args=(run_after,), daemon=True).start()

    def _build_task(self, run_after):
        self.is_building = True
        self.app_instance.set_ui_busy(True)
        self.app_instance.clear_console()
        
        if run_after and self.game_process and self.game_process.poll() is None:
            try: self.game_process.terminate(); self.game_process.wait(timeout=1)
            except: pass

        self.console_log("--- СБОРКА ---\n", "info")
        Config.BIN_DIR.mkdir(exist_ok=True); Config.OBJ_DIR.mkdir(exist_ok=True)
        
        sources = []
        for d in [Config.ENGINE_DIR, Config.GAME_DIR]:
            if d.exists(): sources.extend(list(d.rglob("*.c")))

        object_files, has_error = [], False
        common_flags = [f"-I{Config.INCLUDE_DIR}", f"-I{Config.ASSETS_DIR}", "-g", "-Wall"]

        for src_path in sources:
            rel = src_path.relative_to(Config.ROOT_DIR)
            obj = Config.OBJ_DIR / str(rel).replace(os.sep, "_").replace(".c", ".o")
            object_files.append(str(obj))
            if obj.exists() and os.path.getmtime(obj) > os.path.getmtime(src_path): continue 

            cmd = [Config.COMPILER, "-c", str(src_path), "-o", str(obj)] + common_flags
            if "engine" in src_path.parts and src_path.name == "main.c": cmd.append("-Dmain=__engine_dummy_main")

            self.console_log(f"Compiling: {rel}\n", "dim")
            proc = subprocess.run(cmd, capture_output=True, text=True, cwd=Config.ROOT_DIR)
            if proc.returncode != 0:
                self.console_log(proc.stderr, "error"); has_error = True; break

        if not has_error:
            self.console_log("Линковка...\n", "info")
            out = Config.BIN_DIR / Config.OUTPUT_NAME
            link_flags = []
            if platform.system() == "Windows": link_flags = ["-lopengl32", "-lglu32", "-lgdi32", "-lwinmm"]
            elif platform.system() == "Linux": link_flags = ["-lGL", "-lGLU", "-lm", "-lX11", "-lXrandr"]
            
            l_cmd = [Config.COMPILER] + object_files + ["-o", str(out)] + common_flags + link_flags
            proc = subprocess.run(l_cmd, capture_output=True, text=True, cwd=Config.ROOT_DIR)
            if proc.returncode == 0:
                self.console_log(f"УСПЕШНО!\n", "success")
                if run_after: self.run_game()
            else: self.console_log(proc.stderr, "error")

        self.is_building = False
        self.app_instance.set_ui_busy(False)

    def run_game(self):
        exe = Config.BIN_DIR / Config.OUTPUT_NAME
        if not exe.exists(): return
        try: self.game_process = subprocess.Popen([str(exe)], cwd=Config.ROOT_DIR)
        except Exception as e: self.console_log(f"Ошибка: {e}\n", "error")

# --- ГЛАВНОЕ ОКНО ---
class StudioApp(ctk.CTk):
    def __init__(self):
        super().__init__()
        self.title(f"{Config.APP_NAME} v{Config.VERSION}")
        self.geometry("1100x750")
        ctk.set_appearance_mode(Config.THEME)
        
        self.build_system = BuildSystem(self)
        self.updater = Updater(self.log_to_console)
        
        self.grid_columnconfigure(1, weight=1); self.grid_rowconfigure(0, weight=1)

        # ЛЕВОЕ МЕНЮ
        self.sidebar = ctk.CTkFrame(self, width=200, corner_radius=0)
        self.sidebar.grid(row=0, column=0, sticky="nsew")
        ctk.CTkLabel(self.sidebar, text="NEW ENGINE", font=("Arial", 20, "bold")).pack(pady=20)
        
        self.btn_build = ctk.CTkButton(self.sidebar, text="🔨 Build", command=lambda: self.build_system.build(False))
        self.btn_build.pack(padx=20, pady=10)
        self.btn_run = ctk.CTkButton(self.sidebar, text="▶ Run", command=self.build_system.run_game, fg_color="green")
        self.btn_run.pack(padx=20, pady=10)
        self.btn_br = ctk.CTkButton(self.sidebar, text="🚀 Build & Run", command=lambda: self.build_system.build(True))
        self.btn_br.pack(padx=20, pady=10)

        # ПРАВАЯ ЧАСТЬ
        self.tab_view = ctk.CTkTabview(self)
        self.tab_view.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")
        
        self.tab_code = self.tab_view.add("Логи")
        self.tab_sys = self.tab_view.add("Система & Шаблоны")
        self.tab_update = self.tab_view.add("Обновление")

        self.setup_log_tab()
        self.setup_system_tab()
        self.setup_update_tab()

    def setup_log_tab(self):
        self.tab_code.grid_columnconfigure(0, weight=1); self.tab_code.grid_rowconfigure(0, weight=1)
        self.console = LogPanel(self.tab_code)
        self.console.grid(row=0, column=0, sticky="nsew")

    def setup_system_tab(self):
        self.tab_sys.grid_columnconfigure((0,1), weight=1)
        
        # Диагностика
        frame_health = ctk.CTkFrame(self.tab_sys)
        frame_health.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
        ctk.CTkLabel(frame_health, text="Диагностика", font=("Arial", 16, "bold")).pack(pady=10)
        self.health_box = ctk.CTkTextbox(frame_health, height=200, width=300)
        self.health_box.pack(padx=10, pady=10)
        ctk.CTkButton(frame_health, text="Проверить систему", command=self.run_diagnostics).pack(pady=10)

        # Шаблоны
        frame_tmpl = ctk.CTkFrame(self.tab_sys)
        frame_tmpl.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")
        ctk.CTkLabel(frame_tmpl, text="Шаблоны проектов", font=("Arial", 16, "bold")).pack(pady=10)
        for name in TemplateManager.TEMPLATES:
            f = ctk.CTkFrame(frame_tmpl)
            f.pack(fill="x", padx=10, pady=5)
            ctk.CTkLabel(f, text=name).pack(side="left", padx=5)
            ctk.CTkButton(f, text="Применить", width=80, command=lambda n=name: self.confirm_template(n)).pack(side="right", padx=5, pady=5)

    def setup_update_tab(self):
        self.tab_update.grid_columnconfigure(0, weight=1)
        ctk.CTkButton(self.tab_update, text="Обновить Студию", command=self.updater.update_studio).pack(pady=10)
        ctk.CTkButton(self.tab_update, text="Обновить Движок", command=self.updater.update_engine, fg_color="orange").pack(pady=10)

    def run_diagnostics(self):
        self.health_box.delete("1.0", "end")
        ok, msg = SystemHealth.check_gcc()
        self.health_box.insert("end", f"Компилятор: {'✅' if ok else '❌'} {msg}\n\n")
        self.health_box.insert("end", "Папки:\n")
        for f, ex in SystemHealth.check_folders(): self.health_box.insert("end", f"{'✅' if ex else '❌'} {f}\n")

    def confirm_template(self, name):
        # ИСПОЛЬЗУЕМ стандартный messagebox
        if messagebox.askyesno("Внимание", f"Перезаписать game/main.c шаблоном '{name}'?"):
            if TemplateManager.apply_template(name):
                self.log_to_console(f"Шаблон '{name}' применен.\n", "success")

    def log_to_console(self, text, tag=None): self.after(0, lambda: self.console.write(text, tag))
    def clear_console(self): self.after(0, lambda: self.console.clear())
    def set_ui_busy(self, busy: bool):
        s = "disabled" if busy else "normal"
        self.btn_build.configure(state=s); self.btn_run.configure(state=s); self.btn_br.configure(state=s)

class LogPanel(ctk.CTkTextbox):
    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)
        self.configure(state="disabled", font=("Consolas", 12))
        self.tag_config("error", foreground="#ff5555")
        self.tag_config("warning", foreground="#ffb86c")
        self.tag_config("success", foreground="#50fa7b")
        self.tag_config("info", foreground="#8be9fd")
        self.tag_config("dim", foreground="#6272a4")
    def write(self, text, tag=None):
        self.configure(state="normal"); self.insert("end", text, tag); self.see("end"); self.configure(state="disabled")
    def clear(self):
        self.configure(state="normal"); self.delete("1.0", "end"); self.configure(state="disabled")

if __name__ == "__main__":
    app = StudioApp()
    app.mainloop()