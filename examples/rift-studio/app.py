#!/usr/bin/env python3
"""RIFT Studio: a small dependency-free GUI for editing and inspecting RIFT files.

The GUI deliberately talks to the project's rift CLI instead of reimplementing
compiler behaviour. Set RIFT_BIN to point at a built executable when it is not
found automatically.
"""

from __future__ import annotations

import os
import queue
import shutil
import subprocess
import tempfile
import threading
import tkinter as tk
from pathlib import Path
from tkinter import filedialog, messagebox, ttk

APP_TITLE = "RIFT Studio"
DEFAULT_SOURCE = """# RIFT Studio scratch file\n# Try Tokenize or Compile to exercise the RIFT toolchain.\n\npattern greeting = /hello/;\n\n"""


class RiftStudio(tk.Tk):
    def __init__(self) -> None:
        super().__init__()
        self.title(APP_TITLE)
        self.geometry("1200x760")
        self.minsize(900, 600)
        self.current_path: Path | None = None
        self.dirty = False
        self.events: queue.Queue[tuple[str, object]] = queue.Queue()
        self._build_style()
        self._build_menu()
        self._build_ui()
        self._set_source(DEFAULT_SOURCE)
        self.after(50, self._drain_events)
        self.protocol("WM_DELETE_WINDOW", self._close)

    def _build_style(self) -> None:
        style = ttk.Style(self)
        try:
            style.theme_use("clam")
        except tk.TclError:
            pass
        style.configure("Title.TLabel", font=("TkDefaultFont", 14, "bold"))
        style.configure("Status.TLabel", padding=(8, 4))
        style.configure("Run.TButton", padding=(12, 6))

    def _build_menu(self) -> None:
        menu = tk.Menu(self)
        file_menu = tk.Menu(menu, tearoff=False)
        file_menu.add_command(label="New", command=self.new_file, accelerator="Ctrl+N")
        file_menu.add_command(label="Open…", command=self.open_file, accelerator="Ctrl+O")
        file_menu.add_command(label="Save", command=self.save_file, accelerator="Ctrl+S")
        file_menu.add_command(label="Save As…", command=self.save_as)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self._close)
        menu.add_cascade(label="File", menu=file_menu)
        run_menu = tk.Menu(menu, tearoff=False)
        for command in ("tokenize", "parse", "compile"):
            run_menu.add_command(label=command.title(), command=lambda c=command: self.run_command(c))
        menu.add_cascade(label="RIFT", menu=run_menu)
        self.config(menu=menu)
        self.bind_all("<Control-n>", lambda _e: self.new_file())
        self.bind_all("<Control-o>", lambda _e: self.open_file())
        self.bind_all("<Control-s>", lambda _e: self.save_file())

    def _build_ui(self) -> None:
        root = ttk.Frame(self, padding=10)
        root.pack(fill="both", expand=True)

        header = ttk.Frame(root)
        header.pack(fill="x", pady=(0, 8))
        ttk.Label(header, text="RIFT Studio", style="Title.TLabel").pack(side="left")
        self.path_label = ttk.Label(header, text="Untitled .rf")
        self.path_label.pack(side="left", padx=12)
        self.rift_label = ttk.Label(header, text=self._rift_display())
        self.rift_label.pack(side="right")

        toolbar = ttk.Frame(root)
        toolbar.pack(fill="x", pady=(0, 8))
        for label, command in (("New", self.new_file), ("Open", self.open_file), ("Save", self.save_file)):
            ttk.Button(toolbar, text=label, command=command).pack(side="left", padx=(0, 5))
        ttk.Separator(toolbar, orient="vertical").pack(side="left", fill="y", padx=6)
        for command in ("tokenize", "parse", "compile"):
            ttk.Button(toolbar, text=command.title(), style="Run.TButton",
                       command=lambda c=command: self.run_command(c)).pack(side="left", padx=3)
        ttk.Button(toolbar, text="Clear Output", command=self.clear_output).pack(side="right")

        paned = ttk.PanedWindow(root, orient="vertical")
        paned.pack(fill="both", expand=True)

        editor_frame = ttk.Frame(paned)
        output_frame = ttk.Frame(paned)
        paned.add(editor_frame, weight=3)
        paned.add(output_frame, weight=2)

        self.editor = tk.Text(editor_frame, undo=True, wrap="none", font=("TkFixedFont"),
                              padx=10, pady=10, insertwidth=2)
        yscroll = ttk.Scrollbar(editor_frame, orient="vertical", command=self.editor.yview)
        xscroll = ttk.Scrollbar(editor_frame, orient="horizontal", command=self.editor.xview)
        self.editor.configure(yscrollcommand=yscroll.set, xscrollcommand=xscroll.set)
        self.editor.grid(row=0, column=0, sticky="nsew")
        yscroll.grid(row=0, column=1, sticky="ns")
        xscroll.grid(row=1, column=0, sticky="ew")
        editor_frame.rowconfigure(0, weight=1)
        editor_frame.columnconfigure(0, weight=1)
        self.editor.bind("<<Modified>>", self._on_modified)

        out_header = ttk.Frame(output_frame)
        out_header.pack(fill="x")
        ttk.Label(out_header, text="Tool output").pack(side="left", padx=5, pady=5)
        self.output = tk.Text(output_frame, wrap="none", state="disabled", font=("TkFixedFont"),
                              padx=10, pady=10)
        out_scroll = ttk.Scrollbar(output_frame, orient="vertical", command=self.output.yview)
        self.output.configure(yscrollcommand=out_scroll.set)
        self.output.pack(side="left", fill="both", expand=True)
        out_scroll.pack(side="right", fill="y")

        self.status = ttk.Label(root, text="Ready", style="Status.TLabel", anchor="w")
        self.status.pack(fill="x", pady=(8, 0))

    def _rift_candidates(self) -> list[str]:
        candidates = []
        if os.environ.get("RIFT_BIN"):
            candidates.append(os.environ["RIFT_BIN"])
        here = Path(__file__).resolve()
        repo = here.parents[2]
        candidates.extend([
            str(repo / "build" / "bin" / "linux-GNU" / "rift"),
            str(repo / "build" / "bin" / "linux-clang" / "rift"),
            str(repo / "build" / "bin" / "windows-mingw" / "rift.exe"),
            str(repo / "bin" / "rift"),
            shutil.which("rift") or "",
        ])
        return [c for c in candidates if c]

    def _rift_display(self) -> str:
        for candidate in self._rift_candidates():
            if Path(candidate).exists() or shutil.which(candidate):
                return f"RIFT: {candidate}"
        return "RIFT: not found"

    def _find_rift(self) -> str | None:
        for candidate in self._rift_candidates():
            if Path(candidate).is_file() and os.access(candidate, os.X_OK):
                return candidate
        return shutil.which("rift")

    def _set_source(self, text: str) -> None:
        self.editor.delete("1.0", "end")
        self.editor.insert("1.0", text)
        self.editor.edit_modified(False)
        self.dirty = False
        self._refresh_title()

    def _on_modified(self, _event=None) -> None:
        if self.editor.edit_modified():
            self.dirty = True
            self._refresh_title()
            self.editor.edit_modified(False)

    def _refresh_title(self) -> None:
        name = self.current_path.name if self.current_path else "Untitled .rf"
        marker = " *" if self.dirty else ""
        self.title(f"{APP_TITLE} — {name}{marker}")
        self.path_label.configure(text=f"{name}{marker}")

    def new_file(self) -> None:
        if not self._confirm_discard():
            return
        self.current_path = None
        self._set_source(DEFAULT_SOURCE)
        self.status.configure(text="New scratch file")

    def open_file(self) -> None:
        if not self._confirm_discard():
            return
        path = filedialog.askopenfilename(filetypes=[("RIFT files", "*.rf *.rift"), ("All files", "*.*")])
        if not path:
            return
        try:
            text = Path(path).read_text(encoding="utf-8")
        except OSError as exc:
            messagebox.showerror(APP_TITLE, str(exc))
            return
        self.current_path = Path(path)
        self._set_source(text)
        self.status.configure(text=f"Opened {path}")

    def save_file(self) -> bool:
        if self.current_path is None:
            return self.save_as()
        try:
            self.current_path.write_text(self.editor.get("1.0", "end-1c"), encoding="utf-8")
            self.dirty = False
            self._refresh_title()
            self.status.configure(text=f"Saved {self.current_path}")
            return True
        except OSError as exc:
            messagebox.showerror(APP_TITLE, str(exc))
            return False

    def save_as(self) -> bool:
        path = filedialog.asksaveasfilename(defaultextension=".rf",
                                            filetypes=[("RIFT files", "*.rf"), ("All files", "*.*")])
        if not path:
            return False
        self.current_path = Path(path)
        return self.save_file()

    def _confirm_discard(self) -> bool:
        if not self.dirty:
            return True
        answer = messagebox.askyesnocancel(APP_TITLE, "Save changes before continuing?")
        if answer is None:
            return False
        return self.save_file() if answer else True

    def _command_input(self) -> tuple[Path, bool]:
        if self.current_path and not self.dirty:
            return self.current_path, False
        fd, name = tempfile.mkstemp(prefix="rift-studio-", suffix=".rf")
        os.close(fd)
        path = Path(name)
        path.write_text(self.editor.get("1.0", "end-1c"), encoding="utf-8")
        return path, True

    def run_command(self, command: str) -> None:
        rift = self._find_rift()
        if not rift:
            messagebox.showerror(APP_TITLE,
                                 "RIFT CLI was not found. Build the repository or set RIFT_BIN.")
            return
        if command == "compile" and self.current_path and self.dirty:
            self.status.configure(text="Compile uses the current unsaved editor buffer")
        path, temporary = self._command_input()
        args = [rift, command, str(path)]
        self._append_output(f"$ {' '.join(args)}\n")
        self.status.configure(text=f"Running rift {command}…")
        threading.Thread(target=self._worker, args=(args, path if temporary else None), daemon=True).start()

    def _worker(self, args: list[str], temporary: Path | None) -> None:
        try:
            proc = subprocess.run(args, text=True, capture_output=True, timeout=60)
            self.events.put(("result", (proc.returncode, proc.stdout, proc.stderr)))
        except (OSError, subprocess.TimeoutExpired) as exc:
            self.events.put(("error", str(exc)))
        finally:
            if temporary:
                try:
                    temporary.unlink()
                except OSError:
                    pass

    def _drain_events(self) -> None:
        try:
            while True:
                kind, payload = self.events.get_nowait()
                if kind == "result":
                    code, stdout, stderr = payload  # type: ignore[misc]
                    if stdout:
                        self._append_output(stdout)
                    if stderr:
                        self._append_output(stderr)
                    state = "completed" if code == 0 else f"failed (exit {code})"
                    self.status.configure(text=f"RIFT command {state}")
                else:
                    self._append_output(f"ERROR: {payload}\n")
                    self.status.configure(text="RIFT command failed")
        except queue.Empty:
            pass
        self.after(50, self._drain_events)

    def _append_output(self, text: str) -> None:
        self.output.configure(state="normal")
        self.output.insert("end", text if text.endswith("\n") else text + "\n")
        self.output.see("end")
        self.output.configure(state="disabled")

    def clear_output(self) -> None:
        self.output.configure(state="normal")
        self.output.delete("1.0", "end")
        self.output.configure(state="disabled")
        self.status.configure(text="Output cleared")

    def _close(self) -> None:
        if self._confirm_discard():
            self.destroy()


if __name__ == "__main__":
    RiftStudio().mainloop()
