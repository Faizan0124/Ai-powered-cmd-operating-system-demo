import os
from src.crypto_utils import encrypt_data, decrypt_data

VFS_ROOT = "VFS/home"

def init_fs():
    if not os.path.exists(VFS_ROOT):
        os.makedirs(VFS_ROOT)

def get_real_path(virtual_path):
    clean_path = os.path.normpath(virtual_path).lstrip('\\').lstrip('/')
    real_path = os.path.join(VFS_ROOT, clean_path)
    if not os.path.abspath(real_path).startswith(os.path.abspath(VFS_ROOT)):
        return None
    return real_path

def create_file(path, content, encrypted=True):
    real_path = get_real_path(path)
    if not real_path: return False, "Invalid path"
    os.makedirs(os.path.dirname(real_path), exist_ok=True)
    
    data = content.encode('utf-8')
    if encrypted:
        data = encrypt_data(data)
        
    with open(real_path, "wb") as f:
        f.write(data)
    return True, "File created."

def read_file(path, encrypted=True):
    real_path = get_real_path(path)
    if not real_path or not os.path.exists(real_path):
        return False, "File not found"
        
    with open(real_path, "rb") as f:
        data = f.read()
        
    if encrypted:
        try:
            data = decrypt_data(data)
        except Exception:
            return False, "Decryption failed or file corrupted."
            
    return True, data.decode('utf-8', errors='ignore')

def list_directory(path=""):
    real_path = get_real_path(path)
    if not real_path or not os.path.exists(real_path):
        return []
    items = []
    for item in os.listdir(real_path):
        item_path = os.path.join(real_path, item)
        is_dir = os.path.isdir(item_path)
        items.append({"name": item, "is_dir": is_dir})
    return items

def search_files(query, path=""):
    real_path = get_real_path(path)
    if not real_path or not os.path.exists(real_path):
        return []
    
    results = []
    for root, dirs, files in os.walk(real_path):
        for name in files:
            if query.lower() in name.lower():
                rel = os.path.relpath(os.path.join(root, name), VFS_ROOT)
                results.append(rel)
    return results

def delete_file(path):
    real_path = get_real_path(path)
    if not real_path or not os.path.exists(real_path):
        return False, "File not found"
    try:
        if os.path.isdir(real_path):
            os.rmdir(real_path)
        else:
            os.remove(real_path)
        return True, "Deleted."
    except Exception as e:
        return False, str(e)
