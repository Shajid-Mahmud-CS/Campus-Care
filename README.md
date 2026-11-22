# CampusCare – Campus Complaint Management System

A C++17 console application for students to submit and track campus complaints and for administrators to manage complaint status.

## Functional modules
1. Student registration and login
2. Complaint submission
3. Complaint listing and category search
4. Complaint status management by administrator
5. Student feedback and rating
6. Status and category statistics
7. File-based persistence (`users.txt`, `complaints.txt`)

## Add your application source

Copy your final, current `CampusCare.cpp` (the exact source you submit) into this project folder, beside this README and `CMakeLists.txt`. The uploaded source is intentionally not replaced with an older copy. The tests include `../CampusCare.cpp`, so the filename and location must match.

## Build

```bash
g++ -std=c++17 CampusCare.cpp -o CampusCare
./CampusCare
```

On Windows with MinGW, use `g++ -std=c++17 CampusCare.cpp -o CampusCare.exe`.

## Default administrator

- Email: `admin@campuscare.local`
- Password: `admin123`

Change this default credential before any real deployment. The current `std::hash` password transformation is not a secure password-storage algorithm; use a dedicated password-hashing algorithm such as Argon2 or bcrypt for production.

## Testing

The `tests/` folder contains GoogleTest unit tests for deterministic utility and model functions. Build and run them using CMake:

```bash
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

The tests include the application source with its `main` renamed. The test process uses the build directory as its working directory so its file database does not overwrite the developer's normal data files.

## Project documents

See `docs/` for requirements, process model, design-pattern analysis, UML, test plan, Git workflow, learning log, and reflection essay.

## Honest evidence note

Documentation and workflow files are templates/instructions, not proof that GitHub branches, reviewed pull requests, merge conflicts, or learning activities have already happened. Create the listed repository history and keep screenshots/logs of the actual work. Do not claim Selenium testing of this console-only application; Selenium requires a browser-based interface.
