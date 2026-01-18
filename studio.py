active_studio_name = \
"Zeminka Studio v0.10.1 stable branch"
"""
An interactive development environment for the Kolya142's engine "ZeminkaEngine".

License: MIT

Mainteiners:
crinbrodev - vibecoded this studio in Russian
Kolya142 - made it useable, deleted code&text trash and translated it to English
"""

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
import re
import json
import hashlib
from datetime import datetime
from tkinter import messagebox, ttk, simpledialog
from pathlib import Path
from typing import List, Optional, Dict, Set, Tuple
from concurrent.futures import ThreadPoolExecutor

# TODOO: Add JSON parser or something like this.
# TODOO: Rewrite it to C++ because Python kinda bad.

THEME_DARK = 0
THEME_LIGHT = 1
# TODO: Add these themes
#  THEME_PAPER = 2
#  THEME_CONTRAST = 3

class Config:
    """
    Class that centerilizes configuration of the IDE.
    """
    APP_NAME = active_studio_name
    THEME = THEME_LIGHT
    ACCENT_COLOR = "blue"

    ROOT_DIR = Path(os.getcwd())

    BIN_DIR = ROOT_DIR / "bin"
    OBJ_DIR = BIN_DIR / "obj"

    INCLUDE_DIR = ROOT_DIR / "include"
    THIRDPARTY_DIR = INCLUDE_DIR / "thirdparty"
    ASSETS_DIR = ROOT_DIR / "assets"
    GAME_DIR = ROOT_DIR / "game"
    ENGINE_DIR = ROOT_DIR / "engine"

    # TODO: MacOS uses they own executeable format, not ELF.
    OUTPUT_WIN64_BINARY = "game.exe"
    OUTPUT_UNIXS_BINARY = "game"

    if platform.system() == "Windows":
        OUTPUT_BINARY = OUTPUT_WIN64_BINARY
    else:
        OUTPUT_BINARY = OUTPUT_UNIXS_BINARY

if Config.THEME == THEME_DARK:
    CS = [
        "#ff5555",  # 0
        "#ffb86c",  # 1
        "#50fa7b",  # 2
        "#8be9fd",  # 3
        "#6272a4",  # 4

        "#1d1d1d",  # 5
        "#ffffff",  # 6
        "#1d1d1d",  # 7
        "#333333",  # 8
        "#ffffff",  # 9
        "#1f538d",  # 10

        "#2d8a2d",  # 11

        "#d68a00",  # 12
    ]
elif Config.THEME == THEME_LIGHT:
    CS = [
        "#ffaaaa",  # 0
        "#ffb86c",  # 1
        "#50fa7b",  # 2
        "#8be9fd",  # 3
        "#6272a4",  # 4

        "#eaeaea",  # 5
        "#000000",  # 6
        "#eaeaea",  # 7
        "#dddddd",  # 8
        "#000000",  # 9
        "#7fb3ed",  # 10

        "#8dea8d",  # 11

        "#d68a00",  # 12
    ]




class LogWidget(ctk.CTkTextbox):
    """
    Loging console widget.
    Used to output logs with color indication.
    """
    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)
        self.configure(state="disabled", font=("FreeMono", 11))

        self.tag_config("error", foreground=CS[0])
        self.tag_config("warning", foreground=CS[1])
        self.tag_config("success", foreground=CS[2])
        self.tag_config("info", foreground=CS[3])
        self.tag_config("dim", foreground=CS[4])
        self.do_scroll = True

    def log(self, text: str, tag: Optional[str] = None):
        self.configure(state="normal")
        self.insert("end", text, tag)
        if self.do_scroll:
            self.see("end")
        self.configure(state="disabled")

    def clear_content(self):
        """Полная очистка консоли."""
        self.configure(state="normal")
        self.delete("1.0", "end")
        self.configure(state="disabled")


def parse_engine_api() -> Dict[str, List[str]]:
    results: Dict[str, List[str]] = {}
    if not Config.INCLUDE_DIR.exists():
        return results

    # Regexp for parsing C function definitions.
    # TODOO: crimbrodev forgot about extern/const/unsigned
    regex = re.compile(r'^([A-Za-z0-9_]+\s+\*?[A-Za-z0-9_]+)\s*\(([^)]*)\);', re.MULTILINE)

    forbidden_words = {'return', 'if', 'else', 'while', 'for', 'switch', 'typedef', 'static'}
    valid_prefixes = (
        'ZE',   # ZeminkaEngine.
        'void', 'char', 'short', 'int', 'long', 'float', 'double',  # C types
        'u8', 's8', 'u16', 's16', 'u32', 's32', 'u64', 's64', 'f32', 'f64'  # Simplified types.
    )

    for header_file in Config.INCLUDE_DIR.rglob("*.h"):
        try:
            raw_code = header_file.read_text(encoding='utf-8', errors='ignore')

            raw_code = re.sub(r'//.*', '', raw_code)
            raw_code = re.sub(r'/\*.*?\*/', '', raw_code, flags=re.DOTALL)

            matches = regex.findall(raw_code)
            if matches:
                rel_path = str(header_file.relative_to(Config.INCLUDE_DIR))
                file_functions = []

                for match in matches:
                    func_head = match[0].strip()
                    func_args = match[1].strip()

                    head_words = func_head.split()
                    first_word = head_words[0] if head_words else ""

                    if first_word in forbidden_words:
                        continue
                    if "__" in func_head:
                        continue
                    if not any(func_head.startswith(p) for p in valid_prefixes):
                        continue
                    file_functions.append(f"{func_head}({func_args});")

                if file_functions:
                    results[rel_path] = file_functions
        except Exception:
            continue
    return results

