##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=OllyDog
ConfigurationName      :=Debug
WorkspacePath          := "E:\Projects\OllyDBG"
ProjectPath            := "E:\Projects\OllyDBG\OllyExt"
IntermediateDirectory  :=Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Administrator
Date                   :=19/08/2015
CodeLitePath           :="d:\Program Files\CodeLite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName).dll
Preprocessors          :=$(PreprocessorSwitch)UNICODE $(PreprocessorSwitch)_UNICODE $(PreprocessorSwitch)_CHAR_UNSIGNED 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="OllyDog.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=windres
LinkOptions            :=  /incremental:no /DEBUG
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)d:/Analyzer/OllyDBG/odsdk $(IncludeSwitch)d:/Analyzer/OllyDBG/python/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)kernel32.lib $(LibrarySwitch)user32.lib $(LibrarySwitch)gdi32.lib $(LibrarySwitch)Shlwapi.lib $(LibrarySwitch)Comctl32.lib $(LibrarySwitch)advapi32.lib $(LibrarySwitch)ollydbg.lib 
ArLibs                 :=  "kernel32.lib" "user32.lib" "gdi32.lib" "Shlwapi.lib" "Comctl32.lib" "advapi32.lib" "ollydbg.lib" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)d:/Analyzer/OllyDBG/odsdk $(LibraryPathSwitch)d:/Analyzer/OllyDBG/python/libs 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcu
CXX      := g++
CC       := gcc
CXXFLAGS :=  /Zi /MD /W3 /EHsc /Od  $(Preprocessors)
CFLAGS   :=  /Zi /MD /W3 /EHsc /Od  $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=d:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/DllMain.cpp$(ObjectSuffix) $(IntermediateDirectory)/CmdBar.cpp$(ObjectSuffix) $(IntermediateDirectory)/Options.cpp$(ObjectSuffix) $(IntermediateDirectory)/CmdExe.cpp$(ObjectSuffix) $(IntermediateDirectory)/CmdLog.cpp$(ObjectSuffix) $(IntermediateDirectory)/CoreApi.cpp$(ObjectSuffix) $(IntermediateDirectory)/Python.cpp$(ObjectSuffix) $(IntermediateDirectory)/Lua.cpp$(ObjectSuffix) $(IntermediateDirectory)/Perl.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(SharedObjectLinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)
	@$(MakeDirCommand) "E:\Projects\OllyDBG/.build-debug"
	@echo rebuilt > "E:\Projects\OllyDBG/.build-debug/OllyDog"

MakeIntermediateDirs:
	@$(MakeDirCommand) "Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/DllMain.cpp$(ObjectSuffix): DllMain.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/DllMain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/DllMain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/DllMain.cpp$(PreprocessSuffix): DllMain.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/DllMain.cpp$(PreprocessSuffix) "DllMain.cpp"

$(IntermediateDirectory)/CmdBar.cpp$(ObjectSuffix): CmdBar.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/CmdBar.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CmdBar.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CmdBar.cpp$(PreprocessSuffix): CmdBar.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CmdBar.cpp$(PreprocessSuffix) "CmdBar.cpp"

$(IntermediateDirectory)/Options.cpp$(ObjectSuffix): Options.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/Options.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Options.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Options.cpp$(PreprocessSuffix): Options.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Options.cpp$(PreprocessSuffix) "Options.cpp"

$(IntermediateDirectory)/CmdExe.cpp$(ObjectSuffix): CmdExe.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/CmdExe.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CmdExe.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CmdExe.cpp$(PreprocessSuffix): CmdExe.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CmdExe.cpp$(PreprocessSuffix) "CmdExe.cpp"

$(IntermediateDirectory)/CmdLog.cpp$(ObjectSuffix): CmdLog.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/CmdLog.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CmdLog.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CmdLog.cpp$(PreprocessSuffix): CmdLog.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CmdLog.cpp$(PreprocessSuffix) "CmdLog.cpp"

$(IntermediateDirectory)/CoreApi.cpp$(ObjectSuffix): CoreApi.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/CoreApi.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/CoreApi.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/CoreApi.cpp$(PreprocessSuffix): CoreApi.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/CoreApi.cpp$(PreprocessSuffix) "CoreApi.cpp"

$(IntermediateDirectory)/Python.cpp$(ObjectSuffix): Python.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/Python.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Python.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Python.cpp$(PreprocessSuffix): Python.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Python.cpp$(PreprocessSuffix) "Python.cpp"

$(IntermediateDirectory)/Lua.cpp$(ObjectSuffix): Lua.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/Lua.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Lua.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Lua.cpp$(PreprocessSuffix): Lua.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Lua.cpp$(PreprocessSuffix) "Lua.cpp"

$(IntermediateDirectory)/Perl.cpp$(ObjectSuffix): Perl.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "E:/Projects/OllyDBG/OllyExt/Perl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/Perl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/Perl.cpp$(PreprocessSuffix): Perl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/Perl.cpp$(PreprocessSuffix) "Perl.cpp"

##
## Clean
##
clean:
	$(RM) -r Debug/


