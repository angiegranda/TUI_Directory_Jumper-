## TUI Change Directory Documentation

## Introduction

This project is a multiplatform tool that lets you switch directories smoothly from the terminal.

## Prerequisites
* CMake ≥ 3.29
* C++20 compiler (g++, clang++, or MSVC)
* Mac/Linux, Windows 10 or older.

## Building the program 
* From the command line:
    - For Mac/Linux: make run-dev
    - For Windows: make run-dev-windows 
* When using Visual Studio, update CMake Settings to select the preset you need.

## Program settings.
It’s recommended to use the project name as the program name, though you can choose whatever you like.

* Mac/Linux: Add this function to your `.bashrc` or `.zshrc`:   
```
program_name() {
cd "$(/PATH/project/build/dev/tcd)" || echo "Invalid path"
}
```

* Windows: In PowerShell, do the following:  
1. First check if you have a `$PROFILE` file already with this command: `Test-Path $PROFILE`.   
It will return false if it doesnt exist or true if it does. 
2. If false, create the file: `New-Item -Path $PROFILE -ItemType File -Force`.
3. Run: `notepad $PROFILE`.  
Inside $PROFILE write and save:   
2. Add and save this inside the file:  
```
function program_name {
    $exe = "C:\PATH\project\build\dev\tcd.exe"
    $path = & $exe
    if ($path -and (Test-Path $path)) {
        Set-Location $path
    } else {
        Write-Host "Invalid path returned: $path"
    }
}
```
3. Update the changes: `. $PROFILE`.  
4. Call `program_name` from anywhere to run the program.  

## User Documentation 

You’ll need to build the program unless you’ve already downloaded the executable (tcd for Mac/Linux or tcd.exe for Windows). Then follow the program setup steps above.

**Quick Access View**

The program starts in Quick Access, which shows paths you’ve visited most often or recently. The more you use it, the better it gets at suggesting relevant paths.

The actions are:  
1. [`j`] Move cursor down and [`k`] up thorough the top paths by relevance order.  
2. [`c`] Switch to Navigation mode to pick a directory manually.  
3. [`x`] Clear history (start fresh with suggestions).  
3. [`s`] Select a suggested path.  
4. [`q`] Quit – returns you to the directory where you launched the program (doesn’t affect history). 

**Navigation View**

This lets you manually browse directories and preview text files.

The actions are:
1. [`j`] Move down and [`k`] up the cursor in the current directory.  
2. [`c`] Change to Quick Access. 
3. [`h`] Go up to the parent directory. 
4. [`l`] Enter the selected directory (only works if it’s a directory). 
5. [`s`] Selects the file/directory the cursor is on.
6. [`q`] Quit – stays in the directory where the program was launched. 

**Modifiable settings at Constants.h and TerminalConstants.h**
- `MAX_QUICK_ACCESS_ITEMS` set to display up to 30 top paths, can be changed, the final amounts depends on the visible screen and the maximum number of top paths.
- For Windows: `WINDOWS_TERMINAL_WIDTH` (90) and `WINDOWS_TERMINAL_HEIGHT` (30) can be set to be different values.
- Text colors and highlights can also be changed in TerminalConstants. 
The colors are thought to work well on dark mode since it is the most popular mode used by terminal users.

## Dev Documentation

The project only uses the standard library. Exceptions like std::bad_alloc are unlikely, even with directories containing many files, but if something goes wrong, the program safely returns to the starting path.

The architecture pattern is **Model-View-Controller (MVC)** where:  
* Controller: coordinates everything.  
* NavigationView and QuickAccessView: takes data given by controller and prepares the 
output that will be displayed for their specific view.  
* Controller takes input from UI, but output is delegated to the views.  
This keeps presentation logic separate and makes the code easier to maintain and test.  

**Algorithm used for Quick Access**

Most relevant paths are those that are visited frequently or recently.  
Suppose the current timestamp is T, p is a path and t₁, …, tₖ are the timestamps path p was visited.  

The score is:

![\f[ p_{\text{score}} = \sum_{i=t_{1}}^{t_{k}} \lambda^{\,T - i} \f]](path_score_formula.png)

Clearly, the most recent visits will add more value to the score, past visits contribute less but constant visits will also give relevance to frequent paths. After calculating the scores of the paths, we sort them by descendent order. 

Once a path is selected, time T is inserted into the visits vector of that path, T+1 is the updated time and then this data is saved in quick_access.csv.

The format of quick_access.csv is:  
Line 1: Previous time stamp starting by 0. 
Line 2: /Path/,t1, t2,...,tk. 
Line 3: same as previous   

The **src/** contains:  
- **src/Main.cpp**: Creates an instance of Controller and runs it. The program will inmediatly stop if one of the required files needed for the program workflow throws an exception.  
- **src/controller/**: Contains core logic classes. Buffer and Explorer classes handle navigation.  
- **src/ui/**: UI-related classes (NavigationView, QuickAccessView, UI, TerminalWriter, WindowsResizeMonitor). TerminalWriter uses ANSI escape codes which are enabled and works for Windows (+10). If the version would be older then this would be the only file that would need to be modified. WindowsResizeMonitor runs a safe background thread that checks every 200 ms if the window was resized, and updates the display accordingly.  

This project uses [Doxygen](https://www.doxygen.nl) to generate API documentation.

1. Make sure Doxygen is installed on your system.  
   - On Windows: https://www.doxygen.nl/download.html. 
   - On Mac/Linux: use your package manager, e.g., `sudo apt install doxygen` or `brew install doxygen`.  
2. In order to visualize the dependency graph install:  
    - On Mac/Linux: `brew install graphviz`  

3. To generate the documentation, run:  
`doxygen Doxyfile`. 
