## Sample Visual Studio project

Use Visual Studio 2022 to build.

## Caution

Do not start new projects by copying the `.vcxproj` files. They contain unique GUIDs and Visual Studio may misbehave on encountering the same GUID in more than one project.

Always use the *New Project* command of *Visual Studio* to start a new project. Remember to set project *Properties* -> *C/C++* -> *Language* -> *C++ Language Standard* to `c++20` or `c++latest` because the default is `c++14`.

## Hints

The *New Project* command will create a directory `X` containing these files:
- `X.vcxproj` - the list of source files and compilations settings. If lost, it may be recreated (with default settings) using *Create project from existing code*.
- `X.vcxproj.filters` - classification of the source files into virtual folders shown in Visual Studio; not required.
- `X.vcxproj.user` - local configuration of the IDE; it does not affect build but contains debug settings, including program command-line. It may contain machine-specific configuration - do not commit this file into git. Visual Studio will create this file if it is missing.
- `X.sln` - the *Solution* file, containing the name(s) of the project file(s). Visual Studio will recreate this file if it is missing.

You may rename or move the directory afterwards. Renaming the project files will break the Solution but Visual Studio will allow you to repair it. 

Do not put more than one `.vcxproj` file in the same folder. It may cause conflicts between temporary files when building.

