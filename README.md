🎲 Namepicker / Namesorter
(Work in progress)

(The Idea)

A fast, command-line utility written in C++ to randomly assign students, team members, or Discord users into balanced groups. Perfect for teachers managing classrooms or community hosts organizing challenges.

Built with Modern C++ (C++17), featuring constraint handling (e.g., "keep Alice and Bob apart"), file I/O, and reproducible randomization.

✨ Key Features
⚡ Instant Grouping: Randomly distribute names into $N$ groups with perfectly balanced sizes (difference of max 1 member).
🚫 Smart Constraints: Prevent specific pairs from being in the same group (e.g., separating friends who talk too much or ensuring diverse skill levels).
📂 File Support:
Load names from a simple text or CSV file (roster.txt).
Export generated groups to a formatted text file ready for copy-pasting.
🔒 Reproducible Results: Use a --seed flag to generate the exact same groups every time (great for debugging or recreating a specific scenario).
💻 CLI & Scriptable: Designed for the terminal, making it easy to integrate into scripts or use quickly in VS Code.
🛠️ Installation & Build
Prerequisites
A C++ compiler supporting C++17 (GCC 7+, Clang 5+, MSVC 2017+).
CMake (version 3.10+).
Build Steps
Clone or download the repository.
Open the project folder in VS Code.
Install the CMake Tools extension (if not already installed).
Open the terminal (Ctrl + ~) and run:
