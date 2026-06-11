class SettingsManager:
    def __init__(self):
        self.wifi_enabled = True
        self.theme = "light"
        
    def toggle_wifi(self):
        self.wifi_enabled = not self.wifi_enabled
        return self.wifi_enabled
        
    def get_networks(self):
        if self.wifi_enabled:
            return ["Home_Network", "Campus_Wifi", "OS_Lab_Router"]
        return []

    def set_theme(self, theme_name):
        self.theme = theme_name

    def get_theme_colors(self):
        if self.theme == "dark":
            return {"bg": "#2e2e2e", "fg": "#ffffff", "button": "#444444", "accent": "#007acc"}
        else:
            return {"bg": "#f0f0f0", "fg": "#000000", "button": "#e0e0e0", "accent": "#007acc"}
