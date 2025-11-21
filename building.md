# Building
Currently only Visual Studio is supported. There no plans to support any other compiler.
## Visual Studio
You can try and see if there is a project/SLN file in the **build** directory for your version. If not, you can try and migrate one, or make one from scratch.
### Project from Scratch 
Settings may differ slightly depending on your version of Visual Studio.

**Step 1** - Creating a Project
- Create a new empty Visual Studio Solution Project File. I recommend putting this in the BUILD directory. 
- When asked what kind of project you want, select **Empty Win32 Application**.
- Delete all the default folders/filters it comes with (Source Files, Header Files, and Resource Files)

**Step 2** - Adding Source Files
- Add existing files. I recommend making a filter for each of the folders you will be adding.
- To add the source files, you can do this by going to the menu bar, Project > Add Existing Item. Then navigate to the **SRC** directory. Select all the files (CTRL-A) and press add.

**Step 3** - Project Properties
First go to Project > Properties. Change configuration to All Configurations.

- Under Debugging, change:
-- Working Directory to $(TargetDir)
- Under C/C++ > General, change:
-- Warning Level to "Level 4 (/W4)"
-  Under Linker > Input, change:
-- Additional Dependencies to "comctl32.lib"
-  Manifest Tool > Input and Output, change:
-- Additional Manifest Files to "$(InputDir)..\..\..\src\resources\application.manifest"

Next, Change configuration to Release.
- Under C/C++ > Optimization, Change:
-- Optimization to "Full Optimization (/Ox)"

The project should be ready to build after that.