# TODO: I want to do it in engine it self, so it is kinda useless

def convert_obj_to_c(input_path: Path) -> str:
    """Converts .obj to .c."""
    name = input_path.stem.lower().replace(" ", "_")
    vertices_list = []
    faces_list = []

    try:
        with open(input_path, "r", encoding="utf-8") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue

                if line.startswith('v '):
                    parts = line.split()
                    if len(parts) >= 4:
                        v_str = f"    {{{parts[1]}, {parts[2]}, {parts[3]}}}"
                        vertices_list.append(v_str)

                elif line.startswith('f '):
                    parts = line.split()
                    idxs = [str(int(p.split('/')[0]) - 1) for p in parts[1:]]

                    if len(idxs) == 3:
                        faces_list.append(f"    {{{idxs[0]}, {idxs[1]}, {idxs[2]}}}")
                    else:
                        # Splitting polygons to triangles.
                        assert (len(idxs)-3)%2 != 0, f"Invalid polygon in .obj file \"{input_path}\"."
                        for i in range(0, len(idnx), 2):
                            faces_list.append(f"    {{{idxs[i]}, {idxs[i+1]}, {idxs[(i+2)%idnx]}}}")

        code = f"#pragma once\n\n"

        code += f"static const NE_Vertex {name}_v[] = {{\n"
        code += ",\n".join(vertices_list)
        code += "\n}};\n\n"

        code += f"static const NE_Color {name}_c[] = {{\n"
        white = "    {1.0, 1.0, 1.0, 1.0}"
        code += ",\n".join([white] * len(vertices_list))
        code += "\n}};\n\n"

        code += f"static const NE_Face {name}_f[] = {{\n"
        code += ",\n".join(faces_list)
        code += "\n}};\n\n"

        code += f"static const NE_Model {name}_model = {{\n"
        code += f"    .verteces = {name}_v,\n"
        code += f"    .colors = {name}_c,\n"
        code += f"    .faces = {name}_f,\n"
        code += f"    .face_count = {len(faces_list)}\n"
        code += "};\n"
        return code

    except Exception as e:
        return f"Error when parsing .obj file \"{input_file}\": {str(e)}"


