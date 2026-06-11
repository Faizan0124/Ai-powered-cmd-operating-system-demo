# 🚀 Bambolino Operating System

Welcome to **Bambolino OS**, a lightweight, incredibly feature-rich Operating System simulation built completely in C++! 

This project was built to demonstrate core Operating System concepts (Process Management, Memory Monitoring, Multithreading, File Systems, and System Calls) while also featuring **Bambolino**, an integrated AI virtual assistant that can manage the OS for you!

---

## ✨ Amazing Features

- **🧠 Bambolino AI**: Talk to the built-in AI! Just type `bambolino make a file called test.txt` and it will automatically execute the commands for you! 
- **🔊 Sound Effects**: The OS features an integrated WinMM audio engine. Every time a command successfully finishes, it plays a custom `faah.wav` sound effect.
- **🛡️ Secure Virtual File System**: Create, read, and edit files inside the OS. Every file you create is actually locked and **XOR-encrypted** on your real hard drive, meaning nobody can read them without logging into the OS!
- **💻 Real OS Concepts**:
  - `ps`: View real running computer processes.
  - `mem`: View live RAM usage via the Windows Kernel.
  - `bgjob`: Spawn detached C++ background threads.
  - `spawn <app>`: Launch real child processes.
  - `syscall`: Trigger direct low-level Win32 API calls.
- **📝 Admin Logging**: Every action (logins, commands, errors) is securely logged in plaintext for admins to audit.
- **🎨 Beautiful UI**: Enjoy a gorgeous terminal interface packed with ASCII art and ANSI colors!

---

## 🛠️ How to Setup (For Friends!)

It is incredibly easy to get this running on your Windows machine!

### 1. Prerequisites
You only need one thing: **MinGW (g++ compiler)**.
If you don't have it, download and install MinGW so that the `g++` command works in your terminal. 

### 2. The Sound Effect
Make sure you have a sound file named **`faah.wav`** directly inside this main project folder. If it's missing, the OS will still work perfectly, you just won't hear the cool sound effects!

### 3. Build the OS
Simply double-click the **`build.bat`** file! 
This script will automatically gather all the C++ files, link the Windows Memory and Audio libraries, and compile the entire OS into a single executable in just a few seconds.

### 4. Run the OS
Once the build is done, you will see a new file called **`BambolinoOS.exe`**. 
Double-click it (or run it from your terminal) to boot up the system!

---

## 🔑 Default Login Accounts

When you boot up Bambolino OS for the first time, use these credentials to log in:

- **Admin Account** (Has full access to `adduser` and `logs`):
  - Username: `admin`
  - Password: `admin123`

- **Standard User Account**:
  - Username: `user`
  - Password: `user123`

---

## 🐣 Bambolino Easter Eggs

If you want to see Bambolino's custom-programmed personality, try asking it these exact phrases in the terminal:

1. `bambolino who is your favourite teacher`
2. `bambolino who did the most work?`
3. `bambolino yeh nhi kehna tha bambolino.`

---

## 🎓 OS Lab Presentation

If you are grading or presenting this project for an OS Lab, log in as `admin` and type the master command:
**`showcase`**

This will print out a perfectly formatted report detailing exactly where and how all 6 core OS concepts (Process Management, Memory, File Systems, etc.) were implemented in the code!

Enjoy Bambolino OS!
