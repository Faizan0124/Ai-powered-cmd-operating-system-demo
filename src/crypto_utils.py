import hashlib
from cryptography.fernet import Fernet
import os

KEY_FILE = "VFS/.syskey"

def get_or_create_key():
    if not os.path.exists("VFS"):
        os.makedirs("VFS")
    if not os.path.exists(KEY_FILE):
        key = Fernet.generate_key()
        with open(KEY_FILE, "wb") as f:
            f.write(key)
    else:
        with open(KEY_FILE, "rb") as f:
            key = f.read()
    return key

def hash_password(password: str) -> str:
    return hashlib.sha256(password.encode()).hexdigest()

def encrypt_data(data: bytes) -> bytes:
    key = get_or_create_key()
    f = Fernet(key)
    return f.encrypt(data)

def decrypt_data(data: bytes) -> bytes:
    key = get_or_create_key()
    f = Fernet(key)
    return f.decrypt(data)