class StudioApp(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.title(f"{Config.APP_NAME}")
        self.geometry("800x650")  # Some laptops have screen 1280x720.
        ctk.set_appearance_mode("Dark" if Config.THEME == THEME_DARK else "White")

        self.build_sys = BuildCore(self)
        self.prof_var = ctk.StringVar(value="Debug")
        self.hot_reload_active = False
        self.mtime_store = {}
        self.current_obj_path: Optional[Path] = None

        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(0, weight=1)

        self._setup_sidebar()
        self._setup_main_tabs()

    def _setup_sidebar(self):
        self.sidebar = ctk.CTkFrame(self, width=220, corner_radius=0)
        self.sidebar.grid(row=0, column=0, sticky="nsew")

        ctk.CTkLabel(self.sidebar, text="New Engine", font=("Arial", 22, "bold")).pack(pady=30)

        ctk.CTkLabel(self.sidebar, text="Build type:", font=("Arial", 11)).pack(pady=(10, 0))
        ctk.CTkOptionMenu(self.sidebar, values=["Debug", "Release", "Not stripped Release", "Low-optimization Release"], variable=self.prof_var).pack(pady=10, padx=20)

        self.btn_compile = ctk.CTkButton(self.sidebar, text="Build", command=lambda: self.build_sys.request_build(self.prof_var.get()))
        self.btn_compile.pack(pady=5, padx=20)

        self.btn_fcompile = ctk.CTkButton(self.sidebar, text="Force Build", command=lambda: self.build_sys.request_build(self.prof_var.get(), force=True))
        self.btn_fcompile.pack(pady=5, padx=20)

        self.btn_launch = ctk.CTkButton(self.sidebar, text="Run", fg_color=CS[11], command=self.build_sys.execute_game)
        self.btn_launch.pack(pady=5, padx=20)

        self.btn_br = ctk.CTkButton(self.sidebar, text="Build & Run", command=lambda: self.build_sys.request_build(self.prof_var.get(), auto_run=True))
        self.btn_br.pack(pady=5, padx=20)

        self.btn_fbr = ctk.CTkButton(self.sidebar, text="Force Build & Run", command=lambda: self.build_sys.request_build(self.prof_var.get(), auto_run=True, force=True))
        self.btn_fbr.pack(pady=5, padx=20)

        self.sw_auto = ctk.CTkSwitch(self.sidebar, text="Hot build (doesn't work yet :(", command=self.on_toggle_hot_reload)
        self.sw_auto.pack(pady=30)

    def _setup_main_tabs(self):
        self.tabs = ctk.CTkTabview(self)
        self.tabs.grid(row=0, column=1, padx=15, pady=15, sticky="nsew")

        self._init_tab_console(self.tabs.add("Console"))
        self._init_tab_api(self.tabs.add("API Viewer"))
        self._init_tab_assets(self.tabs.add("Assets"))

    def _init_tab_console(self, tab):
        tab.grid_columnconfigure(0, weight=1); tab.grid_rowconfigure((0, 1), weight=1)
        self.issues_view = CompilerIssuesTable(tab); self.issues_view.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
        self.console_view = LogWidget(tab); self.console_view.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)

    def _init_tab_api(self, tab):
        tab.grid_columnconfigure(0, weight=1); tab.grid_rowconfigure(1, weight=1)
        ctk.CTkButton(tab, text="Scan API", command=self.on_api_scan_ui).pack(pady=10)
        self.ui_api_box = ctk.CTkTextbox(tab, font=("FreeMono", 11)); self.ui_api_box.pack(fill="both", expand=True, padx=20, pady=10)

    def _init_tab_assets(self, tab):
        tab.grid_columnconfigure(0, weight=1)
        ctk.CTkLabel(tab, text="Convert .obj to .c", font=("Arial", 18, "bold")).pack(pady=20)
        ctk.CTkButton(tab, text="Choose .obj", command=self.on_asset_select_ui).pack(pady=10)
        self.ui_asset_lbl = ctk.CTkLabel(tab, text="Nothing is choosed", text_color="gray"); self.ui_asset_lbl.pack()
        self.ui_asset_btn = ctk.CTkButton(tab, text="Convert", state="disabled", command=self.on_asset_convert_ui)
        self.ui_asset_btn.pack(pady=20)

    def log_to_console(self, m, t=None): self.after(0, lambda: self.console_view.log(m, t))
    def clear_console(self): self.after(0, self.console_view.clear_content)
    def clear_issues(self): self.after(0, self.issues_view.clear_issues)

    def on_toggle_hot_reload(self):
        return
        self.hot_reload_active = self.sw_auto.get()
        if self.hot_reload_active: threading.Thread(target=self._hot_reload_loop, daemon=True).start()

    def _hot_reload_loop(self):
        return
        while self.hot_reload_active:
            changed = False
            for d in [Config.ENGINE_DIR, Config.GAME_DIR]:
                if d.exists():
                    for f in d.rglob("*.c"):
                        mt = os.path.getmtime(f); s_f = str(f)
                        if s_f not in self.mtime_store or mt > self.mtime_store[s_f]:
                            self.mtime_store[s_f] = mt; changed = True
            if changed: self.after(0, lambda: self.build_sys.request_build(self.prof_var.get(), True))
            time.sleep(1.5)

    def on_api_scan_ui(self):
        self.ui_api_box.delete("1.0", "end")
        api_map = parse_engine_api()
        if not api_map:
            self.ui_api_box.insert("end", "Failed to scan API.")
            return
        for file, funcs in api_map.items():
            self.ui_api_box.insert("end", f"[{file}]\n", "info")
            for f in funcs: self.ui_api_box.insert("end", f"  • {f}\n")
            self.ui_api_box.insert("end", "\n")

    def on_asset_select_ui(self):
        p = ctk.filedialog.askopenfilename(filetypes=[("OBJ", "*.obj")])
        if p:
            self.current_obj_path = Path(p)
            self.ui_asset_lbl.configure(text=self.current_obj_path.name, text_color="white")
            self.ui_asset_btn.configure(state="normal")

    def on_asset_convert_ui(self):
        Config.ASSETS_DIR.mkdir(exist_ok=True)
        res = ModelAssetProcessor.process_obj_to_h(self.current_obj_path)
        (Config.ASSETS_DIR / f"{self.current_obj_path.stem}.h").write_text(res, encoding="utf-8")
        messagebox.showinfo("OK", "Done.")
        self.log_to_console(f"Asset {self.current_obj_path.name} converted successfully.\n", "success")

    def set_ui_busy_state(self, b):
        st = "disabled" if b else "normal"
        self.btn_compile.configure(state=st)
        self.btn_br.configure(state=st)




if __name__ == "__main__":
    try:
        app = StudioApp()
        app.mainloop()
    except Exception as e:
        print(f"FATAL: {e}")
