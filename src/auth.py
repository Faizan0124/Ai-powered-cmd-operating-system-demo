import json
import os
from src.crypto_utils import hash_password

AUTH_FILE = "VFS/users.json"

def initialize_auth():
    if not os.path.exists("VFS"):
        os.makedirs("VFS")
    if not os.path.exists(AUTH_FILE):
        default_users = {
            "admin": {
                "password": hash_password("admin123"),
                "role": "admin"
            },
            "user": {
                "password": hash_password("user123"),
                "role": "user"
            }
        }
        with open(AUTH_FILE, "w") as f:
            json.dump(default_users, f, indent=4)

def authenticate(username, password):
    if not os.path.exists(AUTH_FILE):
        initialize_auth()
    with open(AUTH_FILE, "r") as f:
        users = json.load(f)
    
    if username in users:
        if users[username]["password"] == hash_password(password):
            return users[username]["role"]
    return None

def add_user(requester_role, username, password, role="user"):
    if requester_role != "admin":
        return False, "Permission denied."
    
    with open(AUTH_FILE, "r") as f:
        users = json.load(f)
        
    if username in users:
        return False, "User already exists."
        
    users[username] = {
        "password": hash_password(password),
        "role": role
    }
    with open(AUTH_FILE, "w") as f:
        json.dump(users, f, indent=4)
        
    return True, "User created successfully."
