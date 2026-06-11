import customtkinter as ctk
import tkinter as tk
from tkinter import messagebox, filedialog
from src.filesystem import create_file, read_file
try:
    from PIL import Image, ImageTk
except ImportError:
    pass

class TextEditor(ctk.CTkToplevel):
    def __init__(self, parent, filepath=""):
        super().__init__(parent)
        self.parent_os = parent
        self.title("Text Editor")
        self.geometry("600x400")
        self.filepath = filepath

        self.text_area = ctk.CTkTextbox(self)
        self.text_area.pack(expand=True, fill='both', padx=10, pady=10)
        
        btn_frame = ctk.CTkFrame(self, fg_color="transparent")
        btn_frame.pack(fill='x', padx=10, pady=(0, 10))
        
        save_btn = ctk.CTkButton(btn_frame, text="Save File", command=self.save_file)
        save_btn.pack(side="right")

        if self.filepath:
            success, data = read_file(self.filepath)
            if success:
                self.text_area.insert("1.0", data)
            elif data != "File not found":
                messagebox.showerror("Error", data)

    def save_file(self):
        content = self.text_area.get("1.0", tk.END)
        if not self.filepath:
            dialog = ctk.CTkInputDialog(text="Enter virtual path (e.g., docs/file.txt):", title="Save File")
            path = dialog.get_input()
            if not path: return
            self.filepath = path
        
        success, msg = create_file(self.filepath, content)
        if success:
            messagebox.showinfo("Success", "File saved successfully (Encrypted)")
            if hasattr(self.parent_os, 'refresh_fs'):
                self.parent_os.refresh_fs()
        else:
            messagebox.showerror("Error", msg)

class Calculator(ctk.CTkToplevel):
    def __init__(self, parent):
        super().__init__(parent)
        self.title("Calculator")
        self.geometry("300x400")
        
        self.result_var = tk.StringVar()
        entry = ctk.CTkEntry(self, textvariable=self.result_var, font=("Arial", 24), justify='right')
        entry.pack(fill='x', padx=10, pady=15, ipady=10)

        buttons = [
            '7', '8', '9', '/',
            '4', '5', '6', '*',
            '1', '2', '3', '-',
            'C', '0', '=', '+'
        ]
        
        frame = ctk.CTkFrame(self, fg_color="transparent")
        frame.pack(expand=True, fill='both', padx=10, pady=(0, 10))

        for i, b in enumerate(buttons):
            color = "#1f538d" if b in ['/', '*', '-', '+', '='] else ("#d32f2f" if b == 'C' else None)
            btn = ctk.CTkButton(frame, text=b, font=("Arial", 20), 
                                command=lambda x=b: self.on_click(x), fg_color=color)
            btn.grid(row=i//4, column=i%4, sticky='nsew', padx=2, pady=2)
            
        for i in range(4):
            frame.columnconfigure(i, weight=1)
            frame.rowconfigure(i, weight=1)

    def on_click(self, char):
        if char == 'C':
            self.result_var.set("")
        elif char == '=':
            try:
                res = str(eval(self.result_var.get()))
                self.result_var.set(res)
            except:
                self.result_var.set("Error")
        else:
            self.result_var.set(self.result_var.get() + char)

class Notes(TextEditor):
    def __init__(self, parent):
        super().__init__(parent, filepath="Notes/quick_note.txt")
        self.title("Notes App")

class MediaViewer(ctk.CTkToplevel):
    def __init__(self, parent, filepath=""):
        super().__init__(parent)
        self.title("Media Viewer")
        self.geometry("500x500")

        self.label = ctk.CTkLabel(self, text="Load an image")
        self.label.pack(expand=True)
        
        btn = ctk.CTkButton(self, text="Open System Image", command=self.load_image)
        btn.pack(pady=20)
        
    def load_image(self):
        path = filedialog.askopenfilename()
        if path:
            try:
                img = Image.open(path)
                img.thumbnail((450, 450))
                # CustomTkinter has CTkImage for high DPI support
                self.photo = ctk.CTkImage(light_image=img, dark_image=img, size=(img.width, img.height))
                self.label.configure(image=self.photo, text="")
            except Exception as e:
                messagebox.showerror("Error", "Could not load image.")

class BuiltInWebBrowser(ctk.CTkToplevel):
    def __init__(self, parent):
        super().__init__(parent)
        self.title("Web Browser")
        self.geometry("800x600")
        
        try:
            from tkinterweb import HtmlFrame
            
            top_bar = ctk.CTkFrame(self, height=40)
            top_bar.pack(fill="x", padx=5, pady=5)
            
            self.url_var = tk.StringVar(value="https://www.google.com/?gbv=1")
            url_entry = ctk.CTkEntry(top_bar, textvariable=self.url_var, width=600)
            url_entry.pack(side="left", padx=5)
            url_entry.bind("<Return>", self.load_url)
            
            go_btn = ctk.CTkButton(top_bar, text="Go", width=50, command=self.load_url)
            go_btn.pack(side="left", padx=5)
            
            self.frame = HtmlFrame(self, messages_enabled=False)
            self.frame.pack(fill="both", expand=True)
            self.load_url()
        except ImportError:
            lbl = ctk.CTkLabel(self, text="tkinterweb not installed. Run: pip install tkinterweb")
            lbl.pack(expand=True)

    def load_url(self, event=None):
        import urllib.parse
        url = self.url_var.get().strip()
        if not url.startswith("http://") and not url.startswith("https://"):
            if "." in url and " " not in url:
                url = "https://" + url
            else:
                query = urllib.parse.quote_plus(url)
                url = "https://www.google.com/search?q=" + query + "&gbv=1"
        
        self.url_var.set(url)
        self.frame.load_website(url)
