import sys
from PyQt6.QtCore import QUrl
from PyQt6.QtWidgets import QApplication, QMainWindow, QLineEdit, QToolBar, QWidget, QVBoxLayout
from PyQt6.QtWebEngineWidgets import QWebEngineView

class Browser(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Web Browser")
        self.setGeometry(100, 100, 1024, 768)
        
        # Use dark styling to match OS
        self.setStyleSheet("""
            QMainWindow {
                background-color: #2b2b2b;
            }
            QToolBar {
                background-color: #1e1e1e;
                border: none;
                padding: 5px;
            }
            QLineEdit {
                background-color: #333333;
                color: #ffffff;
                border: 1px solid #555555;
                padding: 5px;
                border-radius: 5px;
                font-family: Arial;
                font-size: 14px;
            }
        """)

        self.browser = QWebEngineView()
        self.browser.setUrl(QUrl("https://www.google.com"))
        
        self.setCentralWidget(self.browser)
        
        navbar = QToolBar("Navigation")
        navbar.setMovable(False)
        self.addToolBar(navbar)
        
        self.url_bar = QLineEdit()
        self.url_bar.setText("https://www.google.com")
        self.url_bar.returnPressed.connect(self.navigate_to_url)
        navbar.addWidget(self.url_bar)
        
        self.browser.urlChanged.connect(self.update_url)

    def navigate_to_url(self):
        url = self.url_bar.text().strip()
        if not url.startswith("http://") and not url.startswith("https://"):
            if "." in url and " " not in url:
                url = "https://" + url
            else:
                import urllib.parse
                query = urllib.parse.quote_plus(url)
                url = "https://www.google.com/search?q=" + query
        self.browser.setUrl(QUrl(url))
        
    def update_url(self, q):
        self.url_bar.setText(q.toString())

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Browser()
    window.show()
    sys.exit(app.exec())
